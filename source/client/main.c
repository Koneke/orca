#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h> 
#include <pthread.h>

int sockfd;
int connected;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void* receive(void* arg)
{
	char buffer[256];

	fd_set socket_fdset;

	while (connected)
	{
		FD_ZERO(&socket_fdset);
		FD_SET(sockfd, &socket_fdset);
		memset(buffer, '\0', 256);

		struct timeval timeout = { .tv_sec = 1, .tv_usec = 0 };
		int rv = select(FD_SETSIZE, &socket_fdset, NULL, NULL, &timeout);

		if (rv == 0)
		{
			continue;
		}

		int n = read(sockfd, buffer, 255);
		if (n < 0) 
		{
			error("ERROR reading from socket");
		}

		if (!strcmp(buffer, "disconnect"))
		{
			connected = 0;
			printf("server disconnected.");
			break;
		}

		printf("%s\n", buffer);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);

	int portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	connected = 1;

	pthread_t receiver_thread;
	pthread_create(&receiver_thread, NULL, receive, NULL);

	char buffer[256];

	if (argc < 3)
	{
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
	{
		error("ERROR opening socket");
	}

	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	memset((char *)&serv_addr, '\0', sizeof(serv_addr));
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		error("ERROR connecting");
	}

	while (connected)
	{
		printf("Please enter the message: ");

		memset(buffer, '\0', 256);
		scanf("%[^\n]%*c", buffer);

		char command[5];
		memcpy(command, buffer, 4);
		command[4] = '\0';

		if (connected)
		{
			n = write(sockfd, buffer, strlen(buffer));
			if (n < 0) 
			{
				error("ERROR writing to socket");
			}

			//fputs(command, stdout);
			printf("\n");

			if (!strcmp(command, "quit"))
			{
				connected = 0;
				printf("quitting\n");
				continue;
			}
		}
	}

	pthread_join(receiver_thread, NULL);
	close(sockfd);
	return 0;
}
