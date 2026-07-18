#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_BUCKETS 10000 // approximadamente 100 elementos por bucket para 1M elementos

// Função para encontrar o valor máximo em um array
float findMax(float array[], int n)
{
    float max = array[0];

#pragma omp parallel for reduction(max : max)
    for (int i = 1; i < n; i++)
    {
        if (array[i] > max)
        {
            max = array[i];
        }
    }
    return max;
}

// Função para inserir um elemento no bucket
void insertionSort(float bucket[], int n)
{
    for (int i = 1; i < n; i++)
    {
        float key = bucket[i];
        int j = i - 1;
        while (j >= 0 && bucket[j] > key)
        {
            bucket[j + 1] = bucket[j];
            j--;
        }
        bucket[j + 1] = key;
    }
}

// Função para realizar o bucket sort
void bucketSort(float array[], int n)
{
    // Encontrar o valor máximo no array
    float max = findMax(array, n);

    int bucketSizes[NUM_BUCKETS];

#pragma omp parallel for
    for (int i = 0; i < NUM_BUCKETS; i++)
        bucketSizes[i] = 0;

    float **buckets = malloc(NUM_BUCKETS * sizeof(float *));
    if (buckets == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

#pragma omp parallel for
    for (int i = 0; i < NUM_BUCKETS; i++)
    {
        buckets[i] = malloc(n * sizeof(float));
        if (buckets[i] == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    // Distribuir os elementos nos buckets
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        int bucketIndex = (array[i] * NUM_BUCKETS) / (max + 1);

        int pos;

#pragma omp atomic capture
        pos = bucketSizes[bucketIndex]++;

        buckets[bucketIndex][pos] = array[i];
    }

    // Ordenar cada bucket individualmente
    for (int i = 0; i < NUM_BUCKETS; i++)
    {
        if (bucketSizes[i] > 0)
        {
            insertionSort(buckets[i], bucketSizes[i]);
        }
    }

    // Concatenar os buckets ordenados de volta no array original
    int arrayIndex = 0;
    for (int i = 0; i < NUM_BUCKETS; i++)
    {
        for (int j = 0; j < bucketSizes[i]; j++)
        {
            array[arrayIndex++] = buckets[i][j];
        }
    }

    for (int i = 0; i < NUM_BUCKETS; i++)
        free(buckets[i]);

    free(buckets);
}

void printArray(float array[], int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%.2f ", array[i]);
    }
    printf("\n");
}

int readFile(const char *filename, float **array)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }

    int n = 0;
    while (fscanf(file, "%f", &(*array)[n]) == 1)
    {
        n++;
        *array = realloc(*array, sizeof(float) * (n + 1));
    }
    fclose(file);
    return n;
}

void exportArrayToFile(float *array, int n, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Erro ao criar o arquivo de saída!\n");
        return;
    }

    for (int i = 0; i < n; i++)
    {
        fprintf(file, "%.2f\n", array[i]);
    }

    fclose(file);
}

int main()
{
    omp_set_num_threads(8);

    float *array = malloc(sizeof(float) * 1);
    // const char *filename = "dados.txt";
    const char *filename = "./data/1M_dados_aleatorios.txt";

    int n = readFile(filename, &array);
    if (n == -1)
    {
        return 1;
    }

    printf("Array original: \n");
    printArray(array, 10);

    bucketSort(array, n);

    // printf("Array ordenado: \n");
    // printArray(array, 10);

    exportArrayToFile(array, n, "./data/1M_dados_aleatorios_ordenados.txt");

    free(array);
    return 0;
}
