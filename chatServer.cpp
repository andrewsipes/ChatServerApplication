#include "chatServer.h"
#include <iostream>


chatServer::chatServer() {
	//Initialize the wsaData object
	WSADATA wsaData;
	WSAStartup(WINSOCK_VERSION, &wsaData);

	MessageHandler = new messageHandler;
	ClientHandler = new clientHandler(*MessageHandler);

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

	lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (lSocket == INVALID_SOCKET)
		return SETUP_ERROR;

	//Bind
	cAddr.sin_family = AF_INET;					//set family of addresses
	cAddr.sin_addr.S_un.S_addr = INADDR_ANY;	//take any IP	
	cAddr.sin_port = htons(port);				//convert for TCP/IP

	if (bind(lSocket, (SOCKADDR*)&cAddr, sizeof(cAddr)) == SOCKET_ERROR) {	//Try to bind
		return BIND_ERROR;
	}

	//Listen
	if (listen(lSocket, capacity) == SOCKET_ERROR) {
		return SETUP_ERROR;
	}

	socketList.push_back(lSocket);
	FD_ZERO(&readySet);
	FD_ZERO(&masterSet);

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

	//ClientHandler->handleClients(lSocket);

	select(0, &readySet, NULL, NULL, NULL);

	for (SOCKET socket : socketList) {

		if (socket == lSocket) {

			SOCKET comSocket = accept(lSocket, NULL, NULL);

			socketList.push_back(comSocket);
			
		}

	}


	return true;
}
