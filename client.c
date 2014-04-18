#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/stat.h>

void filePart(int socketfd){
	size_t ret;
	char buffer[2148];
	ret = read(socketfd, buffer, 2048);
	FILE* fp = fopen(buffer, "w");
	ret = read(socketfd, buffer, 2048);
	while(ret >0){
		fwrite(buffer, 1, ret, fp);
		ret = read(socketfd, buffer, 2048); 
	}
	fclose(fp);
	
}
int main(){
	int socketfd;
	int i;
	struct sockaddr_in dest;
	socketfd[i] = socket(AF_INET,SOCK_STREAM,0);
	bzero(&dest,sizeof(dest));
	dest.sin_port = htons(9877);
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr("127.0.0.1");
	connect(socketfd[i],(struct sockaddr*)&dest, sizeof(dest));
	mkdir("./download",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	filePart(socketfd);
	close(socketfd);
	return 0;
   
}
