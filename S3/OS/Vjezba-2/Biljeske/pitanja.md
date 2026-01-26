# Operacijski sustavi - Vježba 2
## Pitanja i odgovori
---
1. Napravi novi proces u kodu

Koristimo `fork()` da napravimo novi proces.

---
2. Napravi novu dretvu u kodu

Koristimo `pthread_create()` da napravimo novu dretvu.

---
3. Što je to dretva i čemu služi?

Dretva je osnovna jedinica izvođenja programa u procesu. Svaki proces može imati jednu ili više dretvi, koje mogu paralelno izvoditi program.

---
4. Što je to proces?

Proces je okolina u kojem se izvodi program. Sastoji se od segmenta instrukcija, segmenta korisničkih podataka i segmenta sustavskih podataka.

---
5. Što je zajednički spremnik i zašto je važan?

Zajednički spremnik je alocirani memorijski prostor koji više procesa može koristiti kako bi međusobno dijelili podatke.

---
6. Što radi `ipcs`?

Pokazuje informacije o iskorištenim sredstvima za međuprocesnu komunikaciju.

---
7. Čemu služi `ipcrm`?

Služi sa brisanje određenih sredstva za međuprocesnu komunikaciju.

---
8. Kako prokazati procese?

Sa naredbom `$ ps -x`.

---
9. Završi proces u funkciji

Proces u funkciji možemo završiti sa sustavskim pozivom `exit(0)`.

---
10. Što to dijele dvije dretve istoga procesa?

Zajednički memorijski prostor koji je alociran procesu.

---
11. Promjeni kod tako da ispis u 2.b zadatku bude sporiji

```c
int main() {
...
    for (int p = 0; p < broj_procesa; p++) {
        usleep(1000);
        ...
    }
...
}
```

---
12. Gdje se poziva dretva?

U kontekstu vježbe 2.c, dretva se poziva u `main()` funkciji pozivom `pthread_create()`.

---
13. Napiši kod tako da roditelj sačeka završetak proces djeteta

```c
...
for (int i = 0; i < broj_procesa; i++) {
    wait(NULL); 
}
...
```

---
14. Kako `void` može primiti više argumenata?

Pomoću `struct`.

---
15. Što je/gdje je kritični odsječak u kodu?

Kritički odsječak je dio koda koji u jednom trenutku može izvršavati samo jedna dretva. U mojem programu funkcija se zove `void kriticki_odsjecak()`.

---
16. Stvori segmente dijeljene memorije veličine X bajtova

Segment dijeljene memorije stvaramo pomoću `shmget`.

```c
shmget(IPC_PRIVATE, X, 0600);
```

---
17. Do kad će dijeljena memorija biti zauzeta ako ju ne obrišemo?

Dijeljena memorije će ostati zauzeta do kad se ručne ne obriše, bilo to putem programa i sustavskog poziva `shmctl` ili putem terminala naredbom `ipcrm -m key_shm`.

---
18. Kako završiti tekuću dretvu?

Dretvu završavamo sustavskim pozivom `pthread_exit(NULL)`.

---
19. Pričekaj da neko dijete završi ako postoji

Može se čekati bilo koje dijete pomoću:

```c
...
for (int i = 0; i < broj_procesa; i++) {
    wait(NULL);
}
...
```

ili se može čekati izvršenje specifičnog djeteta:

```c
...
pid_t process_id = fork();

if (process_id == 0) {
    neka_funkcija();
} else {
    waitpid(pid, &status, 0);
}
...
```

---
20. Ispiši identifikator trenutnog procesa

Identifikator trenutnog procesa se ispisuje pomoću sustavskog poziva `getpid()`, npr. u `main()`:

```c
int main() {
    ...
    printf("ID trenutnog procesa: %d\n", getpid());
    ...
}
```

---
21. Ispiši identifikator trenutne dretve

```c
int main() {
    ...
    pthread_t thread_id = pthread_self();
    printf("ID trenutne dretve: %d\n", (unsigned long)thread_id);
    ...
}
```

---
22. Obriši segment djeljene memorije na kraju `main()` funkcije

Segment djeljene memorije brišemo pomoću briši funkcije koju smo napisali za signal handler u zadatku.

---
23. Objasni razliku između Deckerovog i Lampartovog algoritma



---
