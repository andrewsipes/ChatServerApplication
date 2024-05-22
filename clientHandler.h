#pragma once

#define MAX_CAPACITY 4

#include <WinSock2.h>
#include "messageHandler.h"
#include "user.h"

class clientHandler
{
	messageHandler mh;
	

public:

	std::vector<user> clients;

	int checkCapacity(int _clients);
	int registerUser(char& username, char& password, SOCKET _socket);
	clientHandler(messageHandler& _messageHandler);
};

