#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv)
{
	int num_of_threads = atoi(argv[1]), counter = 0;
	omp_set_num_threads(num_of_threads);
    
	#pragma omp parallel shared(counter)
	{
        int index = omp_get_thread_num();
		while (1)
			if (index == counter)
			{
				printf("Hello from %d process.\n", index);
				counter += 1;
				break;
			}
	}
    
	return 0;
}
