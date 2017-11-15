#include<stdio.h>
#include<stdlib.h>
#include"global.h"
//void normArray(int nchans,int samp);
void *bandCal(void *pa)
{
   int i,check,flag;
   long long int *temp;
   
   float *tempNorm;
   int samp;
   int nacc;
   int bacc;
   int bandTVal;
   int nchans;
    
   samp=0;
   
   nchans = adc.nfft/2;
    
   nacc = adc.nacc;
   bacc = adc.bacc;
   
   temp = (long long int *) malloc(nchans*sizeof(long long int));
    
   
   finalBand = (float*) malloc(nchans*sizeof(float));
   tempNorm = (float*) malloc(sizeof(float)*nchans);
   
   pthread_mutex_lock(&foldStop_mutex);
   flag = foldStop;
   pthread_mutex_unlock(&foldStop_mutex);
   
   while(flag == 1)
   {
   
   
   pthread_mutex_lock(&foldStop_mutex);
   flag = foldStop;
   pthread_mutex_unlock(&foldStop_mutex);
   
   pthread_mutex_lock(&band_mutex);
   check = bandVal;
   pthread_mutex_unlock(&band_mutex);   
   
    
      
   while(check != 1)
   {
      pthread_mutex_lock(&band_mutex);
      check = bandVal;
      pthread_mutex_unlock(&band_mutex);
      
      pthread_mutex_lock(&foldStop_mutex);
      flag = foldStop;
      pthread_mutex_unlock(&foldStop_mutex);
      
      if(flag==0) 
      {
         free(temp);
         free(finalBand);
         
         free(tempNorm);
         //free(tempNorm);
         return;
      }
   }
   
   for(i=0;i<nchans;i++)
   {
      temp[i]+=band[i];
   }
   samp++;
      
   
   if(samp > (bacc)/(nacc*timeSample)) 
   {
      
      //normArray(nchans,samp);
      for(i=0;i<nchans;i++)
      {
         tempNorm[i] = (float)((long double)temp[i]/(long double)samp);
      }
      pthread_mutex_lock(&finalBand_mutex);
      memcpy(finalBand,tempNorm,nchans*sizeof(float));
      pthread_mutex_unlock(&finalBand_mutex);  
      
      samp=0;
   } 
   
    
   
   pthread_mutex_lock(&band_mutex);
   bandVal= 0;
   pthread_mutex_unlock(&band_mutex);
    
   }
}

    

 
          
