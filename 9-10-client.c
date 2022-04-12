#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
	pid_t current_pid;
	int len, maxlen;
	int msqid;
	current_pid = getpid();
	char pathname[] ="9-10-server.c";
	key_t key;
        struct mypidbuf{
		long mtype;
                pid_t pid_client;
		char message[200];
	} pidbuf;

	if((key = ftok(pathname,0)) < 0){
        	printf("Can\'t generate key\n");
        	exit(-1);
    	}

	if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0){
        	printf("Can\'t get msqid\n");
        	exit(-1);
    	} 	
	
	pidbuf.mtype = 1;
	pidbuf.pid_client = current_pid;

	len = sizeof(pid_t)+1; // +1 ??
	if (msgsnd(msqid, (struct mypidbuf *) &pidbuf, 
      		101, 0) < 0){
            	printf("Can\'t send message to queue\n");
            	msgctl(msqid, IPC_RMID, 
                (struct msqid_ds *) NULL);
            	exit(-1);
        }

	while(1){
                if(( len = msgrcv(msqid,
                	(struct mypidbuf *) &pidbuf, 265, 
						current_pid, 0) < 0)){
                     	printf("Can\'t receive message from queue\n");
                     	exit(-1);
                  }
		printf("%s%d\n", pidbuf.message, pidbuf.pid_client);
		if(pidbuf.pid_client == current_pid){
			printf("client_is_closed\n");
			exit(0);
		}
	}	
	return 0;
}
