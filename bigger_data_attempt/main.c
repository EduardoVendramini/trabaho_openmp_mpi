#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define NUM_BUCKETS 10000

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

void bucketSort(float array[], int n)
{
    float max = findMax(array, n);

    // Arrays para guardar o tamanho e posições
    int *bucketSizes = calloc(NUM_BUCKETS, sizeof(int));
    int *currentPos = calloc(NUM_BUCKETS, sizeof(int));

    // 1. PASSO: Contar quantos elementos caem em cada bucket (Paralelo)
    for (int i = 0; i < n; i++)
    {
        int bucketIndex = (array[i] * NUM_BUCKETS) / (max + 1);
        bucketSizes[bucketIndex]++;
    }

    float **buckets = malloc(NUM_BUCKETS * sizeof(float *));
    if (buckets == NULL)
        exit(EXIT_FAILURE);

    // 2. PASSO: Alocar o tamanho EXATO para cada bucket
    for (int i = 0; i < NUM_BUCKETS; i++)
    {
        if (bucketSizes[i] > 0)
        {
            buckets[i] = malloc(bucketSizes[i] * sizeof(float));
            if (buckets[i] == NULL)
                exit(EXIT_FAILURE);
        }
        else
        {
            buckets[i] = NULL;
        }
    }

    // 3. PASSO: Distribuir os elementos (Paralelo)
    for (int i = 0; i < n; i++)
    {
        int bucketIndex = (array[i] * NUM_BUCKETS) / (max + 1);
        int pos;
        pos = currentPos[bucketIndex]++;
        buckets[bucketIndex][pos] = array[i];
    }

    // 4. PASSO: Ordenar cada bucket (Dinâmico pois os tamanhos variam)
    for (int i = 0; i < NUM_BUCKETS; i++)
    {
        if (bucketSizes[i] > 0)
        {
            insertionSort(buckets[i], bucketSizes[i]);
        }
    }

    // 5. PASSO: Calcular a posição de início de cada bucket para concatenação paralela (Prefix Sum)
    int *prefixSums = malloc(NUM_BUCKETS * sizeof(int));
    prefixSums[0] = 0;
    for (int i = 1; i < NUM_BUCKETS; i++)
    {
        prefixSums[i] = prefixSums[i - 1] + bucketSizes[i - 1];
    }

    // 6. PASSO: Concatenar os resultados no array original (Paralelo)
    for (int i = 0; i < NUM_BUCKETS; i++)
    {
        for (int j = 0; j < bucketSizes[i]; j++)
        {
            array[prefixSums[i] + j] = buckets[i][j];
        }
    }

    // Liberar memória
    for (int i = 0; i < NUM_BUCKETS; i++)
        if (buckets[i] != NULL)
            free(buckets[i]);
    free(buckets);
    free(bucketSizes);
    free(currentPos);
    free(prefixSums);
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
    int capacity = 10000; // Começa com uma capacidade razoável
    *array = malloc(capacity * sizeof(float));

    while (fscanf(file, "%f", &(*array)[n]) == 1)
    {
        n++;
        // Dobra a capacidade apenas quando necessário (MUITO mais rápido)
        if (n >= capacity)
        {
            capacity *= 2;
            *array = realloc(*array, capacity * sizeof(float));
            if (*array == NULL)
                exit(EXIT_FAILURE);
        }
    }
    fclose(file);
    return n;
}

void exportArrayToFile(float *array, int n, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
        return;

    for (int i = 0; i < n; i++)
        fprintf(file, "%.2f\n", array[i]);

    fclose(file);
}

double get_time_diff(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main()
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    float *array = NULL;
    const char *filename = "./data/1000000_random_data.txt";

    int n = readFile(filename, &array);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo de leitura do arquivo: %.6f segundos\n", get_time_diff(start, end));
    if (n == -1)
        return 1;

    clock_gettime(CLOCK_MONOTONIC, &start);
    bucketSort(array, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo de ordenação: %.6f segundos\n", get_time_diff(start, end));

    clock_gettime(CLOCK_MONOTONIC, &start);
    exportArrayToFile(array, n, "./data/1000000_sorted_random_data.txt");
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Tempo de exportação do arquivo: %.6f segundos\n", get_time_diff(start, end));

    free(array);
    return 0;
}