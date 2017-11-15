#include "global.h"
#include <stdio.h>
#include <pthread.h>

void initThreads()
{

  int err;
  
  err = pthread_create(&check_thread, NULL, check, NULL);
  if (err != 0) 
  {
    perror("Thread creation failed");
    exit(EXIT_FAILURE);
  }
  printf("acquisistion threard created");  
  
   
  if(adc.filterbank==1 || adc.dedispersed ==1 || adc.fold==1)
  {
    if(adc.correlation == 0 && adc.mixing == 0)
    {
      err = pthread_create(&copy_thread, NULL, copy_file, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
        
      err = pthread_create(&spectra_thread, NULL, spectraCal, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
           
      err = pthread_create(&fwrite_thread, NULL, writeooty2fb, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
    }        
    if(adc.fold == 1)
    { 
      err = pthread_create(&fold_thread, NULL, plotFold, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
         
      if(adc.correlation == 0)
      {
        err = pthread_create(&band_thread, NULL, bandCal, NULL);
        if (err != 0) 
        {
          perror("Thread creation failed");
          exit(EXIT_FAILURE);
        }
         
             
      }
            
    }
         
    if(adc.correlation == 1)
    {
      initCorr();
      err = pthread_create(&fft0_thread, NULL, fft0Cal, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
        
      err = pthread_create(&fft1_thread, NULL, fft1Cal, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
            
      err = pthread_create(&corr_thread, NULL, correlation, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
            
      err = pthread_create(&corrSpec_thread, NULL, corrSpectraCal, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
            
      err = pthread_create(&fwrite_thread, NULL, writeooty2fb, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
            
    }
         
    if(adc.mixing == 1)
    {
      initMixing();
      err = pthread_create(&mixing0_thread, NULL, mixing0, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
      printf("mixing0 thread created \n");
      err = pthread_create(&mixing1_thread, NULL, mixing1, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
      printf("mixing1 thread created\n");
      err = pthread_create(&mixCal_thread, NULL, mixCal, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
      printf("mixingCal thread created\n");
      err = pthread_create(&fwrite_thread, NULL, writeooty2fb, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
      printf("fwrite thread created\n");
    }
         
         
    if(adc.dedispersed == 1 || adc.fold == 1)
    {
      err = pthread_create(&dedisperse_thread, NULL, dedisperse_data, NULL);
      if (err != 0) 
      {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
      }
    }
         
        
    if(adc.correlation == 0 && adc.mixing == 0 && adc.ips==0)
    {
      err = pthread_join(copy_thread, NULL);
      if (err != 0) 
      {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
      }
      printf("copy_thread joined\n");
            
      err = pthread_join(spectra_thread, NULL);
      if (err != 0) 
      {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
      }  
      printf("spectra_thread joined\n");
    }
  }
  if(adc.ips == 1)
  {
    initCorr();
    err = pthread_create(&fft0_thread, NULL, fft0Cal, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("fft0_thread \n");
        
    err = pthread_create(&fft1_thread, NULL, fft1Cal, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("fft1_thread \n");
            
    err = pthread_create(&corr_thread, NULL, correlation, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("corr_thread \n");
             
    err = pthread_create(&corrSpec_thread, NULL, corrSpectraCal, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("corrSpec_thread \n");
             
    err = pthread_create(&ips_thread, NULL, writeIps, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    printf("ips_thread \n");        
  }
   
  if(adc.vlbi==1)
  {
    err = pthread_create(&vlbi_thread, NULL,vlbiData, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
  }
  
  if(adc.gpu==1)
  {
    err = pthread_create(&gpu_thread, NULL,gpudedisp, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
    
    //err = pthread_create(&fold_thread, NULL, plotFold, NULL);
    if (err != 0) 
    {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
  }
   
}
   
