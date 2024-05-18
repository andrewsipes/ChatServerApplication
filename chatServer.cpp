#include "chatServer.h"
#include <iostream>
#include <string>
#include <sstream>

chatServer::chatServer() {
	//Initialize the wsaData object
	WSADATA wsaData;
	WSAStartup(WINSOCK_VERSION, &wsaData);

	//Hostname	
	gethostname(hostname, sizeof(hostname));

	//IP Address	
	struct addrinfo aInfo, *result;
	memset(&aInfo, 0, sizeof(aInfo));
	aInfo.ai_family = AF_INET;										//aInfo.ai_socktype = SOCK_STREAM; Not needed
	getaddrinfo(hostname, NULL, &aInfo, &result);
	struct sockaddr_in* ipv4 = (sockaddr_in*)(result->ai_addr);
	inet_ntop(AF_INET, &ipv4->sin_addr, ipAddr, INET_ADDRSTRLEN); //Convert

	std::cout << "HostName:" << hostname << "\nIPAddress: " << ipAddr << "\n\nLet's setup the server\n";

	int input = SETUP_ERROR;

	while (input != SUCCESS) {
		std::cin.clear();
		std::cout << "\nWhat port should the server listen on? ";
		std::cin >> port;

		input = checkPort(port);
		if (input == INCORRECT_PORT) {
			std::cout << "\nInvalid Port Entered, ports 0-1023 and 49152-65535 are reserved. Please try again\n";
			continue;
		}
	}

	input = SETUP_ERROR;

	while (input != SUCCESS) {
		std::cin.clear();
		std::cout << "\nWhat is the user capacity for this server? ";
		std::cin >> capacity;

		input = ClientHandler->checkCapacity(capacity);

		if (input == CAPACITY_REACHED) {
			std::printf("\nUnfortunately, the max capacity is %d, please try again.\n", MAX_CAPACITY);
			continue;
		}

	}

	input = SETUP_ERROR;

	while (input != SUCCESS) {
		std::cin.clear();
		std::cout << "\nPlease choose one of the following command characters (default is ~): ~, @, #, $ : ";

		std::cin >> commandChar;
		input = checkCommandChar(commandChar);

		if (input != SUCCESS) {
			std::cout << "\nYou did not enter a valid character, would like to use the default? Yes or No. ";

			std::string response; std::cin >> response;

			if (response == "Yes" || response == "yes" || response == "y" || response == "Y") {
				commandChar = DEFAULT_COMMAND_CHAR;
				input = SUCCESS;
			}

			else
				continue;

		}

	}

	std::cout << "\nServer has been Setup, see configuration below:\n" << std::endl;
	std::cout << "Hostname: " << hostname << std::endl;
	std::cout << "IP Address: " << ipAddr << std::endl;
	std::cout << "Port: " << port << std::endl;
	std::cout << "Capacity: " << capacity << std::endl;
	std::cout << "Command Char: " << commandChar << std::endl;
}


//initialize the server
int chatServer::init() {

	MessageHandler = new messageHandler(commandChar);
	ClientHandler = new clientHandler(*MessageHandler);

	lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (lSocket == INVALID_SOCKET)
		return SETUP_ERROR;

	//Bind
	cAddr.sin_family = AF_INET;					
	cAddr.sin_addr.S_un.S_addr = INADDR_ANY;	
	cAddr.sin_port = htons(port);				

	if (bind(lSocket, (SOCKADDR*)&cAddr, sizeof(cAddr)) == SOCKET_ERROR) {
		return BIND_ERROR;
	}

	//Listen
	if (listen(lSocket, capacity + 1) == SOCKET_ERROR) {
		return SETUP_ERROR;
	}

	FD_ZERO(&masterSet);
	FD_SET(lSocket, &masterSet);
	socketList.push_back(lSocket);
	

	return SUCCESS;

}

//verifies selected port
int chatServer::checkPort(uint16_t _port) {
	if (_port < 0 || (_port >= 0 && _port <= 1023) || (_port >= 49152 && _port <= 65535)) {

		return INCORRECT_PORT;			// 0-1023 are common ports
										// 49152 - 65535 are dynamic ports
	}

	else 
		return SUCCESS;
}

//verifies commandCharacter
int chatServer::checkCommandChar(char _character) {

	for (char _char : validChars) {

		if (_character == _char) {
			return SUCCESS;
		}

	}

	return SETUP_ERROR;

}

//This is the loop we are using to handle communication between clients
bool chatServer::run() {

	timeval tv;
	tv.tv_sec = 5;

	readySet = masterSet;
	select(0, &readySet, NULL, NULL, &tv);

	if (FD_ISSET(lSocket, &readySet)){
		SOCKET newSocket = accept(lSocket, NULL, NULL);
		socketList.push_back(newSocket);
		FD_SET(newSocket, &masterSet);

		std::stringstream ss;
		ss << "Welcome to the Chat Server!\nPlease use ' " << commandChar << " ' followed by a command to get started." <<
			"For example, to get list of commands, enter: " << commandChar << "help";

		std::string welcome = ss.str();
		const char* welcomeChar = welcome.c_str();

		sendMessage(newSocket, welcomeChar, 255);

	}
	
	for (SOCKET socket : socketList) {
		if ((FD_ISSET(socket, &readySet))) {
			char* buffer = new char[255];

			readMessage(socket, buffer, 255);

			if (buffer[0] == commandChar) {
				MessageHandler->handleMessage(socket, buffer);
			}

		}
	}

	return true;
}

int chatServer::readMessage(SOCKET _socket, char* buffer, int32_t size)
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

int chatServer::tcpReceive(SOCKET _socket, char& _data, int _length) {

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

int chatServer::sendMessage(SOCKET _socket, const char* data, int32_t length)
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

int chatServer::tcpSend(SOCKET _socket, const char* _data, int16_t _length) {

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

void chatServer::errorVerbose(int error) {

	if (error != SUCCESS) {

		switch (error) {

		case SHUTDOWN:
			std::cout << "ERROR: SHUTDOWN";
			break;
		case DISCONNECT:
			std::cout << "ERROR: DISCONNECT";
			break;
		case BIND_ERROR:
			std::cout << "ERROR: BIND_ERROR";
			break;
		case CONNECT_ERROR:
			std::cout << "ERROR: CONNECT_ERROR";
			break;
		case STARTUP_ERROR:
			std::cout << "ERROR: STARTUP_ERROR";
			break;
		case ADDRESS_ERROR:
			std::cout << "ERROR: ADDRESS_ERROR";
			break;
		case PARAMETER_ERROR:
			std::cout << "ERROR: PARAMETER_ERROR";
			break;
		case MESSAGE_ERROR:
			std::cout << "ERROR: MESSAGE_ERROR";
			break;
		case CLIENT_DISCONNCTED:
			std::cout << "ERROR: CLIENT_DISCONNCTED";
			break;
		case INCORRECT_UN_OR_PW:
			std::cout << "ERROR: INCORRECT_UN_OR_PW";
			break;
		case ALREADY_REGISTERED:
			std::cout << "ERROR: ALREADY_REGISTERED";
			break;
		case REGISTRATION_FAILED:
			std::cout << "ERROR: REGISTRATION_FAILED";
			break;
		case FAILED_TO_SEND:
			std::cout << "ERROR: FAILED_TO_SEND";
			break;
		case CHAR_LIMIT_REACHED:
			std::cout << "ERROR: CHAR_LIMIT_REACHED";
			break;
		case LOG_RETRIEVAL_FAILED:
			std::cout << "ERROR: LOG_RETRIEVAL_FAILED";
			break;
		case INCORRECT_COMMAND:
			std::cout << "ERROR: INCORRECT_COMMAND";
			break;
		case INCORRECT_PORT:
			std::cout << "ERROR: INCORRECT_PORT";
			break;
		case CAPACITY_REACHED:
			std::cout << "ERROR: CAPACITY_REACHED";
			break;
		case INCORRECT_IP:
			std::cout << "ERROR: INCORRECT_IP";
			break;
		case NOT_LOGGED_IN:
			std::cout << "ERROR: NOT_LOGGED_IN";
			break;
		case INTERNAL_ERROR:
			std::cout << "ERROR: INTERNAL_ERROR";
			break;
		case SETUP_ERROR:
			std::cout << "ERROR: SETUP_ERROR";
			break;
		default:
			std::cout << "Unknown error";
			break;
		}

		std::cout << "\n";
	}
}

chatServer::~chatServer() {
	delete ClientHandler;
	delete MessageHandler;
}

