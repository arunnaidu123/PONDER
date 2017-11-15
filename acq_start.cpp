#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
void *status = (void *)0;
unsigned long long int *statusVal;
#include "global.cuh"
//#include "global.h"
#include "define.h"
//#include "shared.h"
#include "defshared.h"
#include "struct.cuh"
//#include "define.cuh"
#include "time.h"
int shmid,statusId;//semaphore IDs 
int err=0; 
  
unsigned long long int bufferLen,totalCount=0,diff=0,ptr,tempVal;

int flag,flag_ips=0,count=0;

void *check(void *pa)
{
  int checkFlag;
  checkFlag=1;
  while(checkFlag==1)
  {
    err= semaphore_p(sem_id);
    if(err !=1) 
    {
      printf("semaphore-v error start_file \n");
    }
   
    checkFlag = flagbuff[0];
   
    err= semaphore_v(sem_id);
    if(err !=1)
    {
      printf("semaphore-v error start_file \n");
    }
  }
  if(checkFlag==3)
  {
    count = 10000;
    flag_ips = 0;
     
  }
  stop();
  
}
int main()
{
  int acq_id,statusId,err;
  initSems(); 
  adc = inputParameters();
  if(adc.gpu==1)
  {
    read_par();
    //allocateBuffers();
  }
  
  allocateSharedMemory();
  
  if(adc.ips==1) flag_ips=1;
  err= semaphore_p(sem_id);
  if(err !=1) 
  {
    printf("semaphore-v error start_file \n");
  }
  attachSharedMemory();
    
  
  err= semaphore_v(sem_id);
  if(err !=1)
  {
    printf("semaphore-v error start_file \n");
  }
    
  while(flag_ips==1 || adc.repeat>count)
  {  
    printf("this is the condition %d %d %d \n\n",flag_ips,adc.repeat,count);
    
    if(adc.gpu==1)
    {
    //read_par();
    allocateBuffers();
    }
    err= semaphore_p(sem_id);
    if(err !=1) 
    {
      printf("semaphore-v error start_file \n");
    }
    
    flagbuff[0]=1;
   
    err= semaphore_v(sem_id);
    if(err !=1)
    {
      printf("semaphore-v error start_file \n");
    }
  
    if(adc.ips==1)
    {
     // sleep(2);
      flag_ips = read_ips(adc.ipsfile);
    }
    
    err= semaphore_v(start_id);
    if(err !=1)
    {
        printf("semaphore-v error start_file \n");
    }
    
    int bin=1,i;
    char temp[500];
    long double mjd;    
    
    headerVal=0;
    nsblkVal=0;
    foldVal = 0;
  
    initStop(); 
  
    
    err= semaphore_p(sem_id);
    if(err !=1) 
    {
      printf("semaphore-v error start_file \n");
    }
   
    flagbuff[0]=1;
   
    err= semaphore_v(sem_id);
    if(err !=1)
    {
      printf("semaphore-v error start_file \n");
    }
  
    semaphore_p(sem_fft0);
    semaphore_p(sem_fft1);
    semaphore_p(sem_fft2);
    semaphore_p(sem_fft3); 
   
    for(i=0;i<5;i++)
    {
      fft0Status[i] = 0;
      fft1Status[i] = 0;
      fft2Status[i] = 0;
      fft3Status[i] = 0;
    }
  
    semaphore_v(sem_fft0);
    semaphore_v(sem_fft1);
    semaphore_v(sem_fft2);
    semaphore_v(sem_fft3);
  
    printf("\n\n ready for restart ----\n\n");
    initStop();
  
    factor0 = adc.factor;
    factor1 = adc.factor;
   
    mjd = mjdnow();
    printf("%Lf \n",mjd);
    sprintf(temp,"%Lf",mjd);
    strcpy(filename,adc.location);
    strcat(filename,adc.nameOfSource);
    //if(adc.mixing == 1) strcat(filename,"_p1_ver6.3_mix");
    //if(adc.correlation == 1) strcat(filename,"_p1_ver6.3_corr");
    //if(adc.ips == 1) strcat(filename,"_p1_ver6.3_ips");
    //if(adc.lChannel == 0) strcat(filename,"_p1_ver6.3_ch0");
    //if(adc.lChannel == 1) strcat(filename,"_p1_ver6.3_ch1");
      
       
    if(adc.ignore==1)
    {
      strcpy(ignoreFile,adc.ignoreFile);
      ifp = fopen(ignoreFile,"r");
      if(ifp==NULL)
      {
        printf("cannot open the ignore file");
        adc.ignore = 0;
      }
    }
    strcat(filename,"_");
    strcat(filename,temp);
    strcpy(bfilename,filename);
    strcat(bfilename,"_ort.bin");
    strcpy(ffilename,filename);
    strcat(ffilename,"_ort.fil");
    strcpy(dfilename,filename);
    strcat(dfilename,"_ort.tim");
    strcpy(ifilename,adc.nameOfSource);
    strcat(ifilename,"_ort_ips.asc");
    strcpy(cfilename,filename);
    strcat(cfilename,"_ort_coh.tim");
    nfft = adc.nfft;
    if(adc.gpu!=1) timeSample = nfft*(1/(double)adc.samplingRate)*pow(10,-6);
    else timeSample = (1/(double)adc.samplingRate)*pow(10,-6);
    fflush(stdout);
    bufferLen = adc.bufferLen;
     
    initMutex();
    printf("mutexs are initialized \n");
  	initThreads();
  	printf("Threads are initialized \n");
  	joinThreads();     
  	fflush(stdout);     
  	
    
  
    count++;
    printf("Finished ......\n");	
  }      
  err= semaphore_p(sem_id);
  if(err !=1) 
  {
    printf("semaphore-v error start_file \n");
  }
   
  dettachSharedMemory();
   
  err= semaphore_v(sem_id);
  if(err !=1)
  {
    printf("semaphore-v error start_file \n");
  }
     
  return 0;
    
}
    
