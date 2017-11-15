#include<stdio.h>
#include<stdlib.h>
#include "global.h"
#include <cpgplot.h>
#include "sigproc.h"
#include "header.h"
 
#define xx 5000
#define yy 5000
#define fx 100
#define fy 3000
#define bx 100
#define by 100
#define w 4800
#define h 2000 

float* normProf(long double *temp , int nbins);
long double maxArray(long double *temp , int nbins);
float maxBand(float *temp , int nchans);
float minBand(float *temp , int nchans);

struct parameters check_adc;
struct POLYCO polyco;
void *plotFold(void *pa)
{
   int i,check;
   float f,g,*xb,max,*xval,min,bmin,bmax;
   //FILE *fp;
   long int nacc = 0;
   
   long double *temp;
   float *prof;
   long double phase;
   double mjd;
   long long int samp=0, samp1 =0;
   int bin =0;
   double tsmp;
   int flag;
   int nbins;
   long double psec;
   //double tstart1;
   check_adc = inputParameters();
   nbins = check_adc.nbins;
    
   
   pthread_mutex_lock(&foldStop_mutex);
   flag = foldStop;
   pthread_mutex_unlock(&foldStop_mutex);
   
    
   
   //mjd = mjdnow();
   //tstart1 = tstart;
    tsmp = check_adc.nacc*timeSample;
   while(flag==1) {
   if(samp1==0 || nbins != check_adc.nbins)
   {
      samp1=0;
      nbins=check_adc.nbins;
      temp = (long double *) malloc(nbins*sizeof(long double));
      
   }
   xval = (float *) malloc(nbins*sizeof(float));
   xb = (float *) malloc(check_adc.nfft/2*sizeof(float));
   if(samp1 == 0)
   {
      for(i=0;i<nbins;i++)
      {
         //prof[i] = 0.0;
         xval[i] = (float) i;
         xval[i] = ((xval[i]/nbins)*w)+fx;
         temp[i] = 0.0;
      }
      
   }
   
   pthread_mutex_lock(&fold_mutex);
   check = foldVal;
   pthread_mutex_unlock(&fold_mutex);
   while(check != 1)
   {
      pthread_mutex_lock(&fold_mutex);
      check = foldVal;
      pthread_mutex_unlock(&fold_mutex);
      
      pthread_mutex_lock(&foldStop_mutex);
      flag = foldStop;
      pthread_mutex_unlock(&foldStop_mutex);
      
      if(flag==0) 
      {
         free(temp);
         free(xval);
         free(xb);
         free(timeSeries);
         return;
      }
   }
   //fp = fopen("data.dat","w"); 
   check_adc = inputParameters();
  
   
   
   
   mjd = tstart +(tsmp*samp)/(86400);
   get_nearest_polyco(adc.polyco_file,mjd,&polyco); 
   psec=polyco_period(mjd,polyco);
   printf(" \n $$$$ %Lf %f ## %lf ## %lf++",temp[2],timeSeries[2],tsmp,(tsmp*samp)/(86400));
   for(i=0;i<nsout1;i++)
   {
      
      phase = (long double)(((long double)samp)*((long double)tsmp)/psec);
      phase = phase - (long int)(phase);
      samp1++;
      samp++;
      bin = (int)(((long double) nbins )*phase);
      temp[bin] += timeSeries[i];
      //printf("temp[bin] == %Lf",temp[bin]);
   }   
        
   prof = normProf(temp,nbins); 
   for(i=0;i<check_adc.nfft/2;i++)
   {
      xb[i] = (float)i;
     
   }
   max = maxBand(prof,nbins);
   min = minBand(prof,nbins);
   
   
   
   //printf("%f %f %d \n",max,min,nsblk1);
   
   if(samp1 > (int)(check_adc.facc/(check_adc.nacc*timeSample)))
   {
      free(temp);
      samp1 =0;
   } 
   cpgbeg(0,"/xs",1,1);
    
   cpgenv(0,xx,0,yy,0,-2);
   //cpgscr(16,0.5,0.5,0.0);
   cpgaxis("N", fx, fy, fx, fy+h, -0.2+min, max+0.2, 0.5, 5, 0.3, 0.0, 0.5, -1, 0);
   cpgaxis("N", fx, fy, fx+w, fy, 1, (float) nbins, ((float)nbins/10), 5, -0.3, 0.0, 0.5, 0.5, 0);
   for(i=0;i<nbins;i++)
   {
      xval[i] = (((float)i/(float)nbins)*w)+fx;
	   prof[i] = fy+h*(prof[i]+0.2-min)/(max-min+0.4);
	}
	
	cpgmtxt("B",-34,0.5,.5,"PULSE PROFILE");
	if(check_adc.correlation == 0 && check_adc.gpu != 1)
	{
	   cpgmtxt("B",-15,0.5,.5,"BAND SHAPE");
	   pthread_mutex_lock(&finalBand_mutex);
	   bmax = maxBand(finalBand,check_adc.nfft/2);
      bmin = minBand(finalBand,check_adc.nfft/2);
      //for(i=0;i<check_adc.nfft/2;i++) fprintf(fp,"%f \n",finalBand[i]);
      //fclose(fp);
      cpgaxis("N", bx, by, bx, by+h, bmin, bmax, ((int)(bmax-bmin)/2), 5, 0.3, 0.0, 0.5, -1, 0);
      cpgaxis("N", bx, by, bx+w, by, 1, check_adc.nfft/2, check_adc.nfft/20, 5, -0.3, 0.0, 0.5, 0.5, 0);
   
   	 
	   for(i=0;i<check_adc.nfft/2;i++)
      {
         xb[i] = ((xb[i]/(check_adc.nfft/2))*w)+bx;
	      finalBand[i] = by+h*(finalBand[i]-bmin)/(bmax-bmin);
	   }
	   cpgline(check_adc.nfft/2,xb,finalBand);
	   pthread_mutex_unlock(&finalBand_mutex);
   }
   cpgline(nbins,xval,prof);
   //cpgline(check_adc.nfft/2,xb,band); 
   
   free(prof);
   //free(temp);
   free(xb);
   free(xval);
   //free(band);
  
   pthread_mutex_lock(&fold_mutex);
   foldVal=0;
   printf("finished in the folding thread %d \n",foldVal);
   fflush(stdout);
   pthread_mutex_unlock(&fold_mutex);
   
   pthread_mutex_lock(&foldStop_mutex);
   flag = foldStop;
   pthread_mutex_unlock(&foldStop_mutex);
   
   }
   return(0);
   
}   

float* normProf(long double *temp , int nbins)
{
   int i;
   long double max;
   float *prof;
   
   prof = (float*) malloc(nbins*sizeof(float));
   
   max = maxArray(temp,nbins);
   //printf("%Lf \n",max);
   //fflush(stdout);
   if(max == 0) max =1;
   
   
   for(i=0;i<nbins;i++)
   {
      prof[i] = (float)(temp[i]/(long double)max);
   }
   
   return prof;
}

long double maxArray(long double *temp , int nbins)
{
   int i;
   long double t=0.0;
   
   for(i=0;i<nbins;i++)
   {
      if(temp[i] > t)
      {
         t = temp[i];
         
      }
   }
   
   return t;
} 

float maxBand(float *temp , int nchans)
{
   int i;
   float t=0.0;
   
   for(i=0;i<nchans;i++)
   {
      if(temp[i] > t)
      {
         t = temp[i];
         
      }
   }
   
   return t;
}  
 
float minBand(float *temp , int nchans)
{
   int i;
   float t = 3000000.0;
   
   for(i=0;i<nchans;i++)
   {
      if(temp[i] < t)
      {
         t = temp[i];
      }
   }
   
   return t;
} 
