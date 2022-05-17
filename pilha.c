/*
 * Funções para implementar uma pilha genérica
 */

#include "pilha.h"
#include <stdlib.h> // por conta de malloc() e free()
#include <errno.h>  // pelos códigos de erro (EINVAL)

/*
 * Cria uma pilha
 *
 * pilha: ponteiro para um ponteiro para a pilha,
 *	que será inicializado com o ponteiro para
 *	a pilha recém criada, em caso de sucesso,
 *	ou com NULL, caso contrário.
 *
 * Retorna:
 *	0 - indica sucesso
 *	1 - indica argumento inválido (ponteiro nulo)
 */
int cria_pilha(pilha_t *pilha)
{
    // Testa para ver se o ponteiro para ponteiro de
    // pilha é válido
    if( pilha == (pilha_t *) NULL )
	return errno = EINVAL;

    // Indica que ainda não há elementos na pilha
    *pilha = (pilha_t) NULL;

    // Informa que a criação foi bem-sucedida
    return 0;
}

/*
 * Coloca um elemento de dados na pilha
 *
 * pilha: ponteiro para a pilha a ser manipulada
 * dados: ponteiro para os dados a serem colocados na pilha
 *	  A área para onde 'dados' aponta deve ter sido
 *	  previamente alocada pelo chamador; no entanto,
 *	  é lĩcito empilhar um ponteiro nulo.
 *
 * Retorna:
 *	0 - indica sucesso
 *	1 - indica erro, e retorna o código em errno
 */
int empilha(pilha_t *pilha, void *dados)
{
    pilha_t p;

    // Testa para ver se o ponteiro para ponteiro de
    // pilha é válido
    if( pilha == (pilha_t *) NULL )
	return errno = EINVAL;

    // Aloca memória para a entrada na pilha e, se falhar,
    if( (p = malloc(sizeof(pilha_t))) == (pilha_t) NULL )
	// retorna o código indicando a condição de erro
	return errno;

    // Preenche essa entrada com os parâmetros
    p->p_dados = dados;
    // ... e indica que este é o último elemento da pilha
    p->p_prox = *pilha;

    // Coloca o elemento recém-alocado na pilha
    *pilha = p;

    // Retorna indicando que tudo correu bem
    return 0;
}

/*
 * Remove da pilha um elemento de dados
 *
 * pilha: ponteiro para a pilha a ser manipulada
 *
 * Retorna:
 *	ponteiro para os dados até então contidos no elemento
 *	recém removido da pilha
 */
void *desempilha(pilha_t *pilha)
{
    pilha_t p;
    void    *dados;

    // Testa para ver se o ponteiro para ponteiro de
    // pilha é válido
    if( pilha == (pilha_t *) NULL ) {
	errno = EINVAL;

	return (void *) NULL; // Se não houver, já retorna NULL
    }

    // Guarda o ponteiro para o elemento que até então
    // esteve no topo da pilha
    p = *pilha;

    // Testa para ver se ainda há algum elemento na pilha
    if( p == (pilha_t) NULL )
	return (void *) NULL; // Se não houver, já retorna NULL

    // Faz o ponteiro para a pilha apontar para o
    // elemento seguinte, efetivamente removendo o
    // elemento do topo
    *pilha = p->p_prox;

    // guardamos os dados a serem retornados,
    dados = p->p_dados;
    // liberamos a memória ocupada por esse elemento de pilha,
    free(p);
    // indicamos que não houve erro,
    errno = ENOERR;
    // e finalmente entregamos os dados ao chamador.
    return dados;
}
