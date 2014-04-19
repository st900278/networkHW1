#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<sys/stat.h>
#include <arpa/inet.h>

char* nameFetch(int socketfd){
	ssize_t ret;
	char buffer[2148];
	char *s;
	ret = read(socketfd, buffer, 2148);

	s = malloc(ret * sizeof(char));
	
	
	strcpy(s, buffer);
	//printf("%s",buffer);
	return s;
}

void fileTrans(int socketfd, char* fileName){
	FILE* fp = fopen(fileName, "wb");
	ssize_t ret;
	char buffer[2148];
	char buf[] = "success";
	write(socketfd, buf, sizeof(buf));
	memset(buffer, 0, sizeof(buffer));
	while(1){
		ret = read(socketfd, buffer, 2048);
		if(ret == 0)break;
		printf("%s",buffer);
		fwrite(buffer, sizeof(char), sizeof(buffer), fp);
	}
	fclose(fp);
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
	close(socketfd);
	return 0;
   
}
