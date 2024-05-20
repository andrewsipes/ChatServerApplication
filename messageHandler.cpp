#include "messageHandler.h"
#include <iostream>
#include <string.h>
#include "errors.h"


messageHandler::messageHandler() {
	commandChar = '~';
}
messageHandler::messageHandler(const char _commandChar){

	commandChar = _commandChar;
	//original way
	//char* help = new char[6] {commandChar, 'h', 'e', 'l', 'p', '\0' };
	//commandStrings.push_back(help);

	std::stringstream ssHelp; ssHelp << commandChar << "help";
	const char* help = stringToChar(ssHelp);
	commandStrings.push_back((char*)help);

	std::stringstream ssReg; ssReg << commandChar << "register";
	const char* reg = stringToChar(ssReg);
	commandStrings.push_back((char*)reg);

}
int messageHandler::handleMessage(SOCKET _socket, char* _data) {

	//help
	if (compareChar(_data, commandStrings[help], strlen(commandStrings[help]))) {
		helpScreen(_socket);
		return HELP_SCREEN;
	}

	//register
	else if (compareChar(_data, commandStrings[reg], strlen(commandStrings[reg]))) {
		return REGISTER;		
	}
}
int messageHandler::tcpSend(SOCKET _socket, const char* _data, int16_t _length) {

	int result;
	int sent = 0;

	while (sent < _length) {
		result = send(_socket, (const char*)_data + sent, _length - sent, 0);

		if (result <= 0) {
			return result;
		}

		sent += result;
	}

	return sent;
}
int messageHandler::tcpReceive(SOCKET _socket, char& _data, int _length) {

	int received = 0;

	do {

		int retrieved = recv(_socket, &_data + received, _length - received, 0);

		if (retrieved < 1)
			return retrieved;

		else
			received += retrieved;

	} while (received < _length);

	return received;
}
int messageHandler::sendMessage(SOCKET _socket, const char* data, int32_t length)
{
	if (length < 0 || length > 255)
		return PARAMETER_ERROR;

	uint8_t message = length;

	if (tcpSend(_socket, (char*)&message, 1) == SOCKET_ERROR) {

		int lastError = WSAGetLastError();

		if (lastError == WSAESHUTDOWN)
			return SHUTDOWN;

		else
			return DISCONNECT;
	}

	int sent = tcpSend(_socket, data, message);

	if (sent == SOCKET_ERROR) {

		int lastError = WSAGetLastError();

		if (lastError == WSAESHUTDOWN)
			return SHUTDOWN;

		else
			return DISCONNECT;
	}

	return sent;
}
int messageHandler::readMessage(SOCKET _socket, char* buffer, int32_t size)
{
	int message = 0;

	int received = recv(_socket, (char*)&message, sizeof(char), 0);

	if (received == SOCKET_ERROR || received == 0) {

		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;

		else
			return DISCONNECT;
	}

	if (message > size) {
		return PARAMETER_ERROR;
	}

	received = tcpReceive(_socket, *buffer, message);

	if (received == SOCKET_ERROR || received == 0) {

		if (WSAGetLastError() == WSAESHUTDOWN)
			return SHUTDOWN;

		else
			return DISCONNECT;
	}

	return received;
}
void messageHandler::helpScreen(SOCKET _socket) {
	std::stringstream ss1;
	ss1 << "\n\n" << commandChar << "help\tProvides list of commands available\n" <<
		"\n" << commandChar << "register\t<username> <password>\nRegisters a user to the server\n" <<
		"\n" << commandChar << "login\t<username> <password>\nlogs a user into the chat server\n" <<
		"\n" << commandChar << "logout\tlogs a user out of the chat server\n\0";

	std::stringstream ss2;

	ss2 << "\n" << commandChar << "getlist\t provides list of active clients\n" <<
		"\n" << commandChar << "send\t<username> <message>\nsends a message to client (255 char limit)\n" <<
		"\n" << commandChar << "send\t<message>\nsends a message all connnected clients\n\0";

	std::stringstream ss3;

	ss3 << "\n" << commandChar << "getlog\t<username>\nretrieves logs for a specific user\n" <<
		"\n" << commandChar << "getlog\tpublic\nretrieves public logs\n\0";

	stringConvertSend(ss1, _socket);
	stringConvertSend(ss2, _socket);
	stringConvertSend(ss3, _socket);
}
//void messageHandler::registerUser(SOCKET _socket, char* _data) {
//
//	int* last = new int;
//
//	//disregard the command so we can get the username then the password
//	char* user = (char*)extractUntilSpace(_data, strlen(commandStrings[1])+1, *last);
//	char* pass = (char*)extractUntilSpace(_data, *last+1, *last);
//
//}
const char* messageHandler::extractUntilSpace(char* _data, int startingElement, int& lastChar) {
	std::stringstream ss;

	for (int i = startingElement; i < strlen(_data); i++) {

		if (_data[i] == ' '){
			lastChar = i;
			break;
		}

		ss << _data[i];
	
	}

	const char* returnChar = stringToChar(ss);
	return returnChar;
}
bool messageHandler::compareChar(const char* _char1, const char* _char2, int length) {
	for (int i = 0; i < length; i++) {
		if (_char1[i] != _char2[i]) {
			return false;
		}
	}
	return true;
}
int messageHandler::stringConvertSend(std::stringstream& ss, SOCKET _socket) {

	std::string convertToString = ss.str();
	const char* message = convertToString.c_str();
	int result = sendMessage(_socket, message, 255);
	return result;
}
const char* messageHandler::stringToChar(std::stringstream& ss) {

	std::string convertToString = ss.str();
	char* message = new char[convertToString.size() + 1];
	std::memcpy(message, convertToString.c_str(), convertToString.size());
	message[convertToString.size()] = '\0';
	return message;
}

const char* messageHandler::stringToChar(std::string str) {

	std::string convertToString = str;
	char* message = new char[convertToString.size() + 1];
	std::memcpy(message, convertToString.c_str(), convertToString.size());
	message[convertToString.size()] = '\0';
	return message;
}
std::string messageHandler::charToString(char* _char) {
	std::stringstream ss;

	for (int i = 0; i < strlen(_char); i++) {
		ss << _char[i];
	}

	return ss.str();
}
messageHandler::~messageHandler() {

	for (char* _char : commandStrings) {
		delete _char;
	}
}
