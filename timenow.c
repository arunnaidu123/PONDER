#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
//#include"shared.h"
//#include"defshared.h"
#include"global.h"
#include<sys/timeb.h>

void timenow(double chr , double cmin , double csec)
{

  double hr,min,sec;
  struct tm *ptr_ts;
  int i=0,flag,err;
    
  time_t raw_time;
  
  while(i==0)
  {  
     err= semaphore_p(sem_id);
      if(err !=1) 
      {
        printf("semaphore-v error start_file \n");
      }
      
      flag =flagbuff[0]; //checkflag is the value should 0 to run if 1 the program stops
      
      err= semaphore_v(sem_id);
      if(err !=1)
      {
        printf("semaphore-v error start_file \n");
      }
      
    if(flag!=1) return;
    time ( &raw_time );
    ptr_ts = localtime ( &raw_time );
    hr = (ptr_ts->tm_hour);
    min = (ptr_ts->tm_min);
	  sec = (ptr_ts->tm_sec);
	  if(hr==chr && min ==cmin && sec==csec)
	  {
	    i=1;
	  }
	  // printf("The time now is %lf %lf %lf \n",hr,min,sec);
	}
  printf("The time now is %lf %lf %lf \n",hr,min,sec);
	
	return ;		
}	

