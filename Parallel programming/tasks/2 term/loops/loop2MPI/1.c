#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define ISIZE 100
#define JSIZE 100

char print_into_file(const char* filename, const char* descriptor,
                    const double* array, const unsigned long int array_size,
                    const unsigned long int block, const int rank)
{
    char error = 0;
    
    FILE* file = fopen(filename, descriptor);
    if (file == NULL)
    {
        printf("Unable to open file in %d process.\n", rank);
        fflush(stdout);
        error = 1;
    }
    else
    {
        for(unsigned long int iterator = 0; iterator < array_size; ++iterator)
        {
            unsigned int i = iterator / JSIZE + rank*block, j = iterator % JSIZE;
            
            if (i >= ISIZE - 3 || j < 2)
                fprintf(file, "%lf ", array[iterator]);
            else
                fprintf(file, "%lf ", sin(0.00001*array[iterator]));
            
            if ((iterator + 1) % JSIZE == 0)
                fprintf(file, "\n");
        }
        
        fclose(file);
    }
    
    return error;
}

int main(int argc, char** argv)
{
    int rank = 0, amount_of_cores = 0, root = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &amount_of_cores);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    unsigned long int block = ISIZE / amount_of_cores;
    unsigned long int number_of_lines = rank + 1 != amount_of_cores ? block : block + ISIZE % amount_of_cores;
    
    double* matrix_part = (double*) calloc (number_of_lines*JSIZE, sizeof(double));
    
    double start_time = 0;
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == root)
        start_time = MPI_Wtime();
    
    for (unsigned long int iterator = 0; iterator < number_of_lines*JSIZE; ++iterator)
    {
        unsigned int i = iterator / JSIZE + rank*block, j = iterator % JSIZE;
        
        if (j < 2 || i >= ISIZE - 3)
            matrix_part[iterator] = (10*i + j);
        else
            matrix_part[iterator] = (10*(i + 3) + j - 2);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == root)
        printf("Execution time: %lf sec.\n", MPI_Wtime() - start_time);
    
    char buf_recv = 0, buf_send = 0;
    
    /*file writing*/
    if (rank == root)
    {
        buf_send = print_into_file("matrix.txt", "w",  matrix_part,
                                   number_of_lines*JSIZE, block, rank);
        
        if (rank + 1 != amount_of_cores)
            MPI_Send(&buf_send, 1, MPI_CHAR, rank + 1, rank,
                     MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(&buf_recv, 1, MPI_CHAR, rank - 1,
                     rank - 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        buf_send = buf_recv;
        
        if (!buf_recv)
            buf_send = print_into_file("matrix.txt", "a",  matrix_part,
                                       number_of_lines*JSIZE, block, rank);

        if (rank + 1 < amount_of_cores)
            MPI_Send(&buf_send, 1, MPI_CHAR, rank + 1, rank, MPI_COMM_WORLD);
    }
    
    free(matrix_part);
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    
    return 0;
}
