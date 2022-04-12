/* Программа 1 для иллюстрации некорректности работы с разделяемой памятью*/
/* Мы организуем разделяемую память для массива из 3-х целых чисел. Первый элемент массива является счетчиком числа запусков программы 1, т. е. данной программы, второй элемент массива - счетчиком числа запусков программы 2, третий элемент массива - счетчиком числа запусков обеих программ */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

int main()
{
    int *array;
    int shmid, semid;
    int new = 1;
    char pathname[] = "8-7v1.c"; // Имя файла, использующееся для генерации ключа. Файл с таким именем должен существовать в текущей директории
    key_t key;
    long i;
    struct sembuf mybuf;

    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((shmid = shmget(key, 3 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0)
    {
        if (errno != EEXIST)
        {
            printf("Can\'t create shared memory\n");
            exit(-1);
        }
        else
        {
            if ((shmid = shmget(key, 3 * sizeof(int), 0)) < 0)
            {
                printf("Can\'t find shared memory\n");
                exit(-1);
            }
            new = 0;
        }
    }

    if ((semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0)
    {
        if (errno != EEXIST)
        {
            printf("Can\'t get semid\n");
            exit(-1);
        }
        else
        {
            if ((semid = semget(key, 1, 0)) < 0)
            {
                printf("Can\'t find sem\n");
                exit(-1);
            }
        }
    }
    else
    {
        mybuf.sem_op = 1;
        mybuf.sem_flg = 0;
        mybuf.sem_num = 0;
        if (semop(semid, &mybuf, 1) < 0)
        {
            printf("Can\'t wait for condition\n");
            exit(-1);
        }
    }

    mybuf.sem_op = -1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop(semid, &mybuf, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(-1);
    }

    if ((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1))
    {
        printf("Can't attach shared memory\n");
        exit(-1);
    }

    if (new)
    {

        array[0] = 1;
        array[1] = 0;
        array[2] = 1;
    }
    else
    {

        array[0] += 1;

        for (i = 0; i < 1000000000L; i++)
            ;
        for (i = 0; i < 1000000000L; i++)
            ;
        for (i = 0; i < 1000000000L; i++)
            ;

        array[2] += 1;
    }

    printf("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n", array[0], array[1], array[2]);

    if (shmdt(array) < 0)
    {
        printf("Can't detach shared memory\n");
        exit(-1);
    }
    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;

    if (semop(semid, &mybuf, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(-1);
    }
    return 0;
}