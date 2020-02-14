#include "shooting_method.h"

void writeResults(char *outputFile, double *source, double dt)
{
    FILE *output = fopen(outputFile, "w");
    if (!output)
    {
        fprintf(stderr, "Error: while opening %s: %s\n", outputFile, strerror(errno));
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    double time = 0;
    
    fprintf(output, "t,x\n");
    for (int i = 0; i < N; i++)
    {
        if (fprintf(output, "%.10f, %.10f\n", time, source[i]) == 0)
        {
            fprintf(stderr, "Error: while writing the output to %s: %s\n", outputFile, strerror(errno));
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        time += dt;
    }

    fclose(output);
}

void shoot()
{
    int rank = 0, size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double T_start = MPI_Wtime();

    int n_start = (N * (rank + 0)) / size,
        n_end   = (N * (rank + 1)) / size;

    if (rank == size - 1)
        n_end = N;

    double dt = T / N, dT = T / size,
           testShot[N] = {},
           testShotReceived[N];

    // init x and v with zero values for first shot
    double x = 0.0, v = 0.0;

    for (int n = n_start; n < n_end; n++)
    {
        x += v * dt;
        v += F(n * dt) * dt;
        testShot[n] = x;
    }

    int recv_test_size[MAX_SIZE] = {},
        recv_test_offs[MAX_SIZE] = {};
    
    for (int rank = 0; rank < size; rank++)
    {
        recv_test_size[rank] = N / size;
        recv_test_offs[rank] = (N * rank) / size;
    }
    
    recv_test_size[size-1] = N - recv_test_offs[size-1];

    MPI_Gatherv(testShot + n_start, n_end - n_start, MPI_DOUBLE,
                testShotReceived, recv_test_size, recv_test_offs, MPI_DOUBLE, ROOT_THREAD, MPI_COMM_WORLD);

    pairs p = {x, v};
    pairs p_start[MAX_SIZE] = {}, p_end[MAX_SIZE] = {};

    MPI_Gather(&p, sizeof(p), MPI_CHAR, p_end, sizeof(p), MPI_CHAR, ROOT_THREAD, MPI_COMM_WORLD);

    if (rank == ROOT_THREAD)
    {
        for (int rank = 1; rank < size; rank++)
        {
            x += v * dT;
            x += p_end[rank].x;
            v += p_end[rank].v;
        }

        // correcting v at the start of the first interval
        p_start[0].x = 0;
        p_start[0].v = (X - x) / T;

        for (int rank = 1; rank < size; rank++)
        {
            // correcting the (x, v) at the end of an interval,
            // considering that we changed (x, v) at the start of it from (0, 0)
            p_start[rank].x = p_end[rank-1].x + p_start[rank-1].x + p_start[rank-1].v * dT;
            p_start[rank].v = p_end[rank-1].v + p_start[rank-1].v;
        }
    }

    // scattering from root corrected (x, v) at the start of the intervals
    MPI_Scatter(p_start, sizeof(p), MPI_CHAR, &p, sizeof(p), MPI_CHAR, ROOT_THREAD, MPI_COMM_WORLD);

    x = p.x;
    v = p.v;

    double correctedShotResults[N] = {};
    for (int n = n_start; n < n_end; n++)
    {
        x += v * dt;
        v += F(n * dt) * dt;
        correctedShotResults[n] = x;
    }

    double finalResults[N] = {};
    int recv_size[MAX_SIZE] = {},
        recv_offs[MAX_SIZE] = {};
    
    for (int rank = 0; rank < size; rank++)
    {
        recv_size[rank] = N / size;
        recv_offs[rank] = (N * rank) / size;
    }
    recv_size[size - 1] = N - recv_offs[size - 1];

    MPI_Gatherv(correctedShotResults + n_start, n_end - n_start, MPI_DOUBLE,
                finalResults, recv_size, recv_offs, MPI_DOUBLE, ROOT_THREAD, MPI_COMM_WORLD);

    if (rank == ROOT_THREAD)
    {
        double T_elapsed = MPI_Wtime() - T_start;
        printf("Execution time is %.5f sec.\n", T_elapsed);

        writeResults("result.csv", finalResults, dt);
        writeResults("test.csv", testShotReceived, dt);
    }
}
