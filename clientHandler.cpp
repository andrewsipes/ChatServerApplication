#include "clientHandler.h"
#include "errors.h"

clientHandler::clientHandler(messageHandler& _messageHandler) {

	mh = _messageHandler;

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

	if (_user == '\0' || _pass == '\0') {
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

	/*bool foundEmptyUser = false;
	for (user* client : clients) {
		if (client->socket == _socket && client->username == "") {
			client->username = userStr;
			client->password = passStr;
			foundEmptyUser = true;
		}
	}

	if (!foundEmptyUser) {*/
		user* newUser = new user(userStr, passStr, 0);
		clients.push_back(newUser);
	//}

	return SUCCESS;
}

int clientHandler::authenticateUser(char& _user, char& _pass, SOCKET _socket) {

	int userLength = strlen(&_user);
	int passLength = strlen(&_pass);

	if (_user == '\0' || _pass == '\0') {
		return PARAMETER_ERROR;
	}

	else if (userLength > MAX_UNPW_CHAR_LENGTH || passLength > MAX_UNPW_CHAR_LENGTH) {
		return CHAR_LIMIT_REACHED;
	}

	else {
		for (user* client : clients) {
			if (mh.compareChar(mh.stringToChar(client->username), &_user, strlen(&_user))) {

				//authenticated
				if (client->connected)
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
								break;
							}

						}
				
						break;
					}
				}
			}
		}


	}
	return SUCCESS;
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