#pragma once

#define MAX_CAPACITY 10

#include <WinSock2.h>
#include "messageHandler.h"
#include "user.h"

class clientHandler
{
	messageHandler message;

public:

	std::vector<user> clients;
	std::vector<SOCKET> clientSockets;
	int checkCapacity(int _clients);
	clientHandler(messageHandler& _messageHandler);
	void handleClients(SOCKET& lSocket);
};

