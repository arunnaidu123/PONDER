/*
*****************************************************************
	   		   MJDNOW.C							
*****************************************************************

A program to calculate the current MJD

kkma on 02 08 2012
Last modified on 04 01 2013 kkma
*****************************************************************
*/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<sys/timeb.h>

long double mjdnow()
{

int dd, mm, yy,mjdd;
double a, b, c, e, f, jd;
long double hr,min,sec,mjd;
float mjd5;

    					
  /************** declaration of struct tm***************/
  //struct tm
  //{
  //int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  //int tm_min;			/* Minutes.	[0-59] */
  //int tm_hour;			/* Hours.	[0-23] */
  //int tm_mday;			/* Day.		[1-31] */
  //int tm_mon;			/* Month.	[0-11] */
  //int tm_year;			/* Year	- 1900.  */
  //int tm_wday;			/* Day of week.	[0-6] */
  //int tm_yday;			/* Days in year.[0-365]	*/
  //int tm_isdst;			/* DST.		[-1/0/1]*/
  //
  //#ifdef	__USE_BSD
  //long int tm_gmtoff;		/* Seconds east of UTC.  */
  // __const char *tm_zone;	/* Timezone abbreviation.  */
  //#else
  //long int __tm_gmtoff;		/* Seconds east of UTC.  */
  //__const char *__tm_zone;	/* Timezone abbreviation.  */
  //#endif
  //};/**************End of Declaration***************/

  time_t now;
  struct tm *d;
  struct tm *ptr_ts;
    
  time(&now);
  d = localtime(&now);
  time_t raw_time;
  time ( &raw_time );
  
  ptr_ts = gmtime ( &raw_time );
  hr = (ptr_ts->tm_hour)/24.0;
  min = (ptr_ts->tm_min+1)/1440.0;
	
    yy = d->tm_year+1900;
    mm = d->tm_mon+1;
    dd = d->tm_mday;

	if (mm==1){
		yy -= 1;
		mm += 12;
	} else if(mm==2){
		yy -= 1;
		mm += 12;
	}

	a = yy/100.0; 
	b = a/4.0; 
	c = 2.0-a+b;
	e = 365.25*(yy+4716.0); 
	f = 30.6001*(mm+1.0);
	jd = c+dd+e+f-1525.0;
	int count=0;
	if (yy%4==0){
		
		count=1;
	} else if (yy%400==0){
		count=1;
		
	} /*else if (yy%100!=0){
		jd ++;
	}*/
	if (count==1 && mm==14 && dd==29){
		jd -=1;
	}
	if (mm==1 && dd==28){
		jd+=1;
	}
	if (count!=1 && mm==3){
		jd =jd;
	}
	if (mm==12 && count!=1){
		jd -= 1;
	}
	if ((yy+1)%(4)==0 && count!=1 && mm==3){
	jd -=1;
	}
	
mjd  = (int)(jd-2400000.0)+hr+min;
mjd5 = (int)((double)jd-2400000.0)+hr+min;
mjdd = (int)(jd-2400000.0);
printf("%1.12Lf",mjd);
//printf("%1.6Lf",mjd);
//printf("%d",mjdd);
return ((long double)((int)jd+hr+min-2400000.0));
}	


