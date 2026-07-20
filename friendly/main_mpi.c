#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

struct timespec start_time, end_time;

double get_time_diff(struct timespec start, struct timespec end)
{
	return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int gcd(int u, int v)
{
	if (v == 0)
		return u;
	return gcd(v, u % v);
}

void compute_fractions(long int start, long int end,
					   long int *the_num, long int *num, long int *den)
{
	long int i, factor, done, sum, n;
	long int idx = 0;

	for (i = start; i <= end; i++)
	{
		sum = 1 + i;
		done = i;
		factor = 2;
		while (factor < done)
		{
			if ((i % factor) == 0)
			{
				sum += (factor + (i / factor));
				if ((done = i / factor) == factor)
					sum -= factor;
			}
			factor++;
		}
		the_num[idx] = i;
		num[idx] = sum;
		den[idx] = i;
		n = gcd(num[idx], den[idx]);
		num[idx] /= n;
		den[idx] /= n;
		idx++;
	}
}

void friendly_numbers_parallel(long int start, long int end, int rank, int size)
{
	long int N = end - start + 1;

	long int local_start, local_end;
	int local_count;
	long int base_count = N / size;
	int remainder = N % size;

	if (rank < remainder)
	{
		local_count = base_count + 1;
		local_start = start + rank * local_count;
	}
	else
	{
		local_count = base_count;
		local_start = start + remainder * (base_count + 1) + (rank - remainder) * base_count;
	}
	local_end = local_start + local_count - 1;

	/* Arrays locais */
	long int *local_the_num = (long int *)malloc(local_count * sizeof(long int));
	long int *local_num = (long int *)malloc(local_count * sizeof(long int));
	long int *local_den = (long int *)malloc(local_count * sizeof(long int));

	/* ---- Cálculo paralelo das frações ---- */
	MPI_Barrier(MPI_COMM_WORLD);
	double t_start = MPI_Wtime();

	compute_fractions(local_start, local_end, local_the_num, local_num, local_den);

	MPI_Barrier(MPI_COMM_WORLD);
	double t_end = MPI_Wtime();
	double elapsed = t_end - t_start;

	double max_time;
	MPI_Reduce(&elapsed, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		printf("Tempo para identificar os números amigáveis (paralelo): %.6f segundos\n", max_time);
	}

	/* ---- Reúne os dados no processo raiz ---- */
	int *recv_counts = NULL;
	int *displs = NULL;
	long int *all_the_num = NULL, *all_num = NULL, *all_den = NULL;

	if (rank == 0)
	{
		recv_counts = (int *)malloc(size * sizeof(int));
		displs = (int *)malloc(size * sizeof(int));

		int total = 0;
		for (int p = 0; p < size; p++)
		{
			int cnt = (p < remainder) ? base_count + 1 : base_count;
			recv_counts[p] = cnt;
			displs[p] = total;
			total += cnt;
		}

		all_the_num = (long int *)malloc(N * sizeof(long int));
		all_num = (long int *)malloc(N * sizeof(long int));
		all_den = (long int *)malloc(N * sizeof(long int));
	}

	MPI_Gatherv(local_the_num, local_count, MPI_LONG,
				all_the_num, recv_counts, displs, MPI_LONG,
				0, MPI_COMM_WORLD);
	MPI_Gatherv(local_num, local_count, MPI_LONG,
				all_num, recv_counts, displs, MPI_LONG,
				0, MPI_COMM_WORLD);
	MPI_Gatherv(local_den, local_count, MPI_LONG,
				all_den, recv_counts, displs, MPI_LONG,
				0, MPI_COMM_WORLD);

	free(local_the_num);
	free(local_num);
	free(local_den);

	/* ---- Comparação final (somente no raiz, igual ao original) ---- */
	if (rank == 0)
	{
		clock_gettime(CLOCK_MONOTONIC, &start_time);

		long int num_of_friendly_numbers = 0;
		for (long int i = 0; i < N; i++)
		{
			for (long int j = i + 1; j < N; j++)
			{
				if ((all_num[i] == all_num[j]) && (all_den[i] == all_den[j]))
				{
					num_of_friendly_numbers++;
					printf("%ld and %ld are FRIENDLY\n", all_the_num[i], all_the_num[j]);
				}
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &end_time);
		printf("Number of friendly numbers: %ld\n", num_of_friendly_numbers);
		printf("Tempo para somar a quantidade de números amigáveis: %.6f segundos\n",
			   get_time_diff(start_time, end_time));

		free(all_the_num);
		free(all_num);
		free(all_den);
		free(recv_counts);
		free(displs);
	}
}

int main(int argc, char **argv)
{
	int rank, size;
	long int start = 1;
	long int end = 262143;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0)
	{
		printf("Number %ld to %ld\n", start, end);
	}

	friendly_numbers_parallel(start, end, rank, size);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
