#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>

#define ROUNDING_ERROR 1e-6

struct data
{
    double value;
    int id_proc;
}typedef data;

int rankToPart(int matrix_dimension, int rank, int amount_of_cores)
{
    int block = matrix_dimension / amount_of_cores;
    
    return rank + 1 != amount_of_cores ?
           block :
           block + matrix_dimension % amount_of_cores;
}

long fillingArray(double* array, const char* filename, long from, long size)
{
    FILE *file = fopen(filename, "r");
    fseek(file, from, 0);
    for (unsigned int i = 0; i < size; ++i)
        fscanf(file, "%lf ", &array[i]);
    
    long result = ftell(file);
    
    fclose(file);
    
    return result;
}

double* readingFile(const char* filename, int rank, int block,
                    int amount_of_cores, int matrix_dimension)
{
    double* line_part = (double*) calloc (block*(matrix_dimension + 1),                                                                                     sizeof(double));
    
    long buf_recv = 0, buf_send = 0;
    
    /*file reading*/
    if (rank == 0)
    {
        buf_send = fillingArray(line_part, filename, 0, block*(matrix_dimension + 1));
        
        if (rank + 1 != amount_of_cores)
            MPI_Send(&buf_send, 1, MPI_LONG, rank + 1, rank,
                     MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(&buf_recv, 1, MPI_LONG, rank - 1,
                     rank - 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        buf_send = fillingArray(line_part, filename, buf_recv, block*(matrix_dimension + 1));

        if (rank + 1 < amount_of_cores)
            MPI_Send(&buf_send, 1, MPI_LONG, rank + 1, rank, MPI_COMM_WORLD);
    }
    
    return line_part;
}

void swapping(double* array1, double* array2, int size, double* extra_memory)
{
    for (int i = 0; i < size; ++i)
        extra_memory[i] = array1[i];
    
    for (int i = 0; i < size; ++i)
        array1[i] = array2[i];
    
    for (int i = 0; i < size; ++i)
        array2[i] = extra_memory[i];
}

void partialPivoting(int rank, int amount_of_cores,
                     int i, int block, double* array,
                     int matrix_dimension, int root,
                     double* extra_memory, int corrected_block)
{
    double max_value = -1;
    int line_with_max_pivot = 0;
    for (int iterator = 0; iterator < block; ++iterator)
    {
        if (iterator + corrected_block*rank >= i)
        {
            double new_max = fabs(array[iterator*(matrix_dimension + 1) + i]);
            if (max_value < new_max)
            {
                line_with_max_pivot = iterator;
                max_value = new_max;
            }
        }
    }
    
    data in = {max_value, rank}, out;
    MPI_Allreduce(&in, &out, 1, MPI_DOUBLE_INT,
                  MPI_MAXLOC, MPI_COMM_WORLD);
    
    /* обмен с root процессом, заходят только 2 процесса */
    if (out.id_proc == rank || rank == root)
    {
        /* зашел proc, у которого value max, является root и нужен обмен между самим собой */
        if (root == out.id_proc &&
            root*corrected_block + line_with_max_pivot != i)
            swapping(&array[line_with_max_pivot*(matrix_dimension + 1)],
                     &array[(i - root*corrected_block)*(matrix_dimension + 1)],
                     matrix_dimension + 1, extra_memory);
        
        /* зашли разные процессы */
        else
            if (root != out.id_proc)
            {
                int dest = out.id_proc == rank ?
                        root : out.id_proc,
                    offset = out.id_proc == rank ?
                        line_with_max_pivot :
                        (i - root*corrected_block);
                
                MPI_Send(&array[offset*(matrix_dimension + 1)],
                         matrix_dimension + 1,MPI_DOUBLE, dest,
                         0, MPI_COMM_WORLD);
            
                MPI_Recv(&array[offset*(matrix_dimension + 1)],
                         matrix_dimension + 1,MPI_DOUBLE, dest,
                         0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
    }
}

int whoIsRoot(int matrix_dimension, int root, int i,
              int amount_of_cores, int corrected_block)
{
    if (i == corrected_block * (root + 1) &&
        i <= matrix_dimension - matrix_dimension % amount_of_cores - corrected_block)
        root++;
    
    return root;
}

int BcastingAndSubtraction(int rank, int i, int block, int root,
                           double* line_part, int matrix_dimension,
                           double* extra_memory, int corrected_block)
{
    int error = 0;
    if (rank == root)
    {
        int offset = (i - root*corrected_block)*(matrix_dimension + 1);
        if (line_part[offset + i] != 0)
        {
            double divider = line_part[offset + i];
            for (int iterator = 0; iterator < matrix_dimension + 1; ++iterator)
            {
                line_part[offset + iterator] /= divider;
                extra_memory[iterator] = line_part[offset + iterator];
            }
        }
        else
        {
            error = 1;
            printf("Linear relationship was detected\n");
        }
    }
    
    MPI_Bcast(&error, 1, MPI_INT, root, MPI_COMM_WORLD);
    if (error)
    {
        free(line_part);
        free(extra_memory);
        return error;
    }
    
    MPI_Bcast(extra_memory, matrix_dimension + 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    
    for (int line = 0; line < block; ++line)
    {
        if (root == rank && line == (i - root*corrected_block))
            continue;
        else
        {
            double multiplier = line_part[line*(matrix_dimension + 1) + i];
            for (int iterator = 0; iterator < matrix_dimension + 1; ++iterator)
            {
                line_part[line*(matrix_dimension + 1) + iterator] -=
                    extra_memory[iterator]*multiplier;
                if (fabs(line_part[line*(matrix_dimension + 1) + iterator]) < ROUNDING_ERROR)
                    line_part[line*(matrix_dimension + 1) + iterator] = 0;
            }
        }
    }
    return error;
}

void printResult(int rank, double* line_part, int block,
                 int matrix_dimension, int amount_of_cores)
{
    int buf_send = 0, buf_recv = 0;
    if (rank == 0)
    {
        printf("Result:\n");
        for (int index = 0; index < block; ++index)
            printf("%07.4lf\n", line_part[index*(matrix_dimension + 1) + matrix_dimension]);
        
        if (rank + 1 != amount_of_cores)
            MPI_Send(&buf_send, 1, MPI_INT, rank + 1, rank,
                     MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(&buf_recv, 1, MPI_INT, rank - 1,
                     rank - 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        for (int index = 0; index < block; ++index)
            printf("%07.4lf\n", line_part[index*(matrix_dimension + 1) + matrix_dimension]);

        if (rank + 1 < amount_of_cores)
            MPI_Send(&buf_send, 1, MPI_INT, rank + 1, rank, MPI_COMM_WORLD);
    }
}

int main(int argc, char** argv)
{
    int rank = 0, amount_of_cores = 0,root = 0,
        matrix_dimension = atoi(argv[1]);
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &amount_of_cores);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int block = rankToPart(matrix_dimension,
                           rank, amount_of_cores),
        corrected_block = rank + 1 == amount_of_cores ?
            block - matrix_dimension % amount_of_cores :
            block;
    
    double *line_part = readingFile("matrix.txt", rank, block,
                                    amount_of_cores, matrix_dimension),
           *extra_memory = (double*) calloc (matrix_dimension + 1,
                                             sizeof(double));
    
    for (int i = 0; i < matrix_dimension; ++i)
    {
        /* определение root в каждом процессе */
        root = whoIsRoot(matrix_dimension, root, i,
                         amount_of_cores, corrected_block);
        
        /* обмен строками в соответсвии с выбором опорного элемента */
        partialPivoting(rank, amount_of_cores, i, block,
                        line_part, matrix_dimension, root,
                        extra_memory, corrected_block);
        
        int error = BcastingAndSubtraction(rank, i, block, root,
                                           line_part, matrix_dimension,
                                           extra_memory, corrected_block);
        if (error)
        {
            MPI_Finalize();
            return 0;
        }
    }
    
    printResult(rank, line_part, block,
                matrix_dimension, amount_of_cores);
    
    free(line_part);
    free(extra_memory);
    
    MPI_Finalize();
    
    return 0;
}
