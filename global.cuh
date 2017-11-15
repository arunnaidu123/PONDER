#define NBYTES_PER_SAMP 4
#include <cuda_runtime.h>
#include <cufft.h>
#include <cufftw.h>
#include <helper_functions.h>
#include <helper_cuda.h>
extern "C"
{
  #include "global.h"
}
//#include "shared.h"
typedef float2 Complex;

int read_par();
void allocateBuffers(void);
void freeBuffers(void);
void getchirp(float2 *chirp,int nfft,int sideband,double fsky,double bw,double dm,double z4c);
void getCoherentSampleSize(void);
void gpufft();
//extern void *acquisition(void *p);
//long double mjdnow();

extern long int pointer1;
extern char cfilename[200];
extern int *bufferVal;
extern float2 *chirp;
//extern char *dataIn;
extern float *dataOut;
extern float *tmp;
extern float2 *signal;
extern int site_id;
extern pthread_mutex_t file_mutex,stop_mutex;
extern struct runtime run_par;
extern struct argStruct arguments;
extern FILE *fpout;
extern float2 *gpuChirp; 
extern cufftHandle planf,planb;
extern char *tempIn;
extern float2 *t_signal,*f_signal;
extern int *dev_nfft;
extern float *temp;
extern pthread_mutex_t data_mutex;
extern int dataVal;
//inline void dedisperse_header(FILE *outptr);
