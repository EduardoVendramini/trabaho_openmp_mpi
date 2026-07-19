#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX 10 // Número de elementos em cada bucket
struct timespec start, end;

double get_time_diff(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

// Função para encontrar o valor máximo em um array
float findMax(float array[], int n)
{
    float max = array[0];
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
    clock_gettime(CLOCK_MONOTONIC, &start);
    float max = findMax(array, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo para encontrar o valor máximo: %.6f segundos\n", get_time_diff(start, end));
    // Encontrar o valor máximo no array

    // Inicializar os buckets
    int bucketCount = MAX;
    float buckets[bucketCount][n];
    int bucketSizes[bucketCount];

    // Inicializar os tamanhos dos buckets
    for (int i = 0; i < bucketCount; i++)
    {
        bucketSizes[i] = 0;
    }

    // Distribuir os elementos nos buckets
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < n; i++)
    {
        int index = (array[i] * bucketCount) / (max + 1);
        buckets[index][bucketSizes[index]] = array[i];
        bucketSizes[index]++;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo para distribuir os elementos nos buckets: %.6f segundos\n", get_time_diff(start, end));

    // Ordenar cada bucket individualmente
    clock_gettime(CLOCK_MONOTONIC, &start);
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < bucketCount; i++)
    {
        if (bucketSizes[i] > 0)
        {
            insertionSort(buckets[i], bucketSizes[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo para ordenar os buckets: %.6f segundos\n", get_time_diff(start, end));

    // Concatenar os buckets ordenados de volta no array original
    clock_gettime(CLOCK_MONOTONIC, &start);
    int index = 0;
    for (int i = 0; i < bucketCount; i++)
    {
        for (int j = 0; j < bucketSizes[i]; j++)
        {
            array[index++] = buckets[i][j];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo para concatenar os buckets ordenados: %.6f segundos\n", get_time_diff(start, end));
}

// Função para imprimir o array
void printArray(float array[], int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%.2f ", array[i]);
    }
    printf("\n");
}

// Função para ler os dados de um arquivo
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

void checkSortedData(float *array, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        if (array[i] > array[i + 1])
        {
            printf("O array não está ordenado.\n");
            return;
        }
    }
    printf("O array está ordenado.\n");
}

int main()
{
    clock_gettime(CLOCK_MONOTONIC, &start);

    float *array = malloc(sizeof(float) * 1);
    // const char *filename = "dados.txt";  // Substitua pelo nome do seu arquivo
    const char *filename = "./100k_dados_aleatorios.txt"; // 1M ja da segmentation fault

    int n = readFile(filename, &array);
    if (n == -1)
    {
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo de leitura do arquivo: %.6f segundos\n", get_time_diff(start, end));

    printf("Array original: \n");
    printArray(array, 10);

    clock_gettime(CLOCK_MONOTONIC, &start);
    bucketSort(array, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo de ordenação do array: %.6f segundos\n", get_time_diff(start, end));

    clock_gettime(CLOCK_MONOTONIC, &start);
    checkSortedData(array, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo de verificação do array ordenado: %.6f segundos\n", get_time_diff(start, end));

    printf("Array ordenado: \n");
    printArray(array, 10);

    free(array);
    return 0;
}
