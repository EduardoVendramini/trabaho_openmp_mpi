#include <stdio.h>
#include <stdlib.h>

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
     srand(42);
     int i, j;
     int hist[N] = {0};
     int results[RANGE] = {0};

     listGenerate(hist);

     for (i = 0; i < RANGE; ++i)
     {
          for (j = 0; j < N; j++)
          {
               if (hist[j] == i)
                    results[i]++;
          }
     }
     printHistogram(results, 10);
     return 0;
}
