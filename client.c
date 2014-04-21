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

char buffer[20000000];
void nameFetch(int socketfd, char* downloadListName){
	ssize_t ret;
	char *s;
	char tmp[1024];
	sprintf(tmp, "%8d", strlen(downloadListName));
	memset(buffer, 0, sizeof(buffer));
	write(socketfd, tmp, strlen(tmp));
	write(socketfd, downloadListName, strlen(downloadListName));
}
void fileTrans(int socketfd, char* fileName){
	ssize_t ret;
	int fileSize;
	ret = read(socketfd, buffer, 8);
	char completeName[1024] = "Download/";
	strcat(completeName,fileName);
	FILE* fp = fopen(completeName, "wb");
	ret = read(socketfd, buffer, strtol(buffer, NULL, 10));
	fwrite(buffer, sizeof(char), ret, fp);
	fclose(fp);
	printf("DownloadList download success\n");
}
void fileReq(int socketfd, char * fileName){
	ssize_t ret;
	char tmp[1024];
	memset(buffer, 0, sizeof(buffer));
	memset(tmp, 0 , sizeof(tmp));
	sprintf(tmp, "%8d", strlen(fileName));
	write(socketfd, tmp , strlen(tmp));
	write(socketfd, fileName, strlen(fileName));
	printf("Start download: %s\n", fileName);
	ret = read(socketfd, buffer, 8);
	if(buffer[7] == '0')return;
	ret = read(socketfd, buffer, 8);
	int length = strtol(buffer, NULL, 10);
	memset(buffer, 0, sizeof(buffer));
	char tmppp[1024] = "Download/";
	strcat(tmppp, fileName);
	FILE* fp = fopen(tmppp, "wb");
	int tmpSize = length;
	while(1){
		memset(buffer, 0, sizeof(buffer));
		if(tmpSize>2048){
			ret = read(socketfd, buffer, 2048);
			fwrite(buffer, sizeof(char), ret, fp);
			tmpSize -=2048;
			fflush(fp);
		}
		else{
			ret = read(socketfd, buffer, tmpSize);
			fwrite(buffer, sizeof(char), ret, fp);
			break;
		}
	}
	printf("send success\n");
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
	char* fileData;
	char fileRequest[10000];
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&dest,sizeof(dest));
	dest.sin_port = htons(strtol(argv[2], NULL, 10));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(argv[1]);
	connect(socketfd,(struct sockaddr*)&dest, sizeof(dest));
	mkdir("./Download",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
	
	nameFetch(socketfd, argv[3]);
	fileTrans(socketfd, argv[3]);
	char tmp2[1024];
	ssize_t n;
	strcpy(tmp2, "Download/");
	strcat(tmp2, argv[3]);
	FILE* fp = fopen(tmp2,"r");
	if(fp == NULL){
		printf("download list crashed\n");
		return 0;
	}
	while(1){
		char sig[32];
		memset(fileRequest, 0, sizeof(fileRequest));
		if(fgets(fileRequest, 1024, fp)==NULL){
			sprintf(sig,"%8d",0);
			write(socketfd, sig, 8);
			break;
		}
		sprintf(sig,"%8d",1);
		write(socketfd,sig, 8);
		int a = strlen(fileRequest);
		if(a<=1)continue;
		fileRequest[a-1] = '\0';
		fileRequest[a-2] = '\0';
		fileReq(socketfd, fileRequest);
	}
	close(socketfd);
	return 0;
   
}
