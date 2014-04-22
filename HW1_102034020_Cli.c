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
#include<time.h>

char buffer[20000000];
void nameFetch(int socketfd, char* downloadListName){
	ssize_t ret;
	char *s;
	char tmp[1024];
	sprintf(tmp, "%8zd", strlen(downloadListName));
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
	printf("Download list is downloaded\n");
}
char* fileReq(int socketfd, char * fileName, clock_t t1 ){
	ssize_t ret;
	char tmp[1024];
	char* logText;
	memset(buffer, 0, sizeof(buffer));
	memset(tmp, 0 , sizeof(tmp));
	sprintf(tmp, "%8zd", strlen(fileName));
	write(socketfd, tmp , strlen(tmp));
	write(socketfd, fileName, strlen(fileName));
	printf("Start download: %s\n", fileName);
	ret = read(socketfd, buffer, 8);
	if(buffer[7] == '0'){
		logText = (char*)malloc(67*sizeof(char));
		sprintf(logText,"|%15s |      failed |              - |              - |\n",fileName);
		printf("Download failed\n");
		return logText;
	}
	ret = read(socketfd, buffer, 8);
	int length = strtol(buffer, NULL, 10);
	memset(buffer, 0, sizeof(buffer));
	char tmppp[1024] = "Download/";
	strcat(tmppp, fileName);
	FILE* fp = fopen(tmppp, "wb");
	int tmpSize = length;
	printf("Size : %d\n", tmpSize);
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
	printf("Download success.\n");
	clock_t t2 = clock();
	logText = (char*)malloc(67*sizeof(char));
	sprintf(logText,"|%15s |%12s |%15lf |%15d |\n",fileName ,"success", (t2-t1)/(double)(CLOCKS_PER_SEC), length);
	fclose(fp);
	return logText;
}
int main(int argc, char* argv[]){
	if(argc<4){
		printf("Format wrong.\n");
		return 0;
	}
	int socketfd;
	int i;
	struct sockaddr_in dest;
	FILE* flog;
	char* fileData;
	char fileRequest[10000];
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&dest,sizeof(dest));
	dest.sin_port = htons(strtol(argv[2], NULL, 10));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(argv[1]);
	connect(socketfd,(struct sockaddr*)&dest, sizeof(dest));
	mkdir("./Download",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	flog = fopen("log.txt", "w");
	
	
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
	fputs("|           file |      status |     time(secs) |    size(bytes) |\n",flog);
	fputs("|================|=============|================|================|\n", flog);
	while(1){
		clock_t ti;
		char sig[32];
		memset(fileRequest, 0, sizeof(fileRequest));
		clock_t t1 = clock();
		if(fgets(fileRequest, 1024, fp)==NULL){
			sprintf(sig,"%8d",0);
			write(socketfd, sig, 8);
			break;
		}
		else if(strlen(fileRequest)<=2){
			sprintf(sig,"%8d",2);
			write(socketfd, sig, 8);
			continue;
		}
		sprintf(sig,"%8d",1);
		write(socketfd,sig, 8);
		int a = strlen(fileRequest);
		//printf("length: %d\n",a);
		fileRequest[a-1] = '\0';
		fileRequest[a-2] = '\0';
		char *t;
		t = fileReq(socketfd, fileRequest, t1);
		fputs(t,flog);
		fputs("|----------------|-------------|----------------|----------------|\n", flog);
	}
	//printf("Download successfully\n");
	fclose(fp);
	close(socketfd);
	return 0;
   
}
