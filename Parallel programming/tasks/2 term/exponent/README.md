# Hometask: calculated exponenta through Taylor's row.

## You should type on command line:

    if you want to compile and run:
        -> make run ARGS=your_num_thread
        
    if  if you want to compile:
        -> make compile
    
    if you want to clear all created files:
        -> make clean

#### Task was to calculate the sum of inverse factorials in parallel.
The number of terms and threads are entered via arguments. The following conditions must be met:
1) response determinism for a constant number of threads
2) an algorithm that allows you to get a more accurate answer to a long double (without using long arithmetic)
3) do not recalculate factorials, but use already calculated values

#### Input format:

    -> your_num_proc - int, number of processes involved in program
    
#### Output format:

    -> The result with 20 digits after the decimal point.
    
    Example:
    
    make run ARGS=2:
    Exp is : ...

