/* Modified for FFTW3          BCJ 05/11/2004  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <tgmath.h>
//#include <fftw3.h>

#define TWOPI 6.2831853071796
#define DFFAC 2.41e-10 /*DM (pc cm-3) = DFFAC*D (MHz) */

/* IHS getchirp routine - adapted by SMO APRIL 4 1999 */
/* revised mk */

void getchirp(float2 *chirp,int nfft,int sideband,double fsky,double bw,double dm,double z4c)
{
  double r,s,f;
  int i;
  double taper;
  //FILE *fp;
  //fp = fopen("chirp.dat","w");
  bw = bw*1e-6;
  printf( "Calculating chirp function.");
  fflush(stdout);
  printf(  "\nnfft: %d flag: %d fsky: %f bw: %f dm: %f \n",nfft,sideband,fsky,bw,dm);
  //s = TWOPI*dm/(DFFAC*(1.0+z4c/10000.0));
  s = TWOPI*dm/(DFFAC);
  
  for(i=0;i<nfft;i++) {
    f = i*bw/(nfft/2);
    
    if(f > bw) 
    {
      f -= bw; 
      f = bw -f;
    } 
    
    //r = ((double) sideband)*f*f*s/((fsky+sideband*f)*fsky*fsky);  
    if(i<=nfft/2) r = -1*f*f*s/((fsky+sideband*f)*fsky*fsky);  
    else r = f*f*s/((fsky+sideband*f)*fsky*fsky);
    
    //if(i%1000==0) printf("%lf \n",(fsky+sideband*f));
    if (f > 0.5*bw) taper = 1.0/sqrt(1.0 + pow((f/(0.94*bw)),80));
    else  taper = 1.0/sqrt(1.0 + pow(((bw-f)/(0.84*bw)),80));
//    taper = 1.0; 
      //taper = 1.0/sqrt(1.0 + pow((f/(0.94*bw)),80));
//    chirp[i] = (double)( cos(r) * taper / (float)(nfft) ) + I *
//              ( -1.0* (double)( sin(r) * taper / (float)(nfft) ));
    chirp[i].x = (float)( cos(r) * taper );
    chirp[i].y = ( -1.0* (float)( sin(r) * taper));
    //chirp[i].x = 0.0;
    //chirp[i].y = 0.0;
    //fprintf(fp,"%1.25f \n",chirp[i].x);
//    chirp[i].re = (float)( cos(r) * taper / (float)(nfft) ); 
//    chirp[i].im = -1.0* (float)( sin(r) * taper / (float)(nfft) );
//    chirp[i].im = sin(r) * taper / (nfft);

  }
//  chirp[0] = 0. + I * 0.;
  chirp[0].x = 0.0;
  chirp[0].y = 0.0;
  //fclose(fp);
//  chirp[0].re = 0.;
//  chirp[0].im = 0.;


}

