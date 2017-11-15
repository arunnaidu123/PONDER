#ifndef CHECK_STATUS_H
#define CHECK_STATUS_H

#include <stdio.h>
#include <stdlib.h>
int check_status()
{
    int c,g;
    char f[1];
    FILE *in;
    in = fopen("../copy_status.dat","r");
    c = getc(in);
    f[0] =(char)c;
    g= atoi(f);
    fclose(in);
    return g;
}
#endif
