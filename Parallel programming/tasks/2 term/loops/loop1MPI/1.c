#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define ISIZE 10
#define JSIZE 15

int print_into_file(const char* filename, const char* descriptor,
                    const int block, const int rank, const int size)
{
    int error = 0;
    
    FILE* file = fopen(filename, descriptor);
    if (file == NULL)
    {
        printf("Unable to open file in %d process.\n", rank);
        fflush(stdout);
        error = -1;
    }
    else
    {
        unsigned long int processing_size = rank + 1 != size ? block*JSIZE : (block + ISIZE % size)*JSIZE;
        for(unsigned long int iterator = 0; iterator < processing_size; ++iterator)
        {
            unsigned long int i = iterator / JSIZE + rank*block, j = iterator % JSIZE;
            fprintf(file, "%.6lf ", sin(0.00001*(10*i + j)));
            
            if ((iterator + 1) % JSIZE == 0)
                fprintf(file, "\n");
        }
        
        fclose(file);
    }
    
    return error;
}

int main(int argc, char** argv)
{
    int rank = 0, size = 0, root = 0, block = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    block = ISIZE / size;
    
    double start_time = 0;
	int buf_recv = 0, buf_send = 0;
    MPI_Barrier(MPI_COMM_WORLD);
	if (rank == root)
	{
        start_time = MPI_Wtime();
        buf_send = print_into_file("matrix.txt", "w", block, rank, size);
        if (rank + 1 != size)
            MPI_Send(&buf_send, 1, MPI_INT, rank + 1, rank,
                     MPI_COMM_WORLD);
	}
	else
	{
        MPI_Recv(&buf_recv, 1, MPI_INT, rank - 1,
                 rank - 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
        buf_send = buf_recv;
    
        if (buf_recv != -1)
            buf_send = print_into_file("matrix.txt", "a", block, rank, size);

        if (rank + 1 < size)
            MPI_Send(&buf_send, 1, MPI_INT, rank + 1, rank, MPI_COMM_WORLD);
	}
    
	MPI_Barrier(MPI_COMM_WORLD);
    if (rank == root)
        printf("Execution time: %lf sec.\n", MPI_Wtime() - start_time);
    
	MPI_Finalize();

	return 0;
}
