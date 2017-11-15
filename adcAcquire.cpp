// ----- include standard driver header from library -----
#include "c_header/dlltyp.h"
#include "c_header/regs.h"
#include "c_header/spcerr.h"
#include "c_header/spcm_drv.h"

#include "common/ostools/spcm_oswrap.h"
#include "common/ostools/spcm_ostools.h"

// ----- standard c include files -----
//#include "global.h"
extern "C"
{
#include "global.h"
}
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


void *acquisition(void *p)
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
   int i,err=0;
   
   int checkflag;
  
               // open card
   hCard = spcm_hOpen ("/dev/spcm0");
   if (!hCard)
   {
      printf ("no card found...\n");
      return 0;
   }

    
   adc =  inputParameters();
   
   if(adc.lChannel==2)
      qwToTransfer = 2*adc.timeOfOberservation*MEGA(adc.samplingRate);
   else
      qwToTransfer = adc.timeOfOberservation*MEGA(adc.samplingRate);
   lNotifySize = MEGA_B(adc.segment);
   lBufferSize = MEGA_B(adc.buffer);
   lInputRange = adc.levels;
   bBWLimit = adc.lpfFlag;
   lChannel = adc.lChannel;
   
   
    
   if(adc.binary==1)
   {
      
      fp = fopen(bfilename,"wb");
      if(fp==NULL)
      {
         printf("cannot open the file %s",filename);
         exit(0);
      }
       
       printf("------------------%s------------",bfilename);
   }
   // read type, function and sn and check for A/D card
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


   // do a simple standard setup
   spcm_dwSetParam_i32 (hCard, SPC_CHENABLE,       lChannel+1);                     
   for(i=0;i<2;i++)
   {
      dwError = spcm_dwSetParam_i32 (hCard, SPC_PATH0 +  i * (SPC_PATH1 - SPC_PATH0),   lPath);
      dwError = spcm_dwSetParam_i32 (hCard, SPC_AMP0 +   i * (SPC_AMP1 - SPC_AMP0),     lInputRange);
      dwError = spcm_dwSetParam_i32 (hCard, SPC_FILTER0 + i * (SPC_FILTER1 - SPC_FILTER0),  bBWLimit ?    1 : 0);
   }
   
   spcm_dwGetParam_i32 (hCard, SPC_CHCOUNT, &lChCount);
   //spcm_dwGetParam_i32 (hDrv, SPC_CHENABLE, &lActivatedChannels);
   
   //printf ("Activated channels bitmask is: 0x%08x\n", lActivatedChannels);
   printf ("Number of activated channels with this bitmask: %d\n", lChCount);

   spcm_dwSetParam_i32 (hCard, SPC_PRETRIGGER,     1024);                  // 1k of pretrigger data at start of FIFO mode
   spcm_dwSetParam_i32 (hCard, SPC_CARDMODE,       SPC_REC_FIFO_SINGLE);   // single FIFO mode
   spcm_dwSetParam_i32 (hCard, SPC_TIMEOUT,        120000);                  // timeout 5 s
   if(adc.externalTrigger==0)
   {
      spcm_dwSetParam_i32 (hCard, SPC_TRIG_ORMASK,    SPC_TMASK_SOFTWARE);    // trigger set to software
      spcm_dwSetParam_i32 (hCard, SPC_TRIG_ANDMASK,   0);                     // ...
   }
   else
   {
      //External trigger to the software 
      dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_EXT0_MODE ,       SPC_TM_POS | SPC_TM_REARM);
      dwError += spcm_dwSetParam_i32 (hCard, SPC_TRIG_TERM,                       1);
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
         //pthread_mutex_lock(&acqStop_mutex);
         //flag = acqStop;
         //pthread_mutex_unlock(&acqStop_mutex);
         
         err= semaphore_p(sem_id);
         if(err !=1) 
         {
            printf("semaphore-v error start_file \n");
         }
         checkflag =flagbuff[0];
         err= semaphore_v(sem_id);
         if(err !=1)
         {
         printf("semaphore-v error start_file \n");
         }
         //printf("%d",checkflag);
         //fflush(stdout);
         if(checkflag!=1)
         {
            stop();
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
               
               pthread_mutex_lock(&fft0Status_mutex);
               pthread_mutex_lock(&fft1Status_mutex);
               pthread_mutex_lock(&fft2Status_mutex);
               pthread_mutex_lock(&fft3Status_mutex);
               if(fft0Status[0] == 0 && fft1Status[0] == 0 && fft2Status[0] == 0 && fft3Status[0] == 0)
               {
                  tempVal = fft0Status[1]%bufferLen;
                  // this is the point to do anything with the data
                  if(adc.filterbank==1 || adc.dedispersed ==1)
                  {
                     if(adc.correlation ==1)
                     {
                        memcpy(data0+tempVal,pnData+lPCPos,lNotifySize);
                        memcpy(data1+tempVal,pnData+lPCPos,lNotifySize);
                        //memcpy(data2+tempVal,pnData+lPCPos,lNotifySize);
                        //memcpy(data3+tempVal,pnData+lPCPos,lNotifySize);
                        fft0Status[0]=1;
                        fft1Status[0]=1;
                        fft0Status[1]+= lNotifySize;
                        fft1Status[1]+= lNotifySize;
                        //fft2Status[0]=1;
                        //fft3Status[0]=1;
                        //fft2Status[1]+= lNotifySize;
                        //fft3Status[1]+= lNotifySize;
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
                     //memcpy(data2+tempVal,pnData+lPCPos,lNotifySize);
                     //memcpy(data3+tempVal,pnData+lPCPos,lNotifySize);
                     fft0Status[0]=1;
                     fft1Status[0]=1;
                     fft0Status[1]+= lNotifySize;
                     fft1Status[1]+= lNotifySize;
                     //fft2Status[0]=1;
                     //fft3Status[0]=1;
                     //fft2Status[1]+= lNotifySize;
                     //fft3Status[1]+= lNotifySize;
                  }
                  
                  if(adc.binary==1)
                  {
                     //pthread_mutex_lock(&write_mutex);
                     fwrite (pnData+lPCPos , 1 , lNotifySize , fp );
                     //pthread_mutex_unlock(&write_mutex);
                  }
                  
                  
                   
                  
                  qwTotalMem += lNotifySize;
                  spcm_dwSetParam_i32 (hCard, SPC_DATA_AVAIL_CARD_LEN,  lNotifySize);
                  printf ("%Ld -- %Ld ----- %Ld --- %d---%d\n",lPCPos,lAvailUser,fft0Status[1],pnData[lPCPos],pnData[lPCPos+10]);
                  fflush(stdout);
               }
                
               pthread_mutex_unlock(&fft0Status_mutex);
               pthread_mutex_unlock(&fft1Status_mutex);
               pthread_mutex_unlock(&fft2Status_mutex);
               pthread_mutex_unlock(&fft3Status_mutex);
                
            }
            // check for esape = abort
            if (bKbhit ())
               if (cGetch () == 27)
               break;
         }
      }
   }
   
    
   
   if(adc.binary==1) fclose(fp); //closing the file 
   stop();
   // send the stop command
   dwError = spcm_dwSetParam_i32 (hCard, SPC_M2CMD, M2CMD_CARD_STOP | M2CMD_DATA_STOPDMA);

   // clean up
   printf ("Finished...\n");
   vFreeMemPageAligned (pnData, (uint64) lBufferSize);
    
   spcm_vClose (hCard);
   
   
   
}

