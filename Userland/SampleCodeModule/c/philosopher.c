#include <standardlib.h>

#define MAX_PHILOS 20
#define THINK_MS   700
#define EAT_MS     500

static uint8_t forks[MAX_PHILOS];   /* ids de semáforo */

static void philosopher(uint64_t argc, char *argv[])
{
    int id   = (int)argv[0][0];          /* 0..N-1 codificado en un byte */
    int left = id;                       /* tenedor izquierdo  = id     */
    int right= (id + 1) % argc;          /* tenedor derecho            */

    while (1) {
        printf("P%d pensando\n", id);
        wait(THINK_MS);

        /* toma tenedores (par -> izq primero, impar -> der primero) */
        if (id & 1) {          /* impar */
            sem_wait(forks[right]);
            sem_wait(forks[left]);
        } else {               /* par   */
            sem_wait(forks[left]);
            sem_wait(forks[right]);
        }

        printf("P%d COMIENDO 🍝\n", id);
        wait(EAT_MS);

        /* suelta tenedores */
        sem_post(forks[left]);
        sem_post(forks[right]);
    }
}

int main(int argc, char *argv[])
{
    int N = 5;                       /* por defecto */
    if (argc == 2)
        N = (int)unsigned_str_to_num((uint64_t[]){0}, 3, argv[1]);

    if (N < 2 || N > MAX_PHILOS) {
        printf("phylo: N debe ser 2..%d\n", MAX_PHILOS);
        return 1;
    }

    /* crear N semáforos (un tenedor cada uno) */
    for (int i = 0; i < N; i++) {
        char name[4] = "f0";
        name[1] += i;                 /* "f0", "f1", ... */
        forks[i] = sem_open(name, 1);
    }

    /* lanzar N procesos filósofos */
    for (int i = 0; i < N; i++) {
        char arg[2] = { '0' + i , 0 };
        char *args[] = { arg, NULL };
        createProcess(philosopher, N, args);
    }

    return 0;      /* el padre (shell) vuelve inmediatamente */
}