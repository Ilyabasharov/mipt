#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int num_of_threads = atoi(argv[1]);
	omp_set_num_threads(num_of_threads);
	long double* array = (long double*) calloc (num_of_threads, sizeof(long double));
    
	#pragma omp parallel for ordered shared(array)
    for(unsigned long int i = 1; i <= 1000000000; ++i)
        array[omp_get_thread_num()] += 1.0 / i;

	double sum = 0;
	#pragma omp parallel for ordered shared(array) reduction(+:sum)
	for (int i = num_of_threads - 1; i > -1; i--)
		sum += array[i];

	printf("Harm row sum is %.20lf\n", sum);
    free(array);
    
	return 0;
}
