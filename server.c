#include<string.h>
#include<netinet/in.h>
#include<signal.h>
#include<sys/wait.h>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include <arpa/inet.h>
#define ms(); memset(buffer, 0, sizeof(buffer));
char buffer[2148];
int fexist(char * path){
	FILE* fp = fopen(path,"r");
	if(fp)	return 1;
	else 	return 0;
}
void echo(int cli){
	ssize_t n;
	FILE* fp;
	char tmp[2148];
	char buf[] = "downloadlist.txt";
	write(cli,buf,sizeof(buf));
	fp = fopen("hw1TestFile/DownloadList.txt","rb");
	ms();
	read(cli,buffer,2048);
	
	
	ssize_t ret;
	while(!feof(fp)){
		ms();
		ret = fread(buffer, sizeof(char), 2048, fp);
		printf("%s\n",buffer);
		write(cli, buffer, ret);
	}
	ret = fread(buffer, sizeof(char), 2048, fp);
	write(cli, buffer, ret);
	ms();
	//printf("%d",sizeof(buffer));
	//write(cli,buffer, 0);

	fclose(fp);
	printf("here\n");
	while(1){
		ms();
		ret = read(cli, buffer, 2048);
		puts(buffer);
		strcpy(tmp, "success");
		write(cli, tmp, sizeof(tmp));
		/*
		if(ret<=0)continue;
		printf("get");
		char* s;
		if(!fexist(buffer)){
			strcpy("FILE not exist", buffer);
			write(cli, buffer, sizeof(buffer));
		}
		else{
			strcpy("FILE exist", buffer);
			write(cli, buffer, sizeof(buffer));
		}
		printf("%s",buffer);
		/*
		s = malloc(ret * sizeof(char));
		strcpy(s, buffer);
		read(cli, buffer, 2048);
		FILE* fp = fopen(s,"r");
		while((ret = fread(buffer, sizeof(char), 2048,fp))>0){
			write(cli, buffer, ret);
		}*/
	}

}
void sig4waitpid(int signo){
	int stat;
	pid_t pid;
	while(pid = waitpid(-1, &stat,WCONTINUED)>0);
	printf("child %d terminated\n",pid);
}
int main(int argc, char* argv[]){
    int listenfd;
    struct sockaddr_in server;
    pid_t childpid;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&server,sizeof(server));
    server.sin_port = htons(strtol(argv[1], NULL, 10));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listenfd, (struct sockaddr*)&server, sizeof(server));
    listen(listenfd,10);
    signal(SIGCHLD,sig4waitpid);
    while(1){
        int clientfd;
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);
        clientfd = accept(listenfd, (struct sockaddr*)&client_addr, &addrlen);
        printf("New Connection IP address is: %s port is: %d\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
        if((childpid = fork()) == 0){
            close(listenfd);
            echo(clientfd);
            return 0;
        }
        close(clientfd);
    }

}
