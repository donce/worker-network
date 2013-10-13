all: client server

client: client.cpp common.o
	g++ -o client client.cpp common.o
server: server.cpp common.o
	g++ -o server server.cpp common.o
common.o: common.cpp common.h
	g++ -c common.cpp

clean:
	rm -rf server client common.o
