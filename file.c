#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "string.h"

//using namespace std;
//struct parameters input(void);

struct parameters inputParameters()
{
   //int linecount =0;
   struct parameters initial;
   FILE *fp;
   char a[100],b[100];
   int i,p;
   float f;
   double d;
   long double dd;
   /* open the file */
   fp = fopen("./init.dat", "r");
   if (fp == NULL) {
      //printf("I couldn't open init.dat for reading.\n");
      return initial;
   }
   
   fscanf(fp, "%s %s \n", a , b);
   //printf("%s  %s\n", a, b);
   strcpy(initial.nameOfSource,b);
      
   fscanf(fp, "%s %s \n",a,b);
   //printf("%s   %s\n",a,b);
   strcpy(initial.location,b);
     
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s  %d\n", a, i);
   initial.lChannel = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s  %d\n", a, i);
   initial.correlation = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s  %d\n", a, i);
   initial.mixing = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s  %d\n", a, i);
   initial.ips = i;
      
   fscanf(fp,"%s %d\n",a,&i);
   //printf("%s %d\n", a, i);
   initial.gpu = i;
   
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.vlbi = i;
   
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.rdf = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s  %d\n", a, i);
   initial.nfft = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s  %d\n", a, i);
   initial.samplingRate = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s  %d\n", a, i);
   initial.timeOfOberservation = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s  %d\n", a, i);
   initial.levels = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.lpfFlag = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.externalReference = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.externalTrigger = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.termination = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.nacc = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.binary = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.filterbank = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.dedispersed = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.fold = i;
   
   fscanf(fp, "%s %f\n", a, &f);
   //printf("%s %f\n", a, f);
   initial.dm = f;
      
   fscanf(fp, "%s %f\n", a, &f);
   //printf("%s %f\n", a, f);
   initial.fch1 = f;
   
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.bandFlag = i;
      
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.ignore = i;
      
   fscanf(fp, "%s %s \n",a,b);
   //printf("%s   %s\n",a,b);
   strcpy(initial.ignoreFile,b); 
      
   fscanf(fp, "%s %lf\n", a, &d);
   //printf("%s %lf\n", a, d);
   initial.factor = d;
   
   
   
   fscanf(fp, "%s %s \n",a,b);
   //printf("%s   %s\n",a,b);
   strcpy(initial.polyco_file,b);
    
   
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.nbins = i;
   
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.facc = i;
   
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.bacc = i;
   
   fscanf(fp, "%s %d\n", a, &i);
   //printf("%s %d\n", a, i);
   initial.mark5 = i;
   
   fscanf(fp,"%s %d\n",a,&i);
   //printf("%s %d\n", a, i);
   initial.repeat = i;
   
   fscanf(fp, "%s %s \n",a,b);
   //printf("%s   %s\n",a,b);
   strcpy(initial.ipsfile,b);
   
   /* close the file */
   fclose(fp);
   
   initial.bufferLen = 536870912;
      
   initial.segment = 128;
      
   initial.buffer = 512;   
      
   
 //cout <<initial.termination<<endl;
   return initial;
}

