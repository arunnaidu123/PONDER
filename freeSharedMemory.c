#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/shm.h>
#include"global.h"
#include <sys/sem.h>
#include <sys/types.h>
#include"defshared.h"

void freeSharedMemory()
{
  
  if ((shmctl (id_dataIn, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl data");
    exit (-1);
  }
  
  if ((shmctl (id_ips, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl data");
    exit (-1);
  }
  
  if ((shmctl (id_data, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl data");
    exit (-1);
  }
  
  if ((shmctl (id_data0, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl data0");
    exit (-1);
  }
  
  if ((shmctl (id_data1, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl data1");
    exit (-1);
  }
  
  if ((shmctl (id_data2, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl data2");
    exit (-1);
  }
  
  if ((shmctl (id_data3, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl data3");
    exit (-1);
  }
  
  if ((shmctl (id_fft0, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl fft0");
    exit (-1);
  }
  
  if ((shmctl (id_fft1, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl fft1");
    exit (-1);
  } 
  
  if ((shmctl (id_fft2, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl fft2");
    exit (-1);
  }
  
  if ((shmctl (id_fft3, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl fft3");
    exit (-1);
  }
  
  if ((shmctl (flag_id, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl flag");
    exit (-1);
  }
  
   
}

void dettachSharedMemory()
{
  if (shmdt(data0) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(data1) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(data2) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(data3) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(dataIn) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(data) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(ips) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(fft0Status) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(fft1Status) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(fft2Status) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(fft3Status) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(flagbuff) == -1) 
  {
    fprintf(stderr, "shmdt failed\n");
    exit(EXIT_FAILURE);
  }
}

void delSems()
{
printf("trying to remove the sems \n");
  del_semvalue(sem_fft0);
  del_semvalue(sem_fft1);
  del_semvalue(sem_fft2);
  del_semvalue(sem_fft3);
  del_semvalue(sem_data);
  //del_semvalue(start_id);
  //del_semvalue(sem_id);
  //del_semvalue(id_file);
}
