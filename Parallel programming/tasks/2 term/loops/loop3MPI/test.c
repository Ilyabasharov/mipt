#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ISIZE 10
#define JSIZE 10

int main(int argc, char **argv)
{
    double a[ISIZE][JSIZE]; int i, j;
    
    for (i=0; i<ISIZE; i++)
        for (j=0; j<JSIZE; j++)
            a[i][j] = 10*i+j;
    
    for (i=2; i<ISIZE; i++)
        for (j = 0; j<JSIZE-3; j++)
            a[i][j] = sin(0.00001*a[i-2][j+3]);

    FILE *ff = fopen("result.txt","w");
    for(i=0; i < ISIZE; i++)
    {
        for (j=0; j < JSIZE; j++)
            fprintf(ff, "%0*.*lf ", 3, 4, a[i][j]);
        fprintf(ff, "\n");
    }
    fclose(ff);
    
    return 0;
}
