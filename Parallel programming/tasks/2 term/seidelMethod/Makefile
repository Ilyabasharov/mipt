CC_MPI=mpicc
CFLAGS= -Wall

run:
	$(CC_MPI) $(CFLAGS) -o 1 zeydel.c
	mpirun --hostfile hostfile -np ${CORES} 1

clean:
	rm -rf *.o 1
