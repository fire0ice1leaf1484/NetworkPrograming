#include<stdlib.h>
#include<regex.h>
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#define BUF_SIZE 256

void commun(int);
void DieWithError(char *);
int main(int argc,char** argv){
	
	int cliSock;
	int servSock = socket(PF_INET,SOCK_STREAM,0);
	struct sockaddr_in servAddress;
	struct sockaddr_in clientAddress;
	socklen_t szClientAddr;
	servAddress.sin_family = AF_INET;
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddress.sin_port = htons(80);
	
	bind(servSock,(struct sockaddr *)&servAddress,sizeof(servAddress));
	
	listen(servSock,5);
	while(1){
		szClientAddr = sizeof(clientAddress);
		cliSock = accept(servSock,(struct sockaddr *)&clientAddress,&szClientAddr);
		commun(cliSock);
		close(cliSock);
	}
	close(servSock);
	return 0;
}

void DieWithError(char *errorMessage){
	perror(errorMessage);
	exit(1);
}
void commun(int sock){
	char res[4][BUF_SIZE]={
		"HTTP/1.1 200 OK\r\n",
		"Content-Type: text/html; charset=utf-8\r\n\r\n",
		"<!DOCTYPE html><html><head><title>ネットワークプログラミングのwebサイト",
		"</title></head><body>ネットワーク大好き</body></html>"
	};
	int i; 
	char buf[BUF_SIZE];
	char buf_old[BUF_SIZE];
	char buf2[BUF_SIZE];
	char result[100];
	char *uri;
	result[0] = '\n';
    int len_r;
	regex_t regBuf;
	const char *pattern = "GET[^\\n]+HTTP";
	regmatch_t regMatch[1];
	if(regcomp(&regBuf,pattern,REG_EXTENDED | REG_NEWLINE)!=0){
		DieWithError("regcomp failed");
	}
	buf_old[0] = '\0';
	
    while((len_r = recv(sock, buf, BUF_SIZE, 0)) > 0){
    	if(regexec(&regBuf,buf2,1,regMatch,0) != 0){
    		int startIndex = regMatch[0].rm_so;
    		int endIndex = regMatch[0].rm_so;
    		strncpy(result,buf2 + startIndex,endIndex - startIndex);
    	}
        buf[len_r] = '\0';
        sprintf(buf2,"%s%s", buf_old,buf);
        if (strstr(buf2, "\r\n\r\n")) {
            break;
        }
    	sprintf(buf_old,"%s",buf);
    }
	if (result[0] != '\0') {
		uri = strtok(uri, " ");
		uri = strtok(NULL, " ");
		printf("%s¥n", uri);
	} else {
		DieWithError("No URI");
	}
	regfree(&regBuf);
    if (len_r <= 0)DieWithError("received() failed.");
	
	printf("received HTTP Request.\n");
	for(i=0;i<4;i++){
		if(send(sock,res[i],strlen(res[i]),0)!=strlen(res[i]))DieWithError("send() sent amessage of unexpected bytes");
	}
	if(len_r<=0)DieWithError("recv()failed");
	
}
	