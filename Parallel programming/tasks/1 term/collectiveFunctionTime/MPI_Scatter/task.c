#include <mpi.h>
#include "../extra_functions.h"

#define MEASUREMENTS 50000

int main(int argc, char** argv)
{
	int rank = 0, size = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int send_buf = 0, recv_buf = 0;

    double start_time = 0, end_time = 0, *array = NULL;
    if (rank == 0)
        array = (double*) calloc (MEASUREMENTS, sizeof(double));

    for(int i = 0; i < MEASUREMENTS; i ++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0)
            start_time = MPI_Wtime();

        MPI_Scatter(&send_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0)
        {
            end_time = MPI_Wtime();
            array[i] = end_time - start_time;
        }
    }

    if(rank == 0)
    {
		printf("Calculated error in mean = %.10lf\n", RMSE(array, MEASUREMENTS));
        printf("Wtick = %.10lf\n", MPI_Wtick());
        
        free(array);
    }
    
	MPI_Finalize();
    return 0;
}

