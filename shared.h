extern char *data0,*data1,*data2,*data3,*data,*dataIn;
extern int id_data0,id_data1,id_data2,id_data3,id_data,id_fft0,id_fft1,id_fft2,id_fft3,id_dataIn;
extern long long int *fft0Status;
extern long long int *fft1Status;
extern long long int *fft2Status;
extern long long int *fft3Status;
extern int flag_id;
extern int *flagbuff;
extern pthread_mutex_t fft0Status_mutex,fft1Status_mutex,fft2Status_mutex,fft3Status_mutex;
void allocateSharedMemory();
void attachSharedMemory();
void freeSharedMemory();
void dettachSharedMemory();
void createSems();
void delSems();
void initSems();

struct ips_data
{
  double hr;
  double min;
  double sec;
  char source[20];
  char date[10];
  int no_of_files;
  char filename[100];
};
extern struct ips_data *ips;
extern int id_ips;
extern int id_file;
