#include "global.h"
#include<stdlib.h>
#include<pthread.h>

void initMutex()
{
   int err;
   
   err = pthread_mutex_init(&acqStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   err = pthread_mutex_init(&copyStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   err = pthread_mutex_init(&specStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   err = pthread_mutex_init(&fwriteStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   err = pthread_mutex_init(&ipsStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   err = pthread_mutex_init(&mix0Stop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   err = pthread_mutex_init(&mix1Stop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   err = pthread_mutex_init(&mixCalStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   err = pthread_mutex_init(&fft0Stop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   
   err = pthread_mutex_init(&fft1Stop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   
   err = pthread_mutex_init(&corrStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   
   err = pthread_mutex_init(&corrCalStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   
   err = pthread_mutex_init(&dedispStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   
   err = pthread_mutex_init(&bandStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   
   err = pthread_mutex_init(&foldStop_mutex , NULL);
   if(err !=0)
   {
      perror("write_mutex initialization failed");
      exit(1);
   }
   
   if(adc.filterbank==1 || adc.dedispersed ==1 || adc.fold == 1 )
   {
      err = pthread_mutex_init(&copy_mutex , NULL);
      if(err !=0)
      {
         perror("buffer_mutex initialization failed");
         exit(1);
      }
      
      err = pthread_mutex_init(&spec_mutex , NULL);
      if(err !=0)
      {
         perror("spec_mutex initialization failed");
         exit(1);
      }
        
      err = pthread_mutex_init(&fft0Status_mutex , NULL);
      if(err !=0)
      {
         perror("status_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&fft1Status_mutex , NULL);
      if(err !=0)
      {
         perror("status_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&fft2Status_mutex , NULL);
      if(err !=0)
      {
         perror("status_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&fft3Status_mutex , NULL);
      if(err !=0)
      {
         perror("status_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&fft0_mutex , NULL);
      if(err !=0)
      {
         perror("status_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&fft1_mutex , NULL);
      if(err !=0)
      {
         perror("status_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&fft2_mutex , NULL);
      if(err !=0)
      {
         perror("status_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&fft3_mutex , NULL);
      if(err !=0)
      {
         perror("status_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&flag_mutex , NULL);
      if(err !=0)
      {
         perror("flag_mutex initialization failed");
         exit(1);
      }
         
      err = pthread_mutex_init(&corr_mutex , NULL);
      if(err !=0)
      {
         perror("flag_mutex initialization failed");
         exit(1);
      }
   }
}
