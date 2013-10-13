#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT_MAX_VALUE (2<<15)-1
#define DEFAULT_PORT 1323

#define BUFFER_LENGTH 4*1024

struct WorkRequest {
	int id;
	std::string command;
};

struct WorkResult {
	int id;
	int result;
	int time;
	std::string output;
};


std::string receiveString(int s, int &success);
bool sendString(int s, std::string value);

WorkRequest *receiveWorkRequest(int s, int &success);
bool sendWorkRequest(int s, WorkRequest request);

WorkResult *receiveWorkResult(int s, int &success);
bool sendWorkResult(int s, WorkResult result);

extern char buffer[BUFFER_LENGTH];
