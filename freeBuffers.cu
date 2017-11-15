#include<stdio.h>
#include<stdlib.h>
//#include<fftw3.h>

#include"struct.cuh"
#include"global.cuh"
void freeBuffers(void)
{
  int i;
   

	printf("freeing Buffers \n");
  //*chirp = (fftw_complex*) malloc(sizeof(fftw_complex*)*run_par.threads);
  
   
  /*
  for(i=0;i<run_par.threads;i++)
  {
    free(dataIn[i]);
    free(dataOut[i]); 
    free(voltIn[i]); 
    free(voltOut[i]); 
    free(temp[i]); 
    free(chirp[i]); 
    free(tmp[i]); 
    fftw_destroy_plan(plan_forward[i]);
    fftw_destroy_plan(plan_backward[i]);
  }
  */
  
  //free(dataIn); 
  free(dataOut); 
  //free(voltIn); 
  //free(voltOut); 
  //free(temp); 
  //free(chirp); 
  //free(plan_forward); 
  //free(plan_backward); 
  //free(tempChirp); 
  //free(bufferVal); 
  //free(buffer_mutex); 
  cudaFree(temp); 
  cudaFree(signal);
  //cudaFree(t_signal);
  //cudaFree(f_signal);
  cudaFree(dev_nfft);
  cudaFree(tmp);
  //cudaFree(gpuChirp);
  cudaFree(tempIn);
  cufftDestroy(planf);
}
