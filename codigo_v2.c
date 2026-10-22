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

// Abordagem A
void bolha(Registro v[], int n, long long *comparacoes) {
    for (int j = 0; j < n - 1; j++)
        for (int i = 0; i < n - 1 - j; i++) {
            ++*comparacoes;
            if (v[i].Nivel_PM25 > v[i + 1].Nivel_PM25)
                trocar(&v[i], &v[i + 1]);
        }
}

float consultaA(Registro v[], int n, int k, long long *comparacoes) {
    bolha(v, n, comparacoes);
    return v[k].Nivel_PM25;
}

// Abordagem B
float selecaoB(Registro v[], int n, int k, long long *comparacoes) {
    int limiteEsq = 0, limiteDir = n - 1;

    while (limiteEsq <= limiteDir) {
        float pivo = v[(limiteEsq + limiteDir) / 2].Nivel_PM25;
        int inicioIguais = limiteEsq, i = limiteEsq, fimIguais = limiteDir;

        while (i <= fimIguais) {
            ++*comparacoes;
            if (v[i].Nivel_PM25 < pivo) {
                trocar(&v[inicioIguais], &v[i]);
                inicioIguais++;
                i++;
            } else {
                ++*comparacoes;
                if (v[i].Nivel_PM25 > pivo) {
                    trocar(&v[i], &v[fimIguais]);
                    fimIguais--;
                } else {
                    i++;
                }
            }
        }

        if (k < inicioIguais) limiteDir = inicioIguais - 1;
        else if (k > fimIguais) limiteEsq = fimIguais + 1;
        else return pivo;
    }

    return 0.0f;
}

// Abordagem C
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
    return 0.0f;
}

float medir(const Registro origem[], int n, int k, Consulta func, Medida *medida) {
    Registro copia[n];
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

    printf("\nN = %d registros (selecionando o elemento de indice n/2):\n", n);
    printf("  Abordagem A (bolha)         : %12lld comparacoes | %8.5f segundos\n",
           medidaA.comparacoes, medidaA.segundos);
    printf("  Abordagem B (particionamento): %12lld comparacoes | %8.5f segundos\n",
           medidaB.comparacoes, medidaB.segundos);
    printf("  Abordagem C (contagem)      : %12lld comparacoes | %8.5f segundos\n",
           medidaC.comparacoes, medidaC.segundos);

    if (vabordagemA != vabordagemB || vabordagemA != vabordagemC)
        printf("  ERRO: resultados diferentes! %.1f %.1f %.1f\n",
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
    Registro vetUmidade[n];

    gerarDados(vet, n);

    // usa a umidade no campo que as funcoes comparam
    copiaVet(vet, n, vetUmidade);
    for (int i = 0; i < n; i++)
        vetUmidade[i].Nivel_PM25 = (float)vetUmidade[i].Umidade_Porcentagem;

    int M = n < 10 ? n : 10;
    int indiceMediana = n / 2;
    int indicePercentil90 = (n * 90 + 99) / 100 - 1;
    int indiceDiasSecos = M - 1;

    const char *nomeAbordagem[3] = {
        "ABORDAGEM A - Ordenacao completa (bolha)",
        "ABORDAGEM B - Selecao por particionamento",
        "ABORDAGEM C - Contagem comparativa"
    };
    Consulta funcaoAbordagem[3] = {consultaA, selecaoB, selecaoC};

    float valorMediana[3], valorPercentil90[3], valorDiasSecos[3];

    printf("\n############ RESULTADOS PARA N = %d REGISTROS ############\n", n);

    for (int a = 0; a < 3; a++) {
        Medida medidaMediana, medidaPercentil90, medidaDiasSecos;
        valorMediana[a]     = medir(vet, n, indiceMediana, funcaoAbordagem[a], &medidaMediana);
        valorPercentil90[a] = medir(vet, n, indicePercentil90, funcaoAbordagem[a], &medidaPercentil90);
        valorDiasSecos[a]   = medir(vetUmidade, n, indiceDiasSecos, funcaoAbordagem[a], &medidaDiasSecos);

        printf("\n==================================================\n");
        printf(" %s\n", nomeAbordagem[a]);
        printf("==================================================\n");
        printf(" 1) Mediana PM2.5 (indice %d)\n", indiceMediana);
        printf("    valor encontrado : %.1f ug/m3\n", valorMediana[a]);
        printf("    comparacoes      : %lld\n", medidaMediana.comparacoes);
        printf(" 2) Percentil 90 PM2.5 (indice %d)\n", indicePercentil90);
        printf("    valor encontrado : %.1f ug/m3\n", valorPercentil90[a]);
        printf("    comparacoes      : %lld\n", medidaPercentil90.comparacoes);
        printf(" 3) Dias mais secos: umidade do %do dia mais seco (indice %d)\n", M, indiceDiasSecos);
        printf("    valor encontrado : %.0f%%\n", valorDiasSecos[a]);
        printf("    comparacoes      : %lld\n", medidaDiasSecos.comparacoes);
    }

    if (valorMediana[0] != valorMediana[1] || valorMediana[0] != valorMediana[2] ||
        valorPercentil90[0] != valorPercentil90[1] || valorPercentil90[0] != valorPercentil90[2] ||
        valorDiasSecos[0] != valorDiasSecos[1] || valorDiasSecos[0] != valorDiasSecos[2])
        printf("\nERRO: as abordagens deram resultados diferentes!\n");
    else
        printf("\nConferencia: as 3 abordagens encontraram os mesmos valores.\n");

    long long comparacoesListagem = 0;
    // lista dos dias mais secos
    bolha(vetUmidade, n, &comparacoesListagem);

    printf("\nAs %d leituras mais secas (umidade):\n", M);
    for (int i = 0; i < M; i++) {
        printf("    %d%% (sensor %d, data %s)\n",
               vetUmidade[i].Umidade_Porcentagem,
               vetUmidade[i].ID_Sensor,
               vetUmidade[i].Data);
    }

    printf("\n############ COMPARACAO ENTRE TAMANHOS DE ENTRADA ############\n");
    teste(1000);
    teste(5000);
    teste(10000);

    return 0;
}