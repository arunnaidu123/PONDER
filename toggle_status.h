#ifndef TOGGLE_STATUS_H
#define TOGGLE_STATUS_H
#include <stdio.h>
#include <stdlib.h>
   
   int toggle_status(void)
   {
      FILE *fp;
      int i=1,c,g;
      char f[1];
      /* open the file */
      fp = fopen("copy_status.dat", "r");
      if (fp == NULL) 
      {
          printf("I couldn't open copy_status.dat file.\n");
          exit(0);
      }
    //printf("I am here");
    c = getc(fp);
    f[0] =(char)c;
    g= atoi(f);
    fclose(fp);
    fp = fopen("copy_status.dat", "w");
    if (fp == NULL) 
    {
        printf("I couldn't open copy_status.dat file.\n");
        exit(0);
    }
    if(g==1)
    {
        i=0;
        fprintf(fp,"%d",i);
    }
    if(g!=1)
    {
        i=1;
        fprintf(fp,"%d",i);
    }
    
    
    fclose(fp);
   
    return 0;
   }
#endif
