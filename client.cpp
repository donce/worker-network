#include "common.h"
#include <queue>
#include <time.h>

const char *DEFAULT_HOST = "127.0.0.1";


int s;

std::queue<WorkRequest> requests;

bool serverDataReady() {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(s, &set);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int result = select(s+1, &set, NULL, NULL, &timeout);
	return result >= 1 && FD_ISSET(s, &set);
}

long getTime() {
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
		return 0;
	return ts.tv_sec;
}

void completeWork(WorkRequest work) {
	printf("Working on task #%d: %s\n", work.id, work.command.c_str());
	WorkResult result;
	result.id = work.id;
	int time = getTime();
	result.result = system(work.command.c_str());
	result.time = getTime() - time;
	sendWorkResult(s, result);
}

int main(int argc, char *argv[]) {
	system("pwd");
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

	s = socket(AF_INET, SOCK_STREAM, 0);
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
	while (true) {
		while (serverDataReady()) {
			int success;
			WorkRequest *request = receiveWorkRequest(s, success);
			//if (success == 0) {
			//}
			if (success > 0) {
				printf("Received task: %s.\n", request->command.c_str());
				requests.push(*request);
			}
			else {
				printf("Failed receiving task!\n");
			}
		}
		if (requests.size()) {
			WorkRequest work = requests.front();
			requests.pop();
			completeWork(work);
		}
	}
}
