typedef struct client_s client_t;
struct client_s {
	int connected;
	client_t* next;
	client_t* prev;
	int socket;
	pthread_t thread;
};

void client_write(client_t* client, char* message);
void client_disconnect(client_t* client);
void* client_main(void* arg);
void kill_all_clients();
void remove_client(client_t* client);
client_t* init_client();
