CC_MPI=mpicc
CC=gcc
CFLAGS= -Wall

run:
	$(CC_MPI) $(CFLAGS) -o task task.c
	mpirun --hostfile hostfile -np ${ARGS} task

clean:
	rm -rf *.o task

