#pragma once

#include "logger.h"
#include <string>

class user
{
public:
	std::string username;
	std::string password;

	user(std::string _username, std::string _password);
	~user();
};

