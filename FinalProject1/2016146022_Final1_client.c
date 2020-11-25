#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP "220.149.128.100"
#define SERV_PORT 4260

int main(int argc, char *argv[]){
	int sockfd;
	struct sockaddr_in dest_addr;


	int rcv_byte;
	char buf[512];

	char id[20];
	char pw[20];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1){
		perror("Client-socket() error!\n");
		exit(1);
	}
	else
		printf("Client-socket() sockfd is OK...\n");

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(SERV_PORT);
	dest_addr.sin_addr.s_addr = inet_addr(SERV_IP);

	memset(&(dest_addr.sin_zero), 0, 8);

	if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1){
		perror("Client-connect() error!\n");
		exit(1);
	}
	else
		printf("Client-connect() is OK..\n\n");

	rcv_byte = recv(sockfd, buf, sizeof(buf)+1, 0);
	printf("%s\n", buf);
	
	rcv_byte = recv(sockfd, buf, sizeof(buf)+1, 0);
	printf("%s", buf);
	scanf("%s", id);
	send(sockfd, id, sizeof(id), 0);
	rcv_byte = recv(sockfd, buf, sizeof(buf)+1, 0);
	printf("%s", buf);
	scanf("%s", pw);
	send(sockfd, pw, sizeof(pw), 0);

	rcv_byte = recv(sockfd, buf, sizeof(buf)+1, 0);
	printf("%s\n", buf);

	close(sockfd);


	return 0;
}
