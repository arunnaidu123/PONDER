#include<stdio.h>
#include<stdlib.h>
//#include<fftw3.h>
#include"struct.cuh"
#include"global.cuh"

void allocateBuffers(void)
{
   
	int i;
  printf("Allocating Buffers %d\n",run_par.nfft);
 // dataIn = (char *) malloc(sizeof(char)*run_par.nfft*2);
  printf("Allocated dataIn \n");
  dataOut = (float *) malloc(sizeof(float)*run_par.nfft/2);
  //temp = (float *) malloc(sizeof(float2)*(run_par.nfft));
  //chirp= (float2 *) malloc(sizeof(float2)*(run_par.nfft));
  checkCudaErrors(cudaMalloc((void **)&tempIn, sizeof(char)*2*run_par.nfft));  
  //checkCudaErrors(cudaMalloc((void **)&gpuChirp, sizeof(float2)*(run_par.nfft)));
  checkCudaErrors(cudaMalloc((void **)&signal, sizeof(float2)*run_par.nfft));
  //checkCudaErrors(cudaMalloc((void **)&t_signal, sizeof(float2)*(run_par.nfft/2+1)));
  //checkCudaErrors(cudaMalloc((void **)&f_signal, sizeof(float2)*(run_par.nfft)));
  checkCudaErrors(cudaMalloc((void **)&dev_nfft, sizeof(int)));
  checkCudaErrors(cudaMalloc((void **)&tmp, sizeof(float)*run_par.nfft));
  checkCudaErrors(cudaMalloc((void **)&temp, sizeof(float)*run_par.nfft/run_par.nacc));
  
  checkCudaErrors(cufftPlan1d(&planf, run_par.nfft, CUFFT_C2C, 1));
  //checkCudaErrors(cufftPlan1d(&planb, run_par.nfft, CUFFT_C2R, 1));
  printf("The value of the nfft is %d \n",run_par.nfft); 
  //run_par.sideband = -1;
  //getchirp(chirp,run_par.nfft,-1,334.5,run_par.bw,run_par.dm,0.0);
  //printf("Copying the chirp funtion");
  //cudaMemcpy(gpuChirp, chirp, (run_par.nfft)*sizeof(float2),cudaMemcpyHostToDevice);
  cudaMemcpy(dev_nfft, &run_par.nfft,sizeof(int),cudaMemcpyHostToDevice);   
  fflush(stdout);  
  
}
