#include "common.h"

char buffer[BUFFER_LENGTH];

int receiveMessage(int s, const void *buffer, int length) {
	int pos = 0;
	while (length) {
		int received = recv(s, (char*)buffer+pos, length, 0);
		if (received <= 0)
			return received;
		length -= received;
		pos += received;
	}
	return true;
}

int receiveBuffer(int s, int length) {
	return receiveMessage(s, buffer, length);
}

int receiveInt(int s, int &success) {
	int value = 0;
	success = receiveMessage(s, &value, sizeof(value));
	return value;
}

std::string receiveString(int s, int &success) {
	int length = receiveInt(s, success);
	if (success <= 0)
		return NULL;
	receiveBuffer(s, length);
	buffer[length] = 0;
	std::string value = std::string(buffer);
	return value;
}

WorkRequest *receiveWorkRequest(int s, int &success) {
	WorkRequest *request = new WorkRequest();
	request->id = receiveInt(s, success);
	if (success <= 0)
		return NULL;
	request->command = receiveString(s, success);
	if (success <= 0)
		return NULL;
	return request;
}

WorkResult *receiveWorkResult(int s, int &success) {
	WorkResult *result = new WorkResult();
	result->id = receiveInt(s, success);
	if (success <= 0)
		return NULL;
	result->result = receiveInt(s, success);
	if (success <= 0)
		return NULL;
	result->time = receiveInt(s, success);
	if (success <= 0)
		return NULL;
	result->output = receiveString(s, success);
	if (success <= 0)
		return NULL;
	return result;
}

bool sendMessage(int s, const void *buffer, int length) {
	int pos = 0;
	while (length) {
		int sent = send(s, (char*)buffer+pos, length, 0);
		if (sent == -1) {
			printf("Failed receiving\n");
			return false;
		}
		length -= sent;
		pos += sent;
	}
	return true;
}

bool sendBuffer(int s, int length) {
	return sendMessage(s, buffer, length);
}

bool sendInt(int s, int value) {
	return sendMessage(s, &value, sizeof(value));
}

bool sendString(int s, std::string value) {
	if (!sendInt(s, value.length()))
		return false;
	return sendMessage(s, value.c_str(), value.length());
}

bool sendWorkRequest(int s, WorkRequest request) {
	sendInt(s, request.id);
	sendString(s, request.command);
}

bool sendWorkResult(int s, WorkResult result) {
	sendInt(s, result.id);
	sendInt(s, result.result);
	sendInt(s, result.time);
	sendString(s, result.output);
}
