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
	char ipAddr[INET_ADDRSTRLEN];
	char commandChar;
	
	clientHandler* ClientHandler;
	messageHandler* MessageHandler;

	SOCKET lSocket;	   //Listening Socket
	SOCKET comSocket;	   
	sockaddr_in cAddr; //Empty Address for clients connecting

	//for multiplexing
	fd_set masterSet, readySet;
	std::vector<SOCKET> socketList;
	
public:
	chatServer();
	int init();
	bool run();

private:

	int checkPort(uint16_t _port);
	int checkCommandChar(char _character);

};

