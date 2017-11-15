#include "global.h"
#include <stdio.h>
#include <pthread.h>

void joinThreads()
{
  int err;
  
  err = pthread_join(check_thread, NULL);
  if (err != 0) 
  {
    perror("Thread join failed");
    exit(EXIT_FAILURE);
  }
  printf("acq_thread joined\n");
  
      
  if(adc.ips == 1 || adc.correlation == 1)
  {
    
    err = pthread_join(fft0_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("fft0 thread joined\n");
         
    err = pthread_join(fft1_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("fft1 thread joined\n");   
                   
    err = pthread_join(corr_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("corr thread joined\n");   
                     
    err = pthread_join(corrSpec_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("corrSpec thread joined\n");
    freeCorr();        
  }
  if(adc.mixing == 1)
  {
    
    err = pthread_join(mixing0_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("mixing0 thread joined \n");
    err = pthread_join(mixing1_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("mixing1 thread joined\n");
    err = pthread_join(mixCal_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("mixingCal thread joined\n");
    freeMixing();         
  }
   
  if(adc.ips == 1)
  {
    err = pthread_join(ips_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("ips thread joined\n");
  }
         
  if(adc.correlation == 1 || adc.dedispersed ==1 || adc.fold == 1)
  {
    err = pthread_join(fwrite_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("fwrite thread joined\n");
  }
         
  if(adc.dedispersed == 1 || adc.fold == 1)
  {
    err = pthread_join(dedisperse_thread, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("dedispersed thread joined\n");
      
    if(adc.fold==1)
    {
      err = pthread_join(fold_thread, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
      printf("fold thread joined\n");
    }
         
    if(adc.correlation == 0 && adc.fold == 1)
    {
      
      err = pthread_join(band_thread, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
         
          
    }
  }
  
  if(adc.gpu==1)
  {
    err = pthread_join(gpu_thread, NULL);
    if (err != 0) 
    {
      perror("Thread join failed");
      exit(EXIT_FAILURE);
    }
    printf("gpu_thread joined\n");
    
    //err = pthread_join(fold_thread, NULL);
    if (err != 0) 
    {
      perror("Thread join failed");
      exit(EXIT_FAILURE);
    }
  }
  if(adc.vlbi==1)
  {
    err = pthread_join(vlbi_thread,NULL);
    if(err != 0)
    {
      perror("Thread join failed");
      exit(EXIT_FAILURE);
    }
    printf("vlbi_htread joined\n");
  }
}
