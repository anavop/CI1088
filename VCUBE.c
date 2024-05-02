/* Autor: Elias P. Duarte Jr.
   Data da Última Modificação: 11/abril/2024
   Descrição: Nosso primeiro programa de simulação da disciplina Sistemas Distribuídos.
     Vamos simular N processos, cada um conta o “tempo” independentemente
    Um exemplo simples e significativo para captar o “espírito” da simulação */

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"


#define test 1
#define fault 2
#define recovery 3
#define TimeRange 30.0

#define TimeClock 150.0

#define unkwon -1
#define p_correto 0
#define falho 1

// Utilizado na funcao POPEN
#define BUFSIZE 256

// definindo a estrutura d eprocesso
typedef struct {
   
	int id;		
	int *state;
	int *teste;
	int p_clust;
	int completou_rodada;

} TipoProcesso;
TipoProcesso *processo;

void set_teste(TipoProcesso *  p_processo, int N,int value){
	for (int i = 0; i < N; i++) {
			for(int j = 0; j < N; j++){ 
				processo[i].teste[j] = value;
						}
					}
}

void set_state(TipoProcesso *  p_processo, int N,int value,int id){
	for(int j = 0; j < N; j++){ 
				processo[id].state[j] = value;
				}
					}

void set_rodada(TipoProcesso * processo,int N,int value){
	for (int i = 0; i < N; i++) {
		processo[i].completou_rodada = 0;
	}

}
int status_return(TipoProcesso * p,int token){
	int novo_estado = (status(processo[token].id));
	return  novo_estado;
}


int cont_processos(int token, TipoProcesso*  processo) {

    int p_clust = processo[token].p_clust;
    int pcluster = 1 << p_clust;

    return pcluster;
}
void printe_vetor(TipoProcesso*  processo,int id,int N){
	printf("State Do P%d: ", id);
	for (int i = 0; i < N; i++) 
		printf(" %d ", processo[id].state[i]);
	printf("\n");							
}
int clustfunction (int i, int s, int j) {
	int processo; 
    char buf[BUFSIZE];
	char command[256];
	char teste[1024];
	FILE *fp;

	sprintf(command, "./cisj %d %d %d", i, s, j);  
	fp = popen(command, "r");

    while (fgets(buf, BUFSIZE, fp) != NULL) {
    	processo = atoi(buf);
    }
 
    pclose(fp);
    return (processo);
}
int is_process_correct(TipoProcesso *processo, int N) {
    int i, j;
    for (i = 0; i < N; i++) {
        if (status_return(processo, i) == 0) {
            for (j = 0; j < N; j++) {
                if (processo[i].state[j] == unkwon)
                    return 0; // Se encontrou um estado desconhecido, retorna falso
            }
        }
    }
    return 1; // Se não encontrou estados desconhecidos, retorna verdadeiro
}


int main (int argc, char *argv[]) {
	
static int N,	// número de processos
	token,  // indica o processo que está executando
	event, r, i,
		
	MaxTempoSimulac = 150;
	//incializa contadores
	int r_global = 1;
	int n_rdiag = 0;
	int n_test = 0;
	int n_testDiag = 0;
	int n_event = 0;
	int evento = 0;
	int event_diag = 0;


	static char fa_name[5];


	if(argc != 2) {
		puts ("O input inserido está errado. O uso corretor é o seguinte: ./version02 <número de processos>");
		exit(1);
	}
	
	N = atoi(argv[1]);

	smpl(0, "Trabalho - VCube");
	reset();

	stream(1);

	
	processo = (TipoProcesso *) malloc(sizeof(TipoProcesso) * N);

	for (i = 0; i < N; i++){

		memset(fa_name, '\0', 5);
		sprintf(fa_name, "%d", i);
		processo[i].id = facility(fa_name, 1);
		processo[i].state = (int*) malloc(sizeof(int) * N); 
		processo[i].teste = (int*) malloc(sizeof(int) * N);

		for(int j = 0; j < N; j++){
			processo[i].state[j] = -1; // estado dos demais processos como desconhecido
			processo[i].teste[j] = 0; //  test
		}

		processo[i].state[i] = 0 ;		
		processo[i].p_clust = 1;
	}

	// N de p_clusts
	int n_p_clusts;
	n_p_clusts = log2(N);
	
	printf("\n");
	printf("===========================================================\n"); 
 	printf("Algoritmo VCube - Trabalho 1\n");
  	printf("Aluna: Ana de Vasconcellos Oporto\n");
  	printf("Disciplina: Sistemas Distribuídos\n");
  	printf("Última Modificação: 24 de abril de 2024\n");
  	printf("===========================================================\n\n"); 

	printf("----------------------------------------------------------- \n");
	printf("Existem %d processos e %d p_clusts no sistema.\n", N, n_p_clusts);
	printf("O tempo do intervalo de testes é de 30.0 unidades de tempo.\n");
	printf("O tempo máximo de execução é de %4.1f unidades de tempo.\n", TimeClock);
	printf("\n");
	printf("----------------------------------------------------------- \n");
	printf("\n");


	printf("\n --------------------  RODADA 1 ---------------------- \n\n");


	for (i = 0; i < N; i++){
		schedule (test, TimeRange, i);
	}


	schedule(fault, 0.0, 4);
	schedule(fault, 0.0, 5);
	schedule(fault, 0.0, 6);
	schedule(fault, 0.0, 7);

	while (time() < 150.0){
		cause(&event, &token);

		switch(event) {
			case test:
					if (status_return(processo,token) != 0) 
						break;
					

					set_teste(processo,N,0);
					int n_p_clust = cont_processos(token,processo);

					if (processo[token].p_clust > n_p_clusts) 
						processo[token].p_clust = 1;
					int i = 0;
					while (i<N) {
						int pAux = 0;
						int pos = 1;
						
						while ((pos <= n_p_clust)& (pAux == 0)) {
							int tester_p = clustfunction (i, processo[token].p_clust, pos);
							if (status(processo[tester_p].id) == 0) {
								pAux = 1;
								processo[tester_p].teste[i] = 1;
							} else 
								pos++;
							
						}
					i++;
					}

					// Inicia a rodada
					for(i = 0; i < N; i++) {
						if (processo[token].teste[i] == 1) {
							int p_tested = i;

							if(status_return(processo,p_tested) != 0) { // diferente de correto

								processo[token].state[p_tested] = falho;
		                    	printf(" P%d TESTA P%d --> Falho no Instante  %5.1f \n",token, p_tested, time());

			                    n_test++;

								if (evento == 1) {
									n_testDiag++;
								}
							}
							else if((token != p_tested)&((status_return(processo,p_tested)) == 0) ) {
								processo[token].state[p_tested] = p_correto;
		                    	printf(" P%d TESTA P%d --> Correto no Instante  %5.1f \n",token, p_tested, time());
    
			                    for(int j = 0; j < N; j++) {
									if (processo[p_tested].state[j] > processo[token].state[j]) 
										processo[token].state[j] = processo[p_tested].state[j];
									
								}
								n_test++;

								if (evento == 1) 
									n_testDiag++;
								
								break;
							}
						}
					}
					processo[token].completou_rodada++;
					processo[token].p_clust++;

					int r_aux = 0;
					int t = 0;
					int w = 0;
					while (w<N) {
						// testando se o processo esta correto
						if (processo[w].state[w] == p_correto) {
							if (processo[w].completou_rodada != 0) 
								t = 1;
							
							else 
								t = 0;
							
						}
						w++;
					}

					// Uma rodada de testes ocorreu
					if (t) {
						r_aux = 1;
						set_rodada(processo,N,0); 

						printf("\n");
						for (int j = 0; j < N; j++) {
							if (j <= 9) {
								printe_vetor(processo,j,N);
							} else 
								printe_vetor(processo,j,N);
							
						}

						printf("\n");
						printf("Número de testes realizados até o momento: %d \n", n_test);	
						r_global++;
					}

					if (((evento == 1) &r_aux == 1) ) {	
						processo[i].completou_rodada = 0;
						t = 1;
						int aux = 0;

						// Comparando os vetores state de todos os processos CORRETOS - se os vetores estiverem iguais E o valor do state não é desconhecido, ocorreu um diagnóstico 
							for(int i = 0; i < N; i++) {
								for (aux = 0; aux < N; aux++) {
									for (int j = 0; j < N; j++) {
										if ((processo[i].state[j] != processo[aux].state[j]))  {
											if ((status(processo[i].id) == 0) & (status(processo[aux].id) == 0)) {
												t = 0;
												break;
											}
										} 
									} 
								}
							}

						if (t && !is_process_correct(processo, N)) {
    						t = 0;
							break;
							}

						// Valida se o evento foi detectado por todos os processos corretos
						if ((evento == 1) & ((t == 1) ) )
							event_diag = 1;
						
						printf("\n");

						// Número de rodadas totais para o obter o diagnostico
						if (evento == 1) 
							n_rdiag++;
						//Ocorreu um evento
						if ((evento == 1) & (event_diag == 1)) {
							printf("\n ================================================================================ \n");
							printf("\n -------------------------  DIAGNÓSTICO COMPLETO  ------------------------------- \n");
							printf("\nTempo P/ O Diagnóstico \n");
							printf("O diagnóstico foi completo no tempo %4.1f.\n\n", time());
							printf("Latencia\n");
							printf("Número de rodadas para obter o diagnostico %d \n\n", n_rdiag);
							printf("Número de rodadas de teste %d para diagnóstico.\n\n", n_testDiag);
							printf("\n");
							printf("\n ================================================================================ \n");

							
							set_rodada(processo,N,0);
							n_rdiag = 0;
							n_testDiag = 0;
							r_aux = 0;
							t = 0;
							event_diag = 0;
							evento = 0;
						}
					}

					if (r_aux) {
						printf("\n\n");
						printf("\n --------------------  RODADA %d ---------------------- \n", r_global);	
					}

					schedule(test, TimeRange, token);
					break;

			case fault:
				r = request (processo[token].id, token, 0);
				if (r != 0) {
					// Isso ocorre caso a gente tenha esquecido de que ele já falhou antes
					printf("Não foi possível falhar o processo %d", token);
					exit (1);
				}
				printf("O processo %d falhou no tempo %4.1f\n", token, time());

				processo[token].completou_rodada = 0;
				processo[token].p_clust = 1;
				set_state(processo,N,unkwon,token);
				processo[token].state[token] = falho;
				n_event++;
				evento = 1;
				break;

			case recovery:
				release(processo[token].id, token);
				printf ("O processo %d recuperou no tempo %4.1f\n", token, time());
				schedule (test, TimeRange, token);				
				processo[token].p_clust = 1;
				processo[token].completou_rodada = 0;
				set_state(processo,N,unkwon,token);
				processo[token].state[token] = p_correto;
				n_event++;
				evento = 1;
				break;
		}
	}

	printf("\n Rodada não finalizada. \n");
	printf("\n ================================================================================ \n"); 
	printf("\n ------------------------------ FIM DA SIMULAÇÃO --------------------------------\n");
	printf("\nResultados:");
	printf("\n%d eventosocorreram durante a detecção de falha", n_event);
	printf("\nNúmero de rodadas de testes completas durante a simulação: %d\n", r_global-1);
	printf("Ocorreram %d testes ao total.\n", n_test);
	printf("\n ================================================================================ \n"); 

	return 0;
}