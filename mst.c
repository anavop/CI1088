#include <stdio.h>
#include <stdlib.h>

#include "cisj.h"
#include "smpl.h"

#define build_tree 1
#define fault 2
#define recovery 3

typedef struct {
    int id;
    int s;
} TipoProcesso;

TipoProcesso *processo;

int main(int argc, char *argv[]) {
    static int N, // numero de processos do sistema distribuido
        token,    // indica o processo que esta sendo executado
        next,     // proximo processo a ser testado
        event, r, i, j, s,
        clusters, token_status, root;
    static char fa_name[5];
    static node_set *nodes;

    if (argc < 3) {
        puts("Uso correto: mst <root> <dimensao vCube> <processos falhos>");
        exit(1);
    }

    root = atoi(argv[1]);
    clusters = atoi(argv[2]);
    N = POW_2(clusters);

    smpl(0, "Minimum Spanning Tree");
    reset();
    stream(1);
    processo = (TipoProcesso *)malloc(sizeof(TipoProcesso) * N);

    for (i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
        processo[i].s = -1;
    }
    processo[root].s = clusters + 1;

    // define o evento de construcao da arvore
    // a partir do nodo raiz
    schedule(build_tree, 30.0, root);

    // define os eventos de falha
    for (i = 3; i < argc; i++) {
        schedule(fault, 1.0, atoi(argv[i]));
    }

    puts("=================================================");
    puts("Arvore Geradora Minima Autonomica - mst.c");
    printf("Numero de processos: %d\n", N);
    puts("=================================================");

    while (time() < 30.0 * clusters) {
        cause(&event, &token);

        switch (event) {
        case build_tree:
            if (status(processo[token].id) != 0 || processo[token].s == -1) {
                break;
            }

            printf("%5.1f: [%d] -> ", time(), token);

            for (s = 1; s < processo[token].s; s++) {
                nodes = cis(token, s);

                for (i = 0; i < nodes->size; i++) {
                    next = nodes->nodes[i];

                    if (status(processo[next].id) == 0 && processo[next].s == -1) {
                        printf("%d ", next);
                        processo[next].s = s;

                        if (s > 1) {
                            schedule(build_tree, 30.0, next);
                        }

                        break;
                    }
                }

                set_free(nodes);
            }
            printf("\n");
            break;

        case fault:
            r = request(processo[token].id, token, 0);
            printf("*** O processo %d FALHOU no tempo %4.1f ***\n", token, time());
            break;

        case recovery:
            release(processo[token].id, token);
            printf("*** O processo %d RECUPEROU no tempo %4.1f ***\n", token, time());
            schedule(build_tree, 30.0, token);
            break;
        }
    }
}
