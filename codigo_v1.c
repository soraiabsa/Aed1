
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   

typedef struct{
    int  ID_Sensor;
    char Data[16];//troquei de 11 para 16 pois estva dando erro
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

void bolhaUmidade(Registro v[], int n) {//ordenando o vetor de umidade
    for(int j = 0; j < n - 1; j++){
        for(int i = 0; i < n - 1 - j; i++){
            comparacoes++;                                   // conta ANTES do if
            if(v[i].Umidade_Porcentagem > v[i+1].Umidade_Porcentagem){
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

float abordagemA_limiar(Registro v[], int n, int limiar_critico){
    bolha(v, n);
    int indice = limiar_critico;
    return v[indice].Nivel_PM25;
}

int main(void){
    srand(1);

    int n;
    float mediana = 0.0;

    printf("Informe a quantidade de registros: "); // medições feita, no máximo 10.000
    scanf("%d", &n);

    int k = n/2; //esse é só o primeiro k, pois tem o k do limite percentual e dos dias

    Registro vet[n]; //defini o vetor que tem o tipo de dado registro, ou seja, segue essa ordem de registro

    gerarDados(vet, n); // gera dados para prenncher o vetor 

    for(int i = 0; i < n; i++){
        printf("%d | %s | %.1f | %d | %.1f\n", vet[i].ID_Sensor, vet[i].Data, vet[i].Temperatura_C, vet[i].Umidade_Porcentagem, vet[i].Nivel_PM25);
    } // essa parte é usada para mostrar todas linhas das "leituras" feitas, usado só para teste inicial

    Registro vetCopia[n];//criou outro vetor

    copiaVet(vet, n, vetCopia); //envia para a função copiar o vetor e colocar nesse novo "copia"                 

    comparacoes = 0; //inicializa a variavel 

    mediana = abordagemA(vetCopia, n, k);

    int limiar_critico = (n * 90) / 100;

    float limiar = abordagemA_limiar(vetCopia, n, limiar_critico);

    printf("Comparacoes da bolha: %lld\n", comparacoes);//quant comp foram feitas
    printf("Valor da mediana: %.1f\n", mediana);
    printf("Valor do limiar critico: %.1f\n", limiar);

    return 0;
}

