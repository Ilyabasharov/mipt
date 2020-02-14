#include "shooting_method.h"

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    shoot();

    MPI_Finalize();
    return 0;
}
