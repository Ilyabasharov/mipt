#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int n = atoi(argv[1]);
    FILE *file;
    file = fopen("numbers.txt", "w");
    fprintf(file, "%d\n", n);
    
    for (unsigned int i = 0; i < n; i++)
        fprintf(file, "%d ", rand() % 60000 - 30000);
    
    fclose(file);
    return 0;
}
