#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

const char *DEFAULT_HOST = "127.0.0.1";

int main(int argc, char *argv[]) {
	if (argc > 3) {
		printf("Usage: %s [ip=%s] [port=%d]\n", argv[0], DEFAULT_HOST, DEFAULT_PORT);
		return 1;
	}
	
	int port;
	if (argc == 3) {
		port = atoi(argv[2]);
		if (port < 1 || port > PORT_MAX_VALUE) {
			printf("Invalid port number!\n");
			return 1;
		}
	}
	else
		port = DEFAULT_PORT;

	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		printf("Failed creating socket!\n");
		return 1;
	}
	
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	
	const char *ip = argc > 1 ? argv[1] : DEFAULT_HOST;
	if (!inet_aton(ip, &serv_addr.sin_addr)) {
		printf("Failed converting ip!\n");
		return 1;
	}

	if (connect(s, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("Error connecting!\n");
		return 1;
	}
	printf("Connected.\n");
	while (true);
}
