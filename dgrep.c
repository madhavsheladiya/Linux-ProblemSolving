/*
	For compiling and running this code: 
		

		gcc -o client dgrep.c
		./client "pattern" "file1" "file2"

		For remote connection, change IP address on 123 line
*/


#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>

#include<errno.h>

#include<fcntl.h>


#include<netinet/in_systm.h>
#include<netinet/ip_icmp.h>
#include<netinet/udp.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<arpa/nameser.h>
#include<resolv.h>
#include<net/if.h>
#include<strings.h>
#include<unistd.h>

#define NET_BUF_SIZE 80
#define IP_PROTOCOL 0
#define PORT 8080
#define SA struct sockaddr

#ifdef WIN32
#pragma warning (disable: #num of the warning)
#endif


//clear the content of arrays
void clearBuf(char* b)
{
	int i;
	for (i = 0; i < NET_BUF_SIZE; i++)
		b[i] = '\0';
}


// this function mainly handling the grep command of client and displaying result of server
void child(int sockfd, char *argv[])
{
	char buf[NET_BUF_SIZE];// created buffer
	int n;
	char command[100];//buffer
	clearBuf(buf);// calling function
	write(sockfd, argv[3], sizeof(buf)); (1,madhav, 5)
	write(sockfd, argv[1], sizeof(buf));
	clearBuf(buf);
	read(sockfd, buf, sizeof(buf));
	
	int fd[2];
	int k=pipe(fd);// craeting pipe , fd[0] for using read and fd[1] for using writing
	if(k == -1)//pipe does not created
		exit(1); 
	if(fork() == 0)
	{
		close(fd[0]);
		clearBuf(command);
		sprintf(command, "grep --color=always --with-filename '%s' %s", argv[1],argv[2]);// implementing the grep command on files 
		//																					and storing the results into the command
		system(command);//system() is used to invoke an operating system command from a C/C++ program
	}
	else 
	{
		char op[NET_BUF_SIZE];
		close(fd[1]);//write close
		clearBuf(op);
		read(fd[0], &op, NET_BUF_SIZE);//reading
        //printf("\n");
        for(int i=0 ; i<sizeof(buf) ; i++) 
        {
            if(buf[i] == '\n')
            {
                break;
            }
            else
            {
                printf("%c", buf[i]);
            }
        }
        printf("\n");
		
	}
	
}

int canCreateFile(char* path)// checking that file is present or  not
{
	FILE* file = fopen(path, "r");
  	if(file)
    {
  		fclose(file);
      	return 1;
    }
  	return 0;
}
//main function
int main(int argc, char *argv[])
{
	int sockfd, connfd;
	int a,b;
	struct sockaddr_in servaddr, cli;
	
	if(argc==4) 
	{
		a = canCreateFile(argv[2]);
		b = canCreateFile(argv[3]);
		if( a==0 || b==0)
		{
			printf("\n One of the files is missing.\n");
			exit(0);
		}
	}
	else if(argc!=4)
	{
		printf("\n Error: program needs 4 arguments.\n");
		exit(0);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, IP_PROTOCOL);
	if (sockfd == -1) 
	{
		printf("\nCreation of socket is failed.\n");
		exit(0);
	}
	else
	{
		printf("\nCreation of socket is successful.\n");
	}

	bzero(&servaddr, sizeof(servaddr));//The bzero() function erases the data in the n bytes of the memory
	// starting at the location pointed to by s, by writing zeros (bytes containing '\0') to that area.

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("137.207.82.53");    //137.207.82.53   127.0.0.1
	servaddr.sin_port = htons(PORT);


	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
	{
		printf("\nThe connection to the server failed.\n");
		exit(0);
	}
	else
	{
		printf("\n The connection to the server successful.\n\n");
	}

	child(sockfd, argv);

	close(sockfd);
}