#include<string.h>
#include<netinet/in.h>
#include<signal.h>
#include<sys/wait.h>
void echo(int cli){
	char buffer[2048];
	ssize_t n;
	printf("Connection Closed. IP address is: %s port is: %d\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
	FILE* fp;
	fp = fopen("DownloadList.txt","r");
	write(cli,"DownloadList.txt",16);
	size_t ret = fread(buffer, 1, 2048, fp);
	while(ret>0){
		write(cli, buffer, ret);
		ret = fread(buffer, 1, 2048, fp);
	}
	fclose(fp);
	
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
    server.sin_port = htons(argc[0]);
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
            exit(0);
        }
        close(clientfd);
    }

}
