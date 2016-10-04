#include <stdio.h>
#include "server.h"

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);

	server_start();
	getchar();
	server_kill();

	return 0; 
}
