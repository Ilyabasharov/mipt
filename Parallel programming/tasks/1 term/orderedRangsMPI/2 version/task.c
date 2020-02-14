#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int rank = 0, size = 0;
    
	MPI_Init(&argc, &argv);
   
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    
    int buf_recv = 0, buf_send = 0;
    
    for (int iter = 0; iter < size; ++iter)
    {
        if (rank == buf_recv)
        {
            printf("Hello from %d\n", rank);
            
            if (rank + 1 < size)
            {
                buf_send = rank + 1;
                
                for (int i = 0; i < size; i++)
                    if (i != rank)
                        MPI_Send(&buf_send, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            
            break;
		}
        else
			MPI_Recv(&buf_recv, 1, MPI_INT, MPI_ANY_SOURCE,
                     MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
	}
    
    MPI_Finalize();
    return 0;
}
