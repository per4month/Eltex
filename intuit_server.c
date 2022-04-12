/* Пример простого TCP-сервера для сервиса echo */
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
void main()
{
    int sockfd, newsockfd; /* Дескрипторы для 
слушающего и присоединенного сокетов */
    int clilen; /* Длина адреса клиента */
    int n; /* Количество принятых символов */
    char line[1000]; /* Буфер для приема информации */
    struct sockaddr_in servaddr, cliaddr; /* Структуры 
        для размещения полных адресов сервера и 
        клиента */
    /* Создаем TCP-сокет */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror(NULL);
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family= AF_INET;
    servaddr.sin_port= htons(51000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Настраиваем адрес сокета */
    if(bind(sockfd, (struct sockaddr *) &servaddr, 
    sizeof(servaddr)) < 0){
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    /* Переводим созданный сокет в пассивное (слушающее) 
    состояние. Глубину очереди для установленных 
    соединений описываем значением 5 */
    if(listen(sockfd, 5) < 0){
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    /* Основной цикл сервера */
    while(1){
        /* В переменную clilen заносим максимальную
        длину ожидаемого адреса клиента */
        clilen = sizeof(cliaddr);

        if((newsockfd = accept(sockfd, 
        (struct sockaddr *) &cliaddr, &clilen)) < 0){
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        int pid = fork();
        if (pid == -1) {
            //Error
            perror(NULL);
            close(sockfd);
            close(newsockfd);
            exit(1);
        } else if (pid == 0) {
            //Child
            close(sockfd);
            while((n = read(newsockfd, line, 999)) > 0){
                /* Принятые данные отправляем обратно */
                if((n = write(newsockfd, line, 
                strlen(line)+1)) < 0){
                    perror(NULL);
                    close(sockfd);
                    close(newsockfd);
                    exit(1);
                }
            }
            /* Если при чтении возникла ошибка – завершаем работу */
            if(n < 0){
                perror(NULL);
                close(sockfd);
                close(newsockfd);
                exit(1);
            }
            close(newsockfd);
            exit(0);
            
        }
        else {
            //parent
            close(newsockfd);
        }

    }
}