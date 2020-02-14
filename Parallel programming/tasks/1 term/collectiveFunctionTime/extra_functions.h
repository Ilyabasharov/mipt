#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double MSE(double *buf, int size)
{
    double sum = 0;
    for (int i = 0; i < size; i++)
        sum += buf[i];
    return sum / size;
}

double RMSE(double *buf, int size)
{
    double sum_sq = 0;
    double MSE_ = MSE(buf, size);
    for (int i = 0; i < size; i++)
        sum_sq += pow(buf[i] - MSE_,2);
    
    return sqrt(sum_sq/size);
}

int* My_min(int** matrix, int length, int width)
{
    int* array = (int*) calloc (length, sizeof(int));
    for(int j = 0; j < length; j++)
    {
        int min = matrix[0][j];
        for(int i = 1; i < width; i++)
        {
            if (matrix[i][j] < min)
                min = matrix[i][j];
        }
        array[j] = min;
    }
    
    return array;
}
