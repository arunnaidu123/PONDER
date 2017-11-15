#include "filterbank.h"
#include "global.h"
#include "startacq.h"
#include "outfile.h"
//#include "shared.h"


void *writeIps(void *pa) 
{
  int check,fwrite_check,i;
  int flag; 
  FILE *fp;
  long double tempVal = 0.0;
  float value = 0.0; 
  check = 0;
  while(check==0)
  {
    semaphore_p(sem_fft3);
    check = fft3Status[4];
    semaphore_v(sem_fft3); 
    pthread_mutex_lock(&ipsStop_mutex);
    flag = ipsStop;
    pthread_mutex_unlock(&ipsStop_mutex);
    if(flag!=1) return;
  }
  printf("file to be open \n");
  fp = fopen(ips->filename,"a+");
  if(fp == NULL)
  {
    printf("cannot open the file %s " ,ifilename);
    exit(0);
  }
  chown(ifilename,1000,1000);
  fprintf(fp,"# Date : %s  \t \t Time Stamp: %2d:%2d:%2d \n",ips->date,(int)ips->hr,(int)ips->min,(int)ips->sec);
  fprintf(fp,"# Source : %s \t \t Total Time of Observation: %ds \n",ips->source,(int)ips->no_of_files*52); 
  
  
  pthread_mutex_lock(&ipsStop_mutex);
  flag = ipsStop;
  pthread_mutex_unlock(&ipsStop_mutex);
   
  while(flag==1)
  {
    pthread_mutex_lock(&spec_mutex);
    check = specVal;
    pthread_mutex_unlock(&spec_mutex);
     
    pthread_mutex_lock(&ipsStop_mutex);
    flag = ipsStop;
    pthread_mutex_unlock(&ipsStop_mutex);
         
    if(flag!=1) 
    {
	fclose(fp);
        return;       
    }
     
    
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
  printf("%d \n",flag);
  
  return;
}
 
