#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<sys/timeb.h>

void datenow(char *filename, char *date)
{

int yyyy, ddd, hh, mm, ss;

  time_t now;
  struct tm *d;
  struct tm *ptr_ts;
  time(&now);
  d = localtime(&now);
  time_t raw_time;
  time ( &raw_time );
  ptr_ts = gmtime ( &raw_time );

	
yyyy = d->tm_year+1900;	
ddd = (ptr_ts->tm_yday);
hh = (ptr_ts->tm_hour);
mm = (ptr_ts->tm_min);
ss = (ptr_ts->tm_sec);
ss = 0;
sprintf(filename,"%d%d%02d:%02d:%02d",yyyy,ddd,hh,mm+1,ss);
sprintf(date,"%d %d %02d:%02d:%02d",yyyy,ddd,hh,mm+1,ss);
return;		
}	


