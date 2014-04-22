#include<string.h>
#include<netinet/in.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<sys/socket.h>
#include<limits.h>
#include<unistd.h>
#include<stdlib.h>
#include <arpa/inet.h>
char buffer[20000000];
char buffer2[2148];
int fexisted(char * path){
	FILE* fp = fopen(path,"r");
	if(fp == NULL){
		return 0;
	}
	else {
		fclose(fp);
		return 1;
	}
}
int fsize(char* path){
	struct stat st;
	stat(path,&st);
	return st.st_size;
}
void echo(int cli){
	ssize_t n,ret;
	FILE* fp;
	char tmp[2148];
	struct stat st;
	int fileSize;
	char listname[1024];
	n = read(cli, tmp, 8);
	n = read(cli, buffer2, strtol(tmp, NULL, 10)); 
	memset(tmp, 0, sizeof(tmp));
	//strcpy(listname, "hw1TestFile/");
	strcat(listname, buffer2);
	fileSize =  fsize(listname);


	sprintf(tmp,"%8d",fileSize);
	write(cli,tmp,strlen(tmp));
	fp = fopen(listname,"rb");

	ret = fread(buffer, sizeof(char), fileSize, fp);
	write(cli, buffer, ret);
	fclose(fp);
	// file transfer
	
	while(1){ //read filename size
		memset(buffer,0,sizeof(buffer));
		ret = read(cli,buffer,8);
		//printf("%s\n", buffer);
		if(buffer[7] == '0')break;
		else if(buffer[7] == '2')continue;
		memset(buffer, 0, sizeof(buffer));
		ret = read(cli,buffer,8);
		int lenn = strtol(buffer, NULL, 10);
		memset(buffer, 0, sizeof(buffer));
		ret = read(cli, buffer, lenn); 
		
		char tmpp[1024] = "";
		strcat(tmpp,buffer);
		printf("Downloading : %s\n", tmpp);
		memset(buffer, 0, sizeof(buffer));
		if(!fexisted(tmpp)){
			sprintf(buffer, "%8d",0);
			printf("File not exist.\n");
			write(cli, buffer, 8);
			memset(buffer,0,sizeof(buffer));
			continue;
		}
		else{
			sprintf(buffer, "%8d",1);
			write(cli, buffer, 8);
		}
		int transFileSize = fsize(tmpp);
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp, "%8d", transFileSize);
		memset(buffer, 0, sizeof(buffer));
		write(cli, tmp, strlen(tmp));
		printf("File Size : %s\n",tmp);
		FILE* fp = fopen(tmpp,"rb");
		int tmpSize = transFileSize;
		while(1){
			
			if(tmpSize>2048){
				ret = fread(buffer, sizeof(char), 2048, fp);
				write(cli, buffer, ret);
				tmpSize -=2048;
			}
			else{
				ret = fread(buffer, sizeof(char), tmpSize, fp);
				write(cli, buffer, ret);
				break;
			}
		}
		fclose(fp);
		
	}

}
void sig4waitpid(int signo){
	int stat;
	pid_t pid;
	while(pid = waitpid(-1, &stat,WCONTINUED)>0);
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
        printf("IP address %s port %d  connected\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
        if((childpid = fork()) == 0){
            close(listenfd);
            echo(clientfd);
            printf("IP address %s port %d terminated\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
            return 0;
        }
        close(clientfd);
    }

}
