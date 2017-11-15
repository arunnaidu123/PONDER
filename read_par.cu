#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>
//#include <tgmath.h>
//#include"global.h"
#include "struct.cuh"
#include "global.cuh"

int read_par()
{
   	char temp_str[20];
  run_par.nfft = 128*1024*1024;
  run_par.dm = adc.dm;
  run_par.bw = (adc.samplingRate/2)*1e6;
  run_par.freq = adc.fch1;
  run_par.dt = (1/(2*run_par.bw));
  run_par.nacc = adc.nacc; 
  printf(" \n the sample time is %1.10lf %d %f\n",run_par.dt,run_par.nacc,run_par.dm);
  return(0);
}

