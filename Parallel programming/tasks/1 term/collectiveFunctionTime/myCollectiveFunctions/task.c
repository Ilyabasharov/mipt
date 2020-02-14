#include "../extra_functions.h"
#include <mpi.h>
#include <string.h>

#define MEASUREMENTS 10000

int My_Bcast (void *buf, int count, MPI_Datatype type, int root, MPI_Comm comm)
{
    int size = 0, my_rank = 0;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Status status;
    MPI_Barrier(comm);
    
    if (my_rank == root)
        for (int i = 0; i < size; i++)
            MPI_Send(buf, count, type, i, 0, comm);
    
    MPI_Recv(buf, count, type, root, 0, comm, &status);
    MPI_Barrier(comm);
    
    return MPI_SUCCESS;
}

int My_Reduce (void* sendbuf, void* recvbuf, int count, MPI_Datatype type, MPI_Op op, int root, MPI_Comm comm)
{
    if (type != MPI_INT || op != MPI_MIN)
    {
        printf("invalid data type\n");
        return MPI_ERR_TYPE;
    }
    
    int size = 0, my_rank = 0, error = 0;
    
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Status status;
    
    int** current_buf = (int**) calloc (size, sizeof(int*));
    
    for (int i = 0; i < size; i++)
        current_buf[i] = (int*) calloc (count, sizeof(int));
    
    MPI_Barrier(comm);
    if (my_rank != root)
        MPI_Send(sendbuf, count, type, root, 0, comm);
    else
        for (int i = 0; i < size; i++)
        {
            if (i != root)
                error = MPI_Recv(current_buf[i], count, type, i, 0, comm, &status);
            else
                current_buf[i] = sendbuf;
        }
    
    if (my_rank == root)
        memcpy(recvbuf, (void*)My_min(current_buf, count, size), count*sizeof(int));
    
    MPI_Barrier(comm);
    
    return error;
}

int My_Scatter (void* sbuf, int scount, MPI_Datatype stype, void* rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm)
{
    int size = 0, my_rank = 0, error = 0;
    
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Status status;
    
    MPI_Aint lb, elemSize;
    MPI_Barrier(comm);
    
    if (my_rank == root)
    {
        MPI_Type_get_extent(rtype, &lb, &elemSize);
        for(int i = 0; i < size; i++)
            MPI_Send(((char*)sbuf) + i*scount*elemSize,
                     scount, stype, i, 0,comm);
    }
    
    error = MPI_Recv(rbuf, rcount, rtype, root, 0, comm, &status);
    MPI_Barrier(comm);
    
    return error;
}

int My_Gather(void* sbuf, int scount, MPI_Datatype stype, void* rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm)
{
    int size = 0, my_rank = 0, error = 0;
    
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Status status;

    MPI_Aint elemSize, lb;
    MPI_Barrier(comm);
    
    MPI_Send(sbuf, scount, stype, root, 0, comm);
    if(my_rank == root)
    {
        MPI_Type_get_extent(rtype, &lb, &elemSize);
        for(int i = 0; i < size; i++)
            error = MPI_Recv(((char*)rbuf) + (i * rcount * elemSize),
            rcount, rtype, i, 0, comm, &status);
    }
    
    MPI_Barrier(comm);
    
    return error;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int size = 0, rank = 0;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    
    double start_time = 0, end_time = 0, *measurements = NULL, *my_measurements = NULL;
    int send_buf = 0, recv_buf = 0;
    
    if (rank == 0)
    {
        measurements = (double*) calloc (MEASUREMENTS, sizeof(double));
        my_measurements = (double*) calloc (MEASUREMENTS, sizeof(double));
    }
    
    const char *strings[] = {"Bcast", "Reduce", "Scatter", "Gather"};
    
    for (int j = 0; j < 4; j++)
    {
        for(int i = 0; i < MEASUREMENTS; i ++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            if (rank == 0)
                start_time = MPI_Wtime();
            
            switch (j)
            {
                case 0:
                    MPI_Bcast(&send_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                    {
                        end_time = MPI_Wtime();
                        measurements[i] = end_time - start_time;
                    }
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                        start_time = MPI_Wtime();
                    
                    My_Bcast(&send_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                    {
                        end_time = MPI_Wtime();
                        my_measurements[i] = end_time - start_time;
                    }
                    
                    break;
                    
                case 1:
                    MPI_Reduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                    {
                        end_time = MPI_Wtime();
                        measurements[i] = end_time - start_time;
                    }
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                        start_time = MPI_Wtime();
                    
                    My_Reduce(&send_buf, &recv_buf, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                    {
                        end_time = MPI_Wtime();
                        my_measurements[i] = end_time - start_time;
                    }
                    break;
                    
                case 2:
                    MPI_Scatter(&send_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                    {
                        end_time = MPI_Wtime();
                        measurements[i] = end_time - start_time;
                    }
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                        start_time = MPI_Wtime();
                    
                    My_Scatter(&send_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                    {
                        end_time = MPI_Wtime();
                        my_measurements[i] = end_time - start_time;
                    }
                    break;
                    
                case 3:
                    MPI_Gather(&send_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                    {
                        end_time = MPI_Wtime();
                        measurements[i] = end_time - start_time;
                    }
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                        start_time = MPI_Wtime();
                    
                    My_Gather(&send_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    
                    MPI_Barrier(MPI_COMM_WORLD);
                    if (rank == 0)
                    {
                        end_time = MPI_Wtime();
                        my_measurements[i] = end_time - start_time;
                    }
                    
                default:
                    break;
            }
        }
        
        if (rank == 0)
        {
            printf("%s:\n", strings[j]);
            printf("My creation : = %.10lf\n", MSE(my_measurements, MEASUREMENTS));
            printf("MPI creation: = %.10lf\n", MSE(measurements, MEASUREMENTS));
            printf("\n");
        }
    }
    
    free(measurements);
    free(my_measurements);
    
    MPI_Finalize();
    return 0;
}
