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
	printf("waiting\n");
	write(socketfd, buf, sizeof(buf));
	ret = read(socketfd, buffer, 2048);
	printf("testst");
	printf("%s",buffer);
	while(ret >0){
		fwrite(buffer, 1, sizeof(buffer), fp);
		ret = read(socketfd, buffer, 2048);
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
