#include<stdio.h>
#include<stdlib.h>
#include"global.h"
#include"rdf1.h"
#include<math.h>
 
void header();
void tail();

unsigned short crc16(char *data_p, unsigned short length);

unsigned long long int samp;
FILE *vfp;
long double startTime;
//char *data , *data0 , *data1;
void *vlbiData(void *pa)
{
   int j=0,ii=0;
   int check;
   int flag; 
   double ts;
   int err=0;
   unsigned long long int temp,bufferLen = adc.segment*1024*1024,i=0,length;
   char *tempData,*block;
   
   char tempName[50],vfilename[50];
   startTime = mjdnow();
    
   samp=0; 
   if(adc.mark5 == 1) block = (char *) malloc(2500); 
   if(adc.rdf==1)
   {
      datenow(tempName,utc);
      strcpy(sWord,"RDF1");
      strcpy(stationName,"ORT");
      strcpy(sourceName,adc.nameOfSource);
      strcpy(expName,"TEST");
      strcpy(&receiveMode,"I");
      strcpy(rdrMode,"DAS");
      totalFlowRate = (unsigned short)(adc.samplingRate);
      strcpy(vfilename,"ORT_");
      strcat(vfilename,tempName);
      strcat(vfilename,".rdf");
      vfp = fopen(vfilename,"wb");
      chown(vfilename,1000,1000);
      rdfHeader(vfp);
   }
   else
   {
      strcpy(vfilename,filename);
      strcat(vfilename,"_ort.vlbi");
      vfp = fopen(vfilename,"wb");
      chown(vfilename,1000,1000);
   }
   
   
   if(adc.lChannel==2) tempData = (char *) malloc(bufferLen/2);
   else  tempData = (char *) malloc(bufferLen);
   pthread_mutex_lock(&vlbiStop_mutex);
   flag = vlbiStop;
   pthread_mutex_unlock(&vlbiStop_mutex); 
    
   printf("\n loop started......... \n ");
   while(flag ==1 && adc.mark5 == 0)
   {
      //pthread_mutex_lock(&fft2Status_mutex);
      semaphore_p(sem_fft2);
      if(fft2Status[0]==1)
      {
          
         if(adc.lChannel==2)
         { 
            for(i=0;i<(bufferLen/2);i++)
            {    
               tempData[i] = (char)(int)(((float)data[2*i] + (float)data[2*i+1])/2);
               if(i==0) printf("\n %d %d %d\n",tempData[i],data[2*i+1],data[2*i]);
                
            }
         
            fwrite(tempData,1,bufferLen/2,vfp);
         }
         if(adc.lChannel==0 || adc.lChannel == 1)
         {
            fwrite(data,1,bufferLen,vfp);
            if(i==0) printf("\n %d \n",data[i]);
         }  
            
      }
        
      fft2Status[0]=0;
      //pthread_mutex_unlock(&fft2Status_mutex); 
      semaphore_v(sem_fft2);
      
      pthread_mutex_lock(&vlbiStop_mutex);
      flag = vlbiStop;
      pthread_mutex_unlock(&vlbiStop_mutex);   
   }
   
   while(flag ==1 && adc.mark5 == 1)
   {
      
      //pthread_mutex_lock(&fft2Status_mutex);
      semaphore_p(sem_fft2);
      if(fft2Status[0]==1)
      {
          
         if(adc.lChannel==2)
         { 
            for(i=0;i<(bufferLen/2);i++)
            {    
               tempData[i] = (char)(int)(((float)data[2*i] + (float)data[2*i+1])/2);
               if(i==0) printf("\n %d %d %d\n",tempData[i],data[2*i+1],data[2*i]);
            }
            length = bufferLen/2;
         
            //fwrite(tempData,1,bufferLen/2,vfp);
         }
         if(adc.lChannel==0 || adc.lChannel == 1)
         {
            //fwrite(tempData,1,bufferLen,vfp);
            memcpy(tempData,data,bufferLen);
            if(i==0) printf("\n %d \n",tempData[i]);
            length = bufferLen;
         }  
         
         for(i=0;i<length;i++)
         {
            if(samp%2500==0) header();
            
            block[samp%2500]=tempData[i];
            samp++;
            if(samp%2500==0)
            {
               fwrite(block,1,2500,vfp);
               tail();
            }
         }
            
      }
      fft2Status[0]=0;
      //pthread_mutex_unlock(&fft2Status_mutex);
      semaphore_v(sem_fft2);
      
      pthread_mutex_lock(&vlbiStop_mutex);
      flag = vlbiStop;
      pthread_mutex_unlock(&vlbiStop_mutex); 
   }  
   return;
}

void header()
{
   char *tempTime,*bcdTime;
   long double time,decimal;
   char *finalTime;
   char *c;
   int i;
   unsigned int syncWord = 0xffff;
   unsigned short crc;
   unsigned long long int shift;
   tempTime = (char *) malloc(24);
   finalTime = (char *) malloc(24);
   bcdTime = (char *) malloc(12);
   c = (char *) malloc(1);
   
   time = startTime+((long double)samp/(long double)(adc.samplingRate*86400*(pow(10,6))));
   sprintf(tempTime,"%6.16Lf",time);
   
   decimal = time-((long double)(int)(time));
   shift = (pow(10,9)*decimal);
   //printf(" %s %x %0.16Lf  %Lu\n",tempTime,(int)time,decimal,shift);
   
   //printf(" %s %Lu    ",tempTime,samp);
   for(i=0;i<23;i++)
   {
      if(i==0) finalTime[i] = (char)(int)0;
      if(i>0 && i<6)
      {
         c[0]=(int)tempTime[i-1];
         finalTime[i] = (char)(int)atoi(c);
         if(finalTime[i] > 10) printf("\nsome thing is wrong here %s %d \n",c,finalTime[i]);
      }
      
      if(i>6 && i<24)
      {
         c[0]=(int)tempTime[i-1];
         
         finalTime[i-1] = (char)(int)atoi(c);
      }
   }
   for(i=0;i<12;i++)
   {
      bcdTime[i] = (finalTime[2*i]<<4) + finalTime[2*i+1];
   }    
   crc = crc16(bcdTime,11);
    
   //for(i=0;i<22;i++) printf("%d",finalTime[i]);
    //printf("    ");
   //for(i=0;i<11;i++) printf(" %d",(char)bcdTime[i]);
   //printf("    %x \n",crc);
   fwrite(&syncWord,sizeof(unsigned int),1,vfp);
   fwrite(bcdTime,1,12,vfp);
   fwrite(&crc,sizeof(unsigned short),1,vfp);
   free(tempTime);
   free(finalTime);
   free(bcdTime);
   free(c);
   return;
}

void tail()
{
   char rr,ss,bb,f;
   
   ss = 16;
   bb = 15;
   f = 4;
   f <<=4;
   
   fwrite(&rr,1,1,vfp);
   fwrite(&ss,1,1,vfp);
   fwrite(&bb,1,1,vfp);
   fwrite(&f,1,1,vfp);
}
   
   
