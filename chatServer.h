#pragma once

#define DEFAULT_COMMAND_CHAR 126

#include "clientHandler.h"
#include "messageHandler.h"
#include "logger.h"
#include <stdint.h>
#include "errors.h"

class chatServer
{
	clientHandler ClientHandler;
	uint16_t port;
	int capacity;
	char commandChar;
	const char validChars[5] = { '~', '@', '#', '$'};
	char* hostname;

public:
	chatServer();
	int init(uint16_t port);

private:

	void Setup();
	int checkPort(uint16_t _port);
	int checkCommandChar(char _character);
	~chatServer()
};

