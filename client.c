#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include <arpa/inet.h>
#define ms(); memset(buffer, 0, sizeof(buffer));
char buffer[2148];
char* nameFetch(int socketfd){
	ssize_t ret;
	char *s;
	ms();
	ret = read(socketfd, buffer, 2148);

	s = malloc(ret * sizeof(char));
	strcpy(s, buffer);
	//printf("%s",buffer);
	return s;
}

void fileTrans(int socketfd, char* fileName){
	FILE* fp = fopen(fileName, "wb");
	ssize_t ret;
	
	char buf[] = "success";
	write(socketfd, buf, sizeof(buf));
	while(1){
		ms();
		ret = read(socketfd, buffer, 2048);
		printf("ret = %zd\n",ret);
		if(ret <= 0)break;
		printf("%s\n",buffer);
		fwrite(buffer, sizeof(char), ret, fp);
		fflush(fp);
	}
	printf("test");
	fclose(fp);
}
void fileReq(int socketfd, char * fileName){
	printf("%s",fileName);
	ssize_t ret;
	char buffer[2148];
	write(socketfd, fileName, sizeof(fileName));
	ret = read(socketfd, buffer, 2048);
	printf("%s",buffer);
	//fileTrans(socketfd, fileName);
	
}
int main(int argc, char* argv[]){
	if(argc<4){
		printf("Format wrong.\n");
		return 0;
	}
	int socketfd;
	int i;
	struct sockaddr_in dest;
	char* fileName;
	char fileRequest[10000];
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&dest,sizeof(dest));
	dest.sin_port = htons(strtol(argv[2], NULL, 10));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(argv[1]);
	connect(socketfd,(struct sockaddr*)&dest, sizeof(dest));
	mkdir("./Download",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
	
	fileName = nameFetch(socketfd);
	//printf("%s", fileName);
	fileTrans(socketfd, fileName);
	printf("test");
	scanf("%s", fileRequest);
	fileReq(socketfd, fileRequest);
	close(socketfd);
	return 0;
   
}
