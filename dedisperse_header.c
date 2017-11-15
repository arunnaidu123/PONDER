#include "dedisperse.h"
//#include "outputfile.h" 
void dedisperse_header(FILE *outptr) /* includefile */
{
  /* go no further here if not interested in header parameters */
  
  //output = outptr;
  if (headerless) return;

  /* broadcast the header parameters to the output stream */
   
  send_string("HEADER_START", outptr);

  printf("you are hereeeeeeeeeeee %d \n",nbands);
  fflush(stdout);
  if (!strings_equal(source_name,"")) {
    send_string("source_name", outptr);
    send_string(source_name, outptr);
  }
  send_int("telescope_id",telescope_id, outptr); 
  send_int("machine_id",machine_id, outptr);
  send_coords(src_raj,src_dej,az_start,za_start, outptr);
  if (refdm == -1.0) refdm=userdm;
  if (nbands==1) {
    send_int("data_type",2, outptr);
    send_double("refdm",refdm, outptr);
  } else {
    send_int("data_type",6, outptr);
    send_double("refdm",refdm, outptr);
    send_double("foff",foff*(double)(nchans/nbands), outptr);
  }

  if ( (fch1 == 0.0) && (frequency_table[0] == 0.0) ) 
    printf("help... missing frequency information!");

  if (fch1 == 0.0) 
    send_double("fch1",frequency_table[0], outptr);
  else
    send_double("fch1",fch1, outptr);

  send_int("barycentric",barycentric, outptr);
  send_int("nchans",nbands, outptr);
  send_int("nbits",nobits, outptr);  
  send_double ("tstart",tstart, outptr); 
  send_double("tsamp",tsamp, outptr);
  send_int("nifs",nifs, outptr);
  send_string("HEADER_END", outptr);
}
