# Hometask: harmonic sum.

## You should type on command line:

    if you want to compile and run:
        -> make run ARGS=your_num_proc
    
    if you want to compile:
        -> make compile
    
    if you want to clear all created files:
        -> make clean

#### Task was to calculate harmonic sum by parralel using double data type. The number of terms in the sum are 1.000.000.000.

#### Input format:

    -> your_num_proc - int, number of processes involved in program
    
#### Output format:

    -> calculated harmonic sum.
    
    Example:
    make run ARGS=2:
    Harmonic sum is ....

#### Possible troubles:

    Problem:
    -> macOS: clang: error: unsupported option '-fopenmp'
    Solution:
    -> try to install gcc-9 compiler
