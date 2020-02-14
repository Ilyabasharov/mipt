# Hometask: Shooting method for differential equations.

## You should type on command line:

    if you want to compile and run:
        -> make run CORES=your_cores
    
    if you want to clear all created files:
        -> make clean

#### Implement the parallel firing method on an arbitrary number of processes and numerically solve the following equation with the conditions:
y '' (t) = sin (t)
y (0) = 0
y (3 * PI) = -2
As a result of the calculations, a solution should be obtained in the form of 10,000 points on the segment [0; 3 * PI].

#### Input format:

    -> your_num_proc - int, number of processes involved in program
                             
#### Output format:

    -> result.csv and test.csv
    
