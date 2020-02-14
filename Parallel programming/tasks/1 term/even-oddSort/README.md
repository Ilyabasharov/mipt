# Hometask: even-odd sort using posix threads.

## You should type on command line:

    if you want to compile and run:
        pthread:
            -> make run ARGS=your_num_thread
        pthread+mpi:
            -> make run CORES=your_cores PTHREADS=your_thread
    
    if you want to generate numbers for sorting:
        -> make generate ARGS=your_number_of_digits
    
    if you want to clear all created files:
        -> make clean

#### Task was to create even-odd sort using posix threads. Elapsed time is printed afrer execution.

#### Input format:

    -> your_num_proc - int, number of processes involved in program
    -> all numbers are being picked up by program from 'numbers.txt'
    
#### Output format:

    -> Sorted numbers from 'numbers.txt' are being written to 'answer.txt'
    
    Example:
    
    make run ARGS=2:
    Elapsed time: ...

