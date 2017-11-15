#include "filterbank.h"
#include "global.h"
#include "startacq.h"
#include "outfile.h"
     
void *copy_file(void *pa)
{
  int j=0,ii=0;
  int check;
  int flag; 
  double ts;
  int shmid,sem_id,err=0;
  unsigned long long int temp,bufferLen = adc.buffer*1024*1024,i=0,diff;
    
  pthread_mutex_lock(&copyStop_mutex);
  flag = copyStop;
  pthread_mutex_unlock(&copyStop_mutex); 
    
  bufferVal = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft/2);
  in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);
  out= (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * nfft);
  p  = fftw_plan_dft_1d(nfft, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
  printf("\n loop started......... \n ");
  while(flag ==1)
  {
    semaphore_p(sem_fft0);
    if(fft0Status[0]==1)
    {
      diff = (fft0Status[1]-fft0Status[2]);
      if(diff%nfft!=0)
      {
        printf("error occured in the copy_thread");
        exit(0);
      }
            
      for(i=0;i<(diff/nfft);i++)
      {    
        temp = fft0Status[2]%bufferLen;
        for (j=0; j<nfft; j++)
	      {
	        in[j][0]=(float) ((int)(char)*(data+temp+j));
	        in[j][1]=0.0;
	        fft0Status[2]+=1;
	      }
	      fftw_execute(p);
        pthread_mutex_lock(&copy_mutex);
        check = copyVal;
        pthread_mutex_unlock(&copy_mutex);
        while(check!=0)
        {
          pthread_mutex_lock(&copy_mutex);
          check = copyVal;
          pthread_mutex_unlock(&copy_mutex);
                  
          pthread_mutex_lock(&copyStop_mutex);
          flag = copyStop;
          pthread_mutex_unlock(&copyStop_mutex);
                  
          if(flag!=1) return;
        }                 
	             
	      memcpy(bufferVal,out,sizeof(fftw_complex)*nfft/2);
	             
        pthread_mutex_lock(&copy_mutex);
        copyVal=1;
        pthread_mutex_unlock(&copy_mutex);
      }
        
      fft0Status[0]=0;
    }
    semaphore_v(sem_fft0);  
  }
  return;
}


void *spectraCal(void *pa)
{
  double real_part, img_part, spectra[nfft/2],cumspectra[nfft/2];
  unsigned short j,i;
  int b,c,check1,check2,nacc=adc.nacc;
  c=0;
  b=0;
  unsigned long int isect=0; 
  int flag;   
  int err=0;
    
  ur = (unsigned short*)malloc(sizeof(unsigned short)*nfft/2);
   
  pthread_mutex_lock(&specStop_mutex);
  flag = specStop;
  pthread_mutex_unlock(&specStop_mutex);
   
  pthread_mutex_lock(&spec_mutex);
  specVal=0;
  pthread_mutex_unlock(&spec_mutex);
   
  for (i=0; i<nfft/2; i++)
  {
    spectra[i] = 0.0;
  }
  while(flag==1)
  {
     
    pthread_mutex_lock(&copy_mutex);
    check1 = copyVal;
    pthread_mutex_unlock(&copy_mutex);
     
    pthread_mutex_lock(&spec_mutex);
    check2 = specVal;
    pthread_mutex_unlock(&spec_mutex);
  
    pthread_mutex_lock(&specStop_mutex);
    flag = specStop;
    pthread_mutex_unlock(&specStop_mutex);
     
    if(check1==1 && check2==0)
    {
      isect++;
      for(j=0;j<nfft/2;j++)
	    {
	      real_part = (double) (bufferVal[j][0] * bufferVal[j][0]);
	      img_part  =  (double) (bufferVal[j][1] * bufferVal[j][1]);
	      spectra[j] += (((double) ( real_part +  img_part))
		                  /((double) (nfft/2)));
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
      pthread_mutex_lock(&copy_mutex);
      copyVal=0;
      pthread_mutex_unlock(&copy_mutex);
    }
       
  }
   return;
}

void *writeooty2fb(void *pa) 
{
  double ts;
  long double mjdvalue;
  int check,fwrite_check,i;
  char filename[500];
  char mjd[30];
  unsigned long long int totalwritten;
  int size=0; 
  int isamp=0;
  int dcheck=0;   
  int started = 0;
  unsigned short *tempbuff;
  int write,dedisp;  
  int flag;
  int checkBand;
  int fold;
  if(adc.fold == 1) band = (unsigned short*) malloc(adc.nfft/2*sizeof(unsigned short));
    
  pthread_mutex_lock(&fwriteStop_mutex);
  flag = fwriteStop;
  pthread_mutex_unlock(&fwriteStop_mutex); 
    
  FILE *fp,*fp1;
  int err=0;
  int sflag;  
  ts = timeSample;
  pthread_mutex_lock(&header_mutex);  
  mjdvalue = mjdnow();
  tstart = mjdvalue;
     
  obits=-1;
  machine_id=3;
  telescope_id=2;
  data_type=1;
  fch1=adc.fch1;
  foff=-1*(double)adc.samplingRate/(double)nfft;
  nchans=nfft/2;
  nbits=16;
  tsamp=ts*adc.nacc;
  nifs=1;   
  nbeams=1;
  ibeam=0;
  sumifs=1;
  start_time= 0.0;
  fold = adc.fold;
  write = adc.filterbank;
  dedisp = adc.dedispersed;
    
  if(write==1)
  { 
    fp = fopen(ffilename, "wb");
    if (fp == NULL) 
    {
      printf("I couldn't open stream.dat file.\n");
      exit(0);
    }
    chown(ffilename,1000,1000);
  }
      
   
  if(write==1) filterbank_header(fp); 
  headerVal=1;
  pthread_mutex_unlock(&header_mutex);
  if(dedisp==1 || fold == 1)
  {
    pthread_mutex_lock(&nsblk_mutex);
    check = nsblkVal;
    pthread_mutex_unlock(&nsblk_mutex);
      
    while(check != 1)
    {
      pthread_mutex_lock(&nsblk_mutex);
      check = nsblkVal;
      pthread_mutex_unlock(&nsblk_mutex);
         
      pthread_mutex_lock(&fwriteStop_mutex);
      flag = fwriteStop;
      pthread_mutex_unlock(&fwriteStop_mutex);
         
      if(flag!=1) return;
    }
    tempbuff=(unsigned short *) malloc(2*nsblk1*sizeof(short));
  }
  check=0;  
  sflag=0; 
     
  while(flag==1)
  {
    
    pthread_mutex_lock(&spec_mutex);
    check = specVal;
    pthread_mutex_unlock(&spec_mutex);
     
    pthread_mutex_lock(&fwriteStop_mutex);
    flag = fwriteStop;
    pthread_mutex_unlock(&fwriteStop_mutex);
    
    if(started==1) sflag=1;
      
    if(check==1)
    {
      if(write==1) fwrite_check=fwrite(ur,sizeof(unsigned short),nfft/2,fp);
      if(adc.fold == 1 && adc.correlation == 0)
      {
        pthread_mutex_lock(&band_mutex);
        checkBand = bandVal;
        pthread_mutex_unlock(&band_mutex);
        while(checkBand !=0)
        {
          pthread_mutex_lock(&band_mutex);
          checkBand = bandVal;
          pthread_mutex_unlock(&band_mutex);
           
          pthread_mutex_lock(&fwriteStop_mutex);
          flag = fwriteStop;
          pthread_mutex_unlock(&fwriteStop_mutex);
         
          if(flag!=1)
          {
            free(band);
            return;
          }
        } 
          memcpy(band,ur,sizeof(unsigned short)*nfft/2);
          pthread_mutex_lock(&band_mutex);
          bandVal= 1;
          pthread_mutex_unlock(&band_mutex);
      }
      if(dedisp==1 || fold == 1)
      {
        if(size<nsblk1*2 && started ==0)
        {
          memcpy(&tempbuff[size],ur,(nfft/2)*sizeof(short));
          size += nfft/2;
        }
            
        if(size==nsblk1*2 && started == 0)
        {
          pthread_mutex_lock(&dedisp_mutex);
          dcheck = dedispVal;
          pthread_mutex_unlock(&dedisp_mutex);
                  
          while(dcheck!=0)
          {
            pthread_mutex_lock(&dedisp_mutex);
            dcheck = dedispVal;
            pthread_mutex_unlock(&dedisp_mutex);
            pthread_mutex_lock(&fwriteStop_mutex);
            flag = fwriteStop;
            pthread_mutex_unlock(&fwriteStop_mutex);
         
            if(flag!=1) return;
          }
          memcpy(buff,tempbuff,2*(nsblk1)*sizeof(short));
          started=1;
          pthread_mutex_lock(&dedisp_mutex);
          dedispVal=1;
          pthread_mutex_unlock(&dedisp_mutex);
               
        }
            
        if(sflag == 1)
        {
          pthread_mutex_lock(&dedisp_mutex);
          dcheck = dedispVal;
          pthread_mutex_unlock(&dedisp_mutex);
                  
          while(dcheck!=0)
          {
            pthread_mutex_lock(&dedisp_mutex);
            dcheck = dedispVal;
            pthread_mutex_unlock(&dedisp_mutex);
            pthread_mutex_lock(&fwriteStop_mutex);
            flag = fwriteStop;
            pthread_mutex_unlock(&fwriteStop_mutex);
         
            if(flag!=1) return;
          }
               
          memcpy(&buff[isamp*(nfft/2)],ur,(nfft/2)*sizeof(short));
          pthread_mutex_lock(&dedisp_mutex);
          dedispVal=1;
          pthread_mutex_unlock(&dedisp_mutex);
          isamp++;
          if(isamp*nfft/2 == 2*nsblk1) isamp=0;
        }
               
      }
      pthread_mutex_lock(&spec_mutex);
      specVal=0;
      pthread_mutex_unlock(&spec_mutex);
    }
  }
  return;
}

/*
void *writeIps(void *pa) 
{
  int check,fwrite_check,i;
  int flag; 
  FILE *fp;
  long double tempVal = 0.0;
  float value = 0.0; 
   
  fp = fopen(ifilename,"w");
  if(fp == NULL)
  {
    printf("cannot open the file %s " ,ifilename);
    exit(0);
  }
  chown(ifilename,1000,1000);
  pthread_mutex_lock(&ipsStop_mutex);
  flag = ipsStop;
  pthread_mutex_unlock(&ipsStop_mutex);
         
  while(flag==1)
  {
    pthread_mutex_lock(&spec_mutex);
    check = specVal;
    pthread_mutex_unlock(&spec_mutex);
     
    pthread_mutex_lock(&fwriteStop_mutex);
    flag = fwriteStop;
    pthread_mutex_unlock(&fwriteStop_mutex);
         
    if(flag!=1) return;       
      
    if(check==1)
    {
      for(i=0;i<nfft/2;i++)
      {
        tempVal+= ur[i];
      }
      value = (float) (tempVal/(nfft/2));
         
      tempVal=0.0;
      fprintf(fp,"%f \n",value);
          
      value = 0.0;
      pthread_mutex_lock(&spec_mutex);
      specVal=0;
      pthread_mutex_unlock(&spec_mutex);
    }
  }
  return;
}
*/ 
