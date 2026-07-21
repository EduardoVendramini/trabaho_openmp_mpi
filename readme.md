# Trabalho 2 — Avaliação OpenMP / MPI

## Resumo

Este repositório contém implementações sequenciais e paralelas (OpenMP e MPI) de três problemas usados no trabalho: `Bucket`, `friendly` e `Histo`. O objetivo é comparar desempenho e demonstrar paralelização em trechos críticos.

## Estrutura do repositório

- `Bucket/` — implementação sequencial e OpenMP (ordenação por baldes).
- `friendly/` — detecção de números "amigos" (sequencial, OpenMP, versão MPI não otimizada).
- `histo/` — construções de histograma (sequencial e OpenMP, versão MPI com desempenho pior).
- `analysis.sh`, `readme.md`, e outros arquivos auxiliares.

## Pré-requisitos

- GCC com suporte a OpenMP (ex.: `gcc`)
- MPI (ex.: `mpicc` / OpenMPI)
- Em Windows, recomenda-se usar WSL para execução com ferramentas Unix.

## Como compilar e executar

Exemplos rápidos (executar dentro de cada pasta ou ajustar caminhos):

No WSL (ex.: pasta `Bucket`):

```bash
cd Bucket
# compilação sequencial
gcc -o main main.c
time ./main

# compilação OpenMP (defina OMP_NUM_THREADS antes)
export OMP_NUM_THREADS=8
gcc -fopenmp -o main_omp main_omp.c
time ./main_omp

# compilação MPI (se implementado)
mpicc -o main_mpi main_mpi.c
mpirun -np 4 ./main_mpi
```

## Observações por módulo

Bucket

- Paralelização: foi usado um único `#pragma omp parallel for schedule(dynamic)` no trecho de ordenação dos buckets:

```c
#pragma omp parallel for schedule(dynamic)
for (int i = 0; i < bucketCount; i++) {
    if (bucketSizes[i] > 0)
        insertionSort(buckets[i], bucketSizes[i]);
}
```

- Justificativa: tamanhos dos baldes variam; `dynamic` melhora balanceamento entre threads.
- Resultado: speedup observado com OpenMP.

friendly

- Paralelização: aplicado `#pragma omp parallel for` em trechos de dupla iteração e em cálculos auxiliares.
- Exemplo usado:

```c
#pragma omp parallel for private(j) reduction(+ : num_of_friendly_numbers)
for (i = 0; i < last; i++) {
    for (j = i + 1; j < last; j++) {
        if ((num[i] == num[j]) && (den[i] == den[j])) {
            num_of_friendly_numbers++;
            printf("%ld and %ld are FRIENDLY\n", the_num[i], the_num[j]);
        }
    }
}
```

- Uso de `private`/`reduction` para evitar condições de corrida.
- Resultado: OpenMP trouxe melhora; a versão MPI ficou complexa e apresentou pior desempenho.

Histo

- Paralelização aplicada aos loops de contagem:

```c
#pragma omp parallel for private(j) shared(hist, results)
for (i = 0; i < RANGE; ++i) {
    for (j = 0; j < N; j++) {
        if (hist[j] == i)
            results[i]++;
    }
}
```

- `hist` é somente leitura (compartilhado); `results` é escrito por diferentes threads em posições distintas.
- Resultado: OpenMP proporciona speedup; MPI não apresentou ganhos nesse caso.
