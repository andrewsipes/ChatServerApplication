#pragma once
#include <vector>
#include <WinSock2.h>

class messageHandler
{
	std::vector<char*> commandStrings;

public:
	messageHandler();
	messageHandler(const char commandChar);
	void handleMessage(SOCKET _socket, char* _data);
	int sendMessage(SOCKET _socket, const char* data, int32_t length);
	int readMessage(SOCKET _socket, char* buffer, int32_t size);
	int tcpSend(SOCKET _socket, const char* _data, int16_t _length);
	int tcpReceive(SOCKET _socket, char& _data, int _length);
	bool compareChar(const char* _char1, const char* _char2, int length);
	~messageHandler();
};

