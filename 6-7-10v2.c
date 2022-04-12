// Вторая программа должна брать текст из области разделяемой памяти, печатать его на экране и удалять сегмент разделяемой памяти из системы.

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>

#define memory 10*1024


int main() {
    char *locale = setlocale(LC_ALL, "");

    char* file = "6-7-10v1.c";   // данный файл должен существовать в дирректории
    char* text; // указатель на массив символов в области разделяемой памяти

    int fd;
    if(fd = open(file, O_RDONLY) < 0) {
        printf("Не удаётся открыть файл!\n");
        exit(-1);
    }

    int shmID; 
    key_t key;

    key = ftok(file, 0);
    if(key < 0){
        printf("Не получается сгенерировать ключ\n");
        exit(-1);
    }

    // Поиск области разделяемой памяти
    shmID = shmget(key, memory, 0);
    if (shmID < 0) {
        printf("Не получилось найти область разделяемой памяти\n");
        exit(-1);
    }

    text = (char *) shmat (shmID, NULL, 0);
    if (text == (char*)(-1)) {
        printf("Не получилось включить разделяемую память в адресное пространство текущего процесса\n");
        exit(-1);       
    }

    printf("Попытка прочитать содержимое области разделяемой памяти\n");
    printf("Записано содержимое: %s\n", text);

    // Удаление сегмента разделяемой памяти из системы
    if (shmctl(shmID, IPC_RMID, NULL) < 0) {
        printf("Не получилось удалить разделяемую память из системы\n");
        exit(-1); 
    }

    return 0;
}