#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv)
{
	int threads_num = atoi(argv[1]);
    unsigned long int elements = strtoul(argv[2], NULL, 10);
	omp_set_num_threads(threads_num);
    
	double* weights_array = (double*) calloc (threads_num, sizeof(double));
	double* sum_array = (double*) calloc (threads_num, sizeof(double));
    
	int part = elements / threads_num;
    
    double current = 1.0;
	#pragma omp parallel for shared(sum_array, part, elements, weights_array) firstprivate(current)
	for(unsigned long int i = 0; i < elements; i++)
	{
        if (i == part * (omp_get_thread_num() + 1) - 1)
        {
            weights_array[omp_get_thread_num()] = current;
            if (omp_get_thread_num() == 0)
                weights_array[0] = 1;
        }
        current = current / (1 + i);
        sum_array[omp_get_thread_num()] += current;
	}
    
    current = 1.0;
    for(int i = 0; i < threads_num; i++)
    {
        current *= weights_array[i];
        weights_array[i] = current;
    }
    
	double sum = 0.0;
    
	#pragma omp parallel for ordered shared(sum_array, weights_array) reduction(+:sum)
	for(int i = threads_num - 1; i > -1; i--)
		sum += sum_array[i]*weights_array[i];
    
    free(weights_array);
    free(sum_array);
    
	printf("exit : %.35lf\n", sum + 1);
	printf("value: %.35le\n", M_E);

	return 0;
}
