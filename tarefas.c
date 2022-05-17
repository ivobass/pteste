#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "pteste.h"
#include "pilha.h"
#include "tarefas.h"

/*-----------------------------------------------------------*
 *  Variáveis locais ao módulo (possuem sufixo l_...)
 *-----------------------------------------------------------*/

// Variáveis para a implementação da pilha de casos
static pilha_t		l_pilha_relat; // pilha de casos processados

// Mutexes para proteger variáveis globais em
// acessos potencialmente concorrentes nas tarefas
static pthread_mutex_t	l_mtx_num_falhas;   // para Num_falhas
static pthread_mutex_t	l_mtx_pilha_casos;  // para a pilha de casos
static pthread_mutex_t	l_mtx_pilha_relat;  // para a pilha de relatõrio

// Variáveis para manutenção das tarefas (threads)
static pthread_t	*l_thread_array; // Array de tarefas
static pthread_attr_t	l_thread_attr;   // Atributos das tarefas

/*-----------------------------------------------------------*
 *  Variáveis globais (iniciam por letra maíuscula)
 *-----------------------------------------------------------*/
pilha_t	Pilha_casos;	// pilha de casos a serem processados

/*-----------------------------------------------------------*
 *  Funções locais
 *-----------------------------------------------------------*/

/*
 * resultado_diferente
 *
 * Testa um caso, para ver se o resultado obtido passando-se os
 * parâmetros do caso à função sob teste corresponde a um valor 
 * especificado naquele caso.
 *
 * Retorna:
 *	0 - o resultado esperado é igual ao resultado obtido
 *	1 - o resultado esperado difere do resultado obtido
 *
 * Quando a macro de pré-processamento PARALELIZA_PRINTS estiver
 * definida, essa função pode ser cancelada por pthread_cancel()
 *
 * Fora esse detalhe, essa função é pthread-safe.
 */
int resultado_diferente(caso_t *c) {
    // Se o ponteiro para o caso for nulo, temos um erro.
    if( c == (caso_t *) NULL )
	// Nesse caso, assumimos que o resultado difere
	// do esperado
	return 1;

    // Chamamos a função sob teste (Combinacoes) para
    // calcular o resultado com os parâmetros desse caso
    c->obtido = Combinacoes(c->param1, c->param2);

    // ... e o comparamos com o resultado esperado.
    if( c->obtido == c->esperado )
	// Se forem iguais, retorna 0, sinalizando isso
	return 0;
    else {
    // Se devemos imprimir os diagnósticos dentro das threads
    // (ou seja, paralelizar os prints de diagnósticos),
    #ifdef PARALELIZA_PRINTS
	// imprimimos aqui quaisquer resultados divergentes
	// constatados em uma chamada à função sob teste,
	// no caso, Combinacoes().
	printf("Erro no caso %d: input(%d, %d) esperado/observado: %d / %d\n",
	    c->num_caso, // Número do caso no arquivo
	    c->param1,   // Primeiro parâmetro do caso
	    c->param2,   // Segundo parâmetro do caso
	    c->esperado, // Resultado esperado com tais parâmetros
	    c->obtido    // Resultado obtido com tais parâmetros
	);
	/*
	 * Repare que, como isso é executado pelas threads,
	 * todas rodando em paralelo, a saída pode vir a
	 * ficar toda misturada, e completamente ilegível,
	 * se tivermos muitas threads em execução, com partes
	 * das frases de cada thread se entremeando com partes
	 * de frases das demais threads.
	 *
	 * Isso pode até não ocorrer, já que cada thread
	 * efetua uma única chamada à printf, e as threads
	 * passam boa parte de seu tempo ocupadas na função
	 * sob teste, mas temos que ter em mente a possibilidade
	 * de vir a ocorrer algo assim.
	 */
    #endif // PARALELIZA_PRINTS

	// tendo havido ou não uma impressão, retorna 1,
	// para sinalizar que houve diferença entre os
	// resultados esperado e obtido.
	return 1;
    }
}

/*
 * tarefa
 *
 * Trata um conjunto de casos, removendo-os, um por vez,
 * da pilha de parâmetros de casos.
 *
 * Essa rotina é thread-safe, pois faz uso de mutex para
 * proteger a manipulação dos ponteiro da pilha,
 * e (normalmente) não faz uso de funções não-thread-safe
 * Mas veja a descrição da macro PARALELIZA_PRINTS, usada
 * na função resultad0_diferente(), acima.
 *
 * Inclusive, ela TEM de ser thread-safe, pois é ela que
 * implementa as threads.
 */
void *tarefa(void *args) {
    caso_t  *c; // ponteiro para o caso removido da pilha

    // Enquanto houverem casos a serem processados na pilha
    while( (c = desempilha_caso(& Pilha_casos)) != (caso_t *) NULL ) {
	// Confere o resultado obtido com o esperado e,
	// havendo diferença,
	if( resultado_diferente(c) ) {
	    // incrementa o contador de falhas, que
	    // tem de estar protegido por um mutex
	    pthread_mutex_lock(& l_mtx_num_falhas);
	    ++Num_falhas;
	    pthread_mutex_unlock(& l_mtx_num_falhas);
	}

	// Guarda o caso processado para o relatório final
	empilha_caso(& l_pilha_relat, c);
    }

    // Só para o compilador ficar feliz:
    return (void *) NULL;
}

/*
 * desempilha_caso
 *
 * Remove da pilha um caso pronto para ser proecessado por
 * uma das threads do programa.
 *
 * Essa rotina é thread-safe, pois faz uso de mutex para
 * proteger a manipulação dos ponteiro da pilha
 *
 * Retorna:
 *	Ponteiro para o caso desempilhado, se houver um,
 *	NULL, se não houver, ou em caso de erro.
 *
 * Para distinguir entre a ausência de casos na pilha
 * ou a ocorrência de um erro, o chamador pode usar a
 * variável errno, que conterá zero para indicar sucesso,
 * ou o código de erro, se tiver havido algum.
 */
caso_t *desempilha_caso(pilha_t *pilha) {
    // mutex a ser usado para proteger o acesso à pilha
    pthread_mutex_t	*mtx;
    caso_t		*c = (caso_t *) NULL; // Caso a ser devolvido
    // default: não há caso na pilha, ou a pilha é desconhecida

    // Verifica qual é a pilha a ser consultada,
    // e determina o mutex apropriado para proteger
    // a chamada à rotina de manipulação de pilhas
    if( pilha == & Pilha_casos ) {
    	mtx = & l_mtx_pilha_casos;
    } else if( pilha == & l_pilha_relat ) {
    	mtx = & l_mtx_pilha_relat;
    } else
	return c; // pilha desconhecida; não há caso a retornar

    // Protege com o mutex apropriado a chamada à
    // rotina genérica de manipulação de pilhas,
    // guardando o resutado
    pthread_mutex_lock(mtx);
    c = (caso_t *) desempilha(pilha);
    pthread_mutex_unlock(mtx);

    // Ao final, retorna o caso apropriado ao chamador
    return c;
}

/*
 * empilha_caso
 *
 * Coloca os dados de um caso na pilha de casos para relatorio
 *
 * Essa rotina é thread-safe, pois faz uso de mutex para
 * proteger a manipulação dos ponteiro da pilha
 *
 * pilha: ponteiro para a pilha onde deve empilhar o caso
 * caso:  ponteiro para o caso de teste a ser empilhado
 *
 * Retorna:
 *	0	- sucesso
 *	EINVAL	- o ponteiro para a pilha é nulo
 *	ENOMEM	- houve falha na tentativa de alocar memória
 *		  para os dados a serem empilhados, ou para
 *		  a posição da pilha onde eles teriam de
 *		  ser colocados
 */
int empilha_caso(pilha_t *pilha, caso_t *caso) {
    int     		ret;  // valor a ser retornado
    pthread_mutex_t	*mtx; // mutex a ser usado

    // Verifica qual é a pilha a ser consultada,
    // e determina o mutex apropriado para proteger
    // a chamada à rotina de manipulação de pilhas
    if( pilha == & Pilha_casos )
    	mtx = & l_mtx_pilha_casos;
    else if( pilha == & l_pilha_relat )
    	mtx = & l_mtx_pilha_relat;
    else
	mtx = (pthread_mutex_t *) NULL;

    // Colocamos esses parâmetros na pilha,
    // cujo acesso tem de estar protegido por mutex.
    if( mtx )
	pthread_mutex_lock(mtx);
    ret = empilha(pilha, caso);
    if( mtx )
	pthread_mutex_unlock(mtx);

    // Retornamos o valor que nos tenha sido retornado
    // pela função de empilhamento genérica, empilha()
    return ret;
}

/*
 * Funções de testes, usadas pela imprime_relatorio_de_casos
 */
static int teste_erros   (int a, int b) { return a != b; }
static int teste_acertos (int a, int b) { return a == b; }
static int teste_completo(int a, int b) { return 1; }

/*-----------------------------------------------------------*
 *  Funções globais
 *-----------------------------------------------------------*/

/*
 * imprime_relatorio_de_casos
 *
 * tipo: tipo do relatorio a imprimir:
 *		REL_NENHUM	- nenhum caso é impresso;
 *		REL_ERROS	- somente erros são impressos
 *		REL_CORRETOS	- somente acertos são impressos
 *		REL_COMPLETO	- todos os casos são impressos
 */
void imprime_relatorio_de_casos(tipo_relatorio_t tipo) {
    int     (*teste)(int a, int b); // função de comparação
    caso_t  *c;		// caso corrente
    char    *fmt;	// formato de impressão para a printf()
    char    *aux;	// string auxiliar para a printf()

    // Ajusta os parâmetros do relatório conforme o tipo desejado
    switch (tipo) {
    case REL_NENHUM:
	return; // nada a fazer; podemos simplesmente retornar

    case REL_SOMENTE_ERROS:
	fmt   = "%s [%d](%d, %d): esperado %d; obtido %d\n";
	teste = teste_erros; // casos em que esperado != obtido
	break;

    case REL_SOMENTE_CORRETOS:
	fmt   = "%s [%d](%d, %d): esperado = obtido = %d\n";
	teste = teste_acertos; // casos em que esperado == obtido
	break;

    case REL_COMPLETO:
	fmt   = "%s [%d](%d, %d): esperado %d; obtido %d\n";
	teste = teste_completo; // sempre retorna 1
	break;
    }

    // Busca na pilha cada um dos casos já processados
    while( (c = desempilha_caso(& l_pilha_relat)) ) {
	// se o caso corrente é do tipo desejado,
	if( (*teste)(c->esperado, c->obtido) ) {
	    // ajusta a string auxiliar, conforme o caso,
	    aux = (c->esperado == c->obtido) ?
		"Acerto no caso" :
		"Erro no caso";

	    // e imprime o caso, com os parâmetros apropriados
	    printf(
		fmt,
		aux,
		c->num_caso,
		c->param1,
		c->param2,
		c->esperado,
		c->obtido
	    );
	}

	// Liberamos a memória ocupada por esse caso
	free(c);
    }
}

/*
 * inicializa_tarefas
 *
 * Inicializa todas as variáveis das quais esse módulo depende
 */
void inicializa_tarefas(void) {
    pthread_t *thrd;

    // Tenta inicializar as pilhas de casos e,
    if( cria_pilha(& Pilha_casos) || cria_pilha(& l_pilha_relat) )
	// se não conseguir, não teremos o que fazer no programa
	exit(-1);

    // Tenta alocar memória para o primeiro elemento do array de threads
    if( (thrd = malloc(sizeof(pthread_t))) == (pthread_t *) NULL )
	// e, se não conseguir, não teremos o que fazer no programa
	exit(-1);

    // Guarda a área alocada para o array de threads numa variável global
    l_thread_array = thrd;

    // inicializa os atributos com valores default
    pthread_attr_init(& l_thread_attr);
    // modifica estado de desacoplamento para "joinable"
    pthread_attr_setdetachstate(& l_thread_attr, PTHREAD_CREATE_JOINABLE);
}

/*
 * nova_tarefa
 *
 * Cria uma nova tarefa (thread de proecessamento)
 */
void nova_tarefa(void) {
    // Se estivermos criando a primeira tarefa,
    if( Num_tarefas == 0 ) {
	// Aloca espaço para a primeira posição do array de tarefas
	l_thread_array = malloc(sizeof(pthread_t));

	// Incrementa o número de tarefas
	// Apesar de ser uma global, não precisamos de mutex aqui,
	// pois essa variável só é consultada ou alterada na thread principal
	// ou seja, na thread da função main()
	++Num_tarefas;
    } else
	// Aloca espaço para mais uma tarefa no array de tarefas
	l_thread_array = realloc(l_thread_array, ++Num_tarefas * sizeof(pthread_t));

    // Se não conseguir alocar,
    if( l_thread_array == (pthread_t *) NULL ) {
	// Imprime a mensagem de erro apropriada,
	perror("Alocando memória para mais uma tarefa");
	// ... e termina o programa
	exit(errno);
    }

    // Agora pode-se tentar criar a tarefa, mas,
    // se a tentativa não for bem-sucedida,
    if( pthread_create(
	    l_thread_array + Num_tarefas - 1,
	    & l_thread_attr,
	    tarefa,
	    (void *) NULL)
      ) {
	// imprimimos uma mensagem sobre o erro ocorrido,
	perror("Criando mais uma tarefa");
	// e terminamos o programa.
	exit(errno);
    }
}

/*
 * Aguarda até que todas as tarefas tenham terminado
 * de processar os dados presentes na pilha
 */
void aguarda_tarefas(void) {
    int     i;

    // Apenas chama pthread_join para cada tarefa existente
    for( i = 0; i < Num_tarefas; i++ )
	pthread_join(l_thread_array[i], (void **) NULL);
}
