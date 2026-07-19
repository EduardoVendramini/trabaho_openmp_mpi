#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    FILE *arquivo = fopen("1M_dados_aleatorios.txt", "w");

    if (arquivo == NULL)
    {
        printf("Erro ao criar o arquivo.\n");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < 1000000; i++)
    {
        double valor = (double)rand() / RAND_MAX;
        fprintf(arquivo, "%.2f\n", valor);
    }

    fclose(arquivo);

    printf("Arquivo gerado com sucesso!\n");

    return 0;
}