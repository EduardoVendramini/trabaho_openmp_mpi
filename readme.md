## Comandos
cd ./Bucket

wsl

gcc -o main main.c
time ./main

export OMP_NUM_THREADS=8
gcc -fopenmp -o main_omp main_omp.c
time ./main_omp

mpicc -o main_mpi main_mpi.c

## Bucket
normal -> omp - Speedup:
Usado apenas um pragma no trecho mais demorado. Usado schedule(dynamic) porque no trecho

```
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < bucketCount; i++)
    {
        if (bucketSizes[i] > 0)
        {
            insertionSort(buckets[i], bucketSizes[i]);
        }
    }
```
Cada balde tem tamanho distinto, entao cada thread pega ordena uma determinada quantia de balde conforme necessidade. 
Não é o msm número de baldes pra cada thread.

mpi não tentei

## friendly
normal -> omp - Speedup para computação entre 1 e 262143:
Foi feito nos 2 trechos com for, sendo o primeiro
```
#pragma omp parallel for private(j) reduction(+ : num_of_friendly_numbers)
	for (i = 0; i < last; i++)
	{
		for (j = i + 1; j < last; j++)
		{
			if ((num[i] == num[j]) && (den[i] == den[j]))
			{
				num_of_friendly_numbers++;
				printf("%ld and %ld are FRIENDLY\n",
					   the_num[i], the_num[j]);
			}
		}
	}
```
em que o j é privado por thread pra não haver conflito
e foi feito o reduction pra acumular o numero de elementos amigos na variavel num_of_friendly_numbers.
No outro trecho
```
#pragma omp parallel for private(ii, sum, done, factor, n)
```
Foi feito apenas um parallel for com as variáveis temporárias privadas por thread.

MPI ficou mto complexo e pior performance tmb

## Histo
normal -> omp - Speedup:
Usado so na parte dos 2 for é claro
```
#pragma omp parallel for private(j) shared(hist, results)
     for (i = 0; i < RANGE; ++i)
     {
          for (j = 0; j < N; j++)
          {
               if (hist[j] == i)
                    results[i]++;
          }
     }
```
j é privado para nao interferir na contagem da outra thread
hist é compartilhado pq só tem leitura
results é compartilhado pq cada uma escreve na sua parcela do results

normal -> mpi - Piorou performance
