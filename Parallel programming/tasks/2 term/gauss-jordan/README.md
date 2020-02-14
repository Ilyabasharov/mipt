# Hometask: Gauss-Jordan parallel method for matrix.

## You should type on command line:

    if you want to compile and run:
        -> make run CORES=your_cores MATRIX_DIM=matrix_dimension
    
    if you want to clear all created files:
        -> make clean

#### Task was to create Gauss-Jordan parallel method. I have used description from [algorithm](https://e-maxx.ru/algo/linear_systems_gauss).

#### Input format:

    -> your_num_proc - int, number of processes involved in program
    -> matrix_dimension - matrix_dimension, which pointed in 'matrix.txt'
    -> filled matrix in 'matrix.txt' by the image:
    
        A matrix NxN            b free column Nx1
    -------------------------|------------------
    a_1_1      ...     a_1_N |         b_1
    ...                 ...  |         ...
    ...                 ...  |         ...
    ...                 ...  |         ...
    a_N_1      ...    a_N_N  |         b_N
    
    The linear equation system which will be solved, can be represented as Ax=b. The program finds x vector.
                             
#### Output format:

    -> The program finds x vector. If the matrix is ​​linearly dependent, output will be 'Linear relationship was detected.'
    
    Example:
    
    make run CORES=2 MATRIX_DIM=2
    Solution : ...
