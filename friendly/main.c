#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

void friendly_numbers(long int start, long int end)
{
	long int last = end - start + 1;

	long int *the_num;
	the_num = (long int *)malloc(sizeof(long int) * last);
	long int *num;
	num = (long int *)malloc(sizeof(long int) * last);
	long int *den;
	den = (long int *)malloc(sizeof(long int) * last);

	long int i, j, factor, ii, sum, done, n;

	clock_gettime(CLOCK_MONOTONIC, &start_time);
	for (i = start; i <= end; i++)
	{
		ii = i - start;
		sum = 1 + i;
		the_num[ii] = i;
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
		num[ii] = sum;
		den[ii] = i;
		n = gcd(num[ii], den[ii]);
		num[ii] /= n;
		den[ii] /= n;
	} // end for
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	printf("Tempo para identificar os números amigáveis: %.6f segundos\n", get_time_diff(start_time, end_time));

	clock_gettime(CLOCK_MONOTONIC, &start_time);
	long int num_of_friendly_numbers = 0;
	for (i = 0; i < last; i++)
	{
		for (j = i + 1; j < last; j++)
		{
			if ((num[i] == num[j]) && (den[i] == den[j]))
			{
				num_of_friendly_numbers++;
				printf("%ld and %ld are FRIENDLY\n", the_num[i], the_num[j]);
			}
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	printf("Number of friendly numbers: %ld\n", num_of_friendly_numbers);
	printf("Tempo para somar a quantidade de números amigáveis: %.6f segundos\n", get_time_diff(start_time, end_time));

	free(the_num);
	free(num);
	free(den);
}

int main(int argc, char **argv)
{
	long int start = 1;
	long int end = 262143;
	// long int end = 1000;

	printf("Number %ld to %ld\n", start, end);
	friendly_numbers(start, end);

	return EXIT_SUCCESS;
}
