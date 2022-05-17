/***********************************************************************
 * Funções e tipos de dados para efetuar o gerenciamento das
 * tarefas de processamento dos testes
 **********************************************************************/

#ifndef _TAREFAS_H
#define _TAREFAS_H

#include "pilha.h"

/***********************************************************************
 * Tipos de dados e variáveis para tratar dos casos de teste
 **********************************************************************/
// Tipo de dados representando um caso de teste
typedef struct caso {
    int num_caso;
    int param1;
    int param2;
    int esperado;
    int obtido;
}
    caso_t;

// Tipo de dados para indicar o tipo de relatório
// de casos que desejamos ao final do programa
typedef enum {
    REL_NENHUM,
    REL_SOMENTE_ERROS,
    REL_SOMENTE_CORRETOS,
    REL_COMPLETO
}
    tipo_relatorio_t;

// pilha de casos a serem processados
pilha_t	Pilha_casos;

/***********************************************************************
 * Funções de manipulação da pilha de casos
 **********************************************************************/

// Acrescenta um caso a uma pilha de casos
extern int empilha_caso(pilha_t *pilha, caso_t *caso);

// remove da pilha o caso que está no topo,
// e o entrega ao chamador
extern caso_t *desempilha_caso(pilha_t *pilha);

// Imprime a pilha de sumário de resultados
extern void imprime_relatorio_de_casos(tipo_relatorio_t tipo);

/***********************************************************************
 * Funções de manipulação de tarefas
 *
 * (mas a inicializa_tarefas() também inicializa a pilha de casos)
 **********************************************************************/

// Inicializa as estruturas de dados para tratar de
// tarefas, bem como as estrutras da pilha de casos
extern void inicializa_tarefas(void);

// Cria uma nova tarefa
extern void nova_tarefa(void);

// Aguarda até que todas as tarefas tenham terminado
extern void aguarda_tarefas(void);

#endif // ndef _TAREFAS_H
