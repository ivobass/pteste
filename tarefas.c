#include <stdio.h>
#include <stdlib.h>

#include "pteste.h"

/*-----------------------------------------------------------*
 *  Variáveis globais
 *-----------------------------------------------------------*/

extern int Num_falhas;
extern int Num_tarefas;
extern int Num_casos;

/*-----------------------------------------------------------*
 * Tipo de dados local:
 * implementa a pilha de parâmetros
 *-----------------------------------------------------------*/
typedef struct pilha
{
    int     *p_params;
    struct pilha
	    *p_prox;
}
    pilha_t;

/*-----------------------------------------------------------*
 *  Variáveis locais
 *-----------------------------------------------------------*/

static pilha_t	*pilha; // Ponteiro para a pilha

/*-----------------------------------------------------------*
 *  Funções locais
 *-----------------------------------------------------------*/
pilha_t *desempilha(void)
{
    pilha_t *p, *q = pilha;

    for( p = pilha; p->p_prox; p = p->p_prox )
	q = p;

    // Se for o primeiro elemento da lista encadeada, é o
    // elemento 'fake', o que indica que não há elementos na pilha
    if( p == pilha )
	return (pilha_t *) NULL;
    else
    {
	q->p_prox = (pilha_t *) NULL;

	return p;
    }
}

int testa(int caso, int *n)
{
    int result = Combinacoes(n[0], n[1]);

    if( result != n[2] )
    {
	printf("Erro no caso %d: input(%d, %d) esperado/observado: %d / %d\n",
	    caso, n[0], n[1], n[2], result);
	return 1;
    } else
	return 0;
}


/*-----------------------------------------------------------*
 *  Funções globais
 *-----------------------------------------------------------*/

int inicializa_pilha(void)
{
    // Aloca memória para a primeira entrada da pilha e, se falhar,
    if( (pilha = malloc(sizeof(pilha_t))) == (pilha_t *) NULL )
	// indica a condição de erro
	return 1;

    // Inicializa o ponteiro para o próximo elemento
    pilha->p_prox = (pilha_t *) NULL;
    // Esse elemento, na verdade, é 'fake', e só
    // existe para evitar tratamento de exceções

    return 0;
}

int nova_tarefa(void)
{
    return 0;
}

int empilha(int *nums)
{
    int     i, *n;
    pilha_t *p, *q;

    // Aloca memória para os parâmetros e, se falhar,
    if( (n = malloc((N_NUMS+1) * sizeof(int))) == (int *) NULL )
	// indica a condição de erro
	return 1;

    // Copiamos os parâmetros para a memória recém alocada
    for(i = 0; i < N_NUMS; i++)
    {
	n[i] = nums[i];
    }
    // ... incluindo o número do caso
    n[i] = ++Num_casos;

    // Aloca memória para a entrada na pilha e, se falhar,
    if( (q = malloc(sizeof(pilha_t))) == (pilha_t *) NULL )
	// indica a condição de erro
	return 1;
    // Preenche essa entrada com os parâmetros
    q->p_params = n;
    // ... e indica que este é o último elemento da pilha
    q->p_prox = (pilha_t *) NULL;

    // Percorre a pilha até o último elemento
    for( p = pilha; p->p_prox; p = p->p_prox )
	;
    // Coloca o elemento recém-alocado na pilha
    p->p_prox = q;

    Num_falhas += testa(n[N_NUMS], n);

    return 0;
}
