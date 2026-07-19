#include <stdio.h>
#include <stdlib.h>

int main()
{
    float *array = malloc(sizeof(float) * 1);
    const char *filename = "./data/1000000_sorted_random_data.txt";

    int n = readFile(filename, &array);
    if (n == -1)
    {
        return 1;
    }

    for (int i = 0; i < n - 1; i++)
    {
        if (array[i] > array[i + 1])
        {
            printf("O array não está ordenado.\n");
            free(array);
            return 0;
        }
    }

    printf("O array está ordenado.\n");
    free(array);
    return 0;
}
