#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "struct.cuh"
#include "global.cuh"

#define TWOPI 6.2831853071796
#define DFFAC 2.41e-10 

 
__global__ void complexMul( float2 *a, int *N,double dm,double fsky, int sideband,double bw) 
{
  //int tid = blockIdx.x;    // this thread handles the data at its thread id
  //int tid = threadIdx.x;
  float2 t;
  float2 c;
  double f,s,r,taper;
  float2 at;
  
  s = TWOPI*dm/(DFFAC);
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  
  at = a[tid];
  
  f = tid*bw/(*N/2);
  if(f > bw) 
  {
    f -= bw; 
    f = bw -f;
  }
  if(tid<=(*N)/2) r = -1*f*f*s/((fsky+sideband*f)*fsky*fsky);  
  else r = f*f*s/((fsky+sideband*f)*fsky*fsky);
    
  if (f > 0.5*bw) taper = 1.0/sqrt(1.0 + pow((f/(0.94*bw)),80));
  else  taper = 1.0/sqrt(1.0 + pow(((bw-f)/(0.84*bw)),80));
  c.x = (float)( cos(r) * taper );
  c.y = ( -1.0* (float)( sin(r) * taper));
    
  if (tid < N[0])
  {
    t.x = ((at.x*c.x)-(at.y*c.y))/10000;
    t.y = ((at.x*c.y)+(at.y*c.x))/10000;
    a[tid].x = t.x;
    a[tid].y = t.y;
  }
}

__global__ void typecaste( float2 *a, char *b, int *N)
{
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  //if(tid==10) printf("the value of the dataIn %d \n",b[tid]);
  if (tid < *N)
  {
    a[tid].x = ((float)b[2*tid]+(float)b[2*tid+1])/2; 
    a[tid].y = 0.0;
  }
}

__global__ void scale( float2 *a,float *b,int *N)
{
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  //if(tid==10) printf("the value of the dataIn %d \n",b[tid]);
  if (tid < *N)
  {
    b[tid] = ((a[tid].x*a[tid].x) + (a[tid].y*a[tid].y))/(134217728.0); 
  }
}
 
__global__ void integrate( float *a,float *b,int nacc, int nfft)
{
  float t=0.0;
  int i;
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  //if(tid==10) printf("the value of the dataIn %d \n",b[tid]);
  for(i=0;i<nacc;i++)
  {
    t+=b[tid*nacc+i];
  }
  
  t = t/nacc;
  
  a[tid] = t;
}
 
void gpufft()
{
  
   
  checkCudaErrors(cudaMemcpy(tempIn, dataIn, 2*run_par.nfft*sizeof(char),cudaMemcpyHostToDevice));
  typecaste<<<run_par.nfft/1024,1024>>>(signal,tempIn,dev_nfft);
  checkCudaErrors(cufftExecC2C(planf, signal, signal, CUFFT_FORWARD));
  
  complexMul<<<run_par.nfft/1024,1024>>>( signal, dev_nfft,run_par.dm,334.5,-1,16 );
  checkCudaErrors(cufftExecC2C(planf, signal,signal, CUFFT_INVERSE)); 
  scale<<<run_par.nfft/1024,1024>>>(signal,tmp,dev_nfft);
  integrate<<<(run_par.nfft/run_par.nacc)/1024,1024>>>( temp,tmp, run_par.nacc, run_par.nfft);
  checkCudaErrors(cudaMemcpy(dataOut,&temp[run_par.nfft/(4*run_par.nacc)] , 
                            sizeof(float)*(run_par.nfft/(2*run_par.nacc)),cudaMemcpyDeviceToHost));
   
  return;
}
 
 
 
