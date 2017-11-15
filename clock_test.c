#include <stdio.h>
#include <time.h>
     
int main()
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    sleep(100);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\n %f",cpu_time_used);
}
