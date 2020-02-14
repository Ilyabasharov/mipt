#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ISIZE 100
#define JSIZE 100

int main(int argc, char **argv)
{
    double a[ISIZE][JSIZE]; int i, j;
    FILE *ff;
    
    for (i=0; i<ISIZE; i++)
        for (j=0; j<JSIZE; j++)
            a[i][j] = 10*i + j;
    
    for (i=0; i<ISIZE-3; i++)
        for (j=2; j<JSIZE; j++)
            a[i][j] = sin(0.00001*a[i+3][j-2]);

    ff = fopen("result.txt", "w");
    
    for(i=0; i<ISIZE; i++)
    {
        for (j=0; j<JSIZE; j++)
            fprintf(ff, "%f ", a[i][j]);
        fprintf(ff, "\n");
    }
    fclose(ff);
    
    return 0;
}
