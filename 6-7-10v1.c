// Первая программа должна создавать сегмент разделяемой памяти и копировать туда свой собственный исходный текст

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#define memory 10*1024


int main() {
    char *locale = setlocale(LC_ALL, "");
    int fd;
    if((fd = open("6-7-10v1.c", O_RDONLY)) < 0){ // Если файл открыть не удалось, печатаем об этом сообщение и прекращаем работу
        printf("Не удаётся открыть файл!");
        exit(-1);
    } 
    char* str;
    str = (char*)malloc(memory*sizeof(char));
    int size = read(fd,str,memory);
    str[memory] = 0;
    close(fd);

    char* file = "6-7-10v1.c";
    int shmID; 

    key_t key;
    key = ftok(file, 0);
    if(key < 0){
        printf("Не получается сгенерировать ключ\n");
        exit(-1);
    }

    // Создание области разделяемой памяти
    shmID = shmget(key, memory*sizeof(char), 0666|IPC_CREAT|IPC_EXCL);
    if (shmID < 0) {
        
        if (errno != EEXIST) {
            printf("Не получилось создать разделяемую память\n");
            exit(-1);
        } else {
            
            shmID = shmget(key, memory*sizeof(char), 0);
            if (shmID < 0) {
                printf("Не получилось найти разделяемую память\n");
                exit(-1);
            }
        }

    }

    // Включение области разделяемой памяти в адресное пространство текущего процесса
    char* shared;
    shared = (char *) shmat (shmID, NULL, 0);
    if (str == (char*)(-1)) {
        printf("Не получилось включить разделяемую память в адресное пространство текущего процесса\n");
        exit(-1);       
    }

    memcpy(shared, str, memory);

    printf("Удалось успешно записать тестовую строку в область разделяемой памяти!\n");
    return 0;
}