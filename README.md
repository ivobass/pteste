# pteste
Teste de função em C com pthreads

## Sintaxe de uso:

<pre>pteste {ntarefas} {casos.txt}</pre>

## Operação:

O programa recebe o número de tarefas a criar, e um ficheiro casos.txt, com os casos de teste,
em que cada linha é um caso de teste, com dois números a serem fornecidos à função em teste, e
um terceiro número que é o resultado esperado.

O programa efetua as seguintes operações na tarefa principal:
a) Lê o ficheiro casos.txt
b) Inicia as tarefas de teste
c) Quando todas as tarefas terminam, imprime o número de casos de teste passados, falhados e totais, bem como, para os casos de teste falhados, os dados de entrada e os valores esperado e observado, terminando de seguida.

As tarefas de teste efetuam as seguintes operações, em ciclo:
a) Executam o primeiro caso de teste fornecido, não atribuído; Caso não existam mais casos de teste, terminam;
b) Executam o teste e gravam o resultado, se o teste foi passado ou não, gravando o valor do resultado.
c) Retornam ao passo a)

Apesar das entradas e saídas serem definidas como inteiras, o arquivo de dados a ser lido pode conter vírgulas ou pontos, e o programa ainda assim os deverá tratar de forma adequada.
