#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
//#include"shared.h"
#include"defshared.h"
#include"global.h"
//#include "define.h"

//void allocateSharedMemory();

int read_ips(char *fname)
{
  int i=0;
  //file parameters first and second line
  int num;
  char date[10],ort[3],hour[4],angle[4],c,hra[3],hdec[2],hsec[2],file[20];
  char *buffer;
  // file parameters for the res of the lines
  char source[15],rh[3],rmin[2],rsec[2],dh[3],dmin[2],dsec[2];
  int th,tmin,tsec,no_of_files;
  FILE *fp;
    
  double ch,cmin,csec;
  struct tm *ptr_ts;
  
    
  time_t raw_time;
  time ( &raw_time );
  ptr_ts = localtime ( &raw_time );
  ch = (ptr_ts->tm_hour);
  cmin = (ptr_ts->tm_min);
	csec = (ptr_ts->tm_sec);
	
  printf("The time now is %lf %lf %lf \n",ch,cmin,csec);
  
  
  fp = fopen(fname,"r");
  if(fp==NULL)
  {
    printf("cannot open the file \n");
    exit(0);
  }
  buffer = (char*) malloc(1200);
  
  fgets(buffer,1200,fp);
  fscanf(fp,"%d %s %s %s %s %c %s %s %s %s",&num,date,ort,hour,angle,&c,hra,hdec,hsec,file);
  
  while(!feof(fp)&& i==0)
  {
    fscanf(fp,"%s %s %s %s %s %s %s %d %d %d %d ",
               source,rh,rmin,rsec,dh,dmin,dsec,&th,&tmin,&tsec,&no_of_files);
    fgets(buffer,1200,fp);
    
    if(th >= ch )
    {
      if(th==ch)
      {
        if(tmin >= cmin)
        {
          if(tmin==cmin)
          {
            if(tsec >= csec)
            {
              i=1;
               
            }
          }
          if(tmin>cmin)
          {
            i=1;
             
          }
        }
      }
      if(th>ch)
      {
        i=1;
        
       
      }
    }
  } 
     
  fclose(fp);
  if(i==0)
  {
    return 3;
  }
  if(i==1)
  {
    printf("%s %s %s %s %s %s %s %d %d %d %d \n",
              source,rh,rmin,rsec,dh,dmin,dsec,th,tmin,tsec,no_of_files);                
    ips->hr = (double)th;                
    ips->min = (double)tmin;
    ips->sec = (double)tsec;
    ips->no_of_files = no_of_files;
    
    /*
    ips->hr = (double)ch;                
    ips->min = (double)cmin;
    if(csec>=57)
    {
      ips->sec = (double)csec-57;
      ips->min = cmin+1;
      if(ips->min>=60)
      {
        ips->hr +=1;
        ips->min -= 60;
      } 
    }
    else ips->sec = csec+3;
    */
    
    strcpy(ips->date,date);
    strcpy(ips->source,source);
    printf("\n returning the value %lf %lf %lf \n",ips->hr,ips->min,ips->sec);
    return 1;
  }        
    
  
  
  

  //puts(buffer);
  //printf("%d %s %s %s %s %c %s %s %s %s \n",num,data,ort,hour,angle,c,hra,hdec,hsec,file);
  
  return 0;
}

/*  
int main()
{
  char fname[100];
  strcpy(fname,"si14nov6.par");
  allocateSharedMemory();
  //ips = (struct ips_data *)malloc(sizeof(struct ips_data));
  read_ips(fname);
  return 0;
}
*/
  
  
  
  
  
  
  
  
  
  
  
  
     
  
