#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

void odredi_sumu(int p, int broj_procesa, int broj_elemenata,
                 int velicina_dijela, int glavno_polje[]) {
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

int main() {
    // odredi broj elemenata i velicinu logickih dijelova
    int broj_elemenata = 10001, velicina_dijela;
    while (broj_elemenata > 10000) {
        printf("\nUnesite broj elemenata polja i velicinu dijelova polja: ");
        scanf("%d %d", &broj_elemenata, &velicina_dijela);
    }

    // stvori polje i spremi ga u zajednicku memoriju
    int *glavno_polje;
    int shmid = shmget(IPC_PRIVATE, sizeof(int) * broj_elemenata, 0600);
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

    // pokreni procese
    for (int p = 0; p < broj_procesa; p++) {
        pid_t pid = fork();

        // obradi dijete proces
        if (pid == 0) {
            odredi_sumu(p, broj_procesa, broj_elemenata, velicina_dijela,
                        glavno_polje);
            shmdt((void *)glavno_polje);
            exit(0);
        }
    }

    // roditelj ceka da se izvrse dijete procesi
    for (int i = 0; i < broj_procesa; i++)
        wait(NULL);

    shmdt((void *)glavno_polje);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
