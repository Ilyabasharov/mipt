# Hometask: Ordered rangs.

## You should type on command line:

    if you want to compile and run:
        -> make run ARGS=your_num_proc
    
    if you want to clear all created files:
        -> make clean

#### Task was to print process ranks orderly without using barriers or standby functions, it means after each program call ranks should be sorted.

#### Input format:

    -> your_num_proc - int, number of processes involved in program
    
#### Output format:

    -> Each process should print its rank.
    
    Example:
    
    make run ARGS=4 :
    
    Hello from 0
    Hello from 1
    Hello from 2
    Hello from 3

#### Possible troubles:

    Problem:
    -> macOS: local hostname missing in /etc/hosts
    Solution:
    -> [from Github](https://github.com/microsoft/azure-pipelines-image-generation/issues/932)
       type in command line: export OMPI_MCA_btl=self,tcp
