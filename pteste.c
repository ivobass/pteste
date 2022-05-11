#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "pteste.h"

/*
 * Variáveis globais
 */
int Num_tarefas = 0;
int Num_casos   = 0;
int Num_falhas  = 0;

/*
 * main
 *
 * Ponto de entrada (início) do programa
 */
int main(int argc, char *argv[])
{
    int     ntarefas, nums[N_NUMS];
    FILE    *fcasos;
    char    linha[BUFSIZ];

    // Se o número de argumentos for inválido, terminamos
    if( argc != 3 )
    {
	fprintf(stderr, "\nUse: %s ntarefas casos.txt\n\n", argv[0]);
	return 1001;
    }

    // Se o número de tarefas não for um inteiro >= 1, também terminamos
    if( (ntarefas = atoi(argv[1])) < 1 )
    {
	fprintf(stderr,
	    "\nO número de tarefas deve ser inteiro, e maior que 0\n\n");
	return 1002;
    }

    // Abrimos o arquivo de casos, e também terminamos em caso de erro
    if( (fcasos = fopen(argv[2], "r")) == NULL )
    {
	perror(argv[2]);
	return errno;
    }

    // Agora que temos certeza de que os parâmetros passados estão ok,
    // podemos criar as tarefas
    Num_tarefas = ntarefas;
    while( ntarefas-- )
    	// Se houver erro na criação, terminamos, indicando o problema
	if( nova_tarefa() )
	{
	    perror("Criando tarefas");
	    return errno;
	}

    /*
     * Inicializa a pilha, para que possamos empilhar parâmetros de casos,
     * a serem executados pelas tarefas
     */
    if( inicializa_pilha() )
    {
	// Mas, se houver erro, indica o problema
	perror("Inicializando a pilha");
	return errno;
    }

    // Agora podemos ler o arquivo de casos, linha a linha
    while( fgets(linha, sizeof(linha), fcasos) == linha )
    {
	// lendo 3 inteiros em cada linha, ...
	sscanf(linha, " %d %d %d ", nums, nums+1, nums+2);
	// ... e empilhando os argumentos lidos
	if( empilha(nums) )
	{
	    perror("Empilhando parâmetros");
	    return errno;
	}
    }
    // Ao final, fechamos o arquivo de casos
    fclose(fcasos);

    // Imprimimos os resultados
    printf("Resultados:\nSucesso: %d\nFalhas: %d\nTotal de casos: %d\n",
	Num_casos - Num_falhas,
	Num_falhas,
	Num_casos
    );

    // E retornamos, indicando o sucesso de todo o processo
    return 0;
}
