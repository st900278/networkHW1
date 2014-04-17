#include<string.h>
#include<netinet/in.h>
#include<signal.h>
#include<sys/wait.h>
void echo(int cli){
    char buffer[1000];
    ssize_t n;
    while((n = read(cli,buffer,1000))>0){
        write(cli,buffer,n);   
    }
    printf("error");
}
void sig4wait(int signo){
    int stat;
    pid_t pid;
    pid = wait(&stat);
    printf("child %d terminated\n",pid);
}
void sig4waitpid(int signo){
    int stat;
    pid_t pid;
    while(pid = waitpid(-1, &stat,WCONTINUED)>0);
    printf("child %d terminated\n",pid);
}
int main(){
    int listenfd;
    struct sockaddr_in server;
    pid_t childpid;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&server,sizeof(server));
    server.sin_port = htons(9877);
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
        printf("IP address is: %s port is: %d\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
        if((childpid = fork()) == 0){
            close(listenfd);
            echo(clientfd);
            exit(0);
        }
        close(clientfd);
    }

}
