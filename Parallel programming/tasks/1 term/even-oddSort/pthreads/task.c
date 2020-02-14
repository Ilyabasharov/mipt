#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

void swap(int* a, int *b)
{
    int x = *a;
    *a = *b;
    *b = x;
}

struct transmit
{
  int *a;
  int block;
  int i;
  int id;
};

void* comp(void *param)
{
    struct transmit *tr = (struct transmit*) param;
    for(int i = tr -> block * tr -> id + tr -> i; i < tr -> block * (tr -> id + 1) + tr -> i; i += 2)
        if (tr -> a[i] > tr -> a[i+1])
            swap(&tr -> a[i], &tr -> a[i+1]);
    return 0;
}

int main(int argc, char** argv)
{
    int pt_n = atoi(argv[1]);
	pthread_t tid[pt_n];
	int n = 0, *a = NULL, start = 0, end = 0;
	FILE *fnum;
	FILE *fres;
	struct transmit tr[pt_n];
  
	fnum = fopen("numbers.txt", "r");
	fres = fopen("answer.txt", "w");
	fscanf(fnum, "%d", &n);

	a = (int*) calloc (n, sizeof(int));

	for(int i = 0; i < n; i++)
		fscanf(fnum, "%d", &a[i]);
    
	start = clock();
    int i, z, j = 0;
    
    int reminder = (n / pt_n) % 2;
    
	for(i = 0; i < pt_n - 1; i++)
	{
		tr[i].a = a;
		tr[i].id = i;
		tr[i].block = n / pt_n + reminder;
	}

	for(i = 0; i < n; i++)
	{
        for(z = 0; z < pt_n - 1; z++)
        {
            tr[z].i = i % 2;
            pthread_create(&tid[z], NULL, comp, &tr[z]);
        }
        
        for(j = tr[0].block*(pt_n - 1) + (i % 2); j < n - 1; j += 2)
            if (a[j] > a[j+1])
                swap(&a[j], &a[j+1]);
        
        for(z = 0; z < pt_n - 1; z++)
            pthread_join(tid[z], NULL);
    }

	fclose(fnum);
	end = clock();
	printf("Elapsed time: %.8f\n", ((float)end - start) / 10000);
    
	for (int i = 0; i < n; i++)
		fprintf(fres, "%d ", a[i]);
    
	free(a);
	fclose(fres);
    
	return 0;
}
