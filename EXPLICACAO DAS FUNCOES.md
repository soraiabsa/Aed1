EXPLICACAO DAS FUNCOES - codigo_ofc.c

Estruturas e tipos
- Registro: guarda uma leitura do sensor (ID, data, temperatura, umidade e PM2.5).
- Medida: guarda o resultado de uma medicao (numero de comparacoes e tempo em segundos).
- Consulta: tipo "ponteiro para funcao". Serve para passar consultaA, selecaoB ou
  selecaoC como parametro da funcao medir. Todas recebem (vetor, n, k, contador)
  e devolvem o valor que ficaria na posicao k do vetor ordenado.

Funcoes auxiliares
- gerarDados: preenche o vetor com n registros aleatorios (sensor, data, temperatura,
  umidade entre 15 e 90 e PM2.5 entre 5 e 300).
- copiaVet: copia um vetor de registros para outro. Cada abordagem trabalha numa copia
  para nao bagunçar o vetor original.
- trocar: troca de lugar dois registros.

Abordagem A
- bolha: ordena o vetor pelo PM2.5 (bubble sort) e conta as comparacoes feitas.
  Sempre faz n*(n-1)/2 comparacoes.
- consultaA: chama a bolha para ordenar tudo e devolve o elemento da posicao k.

Abordagem B
- selecaoB: escolhe o elemento do meio como pivo e separa o vetor em tres regioes:
  menores que o pivo, iguais ao pivo e maiores que o pivo.
    * inicioIguais e fimIguais marcam onde comeca e onde termina a regiao dos iguais.
    * limiteEsq e limiteDir marcam a janela onde a resposta ainda pode estar.
  Se k cai na regiao dos iguais, o pivo e a resposta. Se nao, a busca continua so no
  lado onde k esta, descartando o resto. O return 0.0f do final nunca executa
  (so existe para o compilador nao reclamar).

Abordagem C
- selecaoC: para cada elemento, conta quantos sao menores e quantos sao iguais a ele.
  Se k esta entre "menores" e "menores + iguais - 1", esse elemento e a resposta.
  Os iguais sao contados para a funcao nao se perder quando ha valores repetidos.
  O return 0.0f do final nunca executa.

Medicao
- medir: faz uma copia do vetor, zera o contador, roda a funcao escolhida
  (A, B ou C), mede o tempo com clock() e devolve o valor encontrado.
- teste: gera n registros, roda as tres abordagens para o indice n/2 e imprime
  comparacoes e tempo de cada uma. Avisa se os resultados forem diferentes.

main
- le a quantidade de registros (1 a 10000) e gera os dados.
- cria uma copia onde a umidade fica no campo Nivel_PM25, assim as mesmas funcoes
  (A, B e C) tambem selecionam por umidade, sem precisar de funcoes novas.
- para cada abordagem calcula tres valores: mediana do PM2.5, percentil 90 do PM2.5
  e a umidade do 10o dia mais seco, mostrando as comparacoes de cada um.
- confere se as tres abordagens deram o mesmo resultado.
- ordena por umidade com a bolha para listar as 10 leituras mais secas.
- chama teste com 1000, 5000 e 10000 registros para comparar o crescimento.