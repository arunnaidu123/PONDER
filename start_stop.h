#ifndef START_STOP_H
#define START_STOP_H

#include <stdio.h>
#include <stdlib.h>
int start_stop()
{
    int c,g;
    char f[0];
    FILE *in;
    in = fopen("start_stop.dat","r");
    c = getc(in);
    f[0] =(char)c;
    g= atoi(f);
    printf("\n %d \n",g);
    fclose(in);
    return g;
}
#endif
