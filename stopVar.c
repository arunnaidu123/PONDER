#include<stdio.h>
#include<stdlib.h>
#include "global.h"

int initStop()
{
   printf("stared initializing");
   fflush(stdout);
   
   pthread_mutex_lock(&acqStop_mutex);
   acqStop = 1;
   pthread_mutex_unlock(&acqStop_mutex);
   
   pthread_mutex_lock(&copyStop_mutex);
   copyStop = 1;
   pthread_mutex_unlock(&copyStop_mutex);
   
   pthread_mutex_lock(&specStop_mutex);
   specStop = 1;
   pthread_mutex_unlock(&specStop_mutex);
   
   pthread_mutex_lock(&fwriteStop_mutex);
   fwriteStop = 1;
   pthread_mutex_unlock(&fwriteStop_mutex);
   
   pthread_mutex_lock(&ipsStop_mutex);
   ipsStop = 1;
   pthread_mutex_unlock(&ipsStop_mutex);
   
   pthread_mutex_lock(&mix0Stop_mutex);
   mix0Stop = 1;
   pthread_mutex_unlock(&mix0Stop_mutex);
   
   pthread_mutex_lock(&mix1Stop_mutex);
   mix1Stop = 1;
   pthread_mutex_unlock(&mix1Stop_mutex);
   
   pthread_mutex_lock(&mixCalStop_mutex);
   mixCalStop = 1;
   pthread_mutex_unlock(&mixCalStop_mutex);
   
   pthread_mutex_lock(&fft0Stop_mutex);
   fft0Stop = 1;
   pthread_mutex_unlock(&fft0Stop_mutex);
   
   pthread_mutex_lock(&fft1Stop_mutex);
   fft1Stop = 1;
   pthread_mutex_unlock(&fft1Stop_mutex);
   
   pthread_mutex_lock(&corrStop_mutex);
   corrStop = 1;
   pthread_mutex_unlock(&corrStop_mutex);
   
   pthread_mutex_lock(&corrCalStop_mutex);
   corrCalStop = 1;
   pthread_mutex_unlock(&corrCalStop_mutex);
   
   pthread_mutex_lock(&dedispStop_mutex);
   dedispStop = 1;
   pthread_mutex_unlock(&dedispStop_mutex);
   
   pthread_mutex_lock(&bandStop_mutex);
   bandStop = 1;
   pthread_mutex_unlock(&bandStop_mutex);
   
   pthread_mutex_lock(&foldStop_mutex);
   foldStop = 1;
   pthread_mutex_unlock(&foldStop_mutex);
   
   pthread_mutex_lock(&vlbiStop_mutex);
   vlbiStop = 1;
   pthread_mutex_unlock(&vlbiStop_mutex);
   
   pthread_mutex_lock(&gpuStop_mutex);
   gpuStop = 1;
   pthread_mutex_unlock(&gpuStop_mutex);
}

int stop()
{
   pthread_mutex_lock(&copyStop_mutex);
   copyStop = 0;
   pthread_mutex_unlock(&copyStop_mutex);
   
   pthread_mutex_lock(&specStop_mutex);
   specStop = 0;
   pthread_mutex_unlock(&specStop_mutex);
   
   pthread_mutex_lock(&fwriteStop_mutex);
   fwriteStop = 0;
   pthread_mutex_unlock(&fwriteStop_mutex);
   
   pthread_mutex_lock(&ipsStop_mutex);
   ipsStop = 0;
   pthread_mutex_unlock(&ipsStop_mutex);
   
   pthread_mutex_lock(&mix0Stop_mutex);
   mix0Stop = 0;
   pthread_mutex_unlock(&mix0Stop_mutex);
   
   pthread_mutex_lock(&mix1Stop_mutex);
   mix1Stop = 0;
   pthread_mutex_unlock(&mix1Stop_mutex);
   
   pthread_mutex_lock(&mixCalStop_mutex);
   mixCalStop = 0;
   pthread_mutex_unlock(&mixCalStop_mutex);
   
   pthread_mutex_lock(&fft0Stop_mutex);
   fft0Stop = 0;
   pthread_mutex_unlock(&fft0Stop_mutex);
   
   pthread_mutex_lock(&fft1Stop_mutex);
   fft1Stop = 0;
   pthread_mutex_unlock(&fft1Stop_mutex);
   
   pthread_mutex_lock(&corrStop_mutex);
   corrStop = 0;
   pthread_mutex_unlock(&corrStop_mutex);
   
   pthread_mutex_lock(&corrCalStop_mutex);
   corrCalStop = 0;
   pthread_mutex_unlock(&corrCalStop_mutex);
   
   pthread_mutex_lock(&dedispStop_mutex);
   dedispStop = 0;
   pthread_mutex_unlock(&dedispStop_mutex);
   
   pthread_mutex_lock(&bandStop_mutex);
   bandStop = 0;
   pthread_mutex_unlock(&bandStop_mutex);
   
   pthread_mutex_lock(&foldStop_mutex);
   foldStop = 0;
   pthread_mutex_unlock(&foldStop_mutex);
   
   pthread_mutex_lock(&vlbiStop_mutex);
   vlbiStop = 0;
   pthread_mutex_unlock(&vlbiStop_mutex);
   
   pthread_mutex_lock(&gpuStop_mutex);
   gpuStop = 0;
   pthread_mutex_unlock(&gpuStop_mutex);
}
