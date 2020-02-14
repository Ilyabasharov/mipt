#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main (int argv, char* argc[])
{
	int rank = 0, size = 0;
	
	MPI_Init(&argv, &argc);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int buf_recv = 0, buf_send = 0;

	if (rank == 0)
	{
		printf("Hello from %d\n", rank);
        MPI_Send(&buf_send, 1, MPI_INT, rank+1,
                    rank, MPI_COMM_WORLD);
	}
	else
	{
        MPI_Recv(&buf_recv, 1, MPI_INT, rank - 1, rank - 1,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Hello from %d\n", rank);
        if (rank + 1 < size)
            MPI_Send(&buf_send, 1, MPI_INT, rank + 1,
                        rank, MPI_COMM_WORLD);
	}
    
	MPI_Finalize();
	return 0;
}
