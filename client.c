

#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
void echo(int socketfd){
    char buffer[1000];
    int len;
    while(scanf("%s",buffer)){
        len = strlen(buffer);
        write(socketfd, buffer, len);
        if(read(socketfd,buffer,1000)==0){
            printf("server stopped");
            exit(0);
        }
        printf("%s\n",buffer);
    }
}
int main(){
    int socketfd[10];
    int i;
    struct sockaddr_in dest;
    for(i=0;i<10;i++){
        socketfd[i] = socket(AF_INET,SOCK_STREAM,0);
        bzero(&dest,sizeof(dest));
   
        dest.sin_port = htons(9877);
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect(socketfd[i],(struct sockaddr*)&dest, sizeof(dest));
    }
    echo(socketfd[0]);
    return 0;
   
}
