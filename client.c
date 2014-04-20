#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<limits.h>
#include<unistd.h>
#include <arpa/inet.h>

char buffer[65536];
char* nameFetch(int socketfd){
	ssize_t ret;
	char *s;
	memset(buffer, 0, sizeof(buffer));
	ret = read(socketfd, buffer, 8);
	printf("read:%d\n",strtol(buffer, NULL, 10));
	ret = read(socketfd, buffer, strtol(buffer, NULL, 10));
	//printf("%s\n",buffer);
	s = malloc(ret * sizeof(char));
	strcpy(s, buffer);
	//printf("%s",buffer);
	return s;
}
void fileTrans(int socketfd, char* fileName){
	ssize_t ret;
	int fileSize;
	ret = read(socketfd, buffer, 8);
	printf("%s\n",fileName);
	FILE* fp = fopen(fileName, "wb");
	ret = read(socketfd, buffer, strtol(buffer, NULL, 10));
	fwrite(buffer, sizeof(char), ret, fp);
	fclose(fp);
}
void fileReq(int socketfd, char * fileName){
	ssize_t ret;
	char tmp[1024];
	char tmpName[1024] = "hw1TestFile/";
	memset(buffer, 0, sizeof(buffer));
	strcat(tmpName,fileName);
	printf("%s\n",tmpName);
	sprintf(tmp, "%8d", strlen(tmpName));
	printf("fileName length: %d, %d\n", strlen(tmp), strlen(tmpName));
	memset(buffer, 0, sizeof(buffer));
	write(socketfd, tmp, strlen(tmp));
	write(socketfd, tmpName, strlen(tmpName));
	printf("wait\n");
	ret = read(socketfd, buffer, 8);               // file size
	printf("test%zd\n",ret);
	printf("strtol:%d\n", strtol(buffer, NULL, 10));
	ret = recv(socketfd, buffer, strtol(buffer, NULL, 10),MSG_WAITALL);
	FILE* fp = fopen(fileName, "wb");
	fwrite(buffer, sizeof(char), ret, fp);
	fclose(fp);
	
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
	char* fileData;
	char fileRequest[10000];
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&dest,sizeof(dest));
	dest.sin_port = htons(strtol(argv[2], NULL, 10));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(argv[1]);
	connect(socketfd,(struct sockaddr*)&dest, sizeof(dest));
	mkdir("./Download",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
	
	fileData = nameFetch(socketfd);
	//printf("%s", fileName);
	fileTrans(socketfd, fileData);
	while(1){
		printf("Download file name");
		scanf("%s", fileRequest);
		fileReq(socketfd, fileRequest);
	}
	close(socketfd);
	return 0;
   
}
