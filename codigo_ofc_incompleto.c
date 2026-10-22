
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int ID_Sensor;
    char Data[16];
    float Temperatura_C;
    int Umidade_Porcentagem;
    float Nivel_PM25;
} Registro;

typedef struct {
    long long comparacoes;
    double segundos;
} Medida;

typedef float (*Consulta)(Registro[], int, int, long long *);

void gerarDados(Registro v[], int n) {
    for (int i = 0; i < n; i++) {
        v[i].ID_Sensor = rand() % 1000;
        int mes = 1 + rand() % 12;
        int dia = 1 + rand() % 28;
        snprintf(v[i].Data, sizeof v[i].Data, "%02d/%02d/2026", dia, mes);
        v[i].Temperatura_C = 25 + (rand() % 210) / 10.0f;
        v[i].Umidade_Porcentagem = 15 + rand() % 76;
        v[i].Nivel_PM25 = 5 + (rand() % 2960) / 10.0f;
    }
}

void copiaVet(const Registro v[], int n, Registro copia[]) {
    for (int i = 0; i < n; i++) copia[i] = v[i];
}

void trocar(Registro *a, Registro *b) {
    Registro temporario = *a;
    *a = *b;
    *b = temporario;
}

/* Abordagem A: ordenacao bolha por PM2.5 */
void bolha(Registro v[], int n, long long *comparacoes) {
    for (int j = 0; j < n - 1; j++)
        for (int i = 0; i < n - 1 - j; i++) {
            ++*comparacoes;
            if (v[i].Nivel_PM25 > v[i + 1].Nivel_PM25)
                trocar(&v[i], &v[i + 1]);
        }
}

/* Bolha por umidade (usada so para listar os dias mais secos) */
void bolhaUmidade(Registro v[], int n) {
    for (int j = 0; j < n - 1; j++)
        for (int i = 0; i < n - 1 - j; i++)
            if (v[i].Umidade_Porcentagem > v[i + 1].Umidade_Porcentagem)
                trocar(&v[i], &v[i + 1]);
}

float consultaA(Registro v[], int n, int k, long long *comparacoes) {
    bolha(v, n, comparacoes);   /* ordena tudo */
    return v[k].Nivel_PM25;     /* e le direto a posicao k */
}

/* Abordagem B: selecao por particionamento (pivo central, 3 regioes).
   Apos o laco: [esq..lt-1] < pivo, [lt..gt] == pivo, [gt+1..dir] > pivo.
   esq/dir = janela onde a resposta ainda pode estar.
   lt/gt   = fronteiras das regioes dentro da janela (necessarias por causa dos empates). */
float selecaoB(Registro v[], int n, int k, long long *comparacoes) {
    int limiteEsq = 0, limiteDir = n - 1;

    while (limiteEsq <= limiteDir) {
        float pivo = v[(limiteEsq + limiteDir) / 2].Nivel_PM25;
        int lt = limiteEsq, i = limiteEsq, gt = limiteDir;

        while (i <= gt) {
            ++*comparacoes;
            if (v[i].Nivel_PM25 < pivo) {
                trocar(&v[lt], &v[i]);
                lt++;
                i++;
            } else {
                ++*comparacoes;
                if (v[i].Nivel_PM25 > pivo) {
                    trocar(&v[i], &v[gt]);
                    gt--;
                } else {
                    i++;
                }
            }
        }

        if (k < lt) limiteDir = lt - 1;
        else if (k > gt) limiteEsq = gt + 1;
        else return pivo;
    }

    return 0.0f; 
}

//Abordagem C:
float selecaoC(Registro v[], int n, int k, long long *comparacoes) {
    for (int i = 0; i < n; i++) {
        int menores = 0, iguais = 0;
        for (int j = 0; j < n; j++) {
            ++*comparacoes;
            if (v[j].Nivel_PM25 < v[i].Nivel_PM25) menores++;
            else {
                ++*comparacoes;
                if (v[j].Nivel_PM25 == v[i].Nivel_PM25) iguais++;
            }
        }
        if (menores <= k && k < menores + iguais)
            return v[i].Nivel_PM25;
    }
    return 0.0f; /* nunca executa se 0 <= k < n */
}

float medir(const Registro origem[], int n, int k, Consulta func, Medida *medida) {
    Registro copia[n];                 /* cada abordagem trabalha numa copia */
    copiaVet(origem, n, copia);
    medida->comparacoes = 0;
    clock_t inicio = clock();
    float resultado = func(copia, n, k, &medida->comparacoes);
    medida->segundos = (double)(clock() - inicio) / CLOCKS_PER_SEC;
    return resultado;
}

void teste(int n) {
    Registro base[n];
    gerarDados(base, n);
    int k = n / 2;
    Medida medidaA, medidaB, medidaC;
    float vabordagemA = medir(base, n, k, consultaA, &medidaA);
    float vabordagemB = medir(base, n, k, selecaoB, &medidaB);
    float vabordagemC = medir(base, n, k, selecaoC, &medidaC);

    printf("%5d | A: %12lld (%8.5fs) | B: %10lld (%8.5fs) | C: %12lld (%8.5fs)\n",
           n, medidaA.comparacoes, medidaA.segundos,
           medidaB.comparacoes, medidaB.segundos,
           medidaC.comparacoes, medidaC.segundos);

    if (vabordagemA != vabordagemB || vabordagemA != vabordagemC)
        printf("ERRO: resultados diferentes! %.1f %.1f %.1f\n",
               vabordagemA, vabordagemB, vabordagemC);
}

int main(void) {
    srand(1);
    int n;
    printf("Informe a quantidade de registros (1 a 10000): ");
    scanf("%d", &n);

    if (n < 1 || n > 10000) {
        printf("Informe uma quantidade entre 1 e 10000.\n");
        return 1;
    }

    Registro vet[n];
    Registro ordenado[n];
    Registro umidade[n];

    gerarDados(vet, n);
    copiaVet(vet, n, ordenado);   /* copia que sera ordenada por PM2.5 */
    copiaVet(vet, n, umidade);    /* copia que sera ordenada por umidade */

    long long comparacoesA = 0;
    bolha(ordenado, n, &comparacoesA);

    float mediana = (n % 2 == 0)
        ? (ordenado[n / 2 - 1].Nivel_PM25 + ordenado[n / 2].Nivel_PM25) / 2.0f
        : ordenado[n / 2].Nivel_PM25;

    int k90 = (n * 90 + 99) / 100 - 1;
    float limiar = ordenado[k90].Nivel_PM25;

    printf("\nMediana exata PM2.5: %.1f\nPercentil 90 PM2.5: %.1f\nComparacoes da ordenacao A: %lld\n",
           mediana, limiar, comparacoesA);

    bolhaUmidade(umidade, n);
    int M = n < 10 ? n : 10;

    printf("\nAs %d leituras mais secas (umidade):\n", M);
    for (int i = 0; i < M; i++) {
        printf("%d%% (sensor %d, data %s)\n",
               umidade[i].Umidade_Porcentagem,
               umidade[i].ID_Sensor,
               umidade[i].Data);
    }

    printf("\nComparacao das abordagens para selecionar o elemento de indice n/2:\n");
    teste(1000);
    teste(5000);
    teste(10000);

    return 0;
}
