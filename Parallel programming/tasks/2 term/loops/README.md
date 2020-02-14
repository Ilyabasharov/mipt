# Hometask: reseach how to parallel calculate loops.

## You should type on command line:

    if you want to compile and run:
        -> make run ARGS=your_num_thread
        
    if  if you want to test:
        -> make test
    
    if you want to clear all created files:
        -> make clean

#### Task was to code all types of dependencies in loops. My variants were 2a, 1a from [training manual](https://mipt.ru/drec/upload/d52/lab2-arpgyfe27m6.pdf)
The program should not be dependent on the size of the matrix. You can change sizes in code(look at #define...)

#### Input format:

    -> your_num_proc - int, number of processes involved in program
    
#### Output format:

    -> Execution time.
    
    Example:
    
    make run ARGS=2:
    Execution time : ...
    
#### Test results - original and parallel.

    -> make test
    This command will execute original program from training manual and check the results of work are the same.

