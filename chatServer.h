#pragma once

#include "clientHandler.h"
#include "messageHandler.h"
#include "logger.h"
#include <stdint.h>
#include "errors.h"

class chatServer
{
	clientHandler ClientHandler;
	logger PublicLog;
	uint16_t port;
	int capacity;

public:
	chatServer();

	int init(uint16_t port);

private:

	void Setup();
	int checkPort(uint16_t _port);
};

