#include "clientHandler.h"
#include "errors.h"

clientHandler::clientHandler(messageHandler& _messageHandler) {

	message = _messageHandler;

}

void clientHandler::handleClients(SOCKET& lSocket)
{

}

//Takes in Capacity and Verifies if we are in the limit
int clientHandler::checkCapacity(int _clients)
{
	if (_clients > MAX_CAPACITY) {
		return CAPACITY_REACHED;
	}

	else
		return SUCCESS;
}
