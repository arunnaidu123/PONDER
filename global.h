#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <math.h>
#include<pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include "shared.h"
int chown(const char *pathname, uid_t owner, gid_t group);

 
int start_stop();
 
long double mjdnow();
int set_semvalue(int sem_id);
void del_semvalue(int sem_id);
int semaphore_p(int sem_id);
int semaphore_v(int sem_id);

// Initializing the threaded functions
void initMutex();
void initThreads();
void joinThreads();
void *check(void *pa);
void *copy_file(void *p);
void *writeooty2fb(void *p);
void *spectraCal(void *p);
void *bin_file(void *p);
void *fft0Cal(void *pa);
void *fft1Cal(void *pa);
void *fft2Cal(void *pa);
void *fft3Cal(void *pa);
void *dedisperse_data( void *pa) ;
 
void dedisperse_header(FILE *outptr);
void *mixing0(void *pa);
void *mixing1(void *pa);
void *mixCal(void *pa);
void *writeIps(void *pa);
void free_allocation();
void *bandCal(void *pa);
void *correlation(void *pa);
void *plotFold(void *pa);
int read_ips(char *fname);
void timenow(double chr , double cmin , double csec);

 
void *corrSpectraCal(void *pa);
void *vlbiData(void *pa);
int *gpudedisp(void *pa);


void *bandTransfer(void *pa);
 
void initCorr();
void freeCorr();
void freeMixing();
void initMixing();
void datenow(char *filename, char *date);
void rdfHeader(FILE *fp);
int initStop();
int stop();

extern int bandVal;
extern unsigned short *buff;
extern double factor0;
extern double factor1;
extern char filename[500],dfilename[500],ffilename[500],bfilename[500],ifilename[500],ignoreFile[500];

extern float *timeSeries, *finalBand;
extern unsigned short *band;

extern int *ignore;
extern FILE *ifp;

extern double timeSample;
extern int nfft;
extern int nsblk1;
extern int nsout1;
extern int start_id,sem_id;

extern pthread_mutex_t status_mutex;

extern pthread_t fft0_thread,fft1_thread,corr_thread,copy_thread;
extern pthread_t fwrite_thread,spectra_thread,check_thread,corrSpec_thread, fold_thread;
extern pthread_t mixing1_thread,mixCal_thread,ips_thread, band_thread;
extern pthread_t fft2_thread,fft3_thread,dedisperse_thread,corr1_thread,mixing0_thread;
extern pthread_t vlbi_thread, bandTransfer_thread;
extern pthread_t gpu_thread;

extern pthread_mutex_t copy_mutex;
extern pthread_mutex_t spec_mutex;
extern pthread_mutex_t dedisp_mutex;

extern pthread_mutex_t flag_mutex;
extern pthread_mutex_t write_mutex;
extern pthread_mutex_t fft0_mutex;
extern pthread_mutex_t fft1_mutex;
extern pthread_mutex_t fft2_mutex;
extern pthread_mutex_t fft3_mutex;
extern pthread_mutex_t header_mutex;
extern pthread_mutex_t nsblk_mutex;
extern pthread_mutex_t fold_mutex;

extern pthread_mutex_t corr_mutex;

extern pthread_mutex_t count0_mutex;
extern pthread_mutex_t count1_mutex;
extern pthread_mutex_t mix0_mutex;
extern pthread_mutex_t mix1_mutex;
extern pthread_mutex_t dwrite_mutex;

extern int dedispVal;
extern int headerVal;
extern int nsblkVal;
extern int foldVal;
extern long long int count0,count1;
//pthread_mutex_t corr1_mutex;

extern FILE *dedispout;
struct parameters
{
    char nameOfSource[100];
    char location[200];
    int lChannel;
    int correlation;
    int mixing;
    int ips;
    int nfft;
    int samplingRate ;
    int timeOfOberservation;
    int levels ;
    int lpfFlag ;
    int externalReference ;
    int externalTrigger ;
    int edgeRise ;
    int termination ;
    int nacc;
    int binary;
    int filterbank;
    int dedispersed;
    int bufferLen;
    int segment;
    int buffer;
    float dm;
    float fch1;
    int ignore;
    char ignoreFile[500];
    double factor;
    int fold;
    char polyco_file[500];
    int nbins;
    int facc;
    int bacc;
    int vlbi;
    int bandFlag;
    int rdf;
    int mark5;
    int repeat;
    int gpu;
    char ipsfile[500];
};

extern struct parameters inputParameters();
extern struct parameters adc;

// variables for stopping the program....

extern int acqStop, copyStop, specStop, fwriteStop, ipsStop, mix0Stop, mix1Stop, mixCalStop, fft0Stop, fft1Stop, corrStop;
extern int corrCalStop, dedispStop, foldStop, bandStop, vlbiStop, gpuStop;

extern pthread_mutex_t acqStop_mutex, copyStop_mutex, specStop_mutex, fwriteStop_mutex, ipsStop_mutex, mix0Stop_mutex;

extern pthread_mutex_t mix1Stop_mutex, mixCalStop_mutex, fft0Stop_mutex, fft1Stop_mutex, corrStop_mutex, corrCalStop_mutex;

extern pthread_mutex_t dedispStop_mutex, band_mutex, finalBand_mutex, bandStop_mutex, foldStop_mutex, vlbiStop_mutex;

extern pthread_mutex_t gpuStop_mutex;

extern int sem_fft0,sem_fft1,sem_fft2,sem_fft3,sem_data;

#endif
 
