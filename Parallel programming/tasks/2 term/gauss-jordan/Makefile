CC_MPI=mpicc
CFLAGS= -c -Wall 

run:
	$(CC_MPI) $(CFLAGS) -o 1 1.c
	mpirun --hostfile hostfile -np ${CORES} 1 ${MATRIX_DIM}

clean:
	rm 1
