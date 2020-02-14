CC_MPI=mpicc
CC=gcc
CFLAGS= -Wall
.PHONY: test

run:
	$(CC_MPI) $(CFLAGS) -o 1 1.c
	mpirun --hostfile hostfile -np ${ARGS} 1

test:
	$(CC) $(CFLAGS) test.c -o test
	./test
	cmp --silent matrix.txt result.txt && echo 'SUCCESS: Files Are Identical!' || echo 'WARNING: Files Are Different!'

clean:
	rm -rf *.o 1 test matrix.txt result.txt

