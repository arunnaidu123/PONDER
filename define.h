pthread_t fft0_thread,fft1_thread,corr_thread,copy_thread;
pthread_t fwrite_thread,spectra_thread,check_thread,corrSpec_thread, fold_thread;
pthread_t mixing1_thread,mixCal_thread,ips_thread, band_thread;
pthread_t fft2_thread,fft3_thread,dedisperse_thread,corr1_thread,mixing0_thread;
pthread_t vlbi_thread, bandTransfer_thread, gpu_thread;

int sem_fft0,sem_fft1,sem_fft2,sem_fft3,sem_data;
 
int start_stop();
 
pthread_mutex_t status_mutex;
 
long double mjdnow();
unsigned short *buff;
double factor0;
double factor1;
char filename[500],dfilename[500],ffilename[500],bfilename[500],ifilename[500],ignoreFile[500];
// Initializing the threaded functions
int *ignore;
FILE *ifp;
int bandVal;
 int start_id,sem_id;
float *timeSeries, *finalBand;
unsigned short *band;
double timeSample;
int nfft;
int nsblk1;
int nsout1;

pthread_mutex_t copy_mutex;
pthread_mutex_t spec_mutex;
pthread_mutex_t dedisp_mutex;

pthread_mutex_t flag_mutex;
pthread_mutex_t write_mutex;
pthread_mutex_t fft0_mutex;
pthread_mutex_t fft1_mutex;
pthread_mutex_t fft2_mutex;
pthread_mutex_t fft3_mutex;
pthread_mutex_t header_mutex;
pthread_mutex_t nsblk_mutex;
pthread_mutex_t fold_mutex;

pthread_mutex_t corr_mutex;
pthread_mutex_t count0_mutex;
pthread_mutex_t count1_mutex;
pthread_mutex_t mix0_mutex;
pthread_mutex_t mix1_mutex;
pthread_mutex_t dwrite_mutex;

int dedispVal;
int headerVal;
int nsblkVal;
int foldVal;
long long int count0,count1;

FILE *dedispout;

 
struct parameters adc;

// variables for stopping the program....

int acqStop, copyStop, specStop, fwriteStop, ipsStop, mix0Stop, mix1Stop, mixCalStop, fft0Stop, fft1Stop, corrStop;
int corrCalStop, dedispStop, foldStop, bandStop, vlbiStop, gpuStop;

pthread_mutex_t acqStop_mutex, copyStop_mutex, specStop_mutex, fwriteStop_mutex, ipsStop_mutex, mix0Stop_mutex;

pthread_mutex_t mix1Stop_mutex, mixCalStop_mutex, fft0Stop_mutex, fft1Stop_mutex, corrStop_mutex, corrCalStop_mutex;

pthread_mutex_t dedispStop_mutex, band_mutex, finalBand_mutex, bandStop_mutex, foldStop_mutex, vlbiStop_mutex;

pthread_mutex_t gpuStop_mutex;
