/* Autor: Elias P. Duarte Jr.
   Data da Última Modificação: 11/abril/2024
   Descrição: Nosso primeiro programa de simulação da disciplina Sistemas Distribuídos.
     Vamos simular N processos, cada um conta o “tempo” independentemente
    Um exemplo simples e significativo para captar o “espírito” da simulação */

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

// Vamos definir os EVENTOS 
#define test 1
#define fault 2
#define recovery 3


// Vamos definir o descritor do processo

typedef struct{ 
   int id;  
   int state;       // identificador de facility do SMPL
   // outras variáveis locais dos processos são declaradas aqui!
} TipoProcesso;

TipoProcesso *processo;
 

int main (int argc, char *argv[]) {
   
   static int N,   // número de processos
            token,  // indica o processo que está executando
            event, r, i,
            MaxTempoSimulac = 150;

   static char fa_name[5];

   if (argc != 2) {
       puts("Uso correto: tempo <número de processos>");
       exit(1);
    }

   N = atoi(argv[1]);
   

   smpl(0, "Um Exemplo de Simulação");
   reset();
   stream(1);

   // inicializar processos

   processo = (TipoProcesso *) malloc(sizeof(TipoProcesso)*N);

   for (i=0; i<N; i++) {
      memset(fa_name, '\0', 5);
      sprintf(fa_name, "%d", i);
      processo[i].id = facility(fa_name,1);
      processo[i].state = -1;
   }
    // vamos fazer o escalonamento inicial de eventos

    // nossos processos vão executar testes em intervalos de testes
    // o intervalo de testes vai ser de 30 unidades de tempo
    // a simulação começa no tempo 0 (zero) e vamos escalonar o primeiro teste de todos os    
    //       processos para o tempo 30.0

    for (i=0; i<N; i++) {
       schedule(test, 30.0, i); 
    }
    schedule(fault, 31.0, 1);
    schedule(recovery, 61.0, 1);

    // agora vem o loop principal do simulador

    puts("===============================================================");
    puts("           Sistemas Distribuídos Prof. Elias");
    puts("          LOG do Trabalho Prático 0, Tarefa 0");
    puts("      Digitar, compilar e executar o programa tempo.c");
    printf("   Este programa foi executado para: N=%d processos.\n", N); 
    printf("           Tempo Total de Simulação = %d\n", MaxTempoSimulac);
    puts("===============================================================");
   int new_tk = token;
   int * right_proc;
   int * fault_proc;
   int aux_token = token;
 while (time() < 150.0) {
    cause(&event, &token);
    // printf("TOKEN %d \n",token);
    switch (event) {
        case test: 
            if (status(processo[token].id) == 0) {
                int new_tk = (token + 1) % N;
                int tested = 0; // Flag para indicar se foi realizado um teste
                for (int i = 0; i < N; i++) {
                    if (status(processo[new_tk].id) == 0) { 
                        printf("Processo %d testa o processo %d \n", token, new_tk);
                        printf("Processo correto %d ", new_tk);
                        processo[new_tk].state = status(processo[new_tk].id);
                        printf("testado no tempo %4.1f \n", time());
                        schedule(test, 30.0, new_tk);
                        tested = 1;
                        break; // Encerra o loop assim que um processo válido é testado
                    } else { 
                        new_tk = (new_tk + 1) % N; // Avança para o próximo processo
                    }
                }
                if (!tested) { // Se nenhum teste foi realizado
                    printf("Todos os processos estão falhos, nenhum teste pode ser realizado.\n");
                }
            }
            break;

        case fault:
            r = request(processo[token].id, token, 0);
            printf("token %d \n", token);
            printf("Socooorro!!! Sou o processo %d  e estou falhando no tempo %4.1f\n", token, time());
            break;

        case recovery:
            release(processo[token].id, token);
            printf("Viva!!! Sou o processo %d e acabo de recuperar no tempo %4.1f\n", token, time());
            schedule(test, 1.0, token);
            break;
    } // switch
} // while
}
