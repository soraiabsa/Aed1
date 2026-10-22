#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   

typedef struct{
    int  ID_Sensor;
    char Data[11];
    float Temperatura_C;
    int Umidade_Porcentagem;
    float Nivel_PM25;
} Registro;

long long comparacoes = 0;   // NOVO: o placar

void gerarDados(Registro v[], int n) {
    for(int i = 0; i < n; i++){
        v[i].ID_Sensor = rand() % 1000; // Gera um ID aleatório entre 0 e 999
        int mes = 1 + rand() % 12;
        int dia = 1 + rand() % 31;
        snprintf(v[i].Data, sizeof v[i].Data, "%02d/%02d/2026", dia, mes); 
        v[i].Temperatura_C = 25 + (rand() % 210)/10.0f;
        v[i].Umidade_Porcentagem = 15 + rand() % 76; 
        v[i].Nivel_PM25 = 5 + (rand() % 2960)/10.0f;
    }
}

void copiaVet(Registro v[], int n, Registro vetCopia[]) {
    for(int i = 0; i < n; i++){
        vetCopia[i] = v[i];
    }
}

// NOVO: bolha para Registro, ordenando pelo PM2.5, com o contador
void bolha(Registro v[], int n) {
    for(int j = 0; j < n - 1; j++){
        for(int i = 0; i < n - 1 - j; i++){
            comparacoes++;                                   // conta ANTES do if
            if(v[i].Nivel_PM25 > v[i+1].Nivel_PM25){
                Registro aux = v[i];                         // troca as duas fichas
                v[i] = v[i+1];
                v[i+1] = aux;
            }
        }
    }
}

float abordagemA(Registro v[], int n, int k){
    bolha(v, n);
    int indice = k - 1;
    return v[indice].Nivel_PM25;
}

int main(void){
    srand(1);

    int n;
    float mediana = 0.0;

    printf("Informe a quantidade de registros: ");
    scanf("%d", &n);

    int k = n/2;

    Registro vet[n];

    gerarDados(vet, n);

    for(int i = 0; i < n; i++){
        printf("%d | %s | %.1f | %d | %.1f\n", vet[i].ID_Sensor, vet[i].Data, vet[i].Temperatura_C, vet[i].Umidade_Porcentagem, vet[i].Nivel_PM25);
    }

    Registro vetCopia[n];

    copiaVet(vet, n, vetCopia);                  


    comparacoes = 0;

    mediana = abordagemA(vetCopia, n, k);

    printf("Comparacoes da bolha: %lld\n", comparacoes);
    printf("Valor da mediana: %.1f\n", mediana);

    return 0;
}