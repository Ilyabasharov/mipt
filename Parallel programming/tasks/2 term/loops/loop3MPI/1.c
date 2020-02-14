#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define ISIZE 10
#define JSIZE 10

void printIntoFile(const char* filename, const char* descriptor,
                     double* array)
{
    FILE* file = fopen(filename, descriptor);
    if (file == NULL)
    {
        printf("Unable to open file in root process.\n");
        fflush(stdout);
        return;
    }
    else
    {
        for (unsigned int iterator = 0; iterator < ISIZE*JSIZE; ++iterator)
        {
            fprintf(file, "%0*.*lf ", 3, 4, array[iterator]);
            
            if ((iterator + 1) % JSIZE == 0)
                fprintf(file, "\n");
        }
        
        fclose(file);
    }
}

int rankToBlockDualStrings(int rank, int amount_of_cores)
{
    /*отдельный случай*/
    if (amount_of_cores == 1)
        return 2*JSIZE-3;
    
    /* определяем серединный процесс на первую строку*/
    else if (rank == amount_of_cores / 2 - 1)
        return (JSIZE-3) / (amount_of_cores / 2) + (JSIZE-3) % (amount_of_cores / 2);
    
    /* определяем конечный процесс на вторую строку*/
    else if (rank == amount_of_cores - 1)
        return (JSIZE-3) / (amount_of_cores / 2 + amount_of_cores % 2) +
                (JSIZE-3) % (amount_of_cores / 2 + amount_of_cores % 2);
    else
        return (JSIZE-3) / (amount_of_cores / 2 + amount_of_cores % 2);
}

int rankToBlockSingleStrings(int rank, int amount_of_cores)
{
    int part = (JSIZE-3) / amount_of_cores;
    return rank == amount_of_cores - 1 ? part + (JSIZE-3) % amount_of_cores :
        part;
}

int main(int argc, char** argv)
{
    int rank = 0, amount_of_cores = 0, root = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &amount_of_cores);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int block = rankToBlockDualStrings(rank, amount_of_cores);
    
    double* line_part = (double*) calloc (block, sizeof(double)), *matrix = NULL;
    
    int* displs = NULL, *rcounts = NULL;
    int* displs_ = NULL, *rcounts_ = NULL;
    
    if (root == rank)
    {
        /*filling matrix*/
        matrix = (double*) calloc (ISIZE*JSIZE, sizeof(double));
        
        for (unsigned int iterator = 0; iterator < ISIZE*JSIZE; ++iterator)
        {
            unsigned long int i = iterator/JSIZE, j = iterator%JSIZE;
            matrix[iterator] = 10*i + j;
        }
        
        displs = (int*) calloc (amount_of_cores, sizeof(int));
        rcounts = (int*) calloc (amount_of_cores, sizeof(int));
        
        /*правильно расставить displs и rcounts*/
        /*каждому роздано памяти в соотвествии с переносимой строчкой*/
        rcounts[0] = rankToBlockDualStrings(0, amount_of_cores);
        
        for (int i = 1; i < amount_of_cores; ++i)
        {
            rcounts[i] = rankToBlockDualStrings(i, amount_of_cores);
            /*серединный относительно верхней строки*/
            if (i == amount_of_cores / 2)
                displs[i] = displs[i - 1] + rcounts[i - 1] + 3;
            else
                displs[i] = displs[i - 1] + rcounts[i - 1];
        }
        
        if ((ISIZE - 2) % 2 == 1)
        {
            displs_ = (int*) calloc (amount_of_cores, sizeof(int));
            rcounts_ = (int*) calloc (amount_of_cores, sizeof(int));
            
            rcounts_[0] = rankToBlockSingleStrings(0, amount_of_cores);
            
            for (int i = 1; i < amount_of_cores; ++i)
            {
                rcounts_[i] = rankToBlockSingleStrings(i, amount_of_cores);
                displs_[i] = displs_[i - 1] + rcounts_[i];
            }
        }
    }
    
    double start_time = 0;
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == root)
        start_time = MPI_Wtime();
    
    
    /*итератор относительно общих пересылок*/
    for (unsigned int iterator = 0; iterator < (ISIZE - 2) / 2; ++iterator)
    {
        if (rank != root)
            MPI_Scatterv(NULL, NULL,
                         NULL, MPI_DOUBLE, line_part, block, MPI_DOUBLE,
                         root, MPI_COMM_WORLD);
        else
            MPI_Scatterv(&matrix[2*iterator*JSIZE + 3], rcounts,
                         displs, MPI_DOUBLE, line_part, block, MPI_DOUBLE,
                         root, MPI_COMM_WORLD);
        
        for (unsigned int i = 0; i < block; ++i)
            line_part[i] = sin(0.00001*line_part[i]);
        
        if (amount_of_cores == 1)
        {
            /*поправка на то что amount_of_cores = 1*/
            for (unsigned int local_iter = JSIZE - 3; local_iter < JSIZE; ++local_iter)
            {
                unsigned int i = 2*iterator + 2, j = local_iter;
                line_part[local_iter] = sin(0.00001*(10*i + j));
            }
        }
        
        if (rank != root)
            MPI_Gatherv(line_part, block, MPI_DOUBLE, NULL,
                        NULL, NULL, MPI_DOUBLE, root, MPI_COMM_WORLD);
        else
            MPI_Gatherv(line_part, block, MPI_DOUBLE, &matrix[(2*iterator + 2)*JSIZE],
                        rcounts, displs, MPI_DOUBLE, root, MPI_COMM_WORLD);
    }
    
    if ((ISIZE - 2) % 2 == 1)
    {
        int block_ = rankToBlockSingleStrings(rank, amount_of_cores);
        
        if (rank != root)
            MPI_Scatterv(NULL, NULL,
                         NULL, MPI_DOUBLE, line_part, block_, MPI_DOUBLE,
                         root, MPI_COMM_WORLD);
        else
            MPI_Scatterv(&matrix[(ISIZE - 3)*JSIZE + 3], rcounts_,
                         displs_, MPI_DOUBLE, line_part, block_, MPI_DOUBLE,
                         root, MPI_COMM_WORLD);
        
        for (unsigned int i = 0; i < block_; ++i)
            line_part[i] = sin(0.00001*line_part[i]);
        
        if (rank != root)
            MPI_Gatherv(line_part, block_, MPI_DOUBLE, NULL,
                        NULL, NULL, MPI_DOUBLE, root, MPI_COMM_WORLD);
        else
            MPI_Gatherv(line_part, block_, MPI_DOUBLE, &matrix[(ISIZE - 1)*JSIZE],
                        rcounts_, displs_, MPI_DOUBLE, root, MPI_COMM_WORLD);
    }
    
    if (rank == root)
        printf("Execution time: %lf sec.\n", MPI_Wtime() - start_time);
    
    /*file writing*/
    if (rank == root)
    {
        free(displs);
        free(rcounts);
        
        if ((ISIZE - 2) % 2 == 1)
        {
            free(displs_);
            free(rcounts_);
        }
        
        printIntoFile("matrix.txt", "w",  matrix);
        free(matrix);
    }
    
    free(line_part);
    
    MPI_Finalize();
    
    return 0;
}
