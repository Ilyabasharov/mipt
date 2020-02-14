CC=mpicc
CFLAGS= -Wall

run:
	$(CC) $(CFLAGS) -o task task.c -lpthread
	mpirun --hostfile hostfile -np ${CORES} task ${PTHREADS}

generate:
	$(CC) $(CFLAGS) -o gen ../gen.c
	./gen ${ARGS}

clean:
	rm -rf *.o task gen

