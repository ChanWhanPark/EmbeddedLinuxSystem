#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP = "220.149.128.100"
#define SERV_PORT 4260 
#define BACKLOG 10
#define INIT_MSG	"================\nHello! I'm P2P File Sharing Server..\nPlease, LOG-IN!\n=============\n"
#define USER1_ID "user1"
#define USER1_PW "passwd1"
#define USER2_ID "user2"
#define USER2_PW "passwd2"

#define ID_MESSAGE "ID: "
#define PW_MESSAGE "PW: "
#define SUCCESS "Log-in success!!"
#define FAIL "Log-in Fail.."

int main(){
	int sockfd, new_fd;

	struct sockaddr_in my_addr;

	struct sockaddr_in their_addr;
	unsigned int sin_size;

	int rcv_byte, id_byte, pw_byte;
	char buf[512];

	char id[20];
	char pw[20];
	int val = 1;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		perror("Server-socket() error!\n");
		exit(1);
	}
	else
		printf("Server-socket() sockfd is OK...\n");

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(SERV_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	memset(&(my_addr.sin_zero), 0, 8);

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val)) < 0){
		perror("setsockopt\n");
		close(sockfd);
		return -1;
	}

	if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1){
		perror("Server-bind() error!\n");
		exit(1);
	}
	else
		printf("Server-bind() is OK...\n");

	if (listen(sockfd, BACKLOG) == -1){
		perror("listen() error!\n");
		exit(1);
	}
	else
		printf("listen() is OK...\n");
	while(1){
		sin_size = sizeof(struct sockaddr_in);
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd > 0) printf("accept() is OK\n");
		send(new_fd, INIT_MSG, strlen(INIT_MSG)+1, 0);

		pid_t pid;
		pid = fork();

		if (pid == 0){
			send(new_fd, ID_MESSAGE, strlen(ID_MESSAGE)+1, 0);
			id_byte = recv(new_fd, id, sizeof(id)+1, 0);	
			send(new_fd, PW_MESSAGE, strlen(PW_MESSAGE)+1, 0);
			pw_byte = recv(new_fd, pw, sizeof(pw)+1, 0);
			printf("===========================\n");
			printf("User Information\n");
			printf("ID: %s, PW: %s\n", id, pw);
			printf("===========================\n");

			if ((strcmp(id, USER1_ID)==0) && (strcmp(pw, USER1_PW) == 0)){
				char *msg = "Log-in success!! [user1] *^^*";
				send(new_fd, msg, strlen(msg)+1, 0);
				printf("%s\n", msg);
			}
			else if ((strcmp(id, USER2_ID) == 0) && (strcmp(pw, USER2_PW) == 0)){
				char *msg = "Log-in success!! [user2] *^^*";
				send(new_fd, msg, strlen(msg)+1, 0);
				printf("%s\n", msg);
			}
			else {
				char *msg = "Log-in fail: Incorrect password..";
				send(new_fd, msg, strlen(msg)+1, 0);
				printf("%s\n", msg);
			}
			sleep(1);
		}
		else if (pid > 0){
			sleep(1);
		}
		else{
			printf("Fork error..\n");
			exit(1);
		}
		
	}
	close(new_fd);
	close(sockfd);

	return 0;
}

