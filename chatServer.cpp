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
	struct addrinfo a4Info, *result;
	memset(&a4Info, 0, sizeof(a4Info));
	a4Info.ai_family = AF_INET;										//aInfo.ai_socktype = SOCK_STREAM; Not needed
	getaddrinfo(hostname, NULL, &a4Info, &result);
	struct sockaddr_in* ipv4 = (sockaddr_in*)(result->ai_addr);
	inet_ntop(AF_INET, &ipv4->sin_addr, ipv4Addr, INET_ADDRSTRLEN); //Convert

	//IP Address	
	struct addrinfo a6Info;
	memset(&a6Info, 0, sizeof(a6Info));
	a6Info.ai_family = AF_INET6;										//aInfo.ai_socktype = SOCK_STREAM; Not needed
	getaddrinfo(hostname, NULL, &a6Info, &result);
	struct sockaddr_in6* ipv6 = (sockaddr_in6*)(result->ai_addr);
	inet_ntop(AF_INET6, &ipv6->sin6_addr, ipv6Addr, INET6_ADDRSTRLEN); //Convert
	log.logEntry("\nHostName:" + MessageHandler->charToString(hostname) + "\nIPv4 Address: " + ipv4Addr + "\nIPv6 Address: " + ipv6Addr + "\n\nLet's setup the server\n");

	int input = SETUP_ERROR;

	while (input != SUCCESS) {
		std::cin.clear();
		log.logEntry("\nWhat port should the server listen on? ");
		std::cin >> port;
		log.logEntryNoVerbose("\nUser Entered: " + std::to_string(port));

		input = checkPort(port);
		if (input == INCORRECT_PORT) {
			log.logEntry("\nInvalid Port Entered, ports 0-1023 and 49152-65535 are reserved. Please try again");
		}

		else if (input == SUCCESS) {
			log.logEntry("Port " + std::to_string(port) + " will be used\n");
		}
		std::cin.clear();
		std::cin.ignore();
	}

	input = SETUP_ERROR;
	
	while (input != SUCCESS) {
	
		log.logEntry("\nWhat is the user capacity for this server? ");
		std::cin >> capacity;
		log.logEntryNoVerbose("\nUser Entered: " + std::to_string(capacity));

		input = ClientHandler->checkCapacity(capacity);

		if (input == CAPACITY_REACHED) {
			log.logEntry("Unfortunately, the max capacity is " + std::to_string(MAX_CAPACITY) + ". Please try again.");

		}

		else if (input == SETUP_ERROR) {
			log.logEntry("\nYou must set capacity between 1-4 clients");
		}

		else if (input == SUCCESS) {
			log.logEntry("Capacity is set at " + std::to_string(capacity) + " clients.");
		}

		std::cin.clear();
		std::cin.ignore();

	}

	input = SETUP_ERROR;

	while (input != SUCCESS) {
		log.logEntry("\n\nPlease choose one of the following command characters(default is ~) : ~, @, #, $ : ");

		std::cin >> commandChar;
		input = checkCommandChar(commandChar);
		log.logEntryNoVerbose("\nUser Entered: " + MessageHandler->charToString(&commandChar));

		if (input != SUCCESS) {
			log.logEntry("\nYou did not enter a valid character, would like to use the default? Yes or No. ");

			std::string response; std::cin >> response;
			log.logEntryNoVerbose("\nUser Entered: " + response);

			if (response == "Yes" || response == "yes" || response == "y" || response == "Y") {
				commandChar = DEFAULT_COMMAND_CHAR;
				input = SUCCESS;
			}

			else if (input == SUCCESS) {
				log.logEntry(MessageHandler->charToString(&commandChar) + " will be the command character");
			}

		}

		std::cin.clear();
		std::cin.ignore();
	}
	system("CLS");
	log.logEntry("\nServer has been Setup, see configuration below:");
	log.logEntry("\nHostname: " + MessageHandler->charToString(hostname));
	log.logEntry("\nIPv4 Address: " + MessageHandler->charToString(ipv4Addr));
	log.logEntry("\nIPv6 Address : " + MessageHandler->charToString(ipv6Addr));
	log.logEntry("\nPort: " + std::to_string(port));
	log.logEntry("\nCapacity: " + std::to_string(capacity));
	log.logEntry("\nCommand Char: " + MessageHandler->charToString(&commandChar));
	log.logEntry("\nWaiting for connections...");

	commandStr = MessageHandler->charToString(&commandChar);
	buffer = new char[255];

}

//initialize the server
int chatServer::init() {

	MessageHandler = new messageHandler(commandChar);
	ClientHandler = new clientHandler(*MessageHandler);

	lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	log.logEntryNoVerbose("\nCreating Listening Socket...");
	
	if (lSocket == INVALID_SOCKET) {
		log.logEntry(errorVerbose(lSocket));
		return SETUP_ERROR;
	}

	//Bind
	cAddr.sin_family = AF_INET;					
	cAddr.sin_addr.S_un.S_addr = INADDR_ANY;	
	cAddr.sin_port = htons(port);				
	int result = bind(lSocket, (SOCKADDR*)&cAddr, sizeof(cAddr)); 
	if (result == SOCKET_ERROR) {
		log.logEntry(errorVerbose(BIND_ERROR));
		return BIND_ERROR;
	}

	//Listen
	if (listen(lSocket, capacity + 1) == SOCKET_ERROR) {
		log.logEntry(errorVerbose(SETUP_ERROR));
		return SETUP_ERROR;
	}

	FD_ZERO(&masterSet);
	log.logEntryNoVerbose("\nZeroing Master Set...");
	FD_SET(lSocket, &masterSet);
	log.logEntryNoVerbose("\nAdding Listening Socket to MasterSet..");
	socketList.push_back(lSocket);
	
	return SUCCESS;

}

//verifies selected port
int chatServer::checkPort(uint16_t _port) {
	if (_port < 0 || (_port >= 0 && _port <= 1023) || (_port >= 49152 && _port <= 65535)) {
		log.logEntryNoVerbose("\n" + errorVerbose(INCORRECT_PORT));
		return INCORRECT_PORT;			// 0-1023 are common ports
										// 49152 - 65535 are dynamic ports
	}

	else {
		log.logEntryNoVerbose("\nValid Port Entered");
		return SUCCESS;
	}
}

//verifies commandCharacter
int chatServer::checkCommandChar(char _character) {

	for (char _char : validChars) {

		if (_character == _char) {
			log.logEntryNoVerbose("\nValid Char Entered");
			return SUCCESS;
		}

	}
	log.logEntryNoVerbose("\n" + errorVerbose(SETUP_ERROR));
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

		log.logEntry("\nClient " + std::to_string(newSocket) + " has connected...");

		user* client = ClientHandler->getClient(newSocket);

		logStr = "Welcome to the Chat Server!\nPlease use ' " + commandStr + " ' followed by a command to get started." +
			"For example, to get list of commands, enter: " + commandStr + "help";

		client->log.logEntryNoVerbose("\n" + logStr);
		MessageHandler->stringConvertSend(logStr, newSocket);

	}
	//Create two separate log files: one for user
	//commands and another for public messages
	//(excluding direct messages or DMs).
	
	for (SOCKET socket : socketList) {
		if ((FD_ISSET(socket, &readySet))) {
			
			readMessage(socket, buffer, 255);

			//don't log connection characters that are placed in the buffer
			if (buffer[0] >= 32) {
				//Add user into Vector of users, and log
				ClientHandler->getClient(socket)->log.logEntryNoVerbose("\n" + MessageHandler->charToString(buffer));

				if (buffer[0] == commandChar) {
					int result = MessageHandler->handleMessage(socket, buffer);

					switch (result) {
					case HELP_SCREEN:
						helpScreen(socket);
						break;
					case REGISTER:
						registerUser(socket, buffer);
						break;
					case LOGIN:
						break;
					case MESSAGE:
						break;
					case INCORRECT_COMMAND:
						commandError(socket);
						break;

					}

				}
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

		if (WSAGetLastError() == WSAESHUTDOWN) {
			log.logEntryNoVerbose("\n" + errorVerbose(SHUTDOWN));
			return SHUTDOWN;
		}

		else {
			
			return DISCONNECT;
		}
	}

	if (message > size) {
		return PARAMETER_ERROR;
	}

	received = tcpReceive(_socket, *buffer, message);

	if (received == SOCKET_ERROR || received == 0) {

		if (WSAGetLastError() == WSAESHUTDOWN) {
			log.logEntryNoVerbose("\n" + errorVerbose(SHUTDOWN));
			return SHUTDOWN;
		}

		else {
			return DISCONNECT;
		}
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
	if (length < 0 || length > 255) {
		log.logEntryNoVerbose("\n" + errorVerbose(PARAMETER_ERROR));
		return PARAMETER_ERROR;
	}

	uint8_t message = length;

	if (tcpSend(_socket, (char*)&message, 1) == SOCKET_ERROR) {

		int lastError = WSAGetLastError();

		if (lastError == WSAESHUTDOWN) {
			log.logEntryNoVerbose("\n" + errorVerbose(SHUTDOWN));
			return SHUTDOWN;
		}

		else
			return DISCONNECT;
	}

	int sent = tcpSend(_socket, data, message);

	if (sent == SOCKET_ERROR) {

		int lastError = WSAGetLastError();

		if (lastError == WSAESHUTDOWN) {
			log.logEntryNoVerbose("\n" + errorVerbose(SHUTDOWN));
			return SHUTDOWN;
		}

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

void chatServer::registerUser(SOCKET _socket, char* _buffer) {
	int* last = new int;

	//disregard the command so we can get the username then the password
	char* user = (char*)MessageHandler->extractUntilSpace(_buffer, strlen(MessageHandler->commandStrings[reg]) + 1, *last);
	char* pass = (char*)MessageHandler->extractUntilSpace(_buffer, *last + 1, *last);

	std::string userstr = MessageHandler->charToString(user);
	std::string pwstr = MessageHandler->charToString(pass);
	int result = ClientHandler->registerUser(*user, *pass, _socket); //add some error checking here

	switch (result) {
	case SUCCESS:
		logStr = + "\nCongratulations! " + userstr + " is now registered with password: " + pwstr;		
		break;
	case CHAR_LIMIT_REACHED:		
		logStr =  "\nYour Username or Password is too long. The limit is 20 characters each, please try again.";
		break;
	case PARAMETER_ERROR:
		logStr = "\nUsername or Password was blank, please reference " + commandStr + " help for the correct syntax";
		break;
	case ALREADY_REGISTERED:
		logStr = "\nThis user is already registered, Please Try again\n";
		break;

	}

	ClientHandler->getClient(_socket)->log.logEntryNoVerbose(logStr);
	MessageHandler->stringConvertSend(logStr, _socket);
}

//Debug method for checking errors
std::string chatServer::errorVerbose(int error) {
	if (error != SUCCESS) {
		switch (error) {
		case SHUTDOWN:
			return "\nERROR: SHUTDOWN";
		case DISCONNECT:
			return "\nERROR: DISCONNECT";
		case BIND_ERROR:
			return "\nERROR: BIND_ERROR";
		case CONNECT_ERROR:
			return "\nERROR: CONNECT_ERROR";
		case STARTUP_ERROR:
			return "\nERROR: STARTUP_ERROR";
		case ADDRESS_ERROR:
			return "\nERROR: ADDRESS_ERROR";
		case PARAMETER_ERROR:
			return "\nERROR: PARAMETER_ERROR";
		case MESSAGE_ERROR:
			return "\nERROR: MESSAGE_ERROR";
		case CLIENT_DISCONNECTED:
			return "\nERROR: CLIENT_DISCONNECTED";
		case INCORRECT_UN_OR_PW:
			return "\nERROR: INCORRECT_UN_OR_PW";
		case ALREADY_REGISTERED:
			return "\nERROR: ALREADY_REGISTERED";
		case REGISTRATION_FAILED:
			return "\nERROR: REGISTRATION_FAILED";
		case FAILED_TO_SEND:
			return "\nERROR: FAILED_TO_SEND";
		case CHAR_LIMIT_REACHED:
			return "\nERROR: CHAR_LIMIT_REACHED";
		case LOG_RETRIEVAL_FAILED:
			return "\nERROR: LOG_RETRIEVAL_FAILED";
		case INCORRECT_COMMAND:
			return "\nERROR: INCORRECT_COMMAND";
		case INCORRECT_PORT:
			return "\nERROR: INCORRECT_PORT";
		case CAPACITY_REACHED:
			return "\nERROR: CAPACITY_REACHED";
		case INCORRECT_IP:
			return "\nERROR: INCORRECT_IP";
		case NOT_LOGGED_IN:
			return "\nERROR: NOT_LOGGED_IN";
		case INTERNAL_ERROR:
			return "\nERROR: INTERNAL_ERROR";
		case SETUP_ERROR:
			return "\nERROR: SETUP_ERROR";
		default:
			return "\nERROR: Unknown error";
		}
	}
}

//sends help screen
void chatServer::helpScreen(SOCKET _socket) {


	std::string str1 =
	 "\n\n" + commandStr + "help\tProvides list of commands available\n" +
		"\n" + commandStr + "register\t<username> <password>\nRegisters a user to the server\n" +
		"\n" + commandStr + "login\t<username> <password>\nlogs a user into the chat server\n" +
		"\n" + commandStr + "logout\tlogs a user out of the chat server\n\0";

	std::string str2 =
		"\n" + commandStr + "getlist\t provides list of active clients\n" +
		"\n" + commandStr + "send\t<username> <message>\nsends a message to client (255 char limit)\n" +
		"\n" + commandStr + "send\t<message>\nsends a message all connnected clients\n\0";

	std::string str3=
		"\n" + commandStr + "getlog\t<username>\nretrieves logs for a specific user\n" +
		"\n" + commandStr + "getlog\tpublic\nretrieves public logs\n\0";

		user* user = ClientHandler->getClient(_socket);
		user->log.logEntryNoVerbose(str1 + str2 + str3);

		MessageHandler->stringConvertSend(str1, _socket);
		MessageHandler->stringConvertSend(str2, _socket);
		MessageHandler->stringConvertSend(str3, _socket);
}

//Sends client a message saying their syntax was wrong
void chatServer::commandError(SOCKET _socket) {
	std::string str = "Error: Syntax is incorrect. Use " + commandStr + "help for command syntax.";

	user* client = ClientHandler->getClient(_socket);
	client->log.logEntryNoVerbose("\n" + str);
	MessageHandler->stringConvertSend("\n" + str, _socket);

}

chatServer::~chatServer() {
	delete ClientHandler;
	delete MessageHandler;
}



