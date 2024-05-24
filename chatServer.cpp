#include "chatServer.h"
#include <iostream>
#include <string>
#include <sstream>

chatServer::chatServer() {
	logPath = "logs/chatServer.txt";

	//Initialize the wsaData object
	WSADATA wsaData;
	WSAStartup(WINSOCK_VERSION, &wsaData);

	//Hostname	
	gethostname(hostname, sizeof(hostname));

	//IPv4 Address	
	struct addrinfo a4Info, *result;
	memset(&a4Info, 0, sizeof(a4Info));
	a4Info.ai_family = AF_INET;										
	getaddrinfo(hostname, NULL, &a4Info, &result);
	struct sockaddr_in* ipv4 = (sockaddr_in*)(result->ai_addr);
	inet_ntop(AF_INET, &ipv4->sin_addr, ipv4Addr, INET_ADDRSTRLEN); //Convert

	//IPv6 Address	
	struct addrinfo a6Info;
	memset(&a6Info, 0, sizeof(a6Info));
	a6Info.ai_family = AF_INET6;										
	getaddrinfo(hostname, NULL, &a6Info, &result);
	struct sockaddr_in6* ipv6 = (sockaddr_in6*)(result->ai_addr);
	inet_ntop(AF_INET6, &ipv6->sin6_addr, ipv6Addr, INET6_ADDRSTRLEN); //Convert
	log.logEntry("\nHostName:" + MessageHandler->charToString(hostname) + "\nIPv4 Address: " + ipv4Addr + "\nIPv6 Address: " + ipv6Addr + "\n\nLet's setup the server\n", logPath);


	//Below code gets parameters for the server from admin user who starts the server
	int input = SETUP_ERROR;

	while (input != SUCCESS) {
		std::cin.clear();
		log.logEntry("\nWhat port should the server listen on? ", logPath);
		std::cin >> port;
		log.logEntryNoVerbose("\nUser Entered: " + std::to_string(port), logPath);

		input = checkPort(port);
		if (input == INCORRECT_PORT) {
			log.logEntry("\nInvalid Port Entered, ports 0-1023 and 49152-65535 are reserved. Please try again", logPath);
		}

		else if (input == SUCCESS) {
			log.logEntry("Port " + std::to_string(port) + " will be used\n", logPath);
		}
		std::cin.clear();
		std::cin.ignore();
	}

	input = SETUP_ERROR;
	
	while (input != SUCCESS) {
	
		log.logEntry("\nWhat is the user capacity for this server? ", logPath);
		std::cin >> capacity;
		log.logEntryNoVerbose("\nUser Entered: " + std::to_string(capacity), logPath);

		input = ClientHandler->checkCapacity(capacity);

		if (input == CAPACITY_REACHED) {
			log.logEntry("Unfortunately, the max capacity is " + std::to_string(MAX_CAPACITY) + ". Please try again.", logPath);

		}

		else if (input == SETUP_ERROR) {
			log.logEntry("\nYou must set capacity between 1-4 clients", logPath);
		}

		else if (input == SUCCESS) {
			log.logEntry("Capacity is set at " + std::to_string(capacity) + " clients.", logPath);
		}

		std::cin.clear();
		std::cin.ignore();

	}

	input = SETUP_ERROR;

	while (input != SUCCESS) {
		log.logEntry("\n\nPlease choose one of the following command characters(default is ~) : ~, @, #, $ : ", logPath);

		std::cin >> commandChar;
		input = checkCommandChar(commandChar);
		log.logEntryNoVerbose("\nUser Entered: " + MessageHandler->charToString(&commandChar), logPath);

		if (input != SUCCESS) {
			log.logEntry("\nYou did not enter a valid character, would like to use the default? Yes or No. ", logPath);

			std::string response; std::cin >> response;
			log.logEntryNoVerbose("\nUser Entered: " + response, logPath);

			if (response == "Yes" || response == "yes" || response == "y" || response == "Y") {
				commandChar = DEFAULT_COMMAND_CHAR;
				input = SUCCESS;
			}

			else if (input == SUCCESS) {
				log.logEntry(MessageHandler->charToString(&commandChar) + " will be the command character", logPath);
			}

		}

		std::cin.clear();
		std::cin.ignore();
	}
	//clear screen and display host info + connections
	system("CLS");
	log.logEntry("\nServer has been Setup, see configuration below:", logPath);
	log.logEntry("\nHostname: " + MessageHandler->charToString(hostname), logPath);
	log.logEntry("\nIPv4 Address: " + MessageHandler->charToString(ipv4Addr), logPath);
	log.logEntry("\nIPv6 Address : " + MessageHandler->charToString(ipv6Addr), logPath);
	log.logEntry("\nPort: " + std::to_string(port), logPath);
	log.logEntry("\nCapacity: " + std::to_string(capacity), logPath);
	log.logEntry("\nCommand Char: " + MessageHandler->charToString(&commandChar), logPath);
	log.logEntry("\nWaiting for connections...", logPath);

	commandStr = MessageHandler->charToString(&commandChar);


}

//initialize the server
int chatServer::init() {

	MessageHandler = new messageHandler(commandChar);
	ClientHandler = new clientHandler(*MessageHandler);

	lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	log.logEntryNoVerbose("\nCreating Listening Socket...", logPath);
	
	if (lSocket == INVALID_SOCKET) {
		log.logEntry(errorVerbose(lSocket), logPath);
		return SETUP_ERROR;
	}

	//Bind
	cAddr.sin_family = AF_INET;					
	cAddr.sin_addr.S_un.S_addr = INADDR_ANY;	
	cAddr.sin_port = htons(port);				
	int result = bind(lSocket, (SOCKADDR*)&cAddr, sizeof(cAddr)); 
	if (result == SOCKET_ERROR) {
		log.logEntry(errorVerbose(BIND_ERROR), logPath);
		return BIND_ERROR;
	}

	//Listen
	if (listen(lSocket, capacity + 1) == SOCKET_ERROR) {
		log.logEntry(errorVerbose(SETUP_ERROR), logPath);
		return SETUP_ERROR;
	}

	FD_ZERO(&masterSet);
	log.logEntryNoVerbose("\nZeroing Master Set...", logPath);
	FD_SET(lSocket, &masterSet);
	log.logEntryNoVerbose("\nAdding Listening Socket to MasterSet..", logPath);
	socketList.push_back(lSocket);
	
	return SUCCESS;

}

//verifies selected port
int chatServer::checkPort(uint16_t _port) {
	if (_port < 0 || (_port >= 0 && _port <= 1023) || (_port >= 49152 && _port <= 65535)) {
		log.logEntryNoVerbose("\n" + errorVerbose(INCORRECT_PORT), logPath);
		return INCORRECT_PORT;			// 0-1023 are common ports
										// 49152 - 65535 are dynamic ports
	}

	else {
		log.logEntryNoVerbose("\nValid Port Entered", logPath);
		return SUCCESS;
	}
}

//verifies commandCharacter
int chatServer::checkCommandChar(char _character) {

	for (char _char : validChars) {

		if (_character == _char) {
			log.logEntryNoVerbose("\nValid Char Entered", logPath);
			return SUCCESS;
		}

	}
	log.logEntryNoVerbose("\n" + errorVerbose(SETUP_ERROR), logPath);
	return SETUP_ERROR;

}

//Main Loop, checks listening sockets and facilitates communication
bool chatServer::run() {
	char* buffer = new char[255];
	timeval tv;
	tv.tv_sec = 5;

	readySet = masterSet;
	select(0, &readySet, NULL, NULL, &tv);

	if (socketList.size() < capacity && FD_ISSET(lSocket, &readySet)){
		SOCKET newSocket = accept(lSocket, NULL, NULL);
		socketList.push_back(newSocket);
		FD_SET(newSocket, &masterSet);

		log.logEntry("\nClient " + std::to_string(newSocket) + " has connected...", logPath);

		user* client = ClientHandler->getClient(newSocket);

		logStr = "Welcome to the Chat Server!\nPlease use ' " + commandStr + " ' followed by a command to get started." +
			"For example, to get list of commands, enter: " + commandStr + "help";

		client->log.logEntryNoVerbose("\n" + logStr, client->logFilepath);
		MessageHandler->stringConvertSend(logStr, newSocket);

	}

	else if (FD_ISSET(lSocket, &readySet)){
		SOCKET newSocket = accept(lSocket, NULL, NULL);
		log.logEntry("\nClient " + std::to_string(newSocket) + " attempted to connect, but capacity has been reached...", logPath);
		logStr = "\nCapacity has been reached, Try again later";
		MessageHandler->sendMessage(newSocket, MessageHandler->stringToChar(logStr), 255);
		shutdown(newSocket, SD_BOTH);
		closesocket(newSocket);		
	}
	
	for (SOCKET socket : socketList) {
		if ((FD_ISSET(socket, &readySet))) {
		
			MessageHandler->readMessage(socket, buffer, 255);

			//Remove socket from the list if it gives you this character, it will get readded anyway
			if (buffer[0] == -51) {

				for (auto iter = socketList.begin(); iter != socketList.end(); ++iter) {
					SOCKET oldSocket = *iter;

					if (oldSocket == socket) {
						socketList.erase(iter);
						break;
					}
				}
			}

			//don't log connection characters that are placed in the buffe
			if (buffer[0] >= 32) {
				//Add user into Vector of users, and log
				user* client = ClientHandler->getClient(socket);
				client->log.logEntryNoVerbose("\n" + MessageHandler->charToString(buffer), client->logFilepath);

				//if command is picked up, check the command, and do the logic associated with it
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
						loginUser(socket, buffer);
						break;
					case LOGOUT:
						logoutUser(socket, buffer);
					case SEND:
						messageToClient(socket, buffer);
						break;
					case GET_LIST:
						getList(socket);
						break;
					case GET_LOG:
						break;
					case INCORRECT_COMMAND:
						commandError(socket);
						break;

					}

				}
			}
	
		}
	}

	delete buffer;
	return true;
}

//register user logic
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
	case ALREADY_CONNECTED:
		logStr = "\nYou cannot register while logged in. Please log out first.";
		break;

	}


	ClientHandler->getClient(_socket)->log.logEntryNoVerbose(logStr, ClientHandler->getClient(_socket)->logFilepath);
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
		user->log.logEntryNoVerbose(str1 + str2 + str3, user->logFilepath);

		MessageHandler->stringConvertSend(str1, _socket);
		MessageHandler->stringConvertSend(str2, _socket);
		MessageHandler->stringConvertSend(str3, _socket);
}

//login logic
void chatServer::loginUser(SOCKET _socket, char* _buffer) {

	int* last = new int;

	//disregard the command so we can get the username then the password
	char* user = (char*)MessageHandler->extractUntilSpace(_buffer, strlen(MessageHandler->commandStrings[login]) + 1, *last);
	char* pass = (char*)MessageHandler->extractUntilSpace(_buffer, *last + 1, *last);

	std::string userstr = MessageHandler->charToString(user);
	std::string pwstr = MessageHandler->charToString(pass);

	int result;
	if (ClientHandler->getClient(_socket)->connected == true) {
		result = ALREADY_CONNECTED;
	}

	else
		result = ClientHandler->authenticateUser(user, pass, _socket);

	switch (result) {
	case SUCCESS:
		logStr = +"\nWelcome " + userstr + "!";
		ClientHandler->getClient(_socket)->connected = true;
		ClientHandler->getClient(_socket)->log.logEntryNoVerbose("\nUser Authenticated Successfully", ClientHandler->getClient(_socket)->logFilepath);
		break;
	case CHAR_LIMIT_REACHED:
		logStr = "\nYour Username or Password is too long. The limit is 20 characters each, please try again.";
		break;
	case PARAMETER_ERROR:
		logStr = "\nUsername or Password was blank, please reference " + commandStr + " help for the correct syntax";
		break;
	case ALREADY_CONNECTED:
		logStr = "\n" + userstr + " is already logged in. Unable to authenticate.";
		break;
	case INCORRECT_UN_OR_PW:
		logStr = "\nIncorrect Username or Password, Please try again.";
		break;
	case USER_NOT_FOUND:
		logStr = "\n" + userstr + " was not found. Please try again.";
		break;
	case INCORRECT_PW:
		logStr = "\nIncorrect Password. Please try again.";
		break;
			
	}

	ClientHandler->getClient(_socket)->log.logEntryNoVerbose(logStr, ClientHandler->getClient(_socket)->logFilepath);
	MessageHandler->stringConvertSend(logStr, _socket);

}

//logout logic
void chatServer::logoutUser(SOCKET _socket, char* _buffer) {

	int logStrLength = strlen(MessageHandler->commandStrings[logout]);

	if (_buffer[0] == -51) {

		for (auto iter = socketList.begin(); iter != socketList.end(); ++iter) {
			SOCKET oldSocket = *iter;

			if (oldSocket == _socket) {
				socketList.erase(iter);
				break;
			}
		}
		log.logEntry("\nClient " + std::to_string(_socket) + " has disconnected...", logPath);
		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
	}

	else if (ClientHandler->getClient(_socket)->connected = false) {
		logStr = "\nYou are not logged in.";
		MessageHandler->stringConvertSend(logStr, _socket);
		ClientHandler->getClient(_socket)->log.logEntryNoVerbose(logStr, ClientHandler->getClient(_socket)->logFilepath);
		MessageHandler->stringConvertSend(logStr, _socket);
	}

	else if (MessageHandler->compareChar(_buffer, MessageHandler->commandStrings[logout], logStrLength)
	&&	strlen(_buffer) == logStrLength) {
		logStr = "\nYou successfully logged out.";
		ClientHandler->getClient(_socket)->connected = false;
		
		for (auto iter = socketList.begin(); iter != socketList.end(); ++iter) {
			SOCKET oldSocket = *iter;

			if (oldSocket == _socket) {
				socketList.erase(iter);
				break;
			}
		}

		ClientHandler->getClient(_socket)->log.logEntryNoVerbose(logStr, ClientHandler->getClient(_socket)->logFilepath);
		MessageHandler->stringConvertSend(logStr, _socket);

		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
		log.logEntry("\n"+ ClientHandler->getClient(_socket)->username + " has logged out", logPath);
	}


	else {
		commandError(_socket);
	}

}

//gets list of connected clients
void chatServer::getList(SOCKET _socket) {
	user* client = ClientHandler->getClient(_socket);

	if (client->connected == true) {
		logStr = "\nConnected Clients: ";

		for (auto* client : ClientHandler->clients) {
			if (client->username != "") {
				logStr = logStr + client->username + ",";
			}
		}
	}

	else {
		logStr = "You must be logged in to use this command.";
	}
	client->log.logEntryNoVerbose(logStr, client->logFilepath);
	MessageHandler->stringConvertSend(logStr, _socket);
}

//Sends client a message saying their syntax was wrong
void chatServer::commandError(SOCKET _socket) {
	std::string str = "Error: Syntax is incorrect. Use " + commandStr + "help for command syntax.";

	user* client = ClientHandler->getClient(_socket);
	client->log.logEntryNoVerbose("\n" + str, ClientHandler->getClient(_socket)->logFilepath);
	MessageHandler->stringConvertSend("\n" + str, _socket);

}

//sends a message to the client
void chatServer::messageToClient(SOCKET _socket, char* _buffer) {

	bool userFound = false;
	user* sender = ClientHandler->getClient(_socket);
	user* receiver = nullptr;
	int* lastChar = new int;
	user* sendToMe = sender;

	//sender->log.logEntryNoVerbose("\n" + MessageHandler->charToString(_buffer), sender->logFilepath);
	char* charToSend = (char*)MessageHandler->extractUntilSpace(_buffer, 0, *lastChar);
	char* receiverName = (char*)MessageHandler->extractUntilSpace(_buffer, *lastChar + 1, *lastChar);


	if (ClientHandler->getClient(_socket)->connected = false) {
		logStr = "\nYou must be logged in to use this command.";
		sender->log.logEntryNoVerbose(logStr, sender->logFilepath);
	}

	else if (MessageHandler->charToString(receiverName) == "public") {

		//broadcast logic
	}

	else {

		charToSend = (char*)MessageHandler->extract(_buffer, *lastChar+1, *lastChar);
		std::string receiverNameStr = MessageHandler->charToString(receiverName);

		for (user* client : ClientHandler->clients) {
			if (client->username == receiverNameStr && client->connected == true) {	
				receiver = client;
				userFound = true;
			}
		}

		if (!userFound) {
			logStr = "\nClient was not found.Please check connected clients.";
			sender->log.logEntryNoVerbose(logStr, sender->logFilepath);
		}

		else if (userFound && receiver != nullptr) {

			logStr = "\n[" + sender->username + "]:" + MessageHandler->charToString(charToSend);

			//should never hit this, fail safe if we receive a message too long
			if (logStr.length() > 255) {
				logStr = "\nMessage Length too long, please consider sending multiple messages.";
				sender->log.logEntryNoVerbose(logStr, sender->logFilepath);

			}

			sendToMe = receiver;
		}
			
			MessageHandler->sendMessage(sendToMe->socket, MessageHandler->stringToChar(logStr), 255);
	}

	delete lastChar;
}

chatServer::~chatServer() {
	delete ClientHandler;
	delete MessageHandler;
}



