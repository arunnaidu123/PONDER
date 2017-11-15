#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "rdf1.h"


//char sWord[4], utc[18], stationName[11], sourceName[11], expName[11];
//char receiveMode[1], rdrMode[3];
//unsigned short headerSize, totalFlowRate;


void rdfHeader(FILE *fp)
{
   int i;
   
   fwrite(sWord,1,4,fp);
   
   headerSize = 63;
   fwrite(&headerSize,2,1,fp);
   fwrite(utc,1,18,fp);
   fwrite(stationName,1,11,fp);
   fwrite(sourceName,1,11,fp);
   fwrite(expName,1,11,fp);
   fwrite(&totalFlowRate,2,1,fp);
   fwrite(&receiveMode,1,1,fp);
   fwrite(rdrMode,1,3,fp);
}
