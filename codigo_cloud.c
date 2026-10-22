/*
 * ICC002 - Algoritmos e Estruturas de Dados I - UFAM 2026/2
 * Analise da Qualidade do Ar e Clima de Manaus com Estatisticas de Ordem
 *
 * Tres abordagens para achar o k-esimo menor elemento:
 *   A) Ordenacao completa (bolha) e acesso direto a v[k]
 *   B) Selecao por particionamento (quickselect, pivo central)
 *   C) Contagem comparativa sequencial (O(n^2))
 *
 * Consultas: mediana do PM2.5, limiar critico (percentil 90) e os M dias mais secos.
 * Nenhuma funcao pronta de ordenacao/selecao da biblioteca padrao e usada.
 */

#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Estrutura de dados                                                  */
/* ------------------------------------------------------------------ */
typedef struct {
    int   ID_Sensor;
    char  Data[16];            // "dd/mm/2026" + '\0' (16 evita estouro)
    float Temperatura_C;
    int   Umidade_Porcentagem;
    float Nivel_PM25;
} Registro;

/* Qual campo cada algoritmo deve usar como chave de comparacao.
   Assim UM unico codigo de A, B e C serve para PM2.5 e para umidade. */
#define CAMPO_PM25    0
#define CAMPO_UMIDADE 1

/* Contador global de comparacoes (o "placar"). Zerado antes de cada medicao. */
long long comparacoes = 0;

/* Devolve o valor do campo escolhido como float (para comparar de forma uniforme). */
float chave(const Registro *r, int campo) {
    return (campo == CAMPO_PM25) ? r->Nivel_PM25 : (float)r->Umidade_Porcentagem;
}

/* Troca dois registros de lugar. */
void troca(Registro *a, Registro *b) {
    Registro aux = *a;
    *a = *b;
    *b = aux;
}

/* ------------------------------------------------------------------ */
/* Geracao e copia dos dados                                           */
/* ------------------------------------------------------------------ */

/* Preenche o vetor com medicoes simuladas (faixas realistas p/ Manaus). */
void gerarDados(Registro v[], int n) {
    for (int i = 0; i < n; i++) {
        v[i].ID_Sensor = rand() % 1000;                 // ID entre 0 e 999
        int mes = 1 + rand() % 12;
        int dia = 1 + rand() % 31;
        snprintf(v[i].Data, sizeof v[i].Data, "%02d/%02d/2026", dia, mes);
        v[i].Temperatura_C        = 25 + (rand() % 210) / 10.0f;   // 25.0 a 45.9
        v[i].Umidade_Porcentagem  = 15 + rand() % 76;              // 15 a 90
        v[i].Nivel_PM25           = 5 + (rand() % 2960) / 10.0f;   // 5.0 a 300.9
    }
}

/* Copia v para vetCopia. Cada algoritmo mexe na SUA copia e o original
   fica intacto, o que garante comparacao justa (mesmos dados p/ todos). */
void copiaVet(Registro v[], int n, Registro vetCopia[]) {
    for (int i = 0; i < n; i++) {
        vetCopia[i] = v[i];
    }
}

/* ------------------------------------------------------------------ */
/* ABORDAGEM A - Ordenacao completa (bolha) + acesso direto            */
/* ------------------------------------------------------------------ */

/* Bolha generica: ordena pelo campo escolhido.
   Faz sempre n(n-1)/2 comparacoes (melhor = pior caso em comparacoes;
   so o numero de trocas muda). Custo: Theta(n^2). */
void bolha(Registro v[], int n, int campo) {
    for (int j = 0; j < n - 1; j++) {
        for (int i = 0; i < n - 1 - j; i++) {
            comparacoes++;                                   // conta ANTES do if
            if (chave(&v[i], campo) > chave(&v[i + 1], campo)) {
                troca(&v[i], &v[i + 1]);
            }
        }
    }
}

/* Abordagem A: ordena tudo e le a posicao k (0-indexada).
   Mediana: k = n/2.  Percentil 90: k = ceil(0.9*n) - 1. */
float abordagemA(Registro v[], int n, int k, int campo) {
    bolha(v, n, campo);
    return chave(&v[k], campo);
}

/* ------------------------------------------------------------------ */
/* ABORDAGEM B - Selecao por particionamento (quickselect)             */
/* ------------------------------------------------------------------ */

/* Acha o k-esimo menor (k 0-indexado) sem ordenar tudo:
 *  1. escolhe o pivo no CENTRO da faixa [esq, dir];
 *  2. particiona: menores que o pivo ficam a esquerda, o resto a direita;
 *  3. o pivo cai na posicao final i. Se i == k, achamos.
 *     Se k < i, descartamos a direita; se k > i, descartamos a esquerda.
 * Diferente do quicksort, so UM lado e processado -> media O(n).
 * Efeito colateral util: ao terminar, v[0..k-1] sao todos <= v[k]
 * (usado para listar os M dias mais secos). */
float abordagemB(Registro v[], int n, int k, int campo) {
    int esq = 0, dir = n - 1;

    while (esq < dir) {
        int meio = esq + (dir - esq) / 2;      // pivo centralizado
        troca(&v[meio], &v[dir]);              // guarda o pivo na ponta direita
        float pivo = chave(&v[dir], campo);

        int i = esq;                           // fronteira dos menores que o pivo
        for (int j = esq; j < dir; j++) {
            comparacoes++;                     // 1 comparacao elemento x pivo
            if (chave(&v[j], campo) < pivo) {
                troca(&v[i], &v[j]);
                i++;
            }
        }
        troca(&v[i], &v[dir]);                 // pivo vai para sua posicao final i

        if (k == i)      return chave(&v[i], campo);   // achou
        else if (k < i)  dir = i - 1;                  // busca so na esquerda
        else             esq = i + 1;                  // busca so na direita
    }
    return chave(&v[esq], campo);              // faixa com 1 elemento: e ele
}

/* ------------------------------------------------------------------ */
/* ABORDAGEM C - Contagem comparativa sequencial                       */
/* ------------------------------------------------------------------ */

/* Para cada elemento v[i], varre TODO o vetor contando:
 *    menores = quantos sao estritamente menores que v[i]
 *    iguais  = quantos sao iguais a v[i] (inclui ele mesmo)
 * Entao v[i] ocupa as posicoes (0-indexadas) menores ... menores+iguais-1
 * na ordenacao. Se k esta nesse intervalo, v[i] e o k-esimo menor.
 *
 * TRATAMENTO DE EMPATES (Questao 2): se so contassemos "<" e testassemos
 * menores == k, com valores repetidos a posicao k poderia nunca ser
 * "exatamente" atingida (ex.: [45,90,30,90,60]: os dois 90 tem menores=3,
 * logo nenhum elemento teria menores==4 e k=4 nunca seria encontrado).
 * Com o intervalo [menores, menores+iguais-1] todo valor repetido cobre
 * as posicoes que lhe cabem e o algoritmo sempre termina com resposta.
 *
 * Comparacoes: contamos 1 por par (v[j], v[i]) examinado.
 * Melhor caso: o 1o elemento ja e a resposta -> n comparacoes.
 * Pior caso: a resposta so aparece no fim -> n*n comparacoes. */
float abordagemC(Registro v[], int n, int k, int campo) {
    for (int i = 0; i < n; i++) {
        float alvo = chave(&v[i], campo);
        int menores = 0, iguais = 0;

        for (int j = 0; j < n; j++) {
            comparacoes++;                              // 1 par comparado
            float x = chave(&v[j], campo);
            if (x < alvo)       menores++;
            else if (x == alvo) iguais++;
        }

        if (menores <= k && k < menores + iguais) {     // v[i] ocupa a posicao k
            return alvo;
        }
    }
    return -1.0f;   // inalcancavel se 0 <= k < n
}

/* ------------------------------------------------------------------ */
/* Utilitarios das consultas                                           */
/* ------------------------------------------------------------------ */

/* Indice (0-indexado) do percentil 90: ceil(0.9*n) - 1, em inteiros. */
int indicePercentil90(int n) {
    return (n * 90 + 99) / 100 - 1;
}

/* Insertion sort simples, usado SO para exibir em ordem os M menores
   (M e pequeno; nao entra na contagem de comparacoes dos algoritmos). */
static void ordenaPequeno(Registro v[], int m) {
    for (int i = 1; i < m; i++) {
        Registro x = v[i];
        int j = i - 1;
        while (j >= 0 && v[j].Umidade_Porcentagem > x.Umidade_Porcentagem) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = x;
    }
}

/* Consulta "M dias mais secos" via Abordagem B:
   seleciona o (M-1)-esimo menor umidade; o particionamento deixa os M
   menores nas posicoes 0..M-1 (fora de ordem); ordenamos so esses M. */
void diasMaisSecos(Registro v[], int n, int M) {
    comparacoes = 0;
    abordagemB(v, n, M - 1, CAMPO_UMIDADE);
    long long compB = comparacoes;

    ordenaPequeno(v, M);
    printf("Os %d dias mais secos (umidade relativa, Abordagem B, %lld comparacoes):\n", M, compB);
    for (int i = 0; i < M; i++) {
        printf("  %d%% de umidade (sensor %d, data %s)\n",
               v[i].Umidade_Porcentagem, v[i].ID_Sensor, v[i].Data);
    }
}

/* ------------------------------------------------------------------ */
/* Tabela de desempenho (numero de comparacoes)                        */
/* ------------------------------------------------------------------ */

/* Roda A, B e C para uma consulta (posicao k) e devolve comparacoes de cada uma.
   Cada abordagem recebe uma copia nova dos dados originais. */
static void medirConsulta(Registro orig[], Registro tmp[], int n, int k,
                          long long *cA, long long *cB, long long *cC) {
    copiaVet(orig, n, tmp); comparacoes = 0; abordagemA(tmp, n, k, CAMPO_PM25); *cA = comparacoes;
    copiaVet(orig, n, tmp); comparacoes = 0; abordagemB(tmp, n, k, CAMPO_PM25); *cB = comparacoes;
    copiaVet(orig, n, tmp); comparacoes = 0; abordagemC(tmp, n, k, CAMPO_PM25); *cC = comparacoes;
}

void tabelaDesempenho(void) {
    int tamanhos[3] = {1000, 5000, 10000};

    printf("\n=== TABELA COMPARATIVA (numero de comparacoes, PM2.5) ===\n");
    printf("%-8s | %-10s | %14s | %14s | %14s\n", "N", "Consulta", "A (bolha)", "B (partic.)", "C (contagem)");
    printf("---------+------------+----------------+----------------+----------------\n");

    for (int t = 0; t < 3; t++) {
        int n = tamanhos[t];
        Registro *orig = malloc(n * sizeof(Registro));
        Registro *tmp  = malloc(n * sizeof(Registro));
        if (!orig || !tmp) { printf("Sem memoria.\n"); exit(1); }

        srand(1);                         // mesma semente: resultados reproduziveis
        gerarDados(orig, n);

        long long a, b, c;
        medirConsulta(orig, tmp, n, n / 2, &a, &b, &c);
        printf("%-8d | %-10s | %14lld | %14lld | %14lld\n", n, "Mediana", a, b, c);

        medirConsulta(orig, tmp, n, indicePercentil90(n), &a, &b, &c);
        printf("%-8d | %-10s | %14lld | %14lld | %14lld\n", n, "P90", a, b, c);

        free(orig);
        free(tmp);
    }
}

/* ------------------------------------------------------------------ */
/* Questao 1 - rastreio passo a passo da Abordagem C                   */
/* ------------------------------------------------------------------ */
void tracoQuestao1(void) {
    int dados[5] = {45, 90, 30, 90, 60};
    int n = 5, k = 3;                 // k = 3 (indice 1) -> posicao 2 em indice 0
    int k0 = k - 1;
    long long total = 0;

    printf("\n=== QUESTAO 1: rastreio da Abordagem C em [45, 90, 30, 90, 60], k = 3 ===\n");
    for (int i = 0; i < n; i++) {
        int menores = 0, iguais = 0;
        for (int j = 0; j < n; j++) {
            total++;
            if (dados[j] < dados[i])       menores++;
            else if (dados[j] == dados[i]) iguais++;
        }
        printf("v[%d]=%d: menores=%d, iguais=%d -> ocupa posicoes %d..%d",
               i, dados[i], menores, iguais, menores + 1, menores + iguais);
        if (menores <= k0 && k0 < menores + iguais) {
            printf("  <-- contem k=%d: RESPOSTA = %d\n", k, dados[i]);
            break;
        }
        printf("  (nao contem k=%d)\n", k);
    }
    printf("Total de comparacoes diretas: %lld\n", total);
}

/* ------------------------------------------------------------------ */
/* main                                                                */
/* ------------------------------------------------------------------ */
int main(void) {
    srand(1);                          // semente fixa -> testes reproduziveis

    int n;
    printf("Informe a quantidade de registros (1 a 10000): ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > 10000) {
        printf("Informe uma quantidade entre 1 e 10000.\n");
        return 1;
    }

    /* malloc em vez de vetor na pilha: evita estouro de stack em N grande */
    Registro *vet     = malloc(n * sizeof(Registro));   // dados originais (nunca alterados)
    Registro *vetCopia = malloc(n * sizeof(Registro));  // copia de trabalho
    if (!vet || !vetCopia) { printf("Sem memoria.\n"); return 1; }

    gerarDados(vet, n);

    /* Mostra os registros so se forem poucos (evita despejar 10000 linhas). */
    if (n <= 20) {
        for (int i = 0; i < n; i++) {
            printf("%d | %s | %.1f | %d | %.1f\n", vet[i].ID_Sensor, vet[i].Data,
                   vet[i].Temperatura_C, vet[i].Umidade_Porcentagem, vet[i].Nivel_PM25);
        }
    }

    int kMed = n / 2;                      // posicao da mediana (0-indexada)
    int kP90 = indicePercentil90(n);       // posicao do percentil 90

    /* ---- Consulta 1: Mediana do PM2.5, pelas 3 abordagens ---- */
    printf("\n--- Mediana do PM2.5 (k = %d) ---\n", kMed);
    copiaVet(vet, n, vetCopia); comparacoes = 0;
    float medA = abordagemA(vetCopia, n, kMed, CAMPO_PM25);
    printf("A (bolha):      %.1f  | %lld comparacoes\n", medA, comparacoes);

    copiaVet(vet, n, vetCopia); comparacoes = 0;
    float medB = abordagemB(vetCopia, n, kMed, CAMPO_PM25);
    printf("B (partic.):    %.1f  | %lld comparacoes\n", medB, comparacoes);

    copiaVet(vet, n, vetCopia); comparacoes = 0;
    float medC = abordagemC(vetCopia, n, kMed, CAMPO_PM25);
    printf("C (contagem):   %.1f  | %lld comparacoes\n", medC, comparacoes);

    /* ---- Consulta 2: Limiar critico (percentil 90) ---- */
    printf("\n--- Limiar critico, percentil 90 (k = %d) ---\n", kP90);
    copiaVet(vet, n, vetCopia); comparacoes = 0;
    float p90A = abordagemA(vetCopia, n, kP90, CAMPO_PM25);
    printf("A (bolha):      %.1f  | %lld comparacoes\n", p90A, comparacoes);

    copiaVet(vet, n, vetCopia); comparacoes = 0;
    float p90B = abordagemB(vetCopia, n, kP90, CAMPO_PM25);
    printf("B (partic.):    %.1f  | %lld comparacoes\n", p90B, comparacoes);

    copiaVet(vet, n, vetCopia); comparacoes = 0;
    float p90C = abordagemC(vetCopia, n, kP90, CAMPO_PM25);
    printf("C (contagem):   %.1f  | %lld comparacoes\n", p90C, comparacoes);

    /* Conferencia: as tres abordagens DEVEM concordar. */
    if (medA == medB && medB == medC && p90A == p90B && p90B == p90C)
        printf("\n[OK] As tres abordagens deram os mesmos resultados.\n");
    else
        printf("\n[ERRO] As abordagens divergiram!\n");

    /* ---- Consulta 3: M dias mais secos ---- */
    int M = 10;
    if (M > n) M = n;
    printf("\n");
    copiaVet(vet, n, vetCopia);
    diasMaisSecos(vetCopia, n, M);

    /* ---- Analise de desempenho e Questao 1 ---- */
    tabelaDesempenho();
    tracoQuestao1();

    free(vet);
    free(vetCopia);
    return 0;
}