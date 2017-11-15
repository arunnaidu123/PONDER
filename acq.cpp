/*
Ooty Pulsar receiver main acquisition program

*/
// ----- include standard driver header from library -----
#include "../c_header/dlltyp.h"
#include "../c_header/regs.h"
#include "../c_header/spcerr.h"
#include "../c_header/spcm_drv.h"

#include "../common/ostools/spcm_oswrap.h"
#include "../common/ostools/spcm_ostools.h"

// ----- standard c include files -----

//extern "C"
//{
//#include "global.h"
//}
#include "global.cuh"
//#include "global.h"
#include "define.h"
//#include "shared.h"
#include "defshared.h"
#include "struct.cuh"
#include "define.cuh"
int shmid,statusId;//semaphore IDs 
int err=0; 
  
unsigned long long int bufferLen,totalCount=0,diff=0,ptr,tempVal;

int flag,count=0;
 
int flag_ips=0;

/*
**************************************************************************
szTypeToName: doing name translation
**************************************************************************
*/

char* szTypeToName (int32 lCardType)
{
  static char szName[50];
  switch (lCardType & TYP_SERIESMASK)
  {
    case TYP_M2ISERIES:     sprintf (szName, "M2i.%04x", lCardType & TYP_VERSIONMASK);      break;
    case TYP_M2IEXPSERIES:  sprintf (szName, "M2i.%04x-Exp", lCardType & TYP_VERSIONMASK);  break;
    case TYP_M3ISERIES:     sprintf (szName, "M3i.%04x", lCardType & TYP_VERSIONMASK);      break;
    case TYP_M3IEXPSERIES:  sprintf (szName, "M3i.%04x-Exp", lCardType & TYP_VERSIONMASK);  break;
    default:                sprintf (szName, "unknown type");                               break;
  }
  return szName;
}


 
int acquisition()
{
  FILE *fp;
  drv_handle  hCard;
  int32       lCardType, lSerialNumber, lFncType;
  char      *pnData;
  char        szErrorTextBuffer[ERRORTEXTLEN];
  uint32      dwError;
  int32       lStatus, lAvailUser, lPCPos,lChannel=0;
  uint64      qwTotalMem = 0;
  uint64      qwToTransfer;
  int32    lLevel0 = 2000, lLevel1 = 1000;
  int32   lPath=1, lInputRange, bBWLimit;
  int lChCount;
  // settings for the FIFO mode buffer handling
  int32       lBufferSize;
  //int32       lNotifySize =   KILO_B(16);
  int32 lNotifySize;
  int i;
  int checkflag,gpuFlag=0,gpuSwap=0,copyStart=0;
  dataVal=0;
  // open card
  hCard = spcm_hOpen ("/dev/spcm0");
  if (!hCard)
  {
    printf ("no card found...\n");
    return 0;
  }
  
  //Reading the file to the stucture adc defined in global.h
  adc =  inputParameters();
  
  /*
  Checking the number of channels from the file and setting the ADC
  */
  if(adc.lChannel==2)
    qwToTransfer = 2*adc.timeOfOberservation*MEGA(adc.samplingRate);
  else
    qwToTransfer = adc.timeOfOberservation*MEGA(adc.samplingRate);
    
  lNotifySize = MEGA_B(adc.segment); //setting up the segment lenght
  lBufferSize = MEGA_B(adc.buffer);  // setting the buffer size multiple of segment size
  lInputRange = adc.levels; // setting the voltage level in mV
  bBWLimit = adc.lpfFlag; // setting up the 20MHz filter
  lChannel = adc.lChannel; // number of channels
  
  
  
  //Finding out wheather the binary file is required or not
  
  if(adc.binary==1)
  {
    fp = fopen(bfilename,"wb");
    if(fp==NULL)
    {
      printf("cannot open the file %s",filename);
      exit(0);
    }
    chown(bfilename,1000,1000); 
  }
  
  // read type, function and sn and check for ADC
  spcm_dwGetParam_i32 (hCard, SPC_PCITYP,         &lCardType);
  spcm_dwGetParam_i32 (hCard, SPC_PCISERIALNO,    &lSerialNumber);
  spcm_dwGetParam_i32 (hCard, SPC_FNCTYPE,        &lFncType);

  switch (lFncType)
  {
    case SPCM_TYPE_AI:  
      printf ("Found: %s sn %05d\n", szTypeToName (lCardType), lSerialNumber);
      break;

    default:
      printf ("Card: %s sn %05d not supported by examplee\n", szTypeToName (lCardType), lSerialNumber);            
      return 0;
  }
  
  
  // do card setup
  spcm_dwSetParam_i32 (hCard, SPC_CHENABLE,       lChannel+1);                     
  for(i=0;i<2;i++)
  {
    dwError = spcm_dwSetParam_i32 (hCard, SPC_PATH0 +  i * (SPC_PATH1 - SPC_PATH0),   lPath);
    dwError = spcm_dwSetParam_i32 (hCard, SPC_AMP0 +   i * (SPC_AMP1 - SPC_AMP0),     lInputRange);
    dwError = spcm_dwSetParam_i32 (hCard, SPC_FILTER0 + i * (SPC_FILTER1 - SPC_FILTER0),  bBWLimit ?    1 : 0);
  }
   
  spcm_dwGetParam_i32 (hCard, SPC_CHCOUNT, &lChCount);
    
  printf ("Number of activated channels with this bitmask: %d\n", lChCount);

  spcm_dwSetParam_i32 (hCard, SPC_PRETRIGGER,     1024);                  // 1k of pretrigger data at start of FIFO mode
  spcm_dwSetParam_i32 (hCard, SPC_CARDMODE,       SPC_REC_FIFO_SINGLE);   // single FIFO mode
  
  
  //Setting up the waiting time for more than 2 minutes just for the minute pulse
  spcm_dwSetParam_i32 (hCard, SPC_TIMEOUT,        120000);                   
  
  //Setting up the External trigger if requested 
  if(adc.ips==1)
  {
    qwToTransfer =  2*52*MEGA(adc.samplingRate)*ips->no_of_files;
    flag_ips=read_ips(adc.ipsfile);
    if(flag_ips!=1)
    {
      printf("End of the parameter file exiting");
      err= semaphore_p(sem_id);
      if(err !=1) 
      {
        printf("semaphore-v error start_file \n");
      }
   
      flagbuff[0]=3;
   
      err= semaphore_v(sem_id);
      if(err !=1)
      {
        printf("semaphore-v error start_file \n");
      }
      return 0;
    }
    timenow(ips->hr,ips->min,ips->sec);
    strcpy(ips->filename,adc.location);
    strcat(ips->filename,adc.nameOfSource);
    strcat(ips->filename,ips->date);
    strcat(ips->filename,".dat");
    semaphore_p(sem_fft3);
    fft3Status[4]=1;
    semaphore_v(sem_fft3);
    printf("sent the trigger \n");
  }
  if(adc.externalTrigger==0)
  {
    spcm_dwSetParam_i32 (hCard, SPC_TRIG_ORMASK,    SPC_TMASK_SOFTWARE);    // trigger set to software
    spcm_dwSetParam_i32 (hCard, SPC_TRIG_ANDMASK,   0);                     // ...
  }
  else
  {
    //External trigger to the software 
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_EXT0_MODE ,       SPC_TM_POS | SPC_TM_REARM);
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_TERM,                       0);
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_EXT0_ACDC,       0);

    // on bSingleSrc flag no other trigger source is used
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_ORMASK, SPC_TMASK_EXT0); 
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_ANDMASK,        0);
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_CH_ORMASK0,     0);
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_CH_ORMASK1,     0);
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_CH_ANDMASK0,    0);
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_CH_ANDMASK1,    0);
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_EXT0_LEVEL0, lLevel0);
    dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_EXT0_LEVEL1, lLevel1);
      
  }     
   
   
  // Externel reference settings 
  if(adc.externalReference==0)
  {
    spcm_dwSetParam_i32 (hCard, SPC_CLOCKMODE,      SPC_CM_INTPLL);         // clock mode internal PLL
  }
  else
  {
    spcm_dwSetParam_i32 (hCard, SPC_CLOCKMODE,      SPC_CM_EXTREFCLOCK);         // external reference clock
    spcm_dwSetParam_i32 (hCard, SPC_REFERENCECLOCK, MEGA(10));
  }
   
  // setting up the sampling rate
  spcm_dwSetParam_i32 (hCard, SPC_SAMPLERATE, MEGA(adc.samplingRate));
  spcm_dwSetParam_i32 (hCard, SPC_CLOCKOUT,       0);                     // no clock output


  // define the data buffer
  pnData = (char*) pvAllocMemPageAligned ((uint64) lBufferSize);
  if (!pnData)
  {
    printf ("memory allocation failed\n");
    spcm_vClose (hCard);
    return 0;
  }

  spcm_dwDefTransfer_i64 (hCard, SPCM_BUF_DATA, SPCM_DIR_CARDTOPC, lNotifySize, pnData, 0, lBufferSize);
   
  // start everything
  dwError = spcm_dwSetParam_i32 (hCard, SPC_M2CMD, M2CMD_CARD_START | M2CMD_CARD_ENABLETRIGGER | M2CMD_DATA_STARTDMA);
   
                      
  // check for error
  if (dwError != ERR_OK)
  {
    spcm_dwGetErrorInfo_i32 (hCard, NULL, NULL, szErrorTextBuffer);
    printf ("%s\n", szErrorTextBuffer);
    vFreeMemPageAligned (pnData, (uint64) lBufferSize);
    spcm_vClose (hCard);
    return 0;
  }


  // run the FIFO mode and loop throught the data
  else
  {
    while (qwTotalMem < qwToTransfer)
    {
      /*
      Semaphores are used for IPC for communication with the external process
      flagbuff is the shared memory used as flag to start and stop the process
      */
      
      
      //protecting flagbuff
      err= semaphore_p(sem_id);
      if(err !=1) 
      {
        printf("semaphore-v error start_file \n");
      }
      
      checkflag =flagbuff[0]; //checkflag is the value should 0 to run if 1 the program stops
      
      err= semaphore_v(sem_id);
      if(err !=1)
      {
        printf("semaphore-v error start_file \n");
      }
      
      
      if(checkflag!=1)
      {
        if(adc.binary==1) fclose(fp); //closing the file 
        stop();//all the other threads are stopped
        flag_ips=3;
        if(checkflag==3) count = 1000;
        //the card and other memory allocations are freed
        
        dwError = spcm_dwSetParam_i32 (hCard, SPC_M2CMD, M2CMD_CARD_STOP | M2CMD_DATA_STOPDMA);
        printf ("Finished... %d\n",checkflag);
        vFreeMemPageAligned (pnData, (uint64) lBufferSize);
        spcm_vClose (hCard);
        return NULL;
      }
         
      if (dwError = spcm_dwSetParam_i32 (hCard, SPC_M2CMD, M2CMD_DATA_WAITDMA) != ERR_OK)
      {
        if (dwError == ERR_TIMEOUT)
          printf ("... Timeout\n");
        else
          printf ("... Error: %d\n", dwError);
          break;
      }
      else
      {
        spcm_dwGetParam_i32 (hCard, SPC_M2STATUS,             &lStatus);
        spcm_dwGetParam_i32 (hCard, SPC_DATA_AVAIL_USER_LEN,  &lAvailUser);
        spcm_dwGetParam_i32 (hCard, SPC_DATA_AVAIL_USER_POS,  &lPCPos);

        if (lAvailUser >= lNotifySize)
        {
          //pthread_mutex_lock(&fft0Status_mutex);
          //pthread_mutex_lock(&fft1Status_mutex);
          //pthread_mutex_lock(&fft2Status_mutex);
          //pthread_mutex_lock(&fft3Status_mutex);
          //printf("Trigger received\n");
          semaphore_p(sem_fft0);
         printf("Trigger received 0\n");
          semaphore_p(sem_fft1);
          printf("Trigger received 1 \n");
          semaphore_p(sem_fft2);
          printf("Trigger received 2\n");
          
          //printf("Trigger received\n");
          
          if(fft0Status[0] == 0 && fft1Status[0] == 0 && fft2Status[0] == 0 && fft3Status[0] == 0)
          {
            tempVal = fft0Status[1]%bufferLen;
            // this is the point to do anything with the data
            if(adc.filterbank==1 || adc.dedispersed ==1|| adc.fold == 1)
            {
              if(adc.correlation ==1)
              {
                memcpy(data0+tempVal,pnData+lPCPos,lNotifySize);
                memcpy(data1+tempVal,pnData+lPCPos,lNotifySize);
                fft0Status[0]=1;
                fft1Status[0]=1;
                fft0Status[1]+= lNotifySize;
                fft1Status[1]+= lNotifySize;
              }
              if(adc.mixing == 1)
              {
                memcpy(data0+tempVal,pnData+lPCPos,lNotifySize);
                memcpy(data1+tempVal,pnData+lPCPos,lNotifySize);
                fft0Status[0]=1;
                fft1Status[0]=1;
                fft0Status[1]+= lNotifySize;
                fft1Status[1]+= lNotifySize;
              }
              if(adc.lChannel == 0 || adc.lChannel == 1)
              {
                memcpy(data+tempVal,pnData+lPCPos,lNotifySize);
                fft0Status[0]=1;
                fft0Status[1]+= lNotifySize;
              }
                      
            }
                 
            if(adc.ips ==1)
            {
              memcpy(data0+tempVal,pnData+lPCPos,lNotifySize);
              memcpy(data1+tempVal,pnData+lPCPos,lNotifySize);
              fft0Status[0]=1;
              fft1Status[0]=1;
              fft0Status[1]+= lNotifySize;
              fft1Status[1]+= lNotifySize;
            }
            if(adc.binary==1)
            {
              fwrite (pnData+lPCPos , 1 , lNotifySize , fp );
            }
            if(adc.vlbi == 1)
            {
              memcpy(data,pnData+lPCPos,lNotifySize);
              fft2Status[0]=1;
              fft2Status[1]+= lNotifySize;
            }
            if(adc.gpu == 1)
            {
              
              if(gpuSwap==0)
              {
                 
                
                memcpy(data2,pnData+lPCPos,lNotifySize);
                
                gpuFlag=1;
                while(gpuFlag!=0)
                {
                  //pthread_mutex_lock(&data_mutex);
                  semaphore_p(sem_fft3);
                  gpuFlag = fft3Status[0];
                  semaphore_v(sem_fft3);
                  //pthread_mutex_unlock(&data_mutex);
                }
                if(copyStart!=0)
                {
                  //pthread_mutex_lock(&data_mutex);
                  semaphore_p(sem_fft3);
                  memcpy(dataIn,data3,lNotifySize);
                  memcpy(&dataIn[lNotifySize],data2,lNotifySize);
                  fft3Status[0]=1;
                  semaphore_v(sem_fft3);
                  //pthread_mutex_unlock(&data_mutex);
                  //pointer1 +=lNotifySize/2;
                }
                //totalSamples+=lNotifySize/2; 
                printf("this is the try \n");
                gpuSwap=1;
                 printf("this is swap %d %d %d %d %d \n",gpuSwap,data2[0],dataIn[lNotifySize],data3[0],dataIn[0]);
                fflush(stdout);
              }
              else
              {
                printf("this is swap %d \n",gpuSwap);
                fflush(stdout);
                memcpy(data3,pnData+lPCPos,lNotifySize);
                gpuSwap=0;
                copyStart=1;
                gpuFlag=1;
                while(gpuFlag!=0)
                {
                  //pthread_mutex_lock(&data_mutex);
                  semaphore_p(sem_fft3);
                  gpuFlag = fft3Status[0];
                  semaphore_v(sem_fft3);
                  //pthread_mutex_unlock(&data_mutex);
                }
                //pthread_mutex_lock(&data_mutex);
                semaphore_p(sem_fft3);
                memcpy(dataIn,data2,lNotifySize);
                memcpy(&dataIn[lNotifySize],data3,lNotifySize);
                fft3Status[0]=1;
                semaphore_v(sem_fft3);
                //pthread_mutex_unlock(&data_mutex);
                printf("this is swap %d %d %d %d %d \n",gpuSwap,data3[0],dataIn[lNotifySize],data2[0],dataIn[0]);
                fflush(stdout);
                //pointer1 +=lNotifySize/2;
                //totalSamples+=lNotifySize/2;
             
              }
              fft0Status[0]=0;
              fft0Status[1]+= lNotifySize;
            }
            qwTotalMem += lNotifySize;
            spcm_dwSetParam_i32 (hCard, SPC_DATA_AVAIL_CARD_LEN,  lNotifySize);
            printf ("%Ld -- %Ld ----- %llu --- %d---%d --- %d -- %d\n",lPCPos,lAvailUser,fft0Status[1],pnData[lPCPos],pnData[lPCPos+1],data[0],data[1]);
            fflush(stdout);
          }
            //pthread_mutex_unlock(&fft0Status_mutex);
            //pthread_mutex_unlock(&fft1Status_mutex);
            //pthread_mutex_unlock(&fft2Status_mutex);
            //pthread_mutex_unlock(&fft3Status_mutex);
            semaphore_v(sem_fft0);
            semaphore_v(sem_fft1);
            semaphore_v(sem_fft2);
             
        }
        if (bKbhit ())
        if (cGetch () == 27)
        break;
      }
    }
  }
   
  if(adc.binary==1) fclose(fp); //closing the file 
  err= semaphore_p(sem_id);
  if(err !=1) 
  {
    printf("semaphore-v error start_file \n");
  }
   
  flagbuff[0]=0;
   
  err= semaphore_v(sem_id);
  if(err !=1)
  {
    printf("semaphore-v error start_file \n");
  }
   
  stop();
  dwError = spcm_dwSetParam_i32 (hCard, SPC_M2CMD, M2CMD_CARD_STOP | M2CMD_DATA_STOPDMA);
  
  printf ("Finished...\n");
  vFreeMemPageAligned (pnData, (uint64) lBufferSize);
    
  spcm_vClose (hCard);
}

int main()
{
  int err,bin=1,i;
  char temp[500];
  long double mjd;    
  
  headerVal=0;
  nsblkVal=0;
  foldVal = 0;
  
   
  createSems();
   
  err=semaphore_p(start_id);
  if(err !=1)
  {
    printf("semaphore-p error copy_file \n");
  } 
    
  adc = inputParameters();
  
  allocateSharedMemory();
  if(adc.ips==1) flag_ips=1;
  err= semaphore_p(sem_id);
  if(err !=1) 
  {
    printf("semaphore-v error start_file \n");
  }
   
  attachSharedMemory();
   
  
  flagbuff[0]=1;
   
  err= semaphore_v(sem_id);
  if(err !=1)
  {
    printf("semaphore-v error start_file \n");
  }
  
  while(count<adc.repeat||flag_ips==1)
  { 
     
  err= semaphore_p(sem_id);
  if(err !=1) 
  {
    printf("semaphore-v error start_file \n");
  }
   
  flagbuff[0]=1;
   
  err= semaphore_v(sem_id);
  if(err !=1)
  {
    printf("semaphore-v error start_file \n");
  }
  
   
  
  printf("Trying to allocate memory \n");
  fflush(stdout); 
  
  semaphore_p(sem_fft0);
  semaphore_p(sem_fft1);
  semaphore_p(sem_fft2);
  semaphore_p(sem_fft3);
          
  for(i=0;i<5;i++)
  {
    fft0Status[i] = 0;
    fft1Status[i] = 0;
    fft2Status[i] = 0;
    fft3Status[i] = 0;
  }
   
  semaphore_v(sem_fft0);
  semaphore_v(sem_fft1);
  semaphore_v(sem_fft2);
  semaphore_v(sem_fft3);
  
  printf("\n\n ready for restart ----\n\n");
  initStop();
  err= semaphore_p(sem_id);
  if(err !=1) 
  {
    printf("semaphore-v error start_file \n");
  }
  flagbuff[0]=1;
  err= semaphore_v(sem_id);
  if(err !=1)
  {
    printf("semaphore-v error start_file \n");
  }
  err=semaphore_p(start_id);
  if(err !=1)
  {
    printf("semaphore-p error copy_file \n");
  } 
  
     
  if(count>0) 
  {
    initStop();
    adc = inputParameters();
    for(i=0;i<5;i++)
    {
      fft0Status[i] = 0;
      fft1Status[i] = 0;
      fft2Status[i] = 0;
      fft3Status[i] = 0;
    }
  }   
  flag_ips = adc.ips;
   
  factor0 = adc.factor;
  factor1 = adc.factor;
  
  mjd = mjdnow();
  printf("%Lf \n",mjd);
  sprintf(temp,"%Lf",mjd);
  strcpy(filename,adc.location);
  strcat(filename,adc.nameOfSource);
  //if(adc.mixing == 1) strcat(filename,"_p1_ver6.3_mix");
  //if(adc.correlation == 1) strcat(filename,"_p1_ver6.3_corr");
  //if(adc.ips == 1) strcat(filename,"_p1_ver6.3_ips");
  //if(adc.lChannel == 0) strcat(filename,"_p1_ver6.3_ch0");
  //if(adc.lChannel == 1) strcat(filename,"_p1_ver6.3_ch1");
    
       
  if(adc.ignore==1)
  {
    strcpy(ignoreFile,adc.ignoreFile);
    ifp = fopen(ignoreFile,"r");
    if(ifp==NULL)
    {
      printf("cannot open the ignore file");
      adc.ignore = 0;
    }
  }
  strcat(filename,"_");
  strcat(filename,temp);
  strcpy(bfilename,filename);
  strcat(bfilename,".bin");
  strcpy(ffilename,filename);
  strcat(ffilename,".fil");
  strcpy(dfilename,filename);
  strcat(dfilename,".tim");
  strcpy(ifilename,adc.nameOfSource);
  strcat(ifilename,"_p1_ver6.3_ips.asc");
  strcpy(cfilename,filename);
  strcat(cfilename,"_coherent.tim");
  nfft = adc.nfft;
  if(adc.gpu!=1) timeSample = nfft*(1/(double)adc.samplingRate)*pow(10,-6);
  else timeSample = (1/(double)adc.samplingRate)*pow(10,-6);
  fflush(stdout);
  bufferLen = adc.bufferLen;
 
  
   
       
  fflush(stdout); 
  acquisition(); 
   
  
  
  count++;
  }
  err= semaphore_p(sem_id);
  if(err !=1) 
  {
    printf("semaphore-v error start_file \n");
  }
  
   
  dettachSharedMemory();
  freeSharedMemory();
  delSems();
  err= semaphore_v(sem_id);
  if(err !=1)
  {
    printf("semaphore-v error start_file \n");
  }
   
  return 0;
}

void free_allocation()
{
  if(adc.correlation == 1 || adc.ips == 1 )
  {
    free(data0);
    free(data1);
  }
   
  if(adc.correlation == 0 && adc.ips == 0)
  {
    free(data0);
  }
  free(ignore);
}
 
