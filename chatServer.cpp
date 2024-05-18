#include "chatServer.h"
#include <iostream>


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

	Setup(); 
}

//Gets the details from the user on the initial setup
void chatServer::Setup() {

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

		input = ClientHandler.checkCapacity(capacity);

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

	std::cout << "\nServer has been Setup, see configuration below:" << std::endl;
	std::cout << "Hostname: " << hostname << std::endl;
	std::cout << "IP Address: " << ipAddr << std::endl;
	std::cout << "Port: " << port << std::endl;
	std::cout << "Capacity: " << capacity << std::endl;
	std::cout << "Command Char: " << commandChar << std::endl;

}

int chatServer::init(uint16_t _port) {

	return 0;
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

		if (_character != _char) {
			return SETUP_ERROR;
		}

		else if (_character == _char) {
			return SUCCESS;
		}
	}

}
