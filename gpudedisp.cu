/*
/usr/local/cuda-5.5/bin/nvcc -I/usr/local/cuda-5.5/samples/common/inc -o simpleCUFFT gpudedisp.c simpleCUFFT.cu read_par.c getCoherentSampleSize.c getchirp.c allocateBuffers.c dedisperse.c send_stuff.c swap_bytes.c error_message.c strings_equal.c dedisperse_header.c freeBuffers.c -lfftw3 -lm -lpthread  -lcufft
*/
#include<stdio.h>
#include<stdlib.h>
#include"struct.cuh"
#include"global.cuh"

#include <pthread.h>
#include "dedisperse.h"
#include<time.h>
#include<sys/timeb.h>
#include<math.h>

//int stop;

char cfilename[200];
int *bufferVal;
float2 *chirp;
//char *dataIn;
float *dataOut;
float *tmp;
float2 *signal;
int site_id;
pthread_mutex_t file_mutex,stop_mutex;
struct runtime run_par;
struct argStruct arguments;
FILE *fpout;
float2 *gpuChirp; 
cufftHandle planf,planb;
char *tempIn;
//float2 *t_signal,*f_signal;
int *dev_nfft;
float *temp;
pthread_mutex_t data_mutex;
pthread_t acquire_thread;
int dataVal=0;
long double mjdnow();
long int pointer1=0;

//void dedisperse_header(FILE *outptr);
//void *acquisition(void *p);


int *gpudedisp(void *pa)
{
  
  int i,check;
  FILE *fpin, *fpar;
  //char fileOut[200],parFile[200];
  //run_par.nacc = 2048*4;
  //run_par.threads=2;
  long int count=0,nread,pointer=0,totalSamples=0;
  long double tstamp=0.0,tempsamp;
  long double dump=0.0;
  float *tempOut;
  int stopCheck=0;
  int err;
  int flag=0;
  //foldVal = 0;
  
  fpout = fopen(cfilename,"wb");
  if(fpout == NULL)
  { 
    printf("cannot open the out put file ...... please check the spce in the location");
    exit(0);
  }
  
  //read_par( fpar, run_par.timeStamp);
  //getCoherentSampleSize();
  run_par.nfft = 1024*1024*128;
  //allocateBuffers();
  tempOut = (float*) malloc(sizeof(float)*run_par.nfft/(2*run_par.nacc));
  
  //pthread_t dedisperse_thread[run_par.threads];
  //pthread_t gpu_thread, write_thread;
  
 
  //run_par.nrdisp = run_par.nfft/2;
  
  count=0;
   
   
   //stop=0;
  machine_id = 3;
  telescope_id = 2;
  refdm = run_par.dm;
  nchans = run_par.nfft/2;
  fch1 = 334.5;
  foff = (-1)*(run_par.bw/nchans);
  nbeams = 1;
  ibeam =0;
  tsamp = run_par.dt*run_par.nacc;
  tstamp = (((long double)(run_par.nfft/4))*run_par.dt)/((long double)86400);
  tempsamp = mjdnow();
  tstart = ((double)tempsamp)+tstamp;
  //output = fpout;
  headerless = 0;
  nbands =1 ;
  nobits = 32;
  nifs = 1;
  dedisperse_header(fpout); 
  
  pthread_mutex_lock(&fold_mutex);
  foldVal=0;
  timeSeries = (float*) malloc(sizeof(float)*run_par.nfft/(2*run_par.nacc));
  pthread_mutex_unlock(&fold_mutex);
  nsout1 = run_par.nfft/(2*run_par.nacc);      
  while(1)
  {
  	//pthread_mutex_lock(&data_mutex);
  	 semaphore_p(sem_fft3);
  	if(fft3Status[0]==1)
  	{
  	  //pointer = count*run_par.nfft/2;
  	  //fseek(fpin,pointer,SEEK_SET);
  	  //nread = fread(dataIn,1,run_par.nfft,fpin);
  	  printf("%d %d %d \n",dataIn[128*1024*1024],dataIn[0],run_par.nfft);
      fflush(stdout);
      gpufft();
  	  //if(nread!=run_par.nfft)
  	  //{
  		  //printf("the file might be at the end or there is some problem \n");
      //}
      /*
      for(i=0;i<run_par.nfft/2;i++)
      {
        //if(i==0) printf("the value of dataOut is %f %f \n",dataOut[0],dataOut[20]);
        dump+=dataOut[i];
      
        if((i+1)%run_par.nacc==0) 
        {
          tempOut[(i+1)/run_par.nacc-1] = (float)(dump/(long double)run_par.nacc);
          //printf("dump %Lf \n",dump); 
          dump = 0.0;
        } 
       
      }
      */
      
      fwrite(dataOut,sizeof(float),run_par.nfft/(2*run_par.nacc),fpout);
    
      totalSamples += run_par.nfft/2;
      printf("the number of samples = %ld and pointer = %ld  %d \n",totalSamples,pointer,dataIn[2]);
      fflush(stdout);
      count++;
      fft3Status[0]=0;
      //if(adc.fold == 1)
      {
        //pthread_mutex_lock(&fold_mutex);
        //check = foldVal;
        //pthread_mutex_unlock(&fold_mutex);
        check=1;
        /*
        while(check != 0)
        {
          pthread_mutex_lock(&fold_mutex);
          check = foldVal;
          pthread_mutex_unlock(&fold_mutex);
         
          pthread_mutex_lock(&foldStop_mutex);
          flag = foldStop;
          pthread_mutex_unlock(&foldStop_mutex);
         
          if(flag==0)
          {
            freeBuffers();
            printf("iam here .%d................ \n",foldVal);
            fflush(stdout);
            return(0);
          }  
        }
        */
        memcpy(timeSeries,tempOut,sizeof(float)*run_par.nfft/(2*run_par.nacc));
        pthread_mutex_lock(&fold_mutex);
        foldVal=1;
        
        pthread_mutex_unlock(&fold_mutex);
      }
      //*/
    }
    //pthread_mutex_unlock(&data_mutex);
    semaphore_v(sem_fft3);
    pthread_mutex_lock(&foldStop_mutex);
    flag = foldStop;
    pthread_mutex_unlock(&foldStop_mutex);
         
    if(flag==0) 
    {
      freeBuffers();
      return(0);
    }
  }
   
  
  freeBuffers(); 
  fclose(fpout);
  //fclose(fpin);
  //fclose(fpar);
}

/*
long double mjdnow()
{
  int dd, mm, yy;
  double a, b, c, e, f, jd;
  long double hr,mi,sec;
  
  time_t now;
  struct tm *d;
  struct tm *ptr_ts;
    
  time(&now);
  d = localtime(&now);
  time_t raw_time;
  time ( &raw_time );
  
  ptr_ts = gmtime ( &raw_time );
  hr = (ptr_ts->tm_hour)/24.0;
  mi = (ptr_ts->tm_min+1)/1440.0;
	
    yy = d->tm_year+1900;
    mm = d->tm_mon+1;
    dd = d->tm_mday;

	if (mm==1){
		yy -= 1;
		mm += 12;
	} else if(mm==2){
		yy -= 1;
		mm += 12;
	}

	a = yy/100.0; 
	b = a/4.0; 
	c = 2.0-a+b;
	e = 365.25*(yy+4716.0); 
	f = 30.6001*(mm+1.0);
	jd = c+dd+e+f-1525.0;
	int count=0;
	if (yy%4==0){
		
		count=1;
	} else if (yy%400==0){
		count=1;
		
	} 
	 
	if (count==1 && mm==14 && dd==29){
		jd -=1;
	}
	if (count!=1 && mm==3){
		jd -=1;
	}
	
  printf("%1.12Lf",(int)jd+hr+mi-2400000.0);
  return ((long double)((int)jd+hr+mi-2400000.0));
			
}	
*/
