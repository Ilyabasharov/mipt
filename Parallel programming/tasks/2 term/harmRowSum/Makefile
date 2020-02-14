CC=gcc-9
CFLAGS= -c -Wall

run: compile
	./task ${ARGS}

compile: task.o 
	$(CC) -o task task.o -fopenmp

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@ -fopenmp

clean:
	rm -rf *.o task

