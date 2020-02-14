#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

typedef struct InitCond
{
    double T_top;
    double T_bot;
    double T_left;
    double T_right;
} InitCond;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    InitCond ic = {100, 200, 300, 400};
    int width = 16, height = 16, max_iter = 1000;
    double tol = 1e-4, *t[2], *data = NULL;
    
    if (rank == 0)
        data = calloc(width * height, sizeof(*data));
    
    int w = width, h = (height + rank) / size;
    t[0] = (double*) calloc (w * (h + 2), sizeof(double));
    t[1] = (double*) calloc (w * (h + 2), sizeof(double));

    int* displs = NULL, *counts = NULL, i, j;
    if (rank == 0)
    {
        displs = (int*) calloc(size, sizeof(int));
        counts = (int*) calloc(size, sizeof(int));
        counts[0] = w * h;
        for (i = 1; i < size; i++)
        {
            displs[i] = displs[i - 1] + counts[i - 1];
            counts[i] = ((height + i) / size) * width;
        }
        
        for (i = 0; i < height; i++)
        {
            data[i * width] = ic.T_left;
            data[(i + 1) * width - 1] = ic.T_right;
        }
        for (j = 0; j < width; j++)
        {
            data[j] = ic.T_top;
            data[(height - 1) * width + j] = ic.T_bot;
        }
        
        double alpha_x, alpha_y;
        for (int i = 1; i < height - 1; i++)
        {
            alpha_x = ((double) i) / height;
            for (int j = 1; j < width - 1; j++)
            {
                alpha_y = ((double) j) / width;
                data[i * width + j] = 0.5 * (ic.T_bot * alpha_x + ic.T_top * (1 - alpha_x) +
                                            ic.T_right * alpha_y + ic.T_left * (1 - alpha_y));
            }
        }
    }

    if (rank == 0)
    {
        MPI_Scatterv(data, counts, displs, MPI_DOUBLE,
                     t[0], w * h, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatterv(data, counts, displs, MPI_DOUBLE,
                     t[1], w * h, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Scatterv(data, counts, displs, MPI_DOUBLE,
                     t[0] + w, w * h, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatterv(data, counts, displs, MPI_DOUBLE,
                     t[1] + w, w * h, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    int iter = -1, curr, next, iend = h + 1;
    double diff = 0, total_diff = 0;
    MPI_Request s_req, r_req;
    if (rank == 0 || rank == size - 1)
        iend = size == 1 ? h - 1 : h;

    while (iter < max_iter)
    {
        iter++;
        curr = iter % 2;
        next = (iter + 1) % 2;

        if (size > 1)
        {
            if (rank == 0)
            {
                MPI_Send(t[curr] + w * (h - 1), w, MPI_DOUBLE, rank + 1, 2, MPI_COMM_WORLD);
                MPI_Recv(t[curr] + w * h, w, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else if (rank != size - 1)
            {
                MPI_Recv(t[curr], w, MPI_DOUBLE, rank - 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(t[curr] + w, w, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);

                MPI_Send(t[curr] + w * h, w, MPI_DOUBLE, rank + 1, 2, MPI_COMM_WORLD);
                MPI_Recv(t[curr] + w * (h + 1), w, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else
            {
                MPI_Recv(t[curr], w, MPI_DOUBLE, rank - 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(t[curr] + w, w, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
            }
        }

        diff = 0;
        for (i = 1; i < iend; i++)
            for (j = 1; j < w - 1; j++)
                diff += fabs(t[0][i * w + j] - t[1][i * w + j]);
        
        MPI_Reduce(&diff, &total_diff, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Bcast(&total_diff, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        total_diff /= height * width;
        
        if (rank == 0 && (iter + 1) % 100 == 0)
            printf("Current iteration: %d, diff = %.7lf\n", iter + 1, total_diff);
        
        if (total_diff < tol && iter > 0)
            break;

        for (j = 1; j < w - 1; j++)
        {
            if (rank != 0)
            {
                MPI_Irecv(t[next] + j, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &r_req);
                MPI_Wait(&r_req, MPI_STATUS_IGNORE);
            }
            for (i = 1; i < iend; i++)
                t[next][i * w + j] = 0.25 * (t[next][(i - 1) * w + j] + t[curr][(i + 1) * w + j] +
                                             t[next][i * w + (j - 1)] + t[curr][i * w + (j + 1)]);
            if (rank != size - 1)
                MPI_Isend(t[next] + w * (iend - 1) + j, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &s_req);
        }
    }

    if (rank == 0)
    {
        printf("Total iterations are %d, total diff: %.7lf\n", iter, total_diff);
        MPI_Gatherv(t[iter % 2], w * h, MPI_DOUBLE, data, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    else
        MPI_Gatherv(t[iter % 2] + w, w * h, MPI_DOUBLE, data, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    free(t[0]);
    free(t[1]);
    
    if (rank == 0)
    {
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height - 1; j++)
                printf("%.1lf ", data[i * width + j]);
            printf("%.1lf\n", data[i * width + height - 1]);
        }
        free(data);
        free(counts);
        free(displs);
    }
    
    MPI_Finalize();
    return 0;
}
