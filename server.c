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
	printf("fexist: %s\n", path);
	FILE* fp = fopen(path,"r");
	if(fp == NULL){

		return 0;
	}
	else {
		printf("yese\n");
		fclose(fp);
		return 1;
	}
}
int fsize(char* path){
	struct stat st;
	stat(path,&st);
	printf("st.st_size is %d\n", st.st_size);
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
	//sprintf(tmp, "%8zd", strlen(listname));
	//sprintf(tmp,"%8d",listname,fileSize);
	
	/*write(cli,tmp, strlen(tmp));
	write(cli,listname, strlen(listname));
	*/

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
		if(buffer[7] == '0')break;
		ret = read(cli,buffer,8);
		
		ret = read(cli, buffer, strtol(buffer, NULL, 10)); 
		char tmpp[1024] = "";
		strcat(tmpp,buffer);
		printf("NeedFile : %s\n", tmpp);
		printf("test\n");
		memset(buffer, 0, sizeof(buffer));
		if(!fexisted(tmpp)){
			sprintf(buffer, "%8d",0);
			write(cli, buffer, 8);
			break;
		}
		else{
			sprintf(buffer, "%8d",1);
			write(cli, buffer, 8);
		}
		printf("ret = %zd\n",ret);
		int transFileSize = fsize(tmpp);
		memset(tmp,0,sizeof(tmp));
		printf("transFileSize = %d\n",transFileSize);
		sprintf(tmp, "%8d", transFileSize);
		printf("%s\n",tmp);
		printf("strlen of tmp: %zd\n",strlen(tmp));
		memset(buffer, 0, sizeof(buffer));
		write(cli, tmp, strlen(tmp));
		printf("tmp: %s\n",tmp);
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
		//ret = fread(buffer, sizeof(char), transFileSize, fp);
		//write(cli, buffer, ret);
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
        printf("IP address %s port %d  connected\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
        if((childpid = fork()) == 0){
            close(listenfd);
            echo(clientfd);
            printf("IP address%s port %d terminated\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
            return 0;
        }
        close(clientfd);
    }

}
