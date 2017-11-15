#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>

int *flagbuff = (int *)0;
int flag;

int main()
{
   int sem_id,err,flag_id;
  
   sem_id = semget((key_t)2222, 1, 0666 | IPC_CREAT);
   if (!set_semvalue(sem_id)) 
   {
      fprintf(stderr, "Failed to initialize semaphore\n");
      exit(EXIT_FAILURE);
   }
    
   
   flag_id= shmget(1111,sizeof(int),0666|IPC_CREAT);
    if (flag_id == -1) 
    {
        fprintf(stderr, "shmget buffer Available failed in the main program\n");
        exit(EXIT_FAILURE);
    }
    
    flagbuff = (int *)shmat(flag_id, NULL, 0);
    if (flagbuff == (int *)-1) 
    {
        fprintf(stderr, "shmat failed in the Buffer Available main program\n");
        exit(EXIT_FAILURE);
    }
    //flag = (int *) flagbuff;  
    
   err= semaphore_p(sem_id);
   if(err !=1) 
   {
      printf("semaphore-v error start_file \n");
   }
   flagbuff[0]=3;
   err= semaphore_v(sem_id);
   if(err !=1)
   {
      printf("semaphore-v error start_file \n");
   }
    
   return 0;
    
}
