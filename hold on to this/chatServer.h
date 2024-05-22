#pragma once

#define DEFAULT_COMMAND_CHAR 126

#include "clientHandler.h"
#include "messageHandler.h"
#include "logger.h"
#include <stdint.h>
#include "errors.h"
#include <winsock2.h>
#include <ws2tcpip.h>

class chatServer
{
	const char validChars[5] = { '~', '@', '#', '$'};
	uint16_t port;
	int capacity;
	char hostname[1024];
	char ipv4Addr[INET_ADDRSTRLEN];
	char ipv6Addr[INET6_ADDRSTRLEN];
	char commandChar;
	
	clientHandler* ClientHandler;
	messageHandler* MessageHandler;
   
	SOCKET lSocket;
	sockaddr_in cAddr; //Empty Address for clients connecting

	//for multiplexing
	fd_set masterSet, readySet;
	std::vector<SOCKET> socketList;
	
public:
	chatServer();
	~chatServer();
	int init();
	bool run();
	void errorVerbose(int error);

private:

	int checkPort(uint16_t _port);
	int checkCommandChar(char _character);
	int readMessage(SOCKET _socket, char* buffer, int32_t size);
	int sendMessage(SOCKET _socket, const char* data, int32_t length);
	int tcpReceive(SOCKET _socket, char& _data, int _length);
	int tcpSend(SOCKET _socket, const char* _data, int16_t _length);
	void registerUser(SOCKET _socket, char* _buffer);
};
