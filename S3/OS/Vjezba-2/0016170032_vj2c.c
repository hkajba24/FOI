#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// zajednicke globalne varijable
int ZASTAVICE[2] = {0, 0};     // koristimo da pratimo koja se dretva zeli sljedece izvrsiti
int PRAVO = 0; // koja dretva se prva izvodi

void prekidna_rutina(int sig) {
    printf("\nPoslan prekidni signal, izlazim iz programa...\n");
    exit(0);
}

void kriticki_odsjecak(int broj_dretve, int i) {
    for (int j = 1; j <= 5; j++) {
        printf("Dretva %d, kriticki odsjecak broj %d: (%d/5)\n",
               broj_dretve + 1, i + 1, j);
        sleep(1);
    }
    printf("\n");
}

void izlaz_kriticki(int prva_dretva, int druga_dretva) {
    PRAVO = druga_dretva;
    ZASTAVICE[prva_dretva] = 0;
}

void ulaz_kriticki(int prva_dretva, int druga_dretva, int i) {
    // oznacujemo da trenutna dretva zeli uci u kriticni kod
    ZASTAVICE[prva_dretva] = 1;

    while (ZASTAVICE[druga_dretva] != 0) {
        // ako nije pravo prve dretve, iskljuci signal
        if (PRAVO == druga_dretva) {
            ZASTAVICE[prva_dretva] = 0;
            // pricekaj da zavrsi druga dretva
            while (PRAVO == druga_dretva);
            ZASTAVICE[prva_dretva] = 1;
        }
    }
    // izvrsi kriticki odsjecak
    kriticki_odsjecak(prva_dretva, i);

    // izadi iz ulaza i promjeni dretve
    izlaz_kriticki(prva_dretva, druga_dretva);
}

void *dakker_algo(void *dretva) {
    int prva_dretva = *(int *)dretva; // prva dretva, tj. trenutna dretva
    int druga_dretva = 1 - prva_dretva;

    for (int i = 0; i < 10; i++) {
        ulaz_kriticki(prva_dretva, druga_dretva, i);
    }

    return NULL;
}

int main() {
    // polja koja koristimo za spremanje dretvi
    pthread_t dretve[2];
    pthread_t brojevi_dretva[2] = {0, 1};

    // deklariramo signal handler
    struct sigaction sigac = {};
    sigac.sa_handler = prekidna_rutina;
    sigaction(SIGINT, &sigac, NULL);

    // stvaramo dretve
    pthread_create(&dretve[0], NULL, dakker_algo, &brojevi_dretva[0]);
    pthread_create(&dretve[1], NULL, dakker_algo, &brojevi_dretva[1]);

    // cekaj izvrsenje
    pthread_join(dretve[0], NULL);
    pthread_join(dretve[1], NULL);

    return 0;
}
