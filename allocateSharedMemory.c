#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/shm.h>
#include"global.h"
#include <sys/sem.h>
#include <sys/types.h>
//#include "shared.h"
#include"defshared.h"
 
void allocateSharedMemory()
{
  int i;
  id_data0 = shmget((key_t)0001,sizeof(char)*1024*1024*512, 0666 | IPC_CREAT);
  if (id_data0 == -1) 
  {
    fprintf(stderr,"shmget failed for data 0\n");
    exit(EXIT_FAILURE);
  }

  id_data1 = shmget((key_t)0002, sizeof(char)*1024*1024*512, 0666 | IPC_CREAT);
  if (id_data1 == -1) 
  {
    fprintf(stderr,"shmget failed for data 1\n");
    exit(EXIT_FAILURE);
  }

  id_data2 = shmget((key_t)0003, sizeof(char)*1024*1024*128, 0666 | IPC_CREAT);
  if (id_data2 == -1) 
  {
    fprintf(stderr,"shmget failed for data 2\n");
    exit(EXIT_FAILURE);
  }

  id_data3 = shmget((key_t)0004, sizeof(char)*1024*1024*128, 0666 | IPC_CREAT);
  if (id_data3 == -1) 
  {
    fprintf(stderr,"shmget failedfor data 3 \n");
    exit(EXIT_FAILURE);
  }

  id_data = shmget((key_t)0005, sizeof(char)*1024*1024*128, 0666 | IPC_CREAT);
  if (id_data == -1) 
  {
    fprintf(stderr,"shmget failed for data\n");
    exit(EXIT_FAILURE);
  }
  
  id_fft0 = shmget((key_t)0006, sizeof(long long int)*5, 0666 | IPC_CREAT);
  if (id_fft0 == -1) 
  {
    fprintf(stderr,"shmget failedfor fft 0 \n");
    exit(EXIT_FAILURE);
  }
  
  id_fft1 = shmget((key_t)0007, sizeof(long long int)*5, 0666 | IPC_CREAT);
  if (id_fft1 == -1) 
  {
    fprintf(stderr,"shmget failedfor fft 1 \n");
    exit(EXIT_FAILURE);
  }
  
  id_fft2 = shmget((key_t)0010, sizeof(long long int)*5, 0666 | IPC_CREAT);
  if (id_fft2 == -1) 
  {
    fprintf(stderr,"shmget failedfor fft 2 \n");
    exit(EXIT_FAILURE);
  }
  
  id_fft3 = shmget((key_t)0011, sizeof(long long int)*5, 0666 | IPC_CREAT);
  if (id_fft3 == -1) 
  {
    fprintf(stderr,"shmget failedfor fft 3 \n");
    exit(EXIT_FAILURE);
  }
  
  id_dataIn = shmget((key_t)0012, sizeof(char)*1024*1024*256, 0666 | IPC_CREAT);
  if (id_data == -1) 
  {
    fprintf(stderr,"shmget failed for dataIn\n");
    exit(EXIT_FAILURE);
  }
  
  id_ips = shmget((key_t)0013,sizeof(struct ips_data),066|IPC_CREAT);
  if (id_ips == -1) 
  {
    fprintf(stderr,"shmget failed for IPS\n");
    exit(EXIT_FAILURE);
  }
  
  flag_id= shmget(1111,sizeof(int),0666|IPC_CREAT);
  if (flag_id == -1) 
  {
    fprintf(stderr, "shmget buffer Available failed in the main program\n");
    exit(EXIT_FAILURE);
  }
    
  
  
  
   
}

void attachSharedMemory()
{
  
    
  data0 = (char*) shmat(id_data0, NULL, 0);
  if (data0 == (char *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }

  data1 = (char*)shmat(id_data1, NULL, 0);
  if (data1 == (char *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }

  data2 = (char*)shmat(id_data2, NULL, 0);
  if (data2 == (char *)-1)   
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }

  data3 = (char*) shmat(id_data3, NULL, 0);
  if (data3 == (char *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
 
  data = (char*) shmat(id_data, NULL, 0);
  if (data == (char *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
  
  dataIn = (char*) shmat(id_dataIn, NULL, 0);
  if (dataIn == (char *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
  
  ips = (struct ips_data*) shmat(id_ips, NULL, 0);
  if (ips == (struct ips_data *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
  
  fft0Status = (long long int*) shmat(id_fft0,NULL,0);
  if (fft0Status == (long long int *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
  
  fft1Status = (long long int*) shmat(id_fft1,NULL,0);
  if (fft1Status == (long long int *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
  
  fft2Status = (long long int*) shmat(id_fft2,NULL,0);
  if (fft2Status == (long long int *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
  
  fft3Status = (long long int*) shmat(id_fft3,NULL,0);
  if (fft3Status == (long long int *)-1) 
  {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
  }
  
  flagbuff = (int *)shmat(flag_id, NULL, 0);
  if (flagbuff == (int *)-1) 
  {
    fprintf(stderr, "shmat failed in the Buffer Available main program\n");
    exit(EXIT_FAILURE);
  }
}
 
void createSems()
{
  sem_fft0 = semget((key_t)1000, 1, 0666 | IPC_CREAT);
  if (!set_semvalue(sem_fft0)) 
  {
    fprintf(stderr, "Failed to initialize semaphore fft0\n");
    exit(EXIT_FAILURE);
  }
  sem_fft1 = semget((key_t)1001, 1, 0666 | IPC_CREAT);
  if (!set_semvalue(sem_fft1)) 
  {
    fprintf(stderr, "Failed to initialize semaphore fft1\n");
    exit(EXIT_FAILURE);
  }
  sem_fft2 = semget((key_t)1002, 1, 0666 | IPC_CREAT);
  if (!set_semvalue(sem_fft2)) 
  {
    fprintf(stderr, "Failed to initialize semaphore fft2\n");
    exit(EXIT_FAILURE);
  }
  sem_fft3 = semget((key_t)1003, 1, 0666 | IPC_CREAT);
  if (!set_semvalue(sem_fft3)) 
  {
    fprintf(stderr, "Failed to initialize semaphore fft3\n");
    exit(EXIT_FAILURE);
  }
  sem_data = semget((key_t)1004, 1, 0666 | IPC_CREAT);
  if (!set_semvalue(sem_data)) 
  {
    fprintf(stderr, "Failed to initialize semaphore data \n");
    exit(EXIT_FAILURE);
  }
  
  id_file = semget((key_t)1005, 1, 0666 | IPC_CREAT);
  if (!set_semvalue(id_file)) 
  {
    fprintf(stderr, "Failed to initialize semaphore\n");
    exit(EXIT_FAILURE);
  }
  
  sem_id = semget((key_t)2222, 1, 0666 | IPC_CREAT);
  if (!set_semvalue(sem_id)) 
  {
    fprintf(stderr, "Failed to initialize semaphore\n");
    exit(EXIT_FAILURE);
  }
  
  start_id = semget((key_t)3333, 1, 0666 | IPC_CREAT);
  if (!set_semvalue(start_id)) 
  {
    fprintf(stderr, "Failed to initialize semaphore\n");
    exit(EXIT_FAILURE);
  }
} 

void initSems()
{

  sem_fft0 = semget((key_t)1000, 1, 0666 | IPC_CREAT);
   
  sem_fft1 = semget((key_t)1001, 1, 0666 | IPC_CREAT);
   
  sem_fft2 = semget((key_t)1002, 1, 0666 | IPC_CREAT);
   
  sem_fft3 = semget((key_t)1003, 1, 0666 | IPC_CREAT);
   
  sem_data = semget((key_t)1004, 1, 0666 | IPC_CREAT);
     
  sem_id = semget((key_t)2222, 1, 0666 | IPC_CREAT);
   
  
  start_id = semget((key_t)3333, 1, 0666 | IPC_CREAT);
  
  id_file = semget((key_t)1005, 1, 0666 | IPC_CREAT);  
}
