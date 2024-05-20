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
int clientHandler::registerUser(char& _user, char& _pass) {

	int userLength = strlen(&_user);
	int passLength = strlen(&_pass);

	if (strlen(&_user) > MAX_UNPW_CHAR_LENGTH || strlen(&_pass) > MAX_UNPW_CHAR_LENGTH) {
		return CHAR_LIMIT_REACHED;
	}

	user newUser(&_user, &_pass);
	clients.push_back(newUser);

	return SUCCESS;
}
