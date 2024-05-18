#include "messageHandler.h"
#include <iostream>
#include "errors.h"

messageHandler::messageHandler() {

}
messageHandler::messageHandler(const char commandChar){

	char* help = new char[6] {commandChar, 'h', 'e', 'l', 'p', '\0' };
	commandStrings.push_back(help);
}
void messageHandler::handleMessage(SOCKET _socket, char* _data) {

	//help
	if (compareChar(_data, commandStrings[0], strlen(commandStrings[0]))) {
		char* help = new char[7]{ 'S', 'U', 'C', 'C', 'E', 'S', 'S' };
		sendMessage(_socket, help, 255);
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

bool messageHandler::compareChar(const char* _char1, const char* _char2, int length) {
	for (int i = 0; i < length; i++) {
		if (_char1[i] != _char2[i]) {
			return false;
		}
	}
	return true;
}
messageHandler::~messageHandler() {

	for (char* _char : commandStrings) {
		delete _char;
	}
}