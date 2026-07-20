CC       = gcc
OMPFLAGS = -fopenmp

TARGETS  = main main_omp

run: $(TARGETS)
	@echo "=== Executando versão sequencial ==="
	time -p ./main
	@echo "\n=== Executando versão OpenMP (OMP_NUM_THREADS=8) ==="
	export OMP_NUM_THREADS=8 && time -p ./main_omp

main: main.c
	$(CC) -o $@ $<

main_omp: main_omp.c
	$(CC) $(OMPFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: run clean
