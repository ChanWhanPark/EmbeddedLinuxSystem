#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP "220.149.128.103"
#define SERV_PORT 4263
#define CLIENT_IP "220.149.128.102"
#define P2P_PORT 4262
#define BACKLOG 10

int main(int argc, char *argv[]){
	int sockfd;
	struct sockaddr_in dest_addr;

	int rcv_byte = 0;
	char buf[512];
	char list[512];

	char id[20];
	char pw[20];
	
	/* Project2 파일 설정 부분 */
	FILE* filelist_pointer;
	FILE* serverlist_pointer;
	FILE* file_pointer[10];
	int filenum;
	char filename[10][20];
	char fnstr[5];
	char mode[5];
	char serverlist[100];
	char serv_filename[100];
	int count = 0;
	int line = 0;
	char fcstring[5];
	int line_count = 0;
	char buffer[100];

	/* Project3 소켓 설정 부분 */
	pid_t pid;
	int socketfd; // 파일 전송 시 사용
	char down_filename[100];
	int index = 0;
	char fbuf[100];
	char transbuf[100];
	char recvbuf[100];
	FILE* download_pointer;
	FILE* upload_pointer;
	char u_lastname[100];
	char d_lastname[100];

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

	printf("IP : %s\n", CLIENT_IP);
	printf("PORT : %d\n", P2P_PORT);

	char PORT[10];
	sprintf(PORT, "%d", P2P_PORT);

	recv(sockfd, buf, sizeof(buf)+1, 0);
	printf("%s\n", buf);
	
	recv(sockfd, buf, sizeof(buf)+1, 0);
	printf("%s", buf);
	scanf("%s", id);
	send(sockfd, id, sizeof(id), 0);
	recv(sockfd, buf, sizeof(buf)+1, 0);
	printf("%s", buf);
	scanf("%s", pw);
	send(sockfd, pw, sizeof(pw), 0);

	recv(sockfd, buf, sizeof(buf)+1, 0);
	printf("%s\n", buf);
	if (buf[7] == 's'){
		/* LAB3 자식 프로세스 생성 */
		pid = fork();
		if (pid > 0){
			int sock_fd, new_fd;
			struct sockaddr_in my_addr;
			struct sockaddr_in their_addr;
			unsigned int sin_size;

			sock_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (sock_fd == -1){
				perror("Server-socket error!");
				exit(1);
			}
			else printf("Server-socket is OK!\n");

			my_addr.sin_family = AF_INET;
			my_addr.sin_port = htons(P2P_PORT);
			my_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);

			memset(&(my_addr.sin_zero), 0, 8);

			if (bind(sock_fd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1){
				perror("server-bind() error!");
				exit(1);
			}
			else printf("server-bind() is OK!\n");

			if (listen(sock_fd, BACKLOG) == -1){
				perror("Listen() error!");
				exit(1);
			}
			else printf("Listen() is OK!\n\n");
			
			while(1){
				sin_size = sizeof(struct sockaddr_in);
				new_fd = accept(sock_fd, (struct sockaddr*)&their_addr, &sin_size);
				recv(new_fd, down_filename, 100, 0);
				printf("%s\n", down_filename);
				upload_pointer = fopen(down_filename, "a+");
				int j = 0;
				while(j<10){
					fgets(transbuf, 100, upload_pointer);
					printf("%s", transbuf);
					if (strcmp(transbuf, u_lastname) == 0) break;
					send(new_fd, transbuf, 100, 0);
					memcpy(u_lastname, transbuf, sizeof(transbuf));
					sleep(1);
					j++;
				}
				close(new_fd);
			}
			close(sock_fd);
		}

		else if (pid == 0){
			/* LAB2 클라이언트 파일리스트 생성 */
			printf("(%s, %s) 클라이언트 파일리스트 생성\n", CLIENT_IP, PORT);
			filelist_pointer = fopen("user_file.lst", "w+");
			printf("추가할 파일수 입력: ");
			scanf("%d", &filenum);
			for(int i=0;i<filenum;i++){
				printf("파일명 입력: ");
				scanf("%s", filename[i]);
				sprintf(filename[i], "%s.txt", filename[i]);
				file_pointer[i] = fopen(filename[i], "w+");
				if (file_pointer[i] == NULL) printf("파일 생성 실패\n");
				else printf("파일 %d 생성 성공\n", i+1);
				/* LAB3 파일 내용 입력 */
				if (file_pointer[i]){
					do{
						index++;
						printf("%d번째 줄 입력: ", index);
						scanf("%s", fbuf);
						if (strcmp(fbuf, ".") != 0){
							fprintf(file_pointer[i], "%s\n", fbuf);
						}
						else {
							fprintf(file_pointer[i], "%s\n", fbuf);
							index = 0;
						}
					}while(strcmp(fbuf, ".") != 0);

				}
				fprintf(filelist_pointer, "%d\t%s\n", count+1,filename[i]);
				count++;
				fclose(file_pointer[i]);
			}
			char* msg = "파일리스트 전송을 시작합니다.\n";
			send(sockfd, msg, strlen(msg)+1, 0);
			sprintf(fnstr, "%d", filenum);
			send(sockfd, fnstr, sizeof(fnstr)+1, 0);
			rcv_byte = recv(sockfd, buf, sizeof(buf)+1, 0);
			printf("%s\n", buf);
			int i = 0;
			while(i < filenum){
				if (rcv_byte != -1){ 
					send(sockfd,filename[i], sizeof(filename[i])+1,0);
					send(sockfd, CLIENT_IP, 51, 0);
					send(sockfd, PORT , 11, 0);
					rcv_byte=recv(sockfd, buf,sizeof(buf)+1,0);
					printf("%s", buf);
					i++;
				}
				rcv_byte=0;
			}
			printf("\n");
			fclose(filelist_pointer);
			/* LAB2 서버 파일리스트 수신 */
			printf("서버의 파일리스트를 수신하겠습니까?(Y/N): ");
			scanf("%s", mode);
			send(sockfd, mode, sizeof(mode)+1, 0);
			if (strcmp(mode, "Y") == 0) {
				serverlist_pointer = fopen("serverlist.lst", "w");
		 		int r = recv(sockfd, fcstring, sizeof(fcstring)+1, 0);
				int cnum = (int)*fcstring - 48;
				int i = 0;
				while (i < cnum) {
					int r = recv(sockfd, serverlist, 50, 0);
					sleep(1);
					if (strcmp(serverlist, serv_filename) == 0) break;
					fprintf(serverlist_pointer, "%s", serverlist);
					memcpy(serv_filename, serverlist, sizeof(serverlist));
					i++;
					printf("%d 줄 수신 완료\n", i);
				}
			/* 특정 정보 확인 */
				printf("확인할 줄 번호를 선택: ");
				scanf("%d", &line);
				fclose(serverlist_pointer);
				serverlist_pointer = fopen("serverlist.lst", "r");
				while(fgets(buffer, 100, serverlist_pointer) != NULL){
					line_count++;
					if (line_count == line){
						printf("NUM\tNAME\tuser\t\tIP\tPORT\n");
						printf("%s\n", buffer); break;
					}
				}
			/* IP 정보와 포트 정보 확인 */
				char *ptr;
				char d_filename[100];
				char d_ip[100];
				int d_port;
				ptr = strtok(buffer, "\t");
				ptr = strtok(NULL, "\t");
				memcpy(d_filename, ptr, sizeof(d_filename));
				ptr = strtok(NULL, "\t");
				ptr = strtok(NULL, "\t");
				memcpy(d_ip, ptr, sizeof(d_ip));
				ptr = strtok(NULL, "\n");
				d_port = atoi(ptr);
				printf("%s %s %d\n", d_filename, d_ip, d_port);

				/* 파일 전송 시도 */
				if (strcmp(d_ip, CLIENT_IP) == 0){
					printf("이미 존재하는 파일입니다.\n");
				}
				else{
					socketfd = socket(AF_INET, SOCK_STREAM, 0);
					if (sockfd == -1){
						perror("Client-socket() error!\n");
						exit(1);
					}
					else printf("Client-socket() sockfd is OK!");

					dest_addr.sin_family = AF_INET;
					dest_addr.sin_port = htons(d_port);
					dest_addr.sin_addr.s_addr = inet_addr(d_ip);
					memset(&(dest_addr.sin_zero), 0, 8);

					if (connect(socketfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1){
						perror("Client-connect() error!\n");
						exit(1);
					}
					else printf("Client-connect() is OK!\n");	
					/* 파일제목 및 내용 전송 */
					send(socketfd, d_filename, 100, 0);
					download_pointer = fopen(d_filename, "w");
					if (download_pointer){
						int j = 0;
						while(j < 10){
							recv(socketfd, recvbuf, 100, 0);
							printf("%s", recvbuf);
							fputs(recvbuf, download_pointer);
							if (strcmp(transbuf, d_lastname) == 0) break;
							memcpy(d_lastname, recvbuf, sizeof(recvbuf));
							sleep(1);
							j++;
						}
					}
					close(socketfd);
				}
			}
			else if (strcmp(mode, "N") == 0) {
				printf("서버 파일리스트 수신 없이 종료\n");
			}
			sleep(1);
		}
		else{
			perror("Fork Error!");
			exit(1);
		}
		
	}
	close(sockfd); 
	return 0;
}
