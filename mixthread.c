#include "filterbank.h"
#include "global.h"
#include "startacq.h"
#include "outfile.h"


void initMixing()
{
  buffer0Val = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft/2);
  buffer1Val = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft/2); 
  in0 = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);
  out0= (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);
  in1 = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);
  out1= (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);
  p0  = fftw_plan_dft_1d(nfft, in0, out0, FFTW_FORWARD, FFTW_ESTIMATE);
  p1  = fftw_plan_dft_1d(nfft, in1, out1, FFTW_FORWARD, FFTW_ESTIMATE);
  ur = (unsigned short*)malloc(sizeof(unsigned short)*nfft/2);
}

void freeMixing()
{
  free(buffer0Val); 
  free(buffer1Val);
  free(in0); 
  free(out0); 
  free(in1); 
  free(out1);
  free(ur);
  fftw_destroy_plan(p0); 
  fftw_destroy_plan(p1); 
}


void *mixing0(void *pa)
{
  int j=0,ii=0;
  int check;
  int flag; 
  double ts;
  int shmid,sem_id,err=0;
  unsigned long long int temp,bufferLen = adc.buffer*1024*1024,i=0,diff;
     
  pthread_mutex_lock(&mix0Stop_mutex);
  flag = mix0Stop;
  pthread_mutex_unlock(&mix0Stop_mutex);
    
  while(flag==1)
  {
    semaphore_p(sem_fft0);
    if(fft0Status[0]==1)
    {
      diff = (fft0Status[1]-fft0Status[2]);
      if(diff%nfft!=0)
      {
        printf("error occured in the copy_thread fft0");
        exit(0);
      }
            
      for(i=0;i<(diff/(nfft*4));i++)
      {    
        temp = fft0Status[2]%bufferLen;
                             
        for (j=0; j<nfft; j++)
	      {
	        in0[j][0]=((float) ((int)(char)*(data0+temp+(2*j))) + (float) ((int)(char)*(data0+temp+(2*j))))/2;
	        in0[j][1]=0.0;
	        fft0Status[2]+=2;
	      }
	      fft0Status[2]+=2*nfft;
	      fftw_execute(p0);
        pthread_mutex_lock(&mix0_mutex);
        check = mix0Val;
        pthread_mutex_unlock(&mix0_mutex);
        while(check!=0)
        {
          pthread_mutex_lock(&mix0_mutex);
          check = mix0Val;
          pthread_mutex_unlock(&mix0_mutex);
               
          pthread_mutex_lock(&mix0Stop_mutex);
          flag = mix0Stop;
          pthread_mutex_unlock(&mix0Stop_mutex);
               
          if(flag!=1)
          {
            semaphore_v(sem_fft0);
            return;
          }
        }
        memcpy(buffer0Val,out0,sizeof(fftw_complex)*nfft/2);
	             
        pthread_mutex_lock(&mix0_mutex);
        mix0Val=1;
        pthread_mutex_unlock(&mix0_mutex);
             
         
      }
        
      fft0Status[0]=0;
    }
    semaphore_v(sem_fft0);
        
  }
     
  return;
}


void *mixing1(void *pa)
{
  int j=0,ii=0;
  int check;
  double ts;
  unsigned long long int temp,bufferLen = adc.buffer*1024*1024,i=0,diff;
  int flag;
    
  pthread_mutex_lock(&mix1Stop_mutex);
  flag = mix1Stop;
  pthread_mutex_unlock(&mix1Stop_mutex); 
    
  while(flag==1)
  {
    semaphore_p(sem_fft1);
    if(fft1Status[0]==1)
    {
      diff = (fft1Status[1]-fft1Status[2]);
      if(diff%nfft!=0)
      {
        printf("error occured in the copy_thread fft1");
        exit(0);
      }
            
      for(i=0;i<(diff/(nfft*4));i++)
      {    
        temp = fft1Status[2]%bufferLen;
        for (j=0; j<nfft; j++)
	      {
	        in1[j][0]=((float) ((int)(char)*(data1+temp+(2*j)+(nfft*2))) + (float) ((int)(char)*(data1+temp+(2*j)+(nfft*2))))/2;
	        in1[j][1]=0.0;
	        fft1Status[2]+=2;
	      }
	      fft1Status[2]+=2*nfft; 
	      fftw_execute(p1);
        pthread_mutex_lock(&mix1_mutex);
        check = mix1Val;
        pthread_mutex_unlock(&mix1_mutex);
        while(check!=0)
        {
          pthread_mutex_lock(&mix1_mutex);
          check = mix1Val;
          pthread_mutex_unlock(&mix1_mutex);
               
          pthread_mutex_lock(&mix1Stop_mutex);
          flag = mix1Stop;
          pthread_mutex_unlock(&mix1Stop_mutex);
               
          if(flag!=1)
          {
            semaphore_v(sem_fft1);
            return;
          }
        }
        memcpy(buffer1Val,out1,sizeof(fftw_complex)*nfft/2);
	             
        pthread_mutex_lock(&mix1_mutex);
        mix1Val=1;
        pthread_mutex_unlock(&mix1_mutex);
             
      }
      fft1Status[0]=0;
         
         
    }
    semaphore_v(sem_fft1);
    pthread_mutex_lock(&mix1Stop_mutex);
    flag = mix1Stop;
    pthread_mutex_unlock(&mix1Stop_mutex);    
  }           
  return;
}


void *mixCal(void *pa)
{
  double real_part, img_part, spectra[nfft/2],cumspectra[nfft/2];
  unsigned short j,i;
  int b,c,checkMix[2],check,nacc=adc.nacc;
  c=0;
  b=0;
  unsigned long int isect=0; 
  int f=0;
  int flag;   
  int err=0;
    
  printf("loop started ............... mixcal\n"); 
  pthread_mutex_lock(&spec_mutex);
  specVal=0;
  pthread_mutex_unlock(&spec_mutex);
   
  pthread_mutex_lock(&mixCalStop_mutex);
  flag = mixCalStop;
  pthread_mutex_unlock(&mixCalStop_mutex);
     
    
  for (i=0; i<nfft/2; i++)
  {
    spectra[i] = 0.0;
  }
  while(flag==1)
  {
     
    pthread_mutex_lock(&mix0_mutex);
    checkMix[0] = mix0Val;
    pthread_mutex_unlock(&mix0_mutex);
    
    pthread_mutex_lock(&mix1_mutex);
    checkMix[1] = mix1Val;
    pthread_mutex_unlock(&mix1_mutex);
     
    pthread_mutex_lock(&spec_mutex);
    check = specVal;
    pthread_mutex_unlock(&spec_mutex);
     
    pthread_mutex_lock(&mixCalStop_mutex);
    flag = mixCalStop;
    pthread_mutex_unlock(&mixCalStop_mutex);
       
      
    if(checkMix[0]==1 && checkMix[1]==1 && check==0)
    {
      isect++;
      if(f==0)
      {
        for(j=0;j<nfft/2;j++)
	      {
	        real_part = (double) (buffer0Val[j][0] * buffer0Val[j][0]);
	        img_part  =  (double) (buffer0Val[j][1] * buffer0Val[j][1]);
	        spectra[j] += (((double) ( real_part +  img_part))
		                        /((double) (nfft/2)));
	      }
        pthread_mutex_lock(&mix0_mutex);
        mix0Val=0;
        pthread_mutex_unlock(&mix0_mutex);
        f=1;
      }
      else 
      {
        for(j=0;j<nfft/2;j++)
	      {
	        real_part = (double) (buffer1Val[j][0] * buffer1Val[j][0]);
	        img_part  =  (double) (buffer1Val[j][1] * buffer1Val[j][1]);
	         
	        spectra[j] += (((double) ( real_part +  img_part))
		                         /((double) (nfft/2)));
	         
        }
        pthread_mutex_lock(&mix1_mutex);
        mix1Val=0;
        pthread_mutex_unlock(&mix1_mutex);
         
        f=0;
      }
     
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
    
  }
  return;
}

