#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* key number */
#define SHMKEY ((key_t) 1497)
// semaphore key
#define SEMKEY ((key_t) 400L)
// number of semaphores being created
#define NSEMS 1

int sem_id; // semaphore id//
//semaphore buffers
static struct sembuf OP = {0,-1,0};
static struct sembuf OV = {0,1,0};
struct sembuf *P =&OP;
struct sembuf *V =&OV;

typedef union{
    int val;
    struct semid_ds *buf;
    ushort *array;
} semunion;

typedef struct
{
    int value;
} shared_mem;

    shared_mem *total;

int POP(); int VOP();
void process1(); void process2(); void process3(); void process4();

int main()
{
    int value, value1;
    int shmid, pid1, pid2, pid3, pid4, ID, status;
    char *shmadd;
    shmadd = (char *) 0;
    semunion semctl_arg;
    semctl_arg.val = 1;

    /* Create semaphores */
    if((sem_id = semget(SEMKEY, NSEMS, IPC_CREAT | 0666))< 0)
        printf("Error in creating the semaphore./n");

    /* Initialize semaphore */
    int semnum = 0;
    value1 = semctl(sem_id, semnum, SETVAL, semctl_arg);
    if ((value = semctl(sem_id, semnum, GETVAL, semctl_arg)) < 1)
        printf("Eror detected in SETVAL.\n");

    if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror ("shmget");
        exit (1);
    }
    if ((total = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1)
    {
        perror ("shmat");
        exit (0);
    }

    total->value = 0;

    if ((pid1 = fork()) == 0)
    {
        process1();
        printf("From Process 1: counter = %d\n", total->value);
    }
    else if ((pid2 = fork()) == 0)
    {
        process2();
        printf("From Process 2: counter = %d\n", total->value);
    }
    else if ((pid3 = fork()) == 0)
    {
        process3();
        printf("From Process 3: counter = %d\n", total->value);
    }
    else if ((pid4 = fork()) == 0)
    {
        process4();
        printf("From Process 4: counter = %d\n", total->value);
    }
    else
    {
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        waitpid(pid3, NULL, 0);
        waitpid(pid4, NULL, 0);
        printf("Child %d pid has just exited.\n", pid1);
        printf("Child %d pid has just exited.\n", pid2);
        printf("Child %d pid has just exited.\n", pid3);
        printf("Child %d pid has just exited.\n", pid4);

        if (shmdt(total) == -1)
        {
        perror ("shmdt");
        exit (-1);
        }

        if (shmctl(shmid, IPC_RMID, NULL) == -1)
        {
            perror ("shmctl");
            exit (-1);
        }

        /* De-allocate semaphore */
        semctl_arg.val = 0;
        if((status = semctl(sem_id, 0, IPC_RMID, semctl_arg)) < 0)
            printf("Error in removing the semaphore.\n");

         printf("End of Program\n");
    }
}

int POP()
{
    int status;
    status = semop(sem_id, P,1);
    return status;
}
int VOP()
{
    int status;
    status = semop(sem_id, V,1);
    return status;
}
void process1()
{
    for (int i = 0; i < 100000; i++){
        POP();
        (total->value)++;
        VOP();
    }
}
void process2()
{
    for (int i = 0; i < 200000; i++){
        POP();
        (total->value)++;
        VOP();
    }
}
void process3()
{
    for (int i = 0; i < 300000; i++){
        POP();
        (total->value)++;
        VOP();
    }
}
void process4()
{
    for (int i = 0; i < 500000; i++){
        POP();
        (total->value)++;
        VOP();
    }
}
