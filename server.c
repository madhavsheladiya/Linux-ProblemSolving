/*
	Run this code in a machine implementing code:
		gcc -o server server.c
		./server
*/

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#define NET_BUF_SIZE 80
#define PORT 8080
#define SA struct sockaddr

//clear the content of arrays
void clearBuf(char* b)
{
	int i;
	for (i = 0; i < NET_BUF_SIZE; i++)
		b[i] = '\0';
}
// it is handling grep command
void handle_client(int connfd)
{
	char pattern[NET_BUF_SIZE], fileName[NET_BUF_SIZE];
	int n;
	char command[100];
	clearBuf(pattern);
	clearBuf(fileName);
	read(connfd, fileName, sizeof(fileName));
	printf("File: %s\n", fileName);
	read(connfd, pattern, sizeof(pattern));
	printf("Pattern: %s\n", pattern);
	sprintf(command, "grep --color=always --with-filename '%s' %s", pattern,fileName);
	system(command);
	int fd[2];
	int k=pipe(fd);
	if(k == -1)
		exit(1); 
	if(fork() == 0){
		close(fd[0]);

		dup2(fd[1], 1);
		clearBuf(command);
		sprintf(command, "grep --color=always --with-filename '%s' %s", pattern,fileName);
		system(command);

	}
	else {
		char output[NET_BUF_SIZE];
		close(fd[1]);
		dup2(fd[0], 0);
		clearBuf(output);
		read(fd[0], &output, NET_BUF_SIZE);
	
		write(connfd, output, sizeof(output));
	}

}
//main function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("The creation of socket is failed\n");
		exit(0);
	}
	else
		printf("The creation of socket is successful\n");
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("The binding of socket is failed\n");
		exit(0);
	}
	else
		printf("The binding of socket is successful\n");

	if ((listen(sockfd, 5)) != 0) {
		printf("Listen of socket is failed\n");
		exit(0);
	}
	else
		printf("Listen of socket is successful\n");
	len = sizeof(cli);

	Verify:;
	printf("\nWaiting for Client's input\n");
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("Acceptance by the server failed.\n");
		exit(0);
	}
	else
		printf("server accepted the client\n");

	handle_client(connfd);
	goto Verify;

	close(sockfd);
}