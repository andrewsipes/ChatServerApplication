#pragma once
#include <vector>
#include <WinSock2.h>

class messageHandler
{
	std::vector<char*> commandStrings;

public:
	messageHandler();	
	int tcpSend(SOCKET _socket, const char* _data, int16_t _length);
	int tcpReceive(SOCKET _socket, char& _data, int _length);

};

