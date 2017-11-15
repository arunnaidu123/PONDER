#include <stdio.h>
#include "sigproc.h"
//#include "outputfile.h"
//FILE *input, *output;
int swapout;
void send_string(char *string, FILE *output) /* includefile */
{
  int len;
  len=strlen(string);
  if (swapout) swap_int(&len);
  fwrite(&len, sizeof(int), 1, output);
  if (swapout) swap_int(&len);
  fwrite(string, sizeof(char), len, output);
  /*fprintf(stderr,"%s\n",string);*/
}

void send_float(char *name,float floating_point, FILE *output) /* includefile */
{
  send_string(name, output);
  if (swapout) swap_float(&floating_point);
  fwrite(&floating_point,sizeof(float),1,output);
  /*fprintf(stderr,"%f\n",floating_point);*/
}

void send_double (char *name, double double_precision, FILE *output) /* includefile */
{
  send_string(name, output);
  if (swapout) swap_double(&double_precision);
  fwrite(&double_precision,sizeof(double),1,output);
  /*fprintf(stderr,"%f\n",double_precision);*/
}

void send_int(char *name, int integer, FILE *output) /* includefile */
{
  send_string(name, output);
  if (swapout) swap_int(&integer);
  fwrite(&integer,sizeof(int),1,output);
  /*fprintf(stderr,"%d\n",integer);*/
}

void send_long(char *name, long integer, FILE *output) /* includefile */
{
  send_string(name, output);
  if (swapout) swap_long(&integer);
  fwrite(&integer,sizeof(long),1,output);
  /*fprintf(stderr,"%ld\n",integer);*/
}

void send_coords(double raj, double dej, double az, double za, FILE *output) /*includefile*/
{
  if ((raj != 0.0) || (raj != -1.0)) send_double("src_raj",raj, output);
  if ((dej != 0.0) || (dej != -1.0)) send_double("src_dej",dej, output);
  if ((az != 0.0)  || (az != -1.0))  send_double("az_start",az, output);
  if ((za != 0.0)  || (za != -1.0))  send_double("za_start",za, output);
}
