#include "global.h"
#include "startacq.h"
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <sys/shm.h>
#include <sys/sem.h>

void initCorr()
{
  in0 = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);
  out0= (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);

  p0  = fftw_plan_dft_1d(nfft, in0, out0, FFTW_FORWARD, FFTW_ESTIMATE);
  in1 = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);
  out1= (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);

  p1  = fftw_plan_dft_1d(nfft, in1, out1, FFTW_FORWARD, FFTW_ESTIMATE);
    
  bufferReal = (double *) malloc(sizeof(double) * nfft/2);
  bufferImg = (double *) malloc(sizeof(double) * nfft/2);
    
  fft0Buffer = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft/2);
  fft1Buffer = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft/2);
  ur = (unsigned short*)malloc(sizeof(unsigned short)*nfft/2);     
}

void freeCorr()
{
  free(in0);
  free(out0);

  fftw_destroy_plan(p0); 
  free(in1);
  free(out1);

  fftw_destroy_plan(p1);
    
  free(bufferReal);
  free(bufferImg);
    
  fftw_free(fft0Buffer);
  fftw_free(fft1Buffer); 
  free(ur);    
}

void *fft0Cal(void *pa)
{
  int j=0,ii=0;
  int check;
  double ts;
  int shmid,sem_id,err=0;
  unsigned long long int temp,bufferLen = adc.bufferLen,i=0,diff;
  int flag; 
    
   
   
  pthread_mutex_lock(&fft0Stop_mutex);
  flag = fft0Stop;
  pthread_mutex_unlock(&fft0Stop_mutex); 
   
  while(flag ==1)
  {
    err=semaphore_p(sem_fft0);
    if(err !=1)
    {
        printf("semaphore-p sem_fft0 error start_file \n");
    }
       
    if(fft0Status[0]==1)
    {
      diff = (fft0Status[1]-fft0Status[2]);
      if(diff%nfft!=0)
      {
        printf("error occured in the copy_thread");
        exit(0);
      }
            
      for(i=0;i<(diff/(nfft*2));i++)
      {    
        temp = fft0Status[2]%bufferLen;
        for (j=0; j<nfft; j++)
	      {
	        in0[j][0]=(float) ((int)*(data0+temp+(2*j)));
	        in0[j][1]=0.0;
	        fft0Status[2]+=2;
	      }
	         
	      fftw_execute(p0);
        pthread_mutex_lock(&fft0_mutex);
        check = fft0Val;
        pthread_mutex_unlock(&fft0_mutex);
        while(check!=0)
        {
          pthread_mutex_lock(&fft0_mutex);
          check = fft0Val;
          pthread_mutex_unlock(&fft0_mutex);
               
          pthread_mutex_lock(&fft0Stop_mutex);
          flag = fft0Stop;
          pthread_mutex_unlock(&fft0Stop_mutex);
              
          if(flag!=1) 
          {
            err = semaphore_v(sem_fft0);
            return;
          }
        }
            
        memcpy(fft0Buffer,out0,sizeof(fftw_complex)*nfft/2);
	             
        pthread_mutex_lock(&fft0_mutex);
        fft0Val=1;
        pthread_mutex_unlock(&fft0_mutex);
            
      }   
        
      fft0Status[0]=0;
           
	  }
         
    err = semaphore_v(sem_fft0);
    if(err !=1)
    {
      printf("semaphore-v fft0 error start_file \n");
    }
        
    pthread_mutex_lock(&fft0Stop_mutex);
    flag = fft0Stop;
    pthread_mutex_unlock(&fft0Stop_mutex);    
     
  }
     
  return;
}

void *fft1Cal(void *pa)
{
  int j=0,ii=0;
  int check;
  double ts;
  int shmid,sem_id,err=0;
  int flag;
    
  unsigned long long int temp,bufferLen = adc.bufferLen,i=0,diff;
   
  pthread_mutex_lock(&fft1Stop_mutex);
  flag = fft1Stop;
  pthread_mutex_unlock(&fft1Stop_mutex);
   
  while(flag ==1)
  {
    err =semaphore_p(sem_fft1);
    if(err !=1)
    {
        printf("semaphore-p sem_fft1 error start_file \n");
    }        
    
    if(fft1Status[0]==1)
    {
      diff = (fft1Status[1]-fft1Status[2]);
      if(diff%nfft!=0)
      {
        printf("error occured in the copy_thread");
        exit(0);
      }
           
      for(i=0;i<(diff/(2*nfft));i++)
      {    
        temp = fft1Status[2]%bufferLen;
        for (j=0; j<nfft; j++)
	      {
	        in1[j][0]=(float) ((int)*(data1+temp+(2*j)+1));
	        in1[j][1]=0.0;
	        fft1Status[2]+=2;
	      }
	      fftw_execute(p1);
        pthread_mutex_lock(&fft1_mutex);
        check = fft1Val;
        pthread_mutex_unlock(&fft1_mutex);
        while(check!=0)
        {
          pthread_mutex_lock(&fft1_mutex);
          check = fft1Val;
          pthread_mutex_unlock(&fft1_mutex);
               
          pthread_mutex_lock(&fft1Stop_mutex);
          flag = fft1Stop;
          pthread_mutex_unlock(&fft1Stop_mutex);
               
          if(flag!=1) 
          {
            printf("\n stop signal reached the fft1 thread \n");
            err= semaphore_v(sem_fft1);
            return;
          }
        }
        memcpy(fft1Buffer,out1,sizeof(fftw_complex)*nfft/2);
	             
        pthread_mutex_lock(&fft1_mutex);
        fft1Val=1;
        pthread_mutex_unlock(&fft1_mutex);
             
             
      }
        
      fft1Status[0]=0;
    }
    err= semaphore_v(sem_fft1);
    if(err !=1)
    {
        printf("semaphore-v sem_fft1 error start_file \n");
    }
      
    pthread_mutex_lock(&fft1Stop_mutex);
    flag = fft1Stop;
    pthread_mutex_unlock(&fft1Stop_mutex);
  }
     
  return;
     
}

 
void *correlation(void *pa)
{
  int checkFft[4],check; 
    
  long long int checkCount0, checkCount1;
  int flag;     
  int err=0 , j=0;
   
  pthread_mutex_lock(&corrStop_mutex);
  flag = corrStop;
  pthread_mutex_unlock(&corrStop_mutex);
     
  pthread_mutex_lock(&corr_mutex);
  corrVal=0;
  pthread_mutex_unlock(&corr_mutex);
   
  pthread_mutex_lock(&fft0_mutex);
  fft0Val=0;
  pthread_mutex_unlock(&fft0_mutex);
    
  pthread_mutex_lock(&fft1_mutex);
  fft1Val=0;
  pthread_mutex_unlock(&fft1_mutex);
     
  while(flag==1)
  {
     
    pthread_mutex_lock(&fft0_mutex);
    checkFft[0] = fft0Val;
    pthread_mutex_unlock(&fft0_mutex);
    
    pthread_mutex_lock(&fft1_mutex);
    checkFft[1] = fft1Val;
    pthread_mutex_unlock(&fft1_mutex);
      
    pthread_mutex_lock(&corr_mutex);
    check = corrVal;
    pthread_mutex_unlock(&corr_mutex);
     
      
    if(checkFft[0]==1 && checkFft[1]==1 && check == 0)
    {
      for(j=0;j<nfft/2;j++)
	    {
	         
	      bufferReal[j] = (long double) (((fft0Buffer[j][0] * fft1Buffer[j][0]) + (fft0Buffer[j][1] * fft1Buffer[j][1]))/factor0);
	      bufferImg[j] = (long double) (((fft0Buffer[j][0] * fft1Buffer[j][1]) - (fft1Buffer[j][0] * fft0Buffer[j][1]))/factor0);
	         
	    }
	   
	     
      pthread_mutex_lock(&fft0_mutex);
      fft0Val=0;
      pthread_mutex_unlock(&fft0_mutex);
    
      pthread_mutex_lock(&fft1_mutex);
      fft1Val=0;
      pthread_mutex_unlock(&fft1_mutex);     
         
      pthread_mutex_lock(&corr_mutex);
      corrVal=1;
      pthread_mutex_unlock(&corr_mutex);  
          
    }
     
    pthread_mutex_lock(&corrStop_mutex);
    flag = corrStop;
    pthread_mutex_unlock(&corrStop_mutex);   
       
  }
  return;
}


void *corrSpectraCal(void *pa)
{
  double spectra[nfft/2],cumspectra[nfft/2];
  unsigned short j,i;
  int b,c,checkCorr[2],check,nacc=adc.nacc;
  c=0;
  b=0;
  unsigned long int isect=0; 
  long double  real_part, img_part;
  int err=0,f=0;
    
  int flag;
   
   
       
  pthread_mutex_lock(&spec_mutex);
  specVal=0;
  pthread_mutex_unlock(&spec_mutex);
     
  pthread_mutex_lock(&corrCalStop_mutex);
  flag = corrCalStop;
  pthread_mutex_unlock(&corrCalStop_mutex);
    
  for (i=0; i<nfft/2; i++)
  {
    spectra[i] = 0.0;
  }
   
  while(flag==1)
  {
    pthread_mutex_lock(&corr_mutex);
    checkCorr[0] = corrVal;
    pthread_mutex_unlock(&corr_mutex);
       
    pthread_mutex_lock(&spec_mutex);
    check = specVal;
    pthread_mutex_unlock(&spec_mutex);
     
    if(checkCorr[0] == 1 && check ==0)
    {
      isect++;
      for(j=0;j<nfft/2;j++)
	    {
	      real_part = (long double) (bufferReal[j] * bufferReal[j]);
	      img_part  =  (long double) (bufferImg[j] * bufferImg[j]);
	      spectra[j] += (double) (( real_part +  img_part)/(long double)(nfft));
	    }
	      
	    pthread_mutex_lock(&corr_mutex);
      corrVal= 0;
      pthread_mutex_unlock(&corr_mutex);
	      
	    if( ( isect % nacc ) == 0 )
	    {
	      for(j=0;j<(nfft/2);j++)
	      spectra[j] /= ((double) (nacc));
                 
        for (c=0;c<nfft/2;c++) 
        {
          if((c<(nfft/2)*0.01) || (c> (nfft/2)*0.99))
          {
            ur[c]=0.0;
          }
          else
          {
            if(adc.bandFlag==1)
            ur[c]=((unsigned short)spectra[(nfft/2)-c-1]);
            else 
            ur[c]=((unsigned short)spectra[c]);
          }
           
        }
                 
        pthread_mutex_lock(&spec_mutex);
        specVal=1;
        pthread_mutex_unlock(&spec_mutex);
                
        for(j=0;j<(nfft/2);j++)
	        spectra[j] = 0.0;
      }
    }
    pthread_mutex_lock(&corrCalStop_mutex);
    flag = corrCalStop;
    pthread_mutex_unlock(&corrCalStop_mutex);
  }
  return;
}

