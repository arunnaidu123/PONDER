struct runtime {
  int  sideband; // single side band for ooty
  double dm;     // DM of the pulsar
  double dt;     // sampling time 1/2*bw
  double bw;     // bandwidth of the observation
  double freq;   // centre frequency of the observation
  int nfft;      // nfft calculated 
  int nacc;      // number of accumulations 
  long double timeStamp;  
}; 

struct argStruct
{
   int threadVal;
   FILE *fp;
};
