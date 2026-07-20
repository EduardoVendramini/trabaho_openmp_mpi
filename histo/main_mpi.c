#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define RANGE 1000
#define N 1000000

void listGenerate(int *hist)
{
     for (int i = 0; i < N; i++)
          hist[i] = rand() % RANGE;
}

void printHistogram(int *hist, int n)
{
     int i, j;
     for (i = 0; i < n; i++)
     {
          printf("[%d] - [%d]\n", i, hist[i]);
     }
}

int main(int argc, char *argv[])
{
     int rank, num_procs;
     MPI_Init(&argc, &argv);
     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
     MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

     int *hist = NULL;

     if (rank == 0)
     {
          srand(42);
          hist = malloc(N * sizeof(int));
          listGenerate(hist);
     }

     int localSize = N / num_procs;

     int *localHist = malloc(localSize * sizeof(int));

     int localResults[RANGE] = {0};
     int results[RANGE] = {0};

     MPI_Scatter(hist,      // vetor de dados a ser enviado
                 localSize, // número de elementos enviados para cada processo
                 MPI_INT,   // tipo de dados dos elementos enviados
                 localHist, // vetor de dados local para receber os elementos
                 localSize, // número de elementos recebidos por cada processo
                 MPI_INT,   // tipo de dados dos elementos recebidos
                 0,         // rank do processo que envia os dados
                 MPI_COMM_WORLD);

     // Cada processo monta seu histograma local
     for (int i = 0; i < RANGE; i++)
     {
          for (int j = 0; j < localSize; j++)
          {
               if (localHist[j] == i)
                    localResults[i]++;
          }
     }

     // Soma todos os histogramas
     MPI_Reduce(localResults,
                results,
                RANGE,
                MPI_INT,
                MPI_SUM,
                0,
                MPI_COMM_WORLD);

     if (rank == 0)
     {
          printHistogram(results, 10);
          free(hist);
     }

     free(localHist);
     MPI_Finalize();
     return 0;
}
