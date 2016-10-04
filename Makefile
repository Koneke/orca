CC = gcc
CFLAGS = -Wall -lpthread

INCLUDES = include

CLIENT_CFILES = $(foreach dir,source/client,$(wildcard $(dir)/*.c))
SERVER_CFILES = $(foreach dir,source/server,$(wildcard $(dir)/*.c))

all: server client

server:
	$(CC) -Iinclude/server -Iinclude/common -std=c99 -o ./server.exe $(SERVER_CFILES) $(CFLAGS)

client:
	$(CC) -Iinclude/client -Iinclude/common -std=c99 -o ./client.exe $(CLIENT_CFILES) $(CFLAGS)

clean:
	rm server.exe
	rm client.exe
