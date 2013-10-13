#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
	if (argc > 2) {
		printf("Usage: &s [port]\n", argv[0]);
		return 1;
	}
	
	int port;
	if (argc == 2) {
		port = atoi(argv[1]);
		if (port < 1 || port > PORT_MAX_VALUE) {
			printf("Invalid port number\n");
			return 1;
		}
	}
	else
		port = DEFAULT_PORT;

	int s = socket(AF_INET, SOCK_STREAM, 0);
	
	
}
