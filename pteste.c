/*
 * UC: 21111 - Sistemas Operativos
 * e-fólio B 2022 (pteste.c)
 *
 * Autor: 000000 - Marcelo Gomes
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "pteste.h"
#include "tarefas.h"

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
    int     ntarefas;
    FILE    *fcasos;
    char    linha[BUFSIZ];
    caso_t  *caso;

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

    // Inicializa as estruturas de dados usadas pelas rotinas
    // de manipulação de tarefas e da pilha de casos
    inicializa_tarefas();

    // Agora podemos ler o arquivo de casos, linha a linha
    while( fgets(linha, sizeof(linha), fcasos) == linha )
    {
	// Aloca memória para os parâmetros e, se falhar,
	if( (caso = malloc(sizeof(caso_t))) == (caso_t *) NULL )
	    // indica a condição de erro
	    return errno;

	// Armazena o número da linha que acaba se ser lida
	// como o número do caso a testar
	caso->num_caso = ++Num_casos;

	// lê 3 inteiros em cada linha, ...
	sscanf(linha, " %d %d %d ",
	    & caso->param1,
	    & caso->param2,
	    & caso->esperado
	);

	// ... e empilha o caso de teste recém-lido
	if( empilha_caso(& Pilha_casos, caso) ) {
	    perror("Empilhando um caso lido");
	    return errno;
	}
    }
    // Ao final, fechamos o arquivo de casos
    fclose(fcasos);

    // Agora que temos certeza de que os parâmetros passados estão ok,
    // e que os casos estão na pilha, podemos criar as tarefas.
    while( ntarefas-- )
	nova_tarefa();

    // Agora as tarefas farão todo o processamento. Só o que temos
    // a fazer é Aguardar até que todas elas tenham terminado.
    aguarda_tarefas();

    // Em seguida, imprimimos o sumário dos resultados,
    printf("Resultados:\nSucesso: %d\nFalhas: %d\nTotal de casos: %d\n",
	Num_casos - Num_falhas,
	Num_falhas,
	Num_casos
    );

    // imprimimos o relatório, caso a caso, dos que tiveram erros
    imprime_relatorio_de_casos(REL_SOMENTE_ERROS);

    // ... e retornamos, indicando o sucesso de todo o processo
    return 0;
}
