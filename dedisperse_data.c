#include "dedisperse.h"
#include "global.h"
//#include "outfile.h"

/* 
   orders incoming blocks of data into dedispersed sub-bands 
*/
/* global variables describing the operating mode */
int remove_mean,ascii, asciipol, stream, swapout, headerless, nbands, userbins, usrdm, baseline, clipping, sumifs, profnum1, profnum2, nobits, wapp_inv, wapp_off;
double refrf,userdm,fcorrect;
float clipvalue,jyf1,jyf2;
int fftshift;
//FILE *dedispout;
void *dedisperse_data(void *pa) /*includefile*/
{ 
    
   float *dedisp, realtime, nextbaseline, *offset, *tmpblk;
   int readnext=0,isamp,bnum,nsamp,i,j,s,c,b,indx,ns[2],soffset,ddidx;
   int ic,ixnb,ixnc,*ishift,maxshift,nsmax,cpb,d,spb,x,nsout,nxb;
    
   
   int baseline;
   int start;
   int check;
   int nsblk;
   int flag; 
    /* set up default globals */
   pthread_mutex_lock(&header_mutex);
   check = headerVal;
   pthread_mutex_unlock(&header_mutex);
  
   while(check!=1)
   {
      pthread_mutex_lock(&header_mutex);
      check = headerVal;
      pthread_mutex_unlock(&header_mutex);
   }
  
   userbins=usrdm=asciipol=stream=clipping=swapout=headerless=0;
   remove_mean=sumifs=wapp_inv=wapp_off=barycentric=0;
   
   
   nbands=1;
   nobits=32;
   ascii=1;
   fftshift=1;
   profnum1 = 0;
   profnum2 = 1000;
   nbands=baseline=1;
   clipvalue=refrf=userdm=fcorrect=0.0;
   jyfactor=jyf1=jyf2=1.0;
   refdm=-1.0;
   nchans = nfft/2;
   userdm =adc.dm;
   nifs =1;
   fch1 = adc.fch1;
   foff= -1*(double)adc.samplingRate/(double)nfft;
   tsamp = timeSample*adc.nacc;
   if(adc.dedispersed == 1)
   {
      dedispout = fopen(dfilename, "wb");
      if (dedispout == NULL) 
      {
         printf("I couldn't open stream.dat file.\n");
         exit(0);
      }
      chown(dfilename,1000,1000);
   }
   if(adc.ignore == 1) 
   {
      ignore = ignored_channels(ifp,nchans);
   }
   else
   {
      ignore = (int *) malloc(sizeof(int)*nchans);
      for(i=0;i<nchans;i++) ignore[i] = 0;
   }
   
   for(i=0;i<nchans;i++) if(ignore[i]==1) printf("%d ",i); 
   
   //dedispout = fopen("dedisperse.tim", "wb");
   //if (dedispout == NULL) 
   //{
      //printf("I couldn't open stream.dat file.\n");
      //exit(0);
   //}
  //output=NULL;
  
  /* calculate table of shift values given the filterbank and DM */
   ishift=dmshift(fch1,foff,nchans,nbands,userdm,refrf,tsamp,frequency_table);
   maxshift=ishift[nchans-1];
  
   /* set the buffer size based on the maximum shift */
   nsblk=256*nchans; nsout=32*nchans;
   nsout1 = nsout;
   /*nsblk=256*nchans; nsout=32768*nchans;*/
   nsmax=maxshift*nifs*nchans;
   if (nsmax>nsblk) nsblk=nsmax;
   nsblk1 = nsblk;
   pthread_mutex_lock(&nsblk_mutex);
   nsblkVal=1;
   pthread_mutex_unlock(&nsblk_mutex);
   nxb=nifs*nbands;
   printf(" \n\n\n you are here %d %d %f %f %f %f \n\n\n ",nsblk,nchans,fch1,foff,userdm,refrf); 
   /* define the buffers and initialize counters */
   dedisp =(float *) malloc(nxb*nsout*sizeof(float));
   timeSeries = (float*) malloc(nxb*nsout*sizeof(float));
   offset =(float *) malloc(nxb*sizeof(float));
   tmpblk =(float *) malloc(nsout*sizeof(float));
   buff=(unsigned short *) malloc(2*nsblk*sizeof(short));
   //buff[1]=(float *) malloc(nsblk*sizeof(float));
   for (i=0;i<nxb;i++) offset[i]=0.0;
   d=bnum=isamp=0;
   ic=nchans*nifs;
   nextbaseline=realtime=0.0;
   
   
   

   /* number of channels per band to dedisperse (cpb) must be an integer */
   cpb=nchans/nbands; 
   if ((cpb*nbands) != nchans) printf("\nsilly sub-band selection!\n");
   
   
  /* main loop - keep going until no more data comes in */
   nsamp=2*nsblk/ic;
   printf(" \n\n\n you are here %d %d %d \n\n\n",nchans,nifs,ic);
   s=0;
   
   if(adc.dedispersed == 1) dedisperse_header(dedispout);
   
   
   
  
   pthread_mutex_lock(&dedisp_mutex);
   dedispVal=0;
   pthread_mutex_unlock(&dedisp_mutex);
         
   pthread_mutex_lock(&dedispStop_mutex);
   flag = dedispStop;
   pthread_mutex_unlock(&dedispStop_mutex);
   
   
   while (flag == 1) 
   {
   
       
      /* dedispersing loop over all samples in this buffer */
         pthread_mutex_lock(&dedisp_mutex);
         check = dedispVal;
         pthread_mutex_unlock(&dedisp_mutex);
         
         pthread_mutex_lock(&dedispStop_mutex);
         flag = dedispStop;
         pthread_mutex_unlock(&dedispStop_mutex);
   
          
	         
	       while(check != 1)
         {
            pthread_mutex_lock(&dedisp_mutex);
            check = dedispVal;
            //printf("the loop started %d \n\n",isamp);
            pthread_mutex_unlock(&dedisp_mutex);
            pthread_mutex_lock(&dedispStop_mutex);
            flag = dedispStop;
            pthread_mutex_unlock(&dedispStop_mutex);
            
            if(flag!=1)
            {
              free(dedisp);
              free(tmpblk);
              free(offset);
              free(buff);
              
              if(adc.dedispersed == 1) fclose(dedispout);
              printf("trying to close the file ");
              fflush(stdout);
              // write_dedisp(dedisp,nsout,nifs,nbands,offset,dedispout);
               return;
            }
         }
         
        
    
         soffset=isamp*nxb;
         /* loop over the IFs */
         for (b=0; b<nbands; b++) 
	      {  
	          
	         
	         /* calculate index of this sample */
	         ddidx=soffset+b;
	         /* clear array element for storing dedispersed subband */
	         dedisp[ddidx]=0.0; 
	         /* loop over the channels in this subband */
	         for (c=b*cpb;c<(b+1)*cpb;c++) 
	         {
	         /* proceed only if selected channel # is not in ignore list */
                    	           
	         
            /* calculate index of sample to be added */
	         indx=(s+ishift[c])*ic+c;
	         //printf("------------------%d %d %d %d %d\n",c,ishift[c],indx,(2*nsblk),indx-(2*nsblk));
	         //fflush(stdout);   
	            
	            if(ignore[c]==0)
	            {
	               if(indx < (2*nsblk))
	                  dedisp[ddidx]+=buff[indx];
	               else
	                  dedisp[ddidx]+=buff[indx-(2*nsblk)];
	            }
	         
	         }
	        
	      }  
	      
	      //printf("you are sssshereeeeeeeeeeee %d %d %d %d %d %d \n",ic,s,isamp,nsout,s+indx,(2*nsblk)-s-indx);
         //fflush(stdout);
	      
            
         isamp++; realtime+=tsamp;
         if (isamp==nsout) 
         {
	         //printf("entered the loop--------------dedisp %d %d \n",isamp,nsout);
	         if (baseline) 
	         {
	            for (i=0;i<nifs;i++) 
	            {
	               ixnb=i*nbands;
	               for (b=0;b<nbands;b++) 
	               {
	                  for (j=0;j<nsout;j++) tmpblk[j]=dedisp[j*nxb+ixnb+b];
	                  offset[ixnb+b]=nrselect(nsout/2,nsout,tmpblk-1);
	                  //fprintf(stderr,"%d %f\n",ixnb+b,offset[ixnb+b]);
	               }
	            }
	         }
	         write_dedisp(dedisp,nsout,nifs,nbands,offset,dedispout);
	         isamp=0;
         }
         s++;
         if(s== nsamp) s=0;
         
         pthread_mutex_lock(&dedisp_mutex);
         dedispVal=0;
         pthread_mutex_unlock(&dedisp_mutex);   
        /* end of loop over samples */
      /* switch to next buffer */
      //bnum=!bnum;
   }     /* end of main loop */
}

/* subtract current offset from the dedisperse time samples and write */
void write_dedisp(float *dedisp, int nsout, int nifs, int nbands, float *offset, FILE *output)/*includefile*/
{
  int s,i,b,ixnb,sxib,n,check;
  static int first=1;
  static float *clipthreshold;
  float *temp,outliers,sample,sumsq;
  char *onebyte;
  short *twobyte;
  int flag;
  //int clipping;
  //int nobits;
  //nobits=32;
  /* multiply outgoing data by Jansky calibration factor if supplied */
  if (jyfactor != 1.0) for (i=0;i<nsout*nifs*nbands;i++) dedisp[i]*=jyfactor;

  if (first) {
    /* allocate an array for holding blocks from a given subband */
    temp=malloc(sizeof(float)*nsout);
    /* allocate an array for saving the clipping threshold */
    clipthreshold=malloc(sizeof(float)*nbands*nifs);
    for (i=0;i<nifs*nbands;i++) clipthreshold[i]=0.0;
  }
		       
  for (i=0;i<nifs;i++) {
    ixnb=i*nbands;
    for (b=0;b<nbands;b++) {
      if (first) {
	/* copy sub-band into temporary store for absolute value */
	for (s=0;s<nsout;s++) {
	  sxib=s*nifs*nbands;
	  temp[s]=fabs(dedisp[sxib+ixnb+b]-offset[ixnb+b]);
	}
	/* find the value below which 90% of the samples lie */
	outliers=nrselect(nsout/10,nsout,temp-1);
	n=0;
	sumsq=0.0;
	/* calculate sum of squares based on the inner 90% of samples */
	for (s=0;s<nsout;s++) {
	  if (temp[s]<outliers) {
	    sumsq+=temp[s]*temp[s];
	    n++;
	  }
	}
	/* now set the threshold based on the sum of squares */
	if (n) 
	  clipthreshold[ixnb+b]=clipvalue*sqrt((double)sumsq/(double)n);
	else 
	  clipping=0;
      }
      for (s=0;s<nsout;s++) {
	sxib=s*nifs*nbands;
	/* subtract off median value of this block */
	sample=dedisp[sxib+ixnb+b]-offset[ixnb+b];
	/* clip this sample if it exceeds the threshold */
	if (fabs(sample)>clipthreshold[ixnb+b] && clipping) sample=0.0;
	/* store the final produce and swap bytes if necessary */
	dedisp[sxib+ixnb+b]=sample;
	if (swapout) swap_float(&dedisp[sxib+ixnb+b]);
      }
    }
  }
  
  /* now write out samples and bat on */
  switch (nobits) {
  case 8:
    onebyte = (char *) malloc(nsout*nifs*nbands);
    for (i=0; i<nsout*nifs*nbands; i++) 
      onebyte[i] = (char) dedisp[i];
    fwrite(onebyte,sizeof(char),nsout*nifs*nbands,output);
    break;
  case 16:
    twobyte = (short *) malloc(nsout*nifs*nbands);
    for (i=0; i<nsout*nifs*nbands; i++) 
      twobyte[i] = (short) dedisp[i];
    fwrite(twobyte,sizeof(short),nsout*nifs*nbands,output);
    break;
  case 32:
    if(adc.dedispersed == 1) 
    {
      //printf("Trying to write to the file");
      //fflush(stdout);
      fwrite(dedisp,sizeof(float),nsout,dedispout);
    }
    if(adc.fold == 1)
    {
      pthread_mutex_lock(&fold_mutex);
      check = foldVal;
      pthread_mutex_unlock(&fold_mutex);
         while(check != 0)
         {
            pthread_mutex_lock(&fold_mutex);
            check = foldVal;
            pthread_mutex_unlock(&fold_mutex);
         
            pthread_mutex_lock(&foldStop_mutex);
            flag = foldStop;
            pthread_mutex_unlock(&foldStop_mutex);
         
            if(flag==0) return;
         }
         memcpy(timeSeries,dedisp,sizeof(float)*nsout);
         pthread_mutex_lock(&fold_mutex);
         foldVal=1;
         //printf("iam here .%d................ \n",foldVal);
         //fflush(stdout);
         pthread_mutex_unlock(&fold_mutex);
      }
      
      
    break;
  default:
    printf("\nrequested output number of bits can only be 8, 16 or 32 here it is %d \n",nobits);
    
    break;
  }

  if (first) {
    first=0;
    free(temp);
  }
}

 
