#pragma once
#include <vector>
#include <WinSock2.h>
#include <sstream>

enum commands {
	help = 0,
	reg,
	login,
};
class messageHandler
{
	char commandChar;

public:
	std::vector<char*> commandStrings;
	messageHandler();
	messageHandler(const char commandChar);
	int handleMessage(SOCKET _socket, char* _data);
	int sendMessage(SOCKET _socket, const char* data, int32_t length);
	int readMessage(SOCKET _socket, char* buffer, int32_t size);
	int tcpSend(SOCKET _socket, const char* _data, int16_t _length);
	int tcpReceive(SOCKET _socket, char& _data, int _length);
	bool compareChar(const char* _char1, const char* _char2, int length);
	int stringConvertSend(std::stringstream& ss, SOCKET _socket);
	int stringConvertSend(std::string _str, SOCKET _socket);
	const char* stringToChar(std::stringstream& ss);
	const char* stringToChar(std::string str);
	std::string charToString(char* _char);
	const char* extractUntilSpace(char* _data, int startingElement, int& lastChar);
	~messageHandler();
};

