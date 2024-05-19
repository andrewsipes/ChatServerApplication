#pragma once

#include "logger.h"

class user
{
	char* username;
	char* password;

public:

	user(char* _username, char* _password);
	~user();
};

