CC_pth=gcc-9
CC=gcc
CFLAGS= -Wall

run:
	$(CC) $(CFLAGS) -o task task.c -lpthread
	./task ${ARGS}

generate:
	$(CC) $(CFLAGS) -o gen ../gen.c
	./gen ${ARGS}

clean:
	rm -rf *.o task gen

