# Hometask: Seidel method.

## You should type on command line:

    if you want to compile and run:
        -> make run CORES=your_cores
    
    if you want to clear all created files:
        -> make clean

#### Solve a two-dimensional stationary thermodynamic problem by parallelizing the Seidel method.
(x, y, i+1) = [(x-1, y, i+1) + (x+1, y, i) + (x, y-1, i+1) + (x, y+1, i)]/4

#### Input format:

    -> your_num_proc - int, number of processes involved in program
                             
#### Output format:

    -> Time haracteristics and result matrix.
    
