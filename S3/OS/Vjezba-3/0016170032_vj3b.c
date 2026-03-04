#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <wait.h>
#include <time.h>

#define SEMAFOR_PISI 0
#define SEMAFOR_PUN 1
#define SEMAFOR_PRAZAN 2

struct zajednicko {
    int ulaz;
    int izlaz;
    int ukupno;
    int m[5];
};

int semID; // id semafora
int shmID; // id zajednickog prostora
struct zajednicko* shared;

// ispitaj semafor -- slicno sem_wait
void sem_ispitaj(int semafor) {
    struct sembuf op;
    op.sem_num = semafor;
    op.sem_op = -1;
    op.sem_flg = 0;
    semop(semID, &op, 1);
}

// postavi semafor -- slicno sem_post
void sem_postavi(int semafor) {
    struct sembuf op;
    op.sem_num = semafor;
    op.sem_op = 1;
    op.sem_flg = 0;
    semop(semID, &op, 1);
}

void proizvodac(int ID, int N) {
    srand(time(NULL) * getpid());
    for (int i = 0; i < N; i++) {
        sem_ispitaj(SEMAFOR_PUN);
        sem_ispitaj(SEMAFOR_PISI);

        int broj = rand() % 1000;

        shared->m[shared->ulaz] = broj;
        shared->ulaz = (shared->ulaz + 1) % 5;

        printf("Proizvodac %d salje '%d'\n", ID, broj);

        sem_postavi(SEMAFOR_PISI);
        sem_postavi(SEMAFOR_PRAZAN);
    }

    printf("Proizvodac %d zavrsio sa slanjem\n", ID);
    exit(0);
}

void potrosac() {
    int zbroj = 0;
    for (int i = 0; i < shared->ukupno; i++) {
        sem_ispitaj(SEMAFOR_PRAZAN);

        zbroj += shared->m[shared->izlaz];
        shared->izlaz = (shared->izlaz + 1) % 5;

        printf(" -- Potrosac prima %d\n", shared->m[shared->izlaz]);

        sem_postavi(SEMAFOR_PUN);
    }
    printf("Potrosac - zbroj primljenih brojeva = %d\n", zbroj);
    exit(0);
}

void brisanje() {
    shmdt(shared);
    shmctl(shmID, IPC_RMID, NULL);
    semctl(semID, 0, IPC_RMID);
}

void prekidna_rutina(int sig) {
    printf("Poslan signal, izlazim iz programa...\n");
    brisanje();
    exit(0);
}

int main(int argc, char *argv[]) {
    int broj_proizvodaca = atoi(argv[1]);
    int broj_generiranih = atoi(argv[2]);

    // obradi signal prekida
    struct sigaction sig_act = {};
    sig_act.sa_handler = prekidna_rutina;
    sigaction(SIGINT, &sig_act, NULL);

    // kreiraj zajednicki prostor
    shmID = shmget(IPC_PRIVATE, sizeof(struct zajednicko), IPC_CREAT | 0600);
    shared = (struct zajednicko*) shmat(shmID, NULL, 0);

    shared->ulaz = 0;
    shared->izlaz = 0;
    shared->ukupno = broj_proizvodaca * broj_generiranih;

    // kreiraj skup semafora
    semID = semget(IPC_PRIVATE, 3, IPC_CREAT | 0600);

    // zadaj vrijednosti semafora u skupu
    semctl(semID, SEMAFOR_PISI, SETVAL, 1);
    semctl(semID, SEMAFOR_PUN, SETVAL, 5);
    semctl(semID, SEMAFOR_PRAZAN, SETVAL, 0);

    // kreiraj procese
    pid_t proces_potrosac1 = fork();
    if (proces_potrosac1 == 0)
        potrosac();

    pid_t proces_potrosac2 = fork();
    if (proces_potrosac2 == 0)
        potrosac

    for (int i = 0; i < broj_proizvodaca; i++) {
        pid_t proces_proizvodac = fork();
        if (proces_proizvodac == 0)
            proizvodac(i+1, broj_generiranih);
    }

    // cekaj da procesi zavrse
    for (int i = 0; i < broj_proizvodaca + 1; i++)
        wait(NULL);

    brisanje();

    return 0;
}
