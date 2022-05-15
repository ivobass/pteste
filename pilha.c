/*
 * Funções para implementar uma pilha genérica
 */

#include "pilha.h"
#include <stdlib.h> // por conta de malloc() e free()

/*
 * Cria uma pilha
 */
int cria_pilha(pilha_t **pilha)
{
    // Aloca memória para a primeira entrada da pilha e, se falhar,
    if( (*pilha = malloc(sizeof(pilha_t))) == (pilha_t *) NULL )
	// indica a condição de erro
	return 1;

    // Inicializa o ponteiro para o próximo elemento
    (*pilha)->p_prox = (pilha_t *) NULL;

    // Esse primeiro elemento, na verdade, é 'fake', e só
    // existe para evitar tratamento de exceções

    return 0;
}

/*
 * Coloca dados na pilha
 */
int empilha(pilha_t *pilha, void *dados)
{
    pilha_t *p;

    // Aloca memória para a entrada na pilha e, se falhar,
    if( (p = malloc(sizeof(pilha_t))) == (pilha_t *) NULL )
	// indica a condição de erro
	return 1;

    // Preenche essa entrada com os parâmetros
    p->p_params = dados;
    // ... e indica que este é o último elemento da pilha
    p->p_prox = (pilha_t *) NULL;

    // Percorre a pilha até o último elemento
    while( pilha->p_prox )
	pilha = pilha->p_prox;

    // Coloca o elemento recém-alocado na pilha
    pilha->p_prox = p;

    // Retorna indicando que tudo correu bem
    return 0;
}

/*
 * Remove da pilha um conjunto de dados
 */
void *desempilha(pilha_t *pilha)
{
    pilha_t *p, *q = pilha;
    void    *dados;

    for( p = pilha; p->p_prox; p = p->p_prox )
	q = p;

    // Se for o primeiro elemento da lista encadeada, é o
    // elemento 'fake', o que indica que não há elementos na pilha
    if( p == pilha )
	return (void *) NULL;
    // caso contrário,
    else
    {
	// Marcamos o fim da pilha no elemento anterior,
	q->p_prox = (pilha_t *) NULL;

	// guardamos os dados a serem retornados,
	dados = p->dados;

	// liberamos a memória ocupada por esse elemento de pilha,
	free(p);

	// e finalmente entregamos os dados ao chamador.
	return dados;
    }
}
