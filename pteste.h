#ifndef PTESTE_H
#define PTESTE_H

#define N_NUMS 3

/*-----------------------------------------------------------*
 *  Variáveis globais
 *-----------------------------------------------------------*/

extern int Num_falhas;	// Casos em que esperado != obtido
extern int Num_tarefas;	// Tarefas executando em paralelo
extern int Num_casos;	// Total de casos a serem testados

/*-----------------------------------------------------------*
 *  Funções globais
 *-----------------------------------------------------------*/

// Função sob teste: recebe 2 ints; devolve 1 int
extern int Combinacoes(int n, int r);

#endif /* PTESTE_H */
