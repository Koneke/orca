#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

#include "error.h"
#include "client.h"

static int alive;
static pthread_t server_thread;

void* server_main(void* arg)
{
	alive = 1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int portno = 2000;
	if (sockfd < 0) 
	{
		error("ERROR opening socket");
	}

	struct sockaddr_in serv_addr;
	memset((char *)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{
		error("ERROR on binding\n");
	}

	int newsockfd;
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	client_t* new_client = init_client();

	printf("hi\n");
	if (listen(sockfd, 5) < 0)
	{
		error("listen");
	}

	fd_set socket_fdset;

	while (alive)
	{
		FD_ZERO(&socket_fdset);
		FD_SET(sockfd, &socket_fdset);

		struct timeval timeout = { .tv_sec = 1, .tv_usec = 0 };
		int rv = select(FD_SETSIZE, &socket_fdset, NULL, NULL, &timeout);

		if (rv == -1)
		{
			error("select");
		}
		else if (rv == 0)
		{
			continue;
		}

		printf("new friend\n");

		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		if (newsockfd < 0) 
		{
			error("ERROR on accept");
		}

		new_client->socket = newsockfd;
		pthread_create(&new_client->thread, NULL, client_main, new_client);
		new_client = init_client();
	}

	kill_all_clients();
	close(sockfd);
	return NULL;
}

void server_start()
{
	pthread_create(&server_thread, NULL, server_main, NULL);
}

void server_kill()
{
	alive = 0;
	pthread_join(server_thread, NULL);
}
