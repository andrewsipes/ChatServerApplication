#include "chatServer.h"
#include <iostream>



chatServer::chatServer() {
	
	std::cout << "Welcome to the Chat Server!\nFirst, we need to setup the server\n";

	Setup(); 
	
}

//Gets the details from the user on the initial setup
void chatServer::Setup() {

	while (true) {
		std::cin.clear();
		std::cout << "\nWhat port should the server listen on? ";
		std::cin >> port;

		if (checkPort(port) == INCORRECT_PORT) {
			std::cout << "\nInvalid Port Entered, ports 0-1023 and 49152 are reserved.\n";
			continue;
		}

		break;

	}

	while (true) {
		std::cin.clear();
		std::cout << "\nWhat is the user capacity for this server? ";
		std::cin >> capacity;

		if (ClientHandler.checkCapacity(capacity) == CAPACITY_REACHED) {
			std::printf("\nUnfortunately, the max capacity is %d, please try again.\n", MAX_CAPACITY);
			continue;
		}

		break;
	}

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

	else SUCCESS;
}
