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
char buffer[65536];
char buffer2[2148];
int fexist(char * path){
	FILE* fp = fopen(path,"r");
	if(fp){
		fclose(fp);
		return 1;
	}
	else {
		fclose(fp);
		return 0;
	}
}
int fsize(char* path){
	struct stat st;
	stat(path,&st);
	//printf("%d", st.st_size);
	return st.st_size;
}
void echo(int cli){
	ssize_t n,ret;
	FILE* fp;
	char tmp[2148];
	struct stat st;
	int fileSize;
	char listname[] = "downloadlist.txt";
	fileSize =  fsize("hw1TestFile/DownloadList.txt");
	sprintf(tmp, "%8zd", strlen(listname));
	//sprintf(tmp,"%8d",listname,fileSize);
	
	write(cli,tmp, strlen(tmp));
	write(cli,listname, strlen(listname));
	sprintf(tmp,"%8d",fileSize);
	write(cli,tmp,strlen(tmp));
	fp = fopen("hw1TestFile/DownloadList.txt","rb");

	ret = fread(buffer, sizeof(char), fileSize, fp);
	write(cli, buffer, ret);
	fclose(fp);
	// file transfer
	
	while(ret = read(cli,buffer,8)){ //read filename size
		printf("filename size:%d\n",(int)strtol(buffer, NULL, 10));
		ret = read(cli, buffer, strtol(buffer, NULL, 10)); // filename
		printf("NeedFile : %s\n", buffer);
		printf("ret = %zd\n",ret);
		int transFileSize = fsize(buffer);
		memset(tmp,0,sizeof(tmp));
		printf("%d",transFileSize);
		sprintf(tmp, "%8d", transFileSize);
		printf("%s",tmp);
		printf("strlen of tmp: %zd\n",strlen(tmp));
		
		write(cli, tmp, strlen(tmp));
		printf("tmp: %s\n",tmp);
		FILE* fp = fopen(buffer,"rb");
		ret = fread(buffer, sizeof(char), transFileSize, fp);
		write(cli, buffer, ret);
		fclose(fp);
		
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
