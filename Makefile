all: client server

client: client.cpp common.h
	g++ -o client client.cpp
server: server.cpp common.h
	g++ -o server server.cpp
