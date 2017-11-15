#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"rdf1.h"

int main(int argc, char *argv[])
{
   FILE *fp;
   
   int i=0;
   fp = fopen(argv[1],"rb");
   if(fp == NULL) 
   {
      printf("cannot open the file");
      exit(0);
   }
   sWord = (char*) malloc(4);
   utc = (char*) malloc(18);
   stationName = (char*) malloc(11);
   sourceName = (char*) malloc(11);
   expName = (char*) malloc(11);
   rdrMode = (char*) malloc(3);
   
   fread(sWord,1,4,fp);
   i+=4;
   printf("Sync-Word: \t\t %s\n",sWord);
   
   fseek(fp,i,SEEK_SET);
   fread(&headerSize,2,1,fp);
   i+=2;
   printf("Header Size: \t\t %d\n",headerSize);
   
   fseek(fp,i,SEEK_SET);
   fread(utc,1,18,fp);
   i+=18;
   //i = strlen(utc);
   printf("UTC: \t\t %s \n",utc);
   
   fseek(fp,i,SEEK_SET);
   fread(stationName,1,11,fp);
   i+=11;
   printf("Station Name: \t\t %s\n",stationName);
   
   fseek(fp,i,SEEK_SET);
   fread(sourceName,1,11,fp);
   i+=11;
   printf("Source Name \t\t %s\n",sourceName);
   
   fseek(fp,i,SEEK_SET);
   fread(expName,1,11,fp);
   i+=11;
   printf("Experiment Name \t\t %s\n",expName);
   
   fseek(fp,i,SEEK_SET);
   fread(&totalFlowRate,2,1,fp);
   i+=2;
   printf("Total Flow Rate: \t\t %d\n",totalFlowRate);
   
   fseek(fp,i,SEEK_SET);
   fread(&receiveMode,1,1,fp);
   i+=1;
   printf("Receive Mode \t\t %c\n",receiveMode);
   
   fseek(fp,i,SEEK_SET);
   fread(rdrMode,1,3,fp);  
   i+=3;
   printf("rdrMode: \t\t %s\n",rdrMode);
   
   return 0;
} 
   

