cd ./Bucket

wsl

gcc -o main main.c
time ./main

export OMP_NUM_THREADS=8
gcc -fopenmp -o main_omp main_omp.c
time ./main_omp
