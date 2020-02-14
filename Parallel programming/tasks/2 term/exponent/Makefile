CC=gcc
CFLAGS= -c -Wall

run: compile
	./task ${ARGS}

compile: 1.o 
	$(CC) -o task task.o -fopenmp

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@ -fopenmp

clean:
	rm -rf *.o task
