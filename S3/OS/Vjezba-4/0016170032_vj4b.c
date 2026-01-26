#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define LINUX 0
#define MICROSOFT 1

int TRENUTNI = -1;

int MOGU_UCI;
int BROJ_PROGRAMERA;

int RED_LINUX = 0;
int RED_MICROSOFT = 0;
int U_RESTORANU = 0;
int UZASTOPNO = 0;

pthread_mutex_t monitor;
pthread_cond_t red_linux;
pthread_cond_t red_microsoft;

void print_stanje(const char *radnja, char tip) {
    // ispis linux
    int linux_slobodno = BROJ_PROGRAMERA - RED_LINUX;
    printf("Red Linux: ");
    for (int i = 0; i < RED_LINUX; i++)
        printf("L");
    for (int i = 0; i < linux_slobodno; i++)
        printf("-");

    // ispis microsoft
    int microsoft_slobodno = BROJ_PROGRAMERA - RED_MICROSOFT;
    printf("  Red Microsoft: ");
    for (int i = 0; i < RED_MICROSOFT; i++)
        printf("M");
    for (int i = 0; i < microsoft_slobodno; i++)
        printf("-");

    // ispis restoran
    printf("  Restoran: ");
    for (int i = 0; i < U_RESTORANU; i++)
        printf("%c", TRENUTNI == LINUX ? 'L' : 'M');

    printf("  --> %c %s\n", tip, radnja);
}

void udi_restoran(int vrsta) {
    pthread_mutex_lock(&monitor);

    while (U_RESTORANU > 0 && (TRENUTNI != vrsta || (UZASTOPNO >= MOGU_UCI && ((vrsta == LINUX && RED_MICROSOFT) || (vrsta == MICROSOFT && RED_LINUX))))) {
        if (vrsta == LINUX) {
            RED_LINUX++;
            print_stanje("u red cekanja", 'L');
            pthread_cond_wait(&red_linux, &monitor);
            RED_LINUX--;
        }
        else {
            RED_MICROSOFT++;
            print_stanje("u red cekanja", 'M');
            pthread_cond_wait(&red_microsoft, &monitor);
            RED_MICROSOFT--;
        }
    }

    if (U_RESTORANU == 0) {
        TRENUTNI = vrsta;
        UZASTOPNO = 0;
    }

    U_RESTORANU++;
    UZASTOPNO++;

    print_stanje("u restoran", vrsta == LINUX ? 'L' : 'M');

    pthread_mutex_unlock(&monitor);
}

void izadi_restoran(int vrsta) {
    pthread_mutex_lock(&monitor);

    U_RESTORANU--;
    print_stanje("iz restorana", vrsta == LINUX ? 'L' : 'M');

    if (U_RESTORANU == 0) {
        UZASTOPNO = 0;

        if (vrsta == LINUX && RED_MICROSOFT)
            pthread_cond_broadcast(&red_microsoft);
        else if (vrsta == MICROSOFT && RED_LINUX)
            pthread_cond_broadcast(&red_linux);
        else {
            pthread_cond_broadcast(&red_linux);
            pthread_cond_broadcast(&red_microsoft);
        }
    }

    pthread_mutex_unlock(&monitor);
}

void *programer(void *arg) {
    // odredi vrstu programera
    int vrsta = *(int*)arg;

    srand(time(NULL) * vrsta);

    usleep(rand() % 100);
    udi_restoran(vrsta);
    usleep(rand() % 100);
    izadi_restoran(vrsta);

    return NULL;
}

int main(int argc, char *argv[]) {
    MOGU_UCI = atoi(argv[1]);
    BROJ_PROGRAMERA = atoi(argv[2]);

    pthread_mutex_init(&monitor, NULL);
    pthread_cond_init(&red_linux, NULL);
    pthread_cond_init(&red_microsoft, NULL);

    pthread_t dretve[2 * BROJ_PROGRAMERA];
    pthread_t vrste[2 * BROJ_PROGRAMERA];

    // pokreni dretve
    int i = 0;
    for (int j = 0; j < BROJ_PROGRAMERA; j++) {
        vrste[i] = LINUX;
        pthread_create(&dretve[i], NULL, programer, &vrste[i]);
        i++;

        vrste[i] = MICROSOFT;
        pthread_create(&dretve[i], NULL, programer, &vrste[i]);
        i++;
    }

    // cekaj da dretve zavrse
    for (int j = 0; j < i; j++) {
        pthread_join(dretve[j], NULL);
    }

    return 0;
}
