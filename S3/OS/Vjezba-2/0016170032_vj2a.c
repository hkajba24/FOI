#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

<<<<<<< HEAD
void odredi_sumu(int p, int broj_procesa, int broj_elemenata,
                 int velicina_dijela, int glavno_polje[]) {
=======
// globalne varijable
int *glavno_polje;
int shmid;

void brisi(int sig) {
    shmdt((void *)glavno_polje);
    if (shmctl(shmid, IPC_RMID, NULL) != -1)
        printf("\nIzbrisan zajednicki prostor...\n");
    printf("Izlazim iz programa...\n\n");
    exit(0);
}

void odredi_sumu(int p, int broj_procesa, int broj_elemenata, int velicina_dijela, int glavno_polje[]) {
>>>>>>> 6156292f511f8cdc77bddddae43657bf35df3f91
    // odredi pocetak i kraj segmenta
    int pocetak = p * velicina_dijela;
int kraj = pocetak + velicina_dijela - 1;
    // ako je izracunati kraj veci od broja elementa, ispravi ga
    if (kraj >= broj_elemenata)
        kraj = broj_elemenata - 1;

    // odredi sumu
    int suma = 0;
    for (int i = pocetak; i <= kraj; i++)
        suma += glavno_polje[i];

    printf("ZBROJ [%d-%d]:\t %d\n", pocetak, kraj, suma);
}

int izracunaj_broj_procesa(int broj_elementa, int velicina_dijela) {
    if (broj_elementa % velicina_dijela == 0)
        return (broj_elementa / velicina_dijela);
    else
        return (abs(broj_elementa / velicina_dijela) + 1);
}

int main(int argc, char *argv[]) {
    // odredi broj elementa i velicinu logickog dijela
    int broj_elemenata = atoi(argv[1]);
    int velicina_dijela = atoi(argv[2]);

    // zadaj id zajednickog prostora
    shmid = shmget(IPC_PRIVATE, sizeof(int) * broj_elemenata, 0600);

    // stvori polje i spremi ga u zajednicku memoriju
    if (shmid == -1) {
        printf("ERROR: nema zajednicke memorije\n");
        exit(1);
    }

    // zadaj vrijednosti u polje
    glavno_polje = (int *)shmat(shmid, NULL, 0);
    srand(time(NULL));
    for (int i = 0; i < broj_elemenata; i++)
        glavno_polje[i] = rand() % 100;

    // ispisi polje
    printf("\n");
    for (int i = 0; i < broj_elemenata; i++)
        printf("%*d. %d\n", 4, i, glavno_polje[i]);

    // odredi broj procesa
    int broj_procesa = izracunaj_broj_procesa(broj_elemenata, velicina_dijela);
    printf("\nBroj procesa: %d\n\n", broj_procesa);

    // signal handler za brisanje zajednicke memorije
    struct sigaction sigac = {};
    sigac.sa_handler = brisi;
    sigaction(SIGINT, &sigac, NULL);

    // pokreni procese
    for (int p = 0; p < broj_procesa; p++) {
        pid_t pid = fork();

        // obradi dijete proces
        if (pid == 0) {
<<<<<<< HEAD
            odredi_sumu(p, broj_procesa, broj_elemenata, velicina_dijela,
                        glavno_polje);
            shmdt((void *)glavno_polje);
=======
            odredi_sumu(p, broj_procesa, broj_elemenata, velicina_dijela, glavno_polje);
>>>>>>> 6156292f511f8cdc77bddddae43657bf35df3f91
            exit(0);
        }
    }

    // roditelj ceka da se izvrse dijete procesi
    for (int i = 0; i < broj_procesa; i++)
        wait(NULL);

    brisi(0);

    return 0;
}
