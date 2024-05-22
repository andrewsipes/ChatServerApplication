#pragma once

#include "logger.h"
#include <string>
#include <winsock2.h>

class user
{
public:
	std::string username;
	std::string password;
	logger log;
	SOCKET socket;
	bool connected;

	user(std::string _username, std::string _password, SOCKET _socket);
	user(SOCKET _socket);
	~user();
};

