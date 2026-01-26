#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int N;
int *unosi;
int broj_doslih = 0;

pthread_mutex_t unos_mutex;
pthread_mutex_t barijera_mutex;
pthread_cond_t barijera_cond;


void barijera() {
    pthread_mutex_lock(&barijera_mutex);

    broj_doslih++;

    if (broj_doslih < N) {
        pthread_cond_wait(&barijera_cond, &barijera_mutex);
    } else {
        broj_doslih = 0;
        pthread_cond_broadcast(&barijera_cond);
        printf("\n");
    }

    pthread_mutex_unlock(&barijera_mutex);
}

void *unos_dretva(void *arg) {
    int id = *(int *)arg;

    pthread_mutex_lock(&unos_mutex);

    // unos broja
    printf("Dretva %d, unesite broj: ", id);
    scanf("%d", &unosi[id]);

    pthread_mutex_unlock(&unos_mutex);

    barijera();

    printf("Dretva %d uneseni broj je %d\n", id, unosi[id]);

    return NULL;
}

int main(int argc, char *argv[]) {
    N = atoi(argv[1]);
    printf("Broj dretava je %d\n\n", N);

    // alociraj broj dretvi i dinamicko polje za unos
    unosi = malloc(N * sizeof(int*));
    pthread_t *dretve = malloc(N * sizeof(pthread_t*));
    int *dretvaID = malloc(N * sizeof(int*));

    // inicijalizacija mutexa i conda
    pthread_mutex_init(&barijera_mutex, NULL);
    pthread_mutex_init(&unos_mutex, NULL);
    pthread_cond_init(&barijera_cond, NULL);

    // pokretanje dretvi
    for (int i = 0; i < N; i++) {
        dretvaID[i] = i + 1;
        pthread_create(&dretve[i], NULL, unos_dretva, &dretvaID[i]);
    }

    // cekaj da dretve zavrse
    for (int i = 0; i < N; i++) {
        pthread_join(dretve[i], NULL);
    }

    // izbrisi mutexe i oslobodi zauzeti memoriju
    pthread_mutex_destroy(&barijera_mutex);
    pthread_mutex_destroy(&unos_mutex);
    pthread_cond_destroy(&barijera_cond);

    free(unosi);
    free(dretve);
    free(dretvaID);

    return 0;
}
