/*
 * Funções e tipos de dados para implementar uma pilha genérica
 */

#ifndef _PILHA_H
#define _PILHA_H

// Tipo de dados para representar a pilha
typedef struct pilha {
    struct pilha
	    *p_prox;	// próximo elemento da pilha
    void    *p_dados;	// dados associados a esse elemento
}
    pilha_t;

// Criação de pilhas
extern int cria_pilha(pilha_t **pilha);

// Empilhamento de dados (coloca dados na pilha)
extern int empilha(pilha_t *pilha, void *dados);

// Desempilhamento de dados (retira dados da pilha)
extern void *desempilha(pilha_t *pilha);

#endif // ndef _PILHA_H
