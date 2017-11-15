#include <stdio.h>
#include <stdlib.h>
   
   int main(void)
   {
      FILE *fp;
      int i=0;
   
      /* open the file */
      fp = fopen("start_stop.dat", "w");
      if (fp == NULL) {
         printf("I couldn't open start_stop.dat file.\n");
         exit(0);
      }
   
      /* write to the file */
      fprintf(fp, "%d",i);
   
      /* close the file */
      fclose(fp);
   
      return 0;
   }
