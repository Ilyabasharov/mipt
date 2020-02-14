CC_MPI=mpicc
CFLAGS= -Wall

run:
	$(CC_MPI) $(CFLAGS) -o main main.c shooting_method.c
	mpirun --hostfile hostfile -np ${CORES} main
clean:
	rm main
