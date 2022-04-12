#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
	int msqid;
	int len, maxlen;
	char pathname[] = "server.c";
	key_t key;
	
	struct mypidbuf{
		long mtype;
		pid_t pid_client;
		char message[200];
	} pidbuf;
	if(( key=ftok (pathname, 0)) < 0){
		printf("cant generate key\n"); exit(-1);}
	
	if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0){
        	printf("Can\'t get msqid\n");
        	exit(-1);
	} 
	while(1){
		printf("waite for clients...\n");
		maxlen = 260;	
		if(( len = msgrcv(msqid, 
         		(struct mypidbuf *) &pidbuf, maxlen, 1, 0) < 0)){
            	        printf("Can\'t receive message from queue\n");
            		exit(-1);
        	}
		strcpy(pidbuf.message, "Hello, from server your PID is:");

		if (pidbuf.mtype == 1 ){ 
			pidbuf.mtype = pidbuf.pid_client;
			if (msgsnd(msqid, (struct mypidbuf *) &pidbuf, 
            			265, 0) < 0){
            			printf("Can\'t send message to queue\n");
            			msgctl(msqid, IPC_RMID, 
                		(struct msqid_ds *) NULL);
            			exit(-1);
        		}
		printf("sendet to client pid: %d\n", pidbuf.pid_client);
		}
		
	}
	return 0;
}	