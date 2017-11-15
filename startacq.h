#include <fftw3.h>
char *data0,*data1,*data2,*data3;
int copyVal;
fftw_complex *bufferVal, *buffer0Val,*fft0Buffer,*fft1Buffer;
fftw_complex *buffer1Val,*fft2Buffer,*fft3Buffer;
double *bufferReal, *bufferImg;
unsigned short *ur;
int specVal;
fftw_plan p0;
fftw_complex  *in0, *out0;
fftw_plan p,p1,p2,p3;
fftw_complex  *in1, *out1;
fftw_complex  *in2, *out2;
fftw_complex  *in3, *out3;
fftw_complex  *in, *out;
int fft0Val,fft1Val,corrVal;

int mix0Val,mix1Val;
