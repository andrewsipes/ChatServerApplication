#include "clientHandler.h"
#include "errors.h"
#include <string>


clientHandler::clientHandler(messageHandler& _messageHandler) {

	mh = _messageHandler;
	loadUsers();

}

//load users from file
void clientHandler::loadUsers() {
	std::string line;
	std::string username, password;

	//read each line from vault text file into the user's data structure
	vault.open("../logs/vault.txt", std::ios_base::in);
	if (vault.is_open()) {
		while (std::getline(vault, line)) {
			std::istringstream iss(line);
			iss >> username >> password;
			user* newUser = new user(username, password, 0);
			clients.push_back(newUser);
		}

		vault.close();
	}
}
//Takes in Capacity and Verifies if we are in the limit
int clientHandler::checkCapacity(int _clients)
{
	if (_clients > MAX_CAPACITY) {
		return CAPACITY_REACHED;
	}

	else if (_clients < 1) {
		return SETUP_ERROR;
	}

	else
		return SUCCESS;
}

//add a user to the vector
int clientHandler::registerUser(char& _user, char& _pass, SOCKET _socket) {

	int userLength = strlen(&_user);
	int passLength = strlen(&_pass);

	if (getClient(_socket)->connected) {
		return ALREADY_CONNECTED;
	}

	else if (_user == '\0' || _pass == '\0') {
		return PARAMETER_ERROR;
	}

	else if (userLength > MAX_UNPW_CHAR_LENGTH || passLength > MAX_UNPW_CHAR_LENGTH) {
		return CHAR_LIMIT_REACHED;
	}

	else {
		for (user* client : clients) {
			if (mh.compareChar(mh.stringToChar(client->username), &_user, strlen(&_user)))
				return ALREADY_REGISTERED;
		}	
	}

	std::string userStr, passStr;
	userStr = mh.charToString(&_user);
	passStr = mh.charToString(&_pass);


	user* newUser = new user(userStr, passStr, 0);
	clients.push_back(newUser);

	vault.open("../logs/vault.txt", std::ios_base::app);
	if (vault.is_open()) {
		vault << userStr + " " + passStr + "\n";
		vault.close();
	}

	return SUCCESS;
}

int clientHandler::authenticateUser(char* _user, char* _pass, SOCKET _socket) {

	int userLength = strlen(_user);
	int passLength = strlen(_pass);

	if (_user[0] == '\0' || _pass[0] == '\0') {
		return PARAMETER_ERROR;
	}

	else if (userLength > MAX_UNPW_CHAR_LENGTH || passLength > MAX_UNPW_CHAR_LENGTH) {
		return CHAR_LIMIT_REACHED;
	}

	else {
		for (user* client : clients) {
			bool userTest = mh.compareChar(mh.stringToChar(client->username), _user, strlen(_user));
			bool passTest = mh.compareChar(mh.stringToChar(client->password), _pass, strlen(_pass));

			if (userTest && !passTest) {
					return INCORRECT_PW;
			}
			
			else if (userTest && passTest) {

				if (strlen(mh.stringToChar(client->username)) != strlen(_user)) {
					return USER_NOT_FOUND;
				}

				else if (strlen(mh.stringToChar(client->password)) != strlen(_pass)) {
					return INCORRECT_PW;
				}

				//authenticated
				else if (client->connected)
					return ALREADY_CONNECTED;

				//now we need to update which socket the user is on
				for (user* clientSocket : clients) {
					if (clientSocket->socket == _socket) {
						clientSocket->username = client->username;
						clientSocket->password = client->password;

						for (auto iter = clients.begin(); iter != clients.end(); ++iter) {

							user* dupClient = *iter;

							//if we find the a duplicate of the user (stale entry or from registration) remove it
							if (dupClient->username == clientSocket->username
								&& dupClient->socket != clientSocket->socket) {

								clients.erase(iter);

								//std::string newFilePath = "logs/" + clientSocket->username + ".txt";
								//std::string oldFilePath = "logs/Client " + std::to_string(_socket) + ".txt";
								//const char* oldPath = oldFilePath.c_str();
								//const char* newPath = newFilePath.c_str();

								//clientSocket->logFilepath = newFilePath;

								////rename file
								//rename(oldPath, newPath);

								return SUCCESS;
							}
						}
					}
				}
			}
		}


	}
	return USER_NOT_FOUND;
}

user* clientHandler::getClient(SOCKET _socket) {
	for (user* client : clients) {
		if (client->socket == _socket)
			return client;
	}

	user* newUser = new user(_socket);
	clients.push_back(newUser);
	return newUser;
}

clientHandler::~clientHandler(){

	for (user* client : clients) {
		delete client;
	}
}