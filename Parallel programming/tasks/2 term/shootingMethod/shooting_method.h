#ifndef __SHOOTING_METHOD__
#define __SHOOTING_METHOD__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <mpi.h>

#define N 10000 // number of points

#define ROOT_THREAD 0
#define MAX_SIZE 12

// Initial conditions
#define F(t) sin(t)
#define T (3 * M_PI)
#define X (-2.0)

typedef struct {
    double x;
    double v;
} pairs;

void writeResults(char *outputFile, double *source, double dt);
void shoot();

#endif
