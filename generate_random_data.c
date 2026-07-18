#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: %s <quantidade_de_elementos>\n", argv[0]);
        return 1;
    }

    long quantidade = atol(argv[1]);

    if (quantidade <= 0)
    {
        printf("A quantidade de elementos deve ser maior que zero.\n");
        return 1;
    }

    char nomeArquivo[100];
    snprintf(nomeArquivo, sizeof(nomeArquivo),
             "./data/%ld_random_data.txt", quantidade);

    FILE *arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL)
    {
        printf("Erro ao criar o arquivo.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    for (long i = 0; i < quantidade; i++)
    {
        double valor = (double)rand() / RAND_MAX;
        fprintf(arquivo, "%.2f\n", valor);
    }

    fclose(arquivo);

    printf("Arquivo \"%s\" gerado com %ld valores.\n",
           nomeArquivo, quantidade);

    return 0;
}
