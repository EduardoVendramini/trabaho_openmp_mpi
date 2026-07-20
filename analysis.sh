#!/bin/bash

dirs=("Bucket" "friendly" "histo")

get_time() {
    local cmd="$1"
    if command -v /usr/bin/time >/dev/null 2>&1; then
        /usr/bin/time -f "%e" sh -c "$cmd >/dev/null 2>&1" 2>&1
    else
        ( time -p sh -c "$cmd >/dev/null 2>&1" ) 2>&1 | awk '/real/ {print $2}'
    fi
}

declare -a alg_names
declare -a seq_times
declare -a par_times
declare -a speedups

for dir in "${dirs[@]}"; do
    if [ ! -d "$dir" ]; then
        echo "Aviso: diretório '$dir' não encontrado. Pulando..." >&2
        continue
    fi

    echo "Processando $dir ..."

    cd "$dir" || continue

    if [ ! -f "main.c" ] || [ ! -f "main_omp.c" ]; then
        echo "  Arquivos fonte ausentes em $dir. Pulando..." >&2
        cd - >/dev/null || exit
        continue
    fi

    gcc -o main main.c 2>/dev/null
    if [ ! -x "main" ]; then
        echo "  Falha na compilação de main.c em $dir" >&2
        cd - >/dev/null || exit
        continue
    fi

    export OMP_NUM_THREADS=8
    gcc -fopenmp -o main_omp main_omp.c 2>/dev/null
    if [ ! -x "main_omp" ]; then
        echo "  Falha na compilação de main_omp.c em $dir" >&2
        cd - >/dev/null || exit
        continue
    fi

    seq_time=$(get_time "./main")
    par_time=$(get_time "./main_omp")

    if [[ -n "$seq_time" && -n "$par_time" && "$par_time" != "0" ]]; then
        speedup=$(echo "scale=4; $seq_time / $par_time" | bc -l 2>/dev/null)
        if [ -z "$speedup" ]; then
            speedup=$(awk "BEGIN {printf \"%.4f\", $seq_time / $par_time}")
        fi
    else
        speedup="N/A"
    fi

    alg_names+=("$dir")
    seq_times+=("$seq_time")
    par_times+=("$par_time")
    speedups+=("$speedup")

    cd - >/dev/null || exit
done

echo -e "\nAlgoritmo, tempo real sequencial (s), tempo real paralelizado (s), speedup"
for i in "${!alg_names[@]}"; do
    printf "%s, %s, %s, %s\n" \
        "${alg_names[$i]}" \
        "${seq_times[$i]}" \
        "${par_times[$i]}" \
        "${speedups[$i]}"
done
