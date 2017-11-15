#include "filterbank.h"

double tstart;

void filterbank_header(FILE *outptr) /* includefile */
{
  int i,j;
  //output=outptr;
  if (obits == -1) obits=nbits;
  /* go no further here if not interested in header parameters */
  if (headerless) return;
  /* broadcast the header parameters to the output stream */
  if (machine_id != 0) {
    send_string("HEADER_START", outptr);
    send_string("rawdatafile", outptr);
    send_string(inpfile, outptr);
    if (!strings_equal(source_name,"")) {
      send_string("source_name", outptr);
      send_string(source_name, outptr);
    }
    send_int("machine_id",machine_id, outptr);
    send_int("telescope_id",telescope_id, outptr);
    send_coords(src_raj,src_dej,az_start,za_start, outptr);
    if (zerolagdump) {
      /* time series data DM=0.0 */
      send_int("data_type",2, outptr);
      refdm=0.0;
      send_double("refdm",refdm, outptr);
      send_int("nchans",1, outptr);
    } else {
      /* filterbank data */
      send_int("data_type",1, outptr);
      send_double("fch1",fch1, outptr);
      send_double("foff",foff, outptr);
      send_int("nchans",nchans, outptr);
    }
    /* beam info */
    send_int("nbeams",nbeams, outptr);
    send_int("ibeam",ibeam, outptr);
    /* number of bits per sample */
    send_int("nbits",obits, outptr);
    /* start time and sample interval */
    send_double("tstart",tstart+(double)start_time/86400.0, outptr);
    send_double("tsamp",tsamp, outptr);
    if (sumifs) {
      send_int("nifs",1, outptr);
    } else {
      j=0;
      for (i=1;i<=nifs;i++) if (ifstream[i-1]=='Y') j++;
//      if (j==0) error_message("no valid IF streams selected!");
      send_int("nifs",j, outptr);
    }
    send_string("HEADER_END", outptr);
  }
  
}
