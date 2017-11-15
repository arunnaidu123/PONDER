/*
**************************************************************************

rec_single_binaryfile.cpp                   (c) Spectrum GmbH , 02/2006

**************************************************************************

this example supports all acquistion cards

Does a continous FIFO transfer and writes data to a binary file. The file
type can be selected to simple binary file or binary file including the
SBench5 format header (*.sbs/*sb5) file. 

change the eFileType variable to select between plain binary and SB5
file. 
  
Change the global flag g_bThread to use the threaded version or the plain
and more simple loop.

Feel free to use this source for own projects and modify it in any kind

**************************************************************************
*/


// ----- standard c include files -----
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace std;
// ----- include of common example librarys -----
#include "../common/spcm_lib_card.h"
#include "../common/spcm_lib_data.h"
#include "../common/spcm_lib_thread.h"
#include "../sb5_file/sb5_file.h"
extern "C" {
#include "global.h"
}
//#include "./file_test/file.h"
//#include "./file_test/start_stop.h"
//#include "check_status.h"
//#include "toggle_status.h"
#include <sys/shm.h>
#include <sys/sem.h>
//#include "sem1.h"
//#include "check_status.h"
//#include "toggle_status.h"
//#include "copy_file.h"
int set_semvalue(int sem_id);
void del_semvalue(int sem_id);
int semaphore_p(int sem_id);
int semaphore_v(int sem_id);
int toggle_status(void);
int check_status(void);
//bool bWorkStop(drv_handle hDrv);
int check_status();
int toggle_status(void);
//pthread_mutex_t copy_mutex;
//pthread_mutex_t spec_mutex;
//pthread_mutex_t status_mutex;
FILE *fpout1;
// ----- this is the global thread flag that defines whether we use the thread or non-thread loop -----
bool g_bThread = false;
key_t shared_creat();
parameters inputParameters();
parameters adc;
//Declaring global variables needed for the shared memory and the semaphore
int shmid,statusId,sem_id,err=0;
//void *shared_memory = (void *)0;
void *status = (void *)0;
unsigned long long int *statusVal,bufferLen,totalCount=0,diff=0,ptr;
char *data;
pthread_t copy_thread,fwrite_thread,spectra_thread,bin_thread,acq_thread;
void *acquisition(void *pa);
struct parameters inputParameters();
/*
**************************************************************************
bDoCardSetup: setup matching the calculation routine
**************************************************************************
*/

bool bDoCardSetup (ST_SPCM_CARDINFO *pstCard)
    {
    int     i;
    int64 llChannelMask;
    
    // set mask for maximal channels
    if (pstCard->lMaxChannels >= 64)
        llChannelMask = 0x01; // -1 is all bits set to 1 = 0xffffffffffffffff
//        llChannelMask = -1; // -1 is all bits set to 1 = 0xffffffffffffffff
    else
//        llChannelMask = ((int64) 1 << pstCard->lMaxChannels) - 1;
      llChannelMask = 0x01;

    // FIFO mode setup, we run continuously and have 16 samples of pre data before trigger event
    // all available channels are activated
    bSpcMSetupModeRecFIFOSingle (pstCard, llChannelMask, 16);
    
    if(adc.externalReference ==1)
    bSpcMSetupClockRefClock (pstCard, MEGA(10), MEGA(adc.samplingRate), true);

// we try to set the samplerate to 1 MHz (M2i) or 20 MHz (M3i) on internal PLL, no clock output
    if (((pstCard->lCardType & TYP_SERIESMASK) == TYP_M2ISERIES) || ((pstCard->lCardType & TYP_SERIESMASK) == TYP_M2IEXPSERIES))
        bSpcMSetupClockPLL (pstCard, MEGA(1), false);
    else
        bSpcMSetupClockPLL (pstCard, MEGA(adc.samplingRate), false);
    printf ("Sampling rate set to %.1f MHz\n", (float) pstCard->lSetSamplerate / 1000000);

    // we set software trigger, no trigger output
    if(adc.externalTrigger == 0)
        bSpcMSetupTrigSoftware (pstCard, false);
    else
        bSpcMSetupTrigExternalLevel (pstCard, SPC_TM_POS | SPC_TM_REARM, 2000, 1000, true, false, 0, true,0);

    // type dependent card setup
    switch (pstCard->eCardFunction)
        {

        // analog acquistion card setup
        case AnalogIn:

            // program all input channels to +/-1 V and 50 ohm termination (if it's available)
            for (i=0; i < pstCard->lMaxChannels; i++)
	      bSpcMSetupPathInputCh (pstCard, i, 1, adc.levels, true, false, false); 
                // bSpcMSetupPathInputCh (pstCard, i, 0, 1000, false, true, true); // setup for M3i card series including new features
            break;

        // digital acquisition card setup
        case DigitalIn:
        case DigitalIO:
            
            // set all input channel groups to 110 ohm termination (if it's available)
            for (i=0; i < pstCard->uCfg.stDIO.lGroups; i++)
                bSpcMSetupDigitalInput (pstCard, i, true);
            break;
        }

    return pstCard->bSetError;
    }



/*
**************************************************************************
Working routine data
**************************************************************************
*/

#define FILENAME "stream_test"

typedef enum E_FILETYPE {eFT_noWrite, eFT_PlainBinary, eFT_SB5_Stream} E_FILETYPE;

struct ST_WORKDATA
    {
    E_FILETYPE  eFileType;
    int64       llSamplesWritten;
    FILE*       hFile;
    char        szFileName[100];
    };



/*
**************************************************************************
Setup working routine
**************************************************************************
*/

bool bWorkInit (void* pvWorkData, ST_BUFFERDATA* pstBufferData)
    {
    ST_WORKDATA* pstWorkData = (ST_WORKDATA*) pvWorkData;

    // setup for the transfer, to avoid overrun we use quite large blocks as this has a better throughput to hard disk
    //    pstBufferData->dwDataBufLen = MEGA_B(128);
    //    pstBufferData->dwDataNotify = MEGA_B(2);
    pstBufferData->dwDataBufLen = MEGA_B(adc.buffer);
    pstBufferData->dwDataNotify = MEGA_B(adc.segment);

    // setup for the work
    pstWorkData->llSamplesWritten = 0;
            //pstWorkData->eFileType =        eFT_PlainBinary;
	            pstWorkData->eFileType =        eFT_noWrite;

    switch (pstWorkData->eFileType)
        {
        case eFT_PlainBinary:
            sprintf (pstWorkData->szFileName, "%s.bin", FILENAME);
            printf ("\nWriting to plan binary file %s\n\n", pstWorkData->szFileName);
            break;

        case eFT_SB5_Stream:
            if (pstBufferData->pstCard->lSetChannels == 1)
                sprintf (pstWorkData->szFileName, "%s.sb5", FILENAME);
            else
                sprintf (pstWorkData->szFileName, "%s.sbs", FILENAME);
            printf ("\nWriting to SBench 5 binary file %s\n\n", pstWorkData->szFileName);
            break;

        case eFT_noWrite:
            printf ("\nno real write, just Simulation\n\n");
        }

    if (pstWorkData->eFileType != eFT_noWrite)
        pstWorkData->hFile = fopen (pstWorkData->szFileName, "w+b");

    // we now have to write the SB5 header if this format has been selected
    if (pstWorkData->hFile && (pstWorkData->eFileType == eFT_SB5_Stream))
        {
        ST_SB5HEAD* pstHeader;
        bool        bReturn;

        pstHeader = pstSB5_AllocHeader (pstBufferData->pstCard->lSetChannels);
        if (!bFillSB5Header (pstBufferData->pstCard, pstHeader, "Test"))
            return false;

        // our pretrigger has been defined to 16 by setup -> that's our x offset
        pstHeader->dXOffset = 16.0 / pstBufferData->pstCard->lSetSamplerate;

        bReturn = bSB5_StoreHeader (pstWorkData->hFile, pstHeader);
        vSB5_FreeHeader (pstHeader);

        return bReturn;
        }
   return ((pstWorkData->hFile != NULL) || (pstWorkData->eFileType == eFT_noWrite));
    
    }



/*
**************************************************************************
bWorkDo: stores data to hard disk
**************************************************************************
*/

bool bWorkDo (void* pvWorkData, ST_BUFFERDATA* pstBufferData)
{
    int flag,i=0;
    ST_WORKDATA*    pstWorkData = (ST_WORKDATA*) pvWorkData;
    uint32          dwWritten;
    //int8 *data;
    char *value;
    unsigned long int temp;
    
    
     
        // write the data and count the samples
         
        
        if(statusVal[0]==0)
        {
        /*if((bufferLen+statusVal[2]-statusVal[1])< pstBufferData->dwDataAvailBytes)
        {
            printf("Race condition reached %ld",bufferLen);
            exit(1);
        }*/    
        temp = (statusVal[1]%bufferLen);
        
        if((bufferLen-temp)>=(pstBufferData->dwDataAvailBytes))
        {
            ptr = (unsigned long int)memcpy((data+temp),pstBufferData->pvDataCurrentBuf,pstBufferData->dwDataAvailBytes);
            value = (char *)pstBufferData->pvDataCurrentBuf; 
            //printf("%d %d %ld ",*(data+temp),*(value),temp);
            statusVal[1]+= pstBufferData->dwDataAvailBytes;
        }
        else
        {
            printf("%ld    ",(bufferLen-temp));
            memcpy(data+temp,pstBufferData->pvDataCurrentBuf,(bufferLen-temp));
            printf("%ld",pstBufferData->dwDataAvailBytes-(bufferLen-temp));
            memcpy(data,pstBufferData->pvDataCurrentBuf+(bufferLen-temp),pstBufferData->dwDataAvailBytes-(bufferLen-temp));
            statusVal[1]+= pstBufferData->dwDataAvailBytes;
            printf("----------");
        }
        //statusVal[2]=pstBufferData->dwDataAvailBytes;
        statusVal[0]=0;
        //printf("\r waiting at smallacq1 ");
            
         
        
        printf("%Ld    %Ld    %Ld  ",statusVal[1],statusVal[2],statusVal[1]-statusVal[2]);
         
        fwrite (pstBufferData->pvDataCurrentBuf, 1, pstBufferData->dwDataAvailBytes, fpout1);
        
        
        //printf("coping started for the next block");
        pstWorkData->llSamplesWritten += pstBufferData->dwDataAvailBytes / pstBufferData->pstCard->lBytesPerSample;
        /*if (dwWritten != pstBufferData->dwDataAvailBytes)
            {
            
           printf("\nData Write error\n");
            return false;
            }*/
	    totalCount = (double) pstBufferData->llDataTransferred / pstBufferData->pstCard->lBytesPerSample / MEGA_B(1);
        // announce the number of data that has been written
        /*printf ("\r%.2f MSamples (sum) written to %s", 
            (double) pstBufferData->llDataTransferred / pstBufferData->pstCard->lBytesPerSample / MEGA_B(1), 
            pstWorkData->szFileName);*/
	//flag = start_stop();
	//printf("%d \n",flag);
	 
	
	if ((double) pstBufferData->llDataTransferred / pstBufferData->pstCard->lBytesPerSample / MEGA_B(1) >= ((adc.samplingRate)*(adc.timeOfOberservation)))
           {
	     //toggle_status();
             //flag =0;
             //free(pstBufferData->pvDataCurrentBuf);
	     statusVal[4]=2;
        } 

    // simulation: just count the data
    //if (pstWorkData->eFileType == eFT_noWrite)
      //  printf ("\r%.2f MSamples (sum) transferred", 
        //    (double) pstBufferData->llDataTransferred / pstBufferData->pstCard->lBytesPerSample / MEGA_B(1));


    
    //return true;
    }
    return true;
}




/*
**************************************************************************
vWorkClose: Close the work and clean up
**************************************************************************
*/

void vWorkClose (void* pvWorkData, ST_BUFFERDATA* pstBufferData)
    {
    ST_WORKDATA* pstWorkData = (ST_WORKDATA*) pvWorkData;

    if (pstWorkData->eFileType != eFT_noWrite)
        if (pstWorkData->hFile)
            {
            if (pstWorkData->eFileType == eFT_SB5_Stream)
                bSB5_UpdateSamples(pstWorkData->hFile, (int32) pstWorkData->llSamplesWritten);

            fclose (pstWorkData->hFile);
            }
    }


    

/*
**************************************************************************
main 
**************************************************************************
*/

void *acquisition(void *pa)
    {
    
   
    char                szBuffer[1024];     // a character buffer for any messages
    ST_SPCM_CARDINFO    stCard;             // info structure of my card
    ST_BUFFERDATA       stBufferData;       // buffer and transfer definitions
    ST_WORKDATA         stWorkData;         // work data for the working functions

    // ------------------------------------------------------------------------
    // init card number 0 (the first card in the system), get some information and print it
    
    statusId= shmget(1111,(5*sizeof(unsigned long long int)),0666|IPC_CREAT);
    if (statusId == -1) 
    {
        fprintf(stderr, "shmget buffer Available failed in the main program\n");
        exit(0);
    }
    status = shmat(statusId, (void *)0, 0);
    if (status == (void *)-1) 
    {
        fprintf(stderr, "shmat failed in the Buffer Available main program\n");
        exit(1);
    }
    statusVal = (unsigned long long int *) status;
    
    statusVal[0]=0;
    statusVal[1]=0;
    statusVal[2]=0;
    statusVal[3]=0;
    statusVal[4]=0;
    
    if (bSpcMInitCardByIdx (&stCard, 0))
        printf (pszSpcMPrintCardInfo (&stCard, szBuffer, sizeof (szBuffer)));
    else
        printf("%d",nSpcMErrorMessageStdOut (&stCard, "Error: Could not open card\n", true));


    // check whether we support this card type in the example
    if ((stCard.eCardFunction != AnalogIn) && (stCard.eCardFunction != DigitalIn) && (stCard.eCardFunction != DigitalIO))
        printf("%d",nSpcMErrorMessageStdOut (&stCard, "Error: Card function not supported by this example\n", false));


    // ------------------------------------------------------------------------
    // do the card setup, error is routed in the structure so we don't care for the return values
    if (!stCard.bSetError)
        bDoCardSetup (&stCard);


    // ------------------------------------------------------------------------
    // setup the data transfer thread and start it, we use atimeout of 5 s in the example
    memset (&stBufferData, 0, sizeof(stBufferData));
    stBufferData.pstCard =      &stCard;
    stBufferData.bStartCard =   true;
    stBufferData.bStartData =   true;
    stBufferData.lTimeout =     5000;
    //printf("adress of the card %f",(double)stBufferData.pstCard);
    // start the threaded version if g_bThread is defined
    if (!stCard.bSetError && g_bThread)
        vDoThreadMainLoop (&stBufferData, &stWorkData, bWorkInit, bWorkDo, vWorkClose, bKeyAbortCheck);

    // start the unthreaded version with a smaller timeout of 100 ms to gain control about the FIFO loop
    stBufferData.lTimeout =     100;
    if (!stCard.bSetError && !g_bThread)
        vDoMainLoop (&stBufferData, &stWorkData, bWorkInit, bWorkDo, vWorkClose, bKeyAbortCheck);


    // ------------------------------------------------------------------------
    // print error information if an error occured
    if (stCard.bSetError)
        printf("%d",nSpcMErrorMessageStdOut (&stCard, "An error occured while programming the card:\n", true));


    // clean up and close the driver
    vSpcMCloseCard (&stCard);

    
    }

int main()
    {
        //pthread_t copy_thread,fwrite_thread,spectra_thread,bin_thread,acq_thread;
        //pthread_attr_t thread_attr1,thread_attr2;
        //FILE *fp;
        int err,bin=1,start_id;
        //int q;
        /* open the file */
        fpout1=fopen64("stream.dat","wb");
        
        start_id = semget((key_t)3333, 1, 0666 | IPC_CREAT);
        if (!set_semvalue(start_id)) 
        {
           fprintf(stderr, "Failed to initialize semaphore\n");
           exit(EXIT_FAILURE);
        }
        err=semaphore_p(start_id);
        if(err !=1)
        {
            printf("semaphore-p error copy_file \n");
        } 
        while(1)
        {
        err=semaphore_p(start_id);
        if(err !=1)
        {
            printf("semaphore-p error copy_file \n");
        } 
               
        adc = inputParameters();
        fflush(stdout);
        bufferLen = adc.bufferLen;
        data = (char*) malloc(adc.bufferLen);
        /*if(adc.binary==0)
        {
        err = pthread_mutex_init(&copy_mutex , NULL);
        if(err !=0)
        {
            perror("buffer_mutex initialization failed");
            exit(1);
        }
        err = pthread_mutex_init(&spec_mutex , NULL);
        if(err !=0)
        {
            perror("buffer_mutex initialization failed");
            exit(1);
        }*/
        
        /*err = pthread_mutex_init(&spec_mutex , NULL);
        if(err !=0)
        {
            perror("buffer_mutex initialization failed");
            exit(1);
        }*/
        
        err = pthread_create(&acq_thread, NULL, acquisition, NULL);
        if (err != 0) 
        {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        
        /*err = pthread_create(&copy_thread, NULL, copy_file, NULL);
        if (err != 0) 
        {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        
        err = pthread_create(&fwrite_thread, NULL, writeooty2fb, NULL);
        if (err != 0) 
        {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        
        err = pthread_create(&spectra_thread, NULL, spectraCal, NULL);
        if (err != 0) 
        {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        */
        
        
       
        
        
        /*err = pthread_join(copy_thread, NULL);
        if (err != 0) 
        {
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }*/
        printf("copy_thread joined\n");
        err = pthread_join(acq_thread, NULL);
        if (err != 0) 
        {
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
        printf("acq_thread joined\n");
        /*err = pthread_join(spectra_thread, NULL);
        if (err != 0) 
        {
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
        printf("spectra_thread joined\n");
        err = pthread_join(fwrite_thread, NULL);
        if (err != 0) 
        {
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
        printf("fwrite_thread joined\n");
        }
        else if(adc.binary == 1)
        {
            err = pthread_create(&bin_thread, NULL, bin_file, NULL);
            if (err != 0) 
            {
                perror("Thread creation failed");
                exit(EXIT_FAILURE);
            }
            //printf("Input some text. Enter ‘end’ to finish\n");
        
            err = pthread_join(bin_thread, NULL);
            if (err != 0) 
            {
                perror("Thread join failed");
                exit(EXIT_FAILURE);
            }
            printf("Thread joined\n");
        }*/
        free(data);
        
        }   
        return 0;
    }
    
    
    
    
    
     
parameters inputParameters()
{
    //int linecount =0;
    parameters initial;
          FILE *fp;
      char a[100],b[100];
      int i,p;
   
      /* open the file */
      fp = fopen("/scratch1/arun/c_cpp1/version5.1/init.dat", "r");
      if (fp == NULL) {
         printf("I couldn't open init.dat for reading.\n");
         return initial;
      }
   
      fscanf(fp, "%s %s \n", a , b);
      printf("%s  %s\n", a, b);
      strcpy(initial.nameOfSource,b);
      
      fscanf(fp, "%s %s \n",a,b);
      printf("%s   %s\n",a,b);
      strcpy(initial.location,b);
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s  %d\n", a, i);
      initial.samplingRate = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s  %d\n", a, i);
      initial.timeOfOberservation = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s  %d\n", a, i);
      initial.levels = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.lpfFlag = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.externalReference = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.externalTrigger = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.edgeRise = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.termination = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.nacc = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.binary = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.bufferLen = i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.segment = (unsigned long long )i;
      
      fscanf(fp, "%s %d\n", a, &i);
      printf("%s %d\n", a, i);
      initial.buffer = i;
      
      /* close the file */
      fclose(fp);
      
      
   
 //cout <<initial.termination<<endl;
    return initial;
}

