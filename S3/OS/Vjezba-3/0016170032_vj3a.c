#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

sem_t sem_generiran; // generiraj javlja drugim dretvama
sem_t sem_procitan; // druge dretve javljaju generatoru

int broj_zadatka;
int broj_dretva;
int generirano = 0;
long long GENERIRANI_BROJ = 0;

void* generiraj(void* dretva) {
    printf("Dretva koja generira brojeve zapocela je s radom. Broj zadatka = %d\n", broj_zadatka);

    for (int i = 0; i < broj_zadatka; i++) {
        // generiraj broj
        GENERIRANI_BROJ = rand() % 1000000001;
        printf("Generiran broj %lld\n", GENERIRANI_BROJ);

        // javi dretvama da je broj generiran
        sem_post(&sem_generiran);
        // cekaj da neka dretva procita broj
        sem_wait(&sem_procitan);
    }

    // javi da je generiranje brojeva gotovo
    generirano = 1;

    // daj svim dretvama dozvolu da se izvrse do kraja
    for (int i = 0; i < broj_dretva; i++)
        sem_post(&sem_generiran);

    return NULL;
}

void* racunaj(void* dretva) {
    int id = *(int*)dretva; // id dretve

    printf("Dretva %d je pocela s radom\n", id);

    while(1) {
        // cekaj da dretva generira broj
        sem_wait(&sem_generiran);

        // provjeri ako su generirani svi brojevi
        if (generirano)
            break;

        // preuzmi broj
        long long x = GENERIRANI_BROJ;

        // javi dretvi da je broj procitan
        sem_post(&sem_procitan);
        printf("Dretva %d je preuzela zadatak %lld\n", id, x);

        long long zbroj = 0;
        for (long long i = 0; i < x; i++) {
            zbroj += i;
        }
        printf("Dretva %d, zadatak %lld, zbroj %lld\n", id, x, zbroj);
    }

    return NULL;
}

void brisi() {
    sem_destroy(&sem_generiran);
    sem_destroy(&sem_procitan);
}

void prekidna_rutina(int sig) {
    brisi();
    exit(0);
}

int main(int argc, char *argv[]) {
    broj_dretva = atoi(argv[1]);
    broj_zadatka = atoi(argv[2]);

    printf("Broj dretva: %d\nBroj generiranih brojeva: %d\n", broj_dretva, broj_zadatka);

    srand(time(NULL));

    struct sigaction sig_act = {};
    sig_act.sa_handler = prekidna_rutina;
    sigaction(SIGINT, &sig_act, NULL);

    sem_init(&sem_generiran, 0, 0);
    sem_init(&sem_procitan, 0, 0);

    pthread_t generiraj_dretva;
    pthread_t racunaj_dretva[broj_dretva];
    int id[broj_dretva];

    pthread_create(&generiraj_dretva, NULL, generiraj, NULL);
    for (int i = 0; i < broj_dretva; i++) {
        id[i] = i+1;
        pthread_create(&racunaj_dretva[i], NULL, racunaj, &id[i]);
    }

    pthread_join(generiraj_dretva, NULL);
    for (int i = 0; i < broj_dretva; i++)
        pthread_join(racunaj_dretva[i], NULL);

    brisi();

    return 0;
}
