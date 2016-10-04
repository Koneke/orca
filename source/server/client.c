#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <pthread.h>

#include "error.h"
#include "client.h"

client_t* client_head;
client_t* client_tail;

void client_write(client_t* client, char* message)
{
	int n = write(client->socket, message, strlen(message));

	if (n < 0)
	{
		error("ERROR writing to socket");
	}
}

void client_disconnect(client_t* client)
{
	if (client->next)
	{
		client->next->prev = client->prev;
	}

	if (client->prev)
	{
		client->prev->next = client->next;
	}

	close(client->socket);
}

void* client_main(void* arg)
{
	client_t* client = (client_t*)arg;

	char buffer[256];

	int client_connected = 1;
	fd_set socket_fdset;

	while (client_connected)
	{
		memset(buffer, '\0', 256);
		FD_ZERO(&socket_fdset);
		FD_SET(client->socket, &socket_fdset);

		struct timeval timeout = { .tv_sec = 1, .tv_usec = 0 };
		int rv = select(FD_SETSIZE, &socket_fdset, NULL, NULL, &timeout);

		if (rv < 0)
		{
			error("client select");
		}

		if (rv == 0)
		{
			continue;
		}

		int n = read(client->socket, buffer, 255);

		if (n < 0)
		{
			error("ERROR reading from socket");
		}

		if (n == 0)
		{
			continue;
		}

		char command[5];
		memcpy(command, buffer, 4);
		command[4] = '\0';

		if (!strcmp(command, "quit"))
		{
			client_connected = 0;
			printf("client quit");
			continue;
		}

		printf("Here is the message: %s\n", buffer);
		client_write(client, "I got your message");
	}

	close(client->socket);
	return NULL;
}

void remove_client(client_t* client)
{
	if (client == client_tail)
	{
		client_tail = client->prev;
	}

	if (client == client_head)
	{
		client_head = client->next;
	}

	if (client->prev)
	{
		client->prev->next = client->next;
	}

	if (client->next)
	{
		client->next->prev = client->prev;
	}
}

void kill_all_clients()
{
	printf("killing clients");

	while (client_tail)
	{
		if (client_tail->connected)
		{
			client_write(client_tail, "disconnect");
			close(client_tail->socket);
		}

		remove_client(client_tail);
	}
}

client_t* init_client()
{
	client_t* new_client = malloc(sizeof(client_t));

	new_client->connected = 0;
	new_client->next = NULL;
	new_client->prev = client_head;
	client_tail = new_client;

	if (new_client->prev)
	{
		new_client->prev->next = new_client;
	}

	return new_client;
}
