#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

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

	int run = 1;

	while (run)
	{
		printf("Please enter the message: ");

		memset(buffer, '\0', 256);
		//fgets(buffer, 255, stdin);
		scanf("%[^\n]%*c", buffer);

		char command[5];
		memcpy(command, buffer, 4);
		command[4] = '\0';

		n = write(sockfd, buffer, strlen(buffer));
		if (n < 0) 
		{
			error("ERROR writing to socket");
		}

		fputs(command, stdout);
		printf("\n");

		if (!strcmp(command, "quit"))
		{
			run = 0;
			printf("quitting\n");
			continue;
		}

		memset(buffer, '\0', 256);

		n = read(sockfd, buffer, 255);
		if (n < 0) 
		{
			error("ERROR reading from socket");
		}

		printf("%s\n", buffer);
	}

	close(sockfd);
	return 0;
}
