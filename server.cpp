#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_WAITING_CONNECTIONS 3
#define MAX_CLIENTS 30


int main(int argc, char *argv[]) {
	if (argc > 2) {
		printf("Usage: &s [port]\n", argv[0]);
		return 1;
	}
	
	int port;
	if (argc == 2) {
		port = atoi(argv[1]);
		if (port < 1 || port > PORT_MAX_VALUE) {
			printf("Invalid port number!\n");
			return 1;
		}
	}
	else
		port = DEFAULT_PORT;

	int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == -1) {
		printf("Failed creating listening socket!\n");
		return 1;
	}

	int on = 1;
	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
		printf("Error setting address reusability!\n");
		return 1;
	}
	
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if (bind(listen_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("Binding listening socket failed!\n");
		return 1;
	}

	if (listen(listen_socket, MAX_WAITING_CONNECTIONS) == -1) {
		printf("Starting to listen socket failed!\n");
		return 1;
	}

	int client_sockets[MAX_CLIENTS];
	for (int i = 0; i < MAX_CLIENTS; ++i)
		client_sockets[i] = -1;

	while (true) {
		fd_set set;
		FD_ZERO(&set);
		FD_SET(listen_socket, &set);
		int maxfd = listen_socket;
		for (int i = 0; i < MAX_CLIENTS; ++i)
			if (client_sockets[i] != -1) {
				FD_SET(client_sockets[i], &set);
				if (client_sockets[i] > maxfd)
					maxfd = client_sockets[i];
			}
		
		int ready = select(maxfd+1, &set, NULL, NULL, NULL);
		if (ready == -1) {
			printf("select() failed\n");
			return 1;
		}
		if (!ready)
			continue;
		if (FD_ISSET(listen_socket, &set)) {
			printf("new connection\n");
			int pos = -1;
			for (int i = 0; i < MAX_CLIENTS; ++i)
				if (client_sockets[i] == -1) {
					pos = i;
					break;
				}
			if (pos == -1)
				printf("Warning, connection refused because server is full.\n");
			else {
				struct sockaddr_in client_addr;
				unsigned int client_length = sizeof(client_addr);
				memset(&client_addr, 0, client_length);
				client_sockets[pos] = accept(listen_socket, (struct sockaddr*)&client_addr,
											 &client_length);
				printf("Worker connected at %s\n", inet_ntoa(client_addr.sin_addr));
			}
		}
		for (int i = 0; i < MAX_CLIENTS; ++i)
			if (client_sockets[i] != -1 && FD_ISSET(client_sockets[i], &set)) {
				printf("receiving from %d client\n", i);
			}
	}

}
