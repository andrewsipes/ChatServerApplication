#include "messageHandler.h"
#include <iostream>
#include <string.h>
#include "errors.h"


messageHandler::messageHandler() {
	commandChar = '~';
}
messageHandler::messageHandler(const char _commandChar){

	commandChar = _commandChar;

	std::stringstream ssHelp; ssHelp << commandChar << "help";
	const char* help = stringToChar(ssHelp);
	commandStrings.push_back((char*)help);

	std::stringstream ssReg; ssReg << commandChar << "register";
	const char* reg = stringToChar(ssReg);
	commandStrings.push_back((char*)reg);

	std::stringstream ssLogin; ssLogin << commandChar << "login";
	const char* login = stringToChar(ssLogin);
	commandStrings.push_back((char*)login);

	std::stringstream ssLogout; ssLogout << commandChar << "logout";
	const char* logout = stringToChar(ssLogout);
	commandStrings.push_back((char*)logout);

	std::stringstream ssGetList; ssGetList << commandChar << "getlist";
	const char* getList = stringToChar(ssGetList);
	commandStrings.push_back((char*)getList);

	std::stringstream ssGetLog; ssGetLog << commandChar << "getlog";
	const char* getLog = stringToChar(ssGetLog);
	commandStrings.push_back((char*)getLog);

	std::stringstream ssSend; ssSend << commandChar << "send";
	const char* send = stringToChar(ssSend);
	commandStrings.push_back((char*)send);

}
int messageHandler::handleMessage(SOCKET _socket, char* _data) {

	//help
	if (compareChar(_data, commandStrings[help], strlen(commandStrings[help]))) {

		int a;
		if (strlen(extractUntilSpace(_data, 0, a)) != strlen(commandStrings[help])) {
			return INCORRECT_COMMAND;
		}

		return HELP_SCREEN;
	}

	//register
	else if (compareChar(_data, commandStrings[reg], strlen(commandStrings[reg]))) {

		int a;
		if (strlen(extractUntilSpace(_data, 0, a)) != strlen(commandStrings[reg])) {
			return INCORRECT_COMMAND;
		}

		return REGISTER;		
	}

	//login
	else if (compareChar(_data, commandStrings[login], strlen(commandStrings[login]))) {
		return LOGIN;
	}

	//logout
	else if (compareChar(_data, commandStrings[logout], strlen(commandStrings[logout]))) {

		if (strlen(_data) != strlen(commandStrings[logout])) {
			return INCORRECT_COMMAND;
		}
		
		return LOGOUT;
	}

	//get list
	else if (compareChar(_data, commandStrings[getList], strlen(commandStrings[getList]))) {

		if (strlen(_data) != strlen(commandStrings[getList])) {
			return INCORRECT_COMMAND;
		}

		return GET_LIST;
	}

	//send
	else if (compareChar(_data, commandStrings[sendMes], strlen(commandStrings[sendMes]))) {

		return SEND;
	}
	
	else if (compareChar(_data, commandStrings[getLog], strlen(commandStrings[getLog]))) {

		return GET_LOG;
	}


	else {
		return INCORRECT_COMMAND;
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
const char* messageHandler::extractUntilSpace(char* _data, int startingElement, int& lastChar) {
	std::stringstream ss;

	for (int i = startingElement; i < strlen(_data); i++) {

		if (_data[i] == ' '){	
			lastChar = i;
			break;
		}

		lastChar = i;
		ss << _data[i];
	
	}

	const char* returnChar = stringToChar(ss);
	return returnChar;
}
const char* messageHandler::extract(char* _data, int startingElement, int& lastChar) {
	std::stringstream ss;

	for (int i = startingElement; i < strlen(_data); i++) {
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
int messageHandler::stringConvertSend(std::string _str, SOCKET _socket) {

	std::string convertToString = _str;
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
