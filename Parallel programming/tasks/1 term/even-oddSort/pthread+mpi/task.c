#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <mpi.h>
#include <limits.h>

struct transmit {
  int *a;
  int block;
  int i;
  int id;
}typedef transmit;

struct data
{
    int value;
    int id_proc;
}typedef data;

int* read_from_file(int *n_ptr, int rank)
{
    int* array = NULL;
    
    if (rank == 0)
    {
        FILE* file = fopen("numbers.txt", "r");
        fscanf(file, "%d", n_ptr);
        
        array = (int*) calloc (*n_ptr, sizeof(int));
        
        for(int i = 0; i < *n_ptr; i++)
            fscanf(file, "%d", &array[i]);
        
        fclose(file);
    }
    
    MPI_Bcast(n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    return array;
}
 
void write_to_file(int n, int *array, int rank)
{
    if (rank == 0)
    {
        FILE* file = fopen("answer.txt", "w");
        
        for(int i = 0; i < n; i++)
            fprintf(file, "%d ", array[i]);
        
        fclose(file);
        free(array);
    }
}

void swap(int* a, int *b)
{
     int x = *a;
      *a = *b;
      *b = x;
}

void* comp(void *param)
{
    transmit *tr = (transmit*) param;
    
    for (int i = tr -> block * tr -> id + tr -> i; i < tr -> block * (tr -> id + 1) + tr -> i; i += 2)
        if (tr -> a[i] > tr -> a[i + 1])
            swap(&tr -> a[i], &tr -> a[i + 1]);
    
    return 0;
}

int* merge_sort(int rank, int part, int* array)
{
    data in = {0, rank}, out;
    int* buffer = NULL, sum = 0, local_iterator = 0;
    
    MPI_Allreduce(&part, &sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    if (rank == 0)
        buffer = (int*) calloc (sum, sizeof(int));
    
    for (int i = 0; i < sum; i++)
    {
        in.value = local_iterator >= part ? INT_MAX : array[local_iterator];
        
        MPI_Allreduce(&in, &out, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);
        
        if (out.id_proc == rank)
            local_iterator ++;
        
        if (rank == 0)
            buffer[i] = out.value;
    }
    
    return buffer;
}

void even_odd_sort(int *array, int pt_n, int n)
{
    pthread_t tid[32];
    struct transmit tr[32];

    for(int i = 0; i < pt_n - 1; i++)
    {
        tr[i].a = array;
        tr[i].id = i;
        tr[i].block = n / pt_n + (((n / pt_n) % 2 == 0) ? 0 : 1);
    }
    
    for (int i = 0; i < n; i++)
    {
        for (int z = 0; z < pt_n - 1; z++)
        {
            tr[z].i = i % 2;
            pthread_create(&tid[z], NULL, comp, &tr[z]);
        }
        
        for (int j = tr[0].block*(pt_n - 1) + (i % 2); j < n - 1; j += 2)
            if (array[j] > array[j+1])
                swap(&array[j], &array[j+1]);
        
        for (int z = 0; z < pt_n - 1; z++)
            pthread_join(tid[z], NULL);
    }
}

int rankToBlocks(int rank, int amount_of_cores, int n)
{
    int part = n / amount_of_cores;
    return rank ? part : part + n % amount_of_cores;
}

int main(int argc, char** argv)
{
    int pt_n = atoi(argv[1]),
        rank = 0, n = 0, amount_of_cores = 0;
    
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &amount_of_cores);
    
    int* array_data = read_from_file(&n, rank);
    
 	int start = clock(),
        part = rankToBlocks(rank, amount_of_cores, n),
        *displs = NULL, *rcounts = NULL,
        *buffer = (int*) calloc (part, sizeof(int));
    
    if (rank == 0)
    {
        displs = (int*) calloc (amount_of_cores, sizeof(int));
        rcounts = (int*) calloc (amount_of_cores, sizeof(int));
        
        rcounts[0] = part;
        
        for (int i = 1; i < amount_of_cores; ++i)
        {
            rcounts[i] = n / amount_of_cores;
            displs[i] = displs[i - 1] + rcounts[i - 1];
        }
    }
    
    MPI_Scatterv(array_data, rcounts, displs, MPI_INT, buffer, part, MPI_INT, 0, MPI_COMM_WORLD);
    
	even_odd_sort(buffer, pt_n, part);
    int* result = merge_sort(rank, part, buffer);
    
 	int end = clock();
    
    free(buffer);
    write_to_file(n, result, rank);
    
    if (rank == 0)
    {
        free(array_data);
        free(displs);
        free(rcounts);
        
        printf("Elapsed time: %.8f\n", (end - start) / 10000.0);
    }
    
	MPI_Finalize();
    
	return 0;
}
