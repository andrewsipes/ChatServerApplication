#include "user.h"

user::user(std::string _username, std::string _password, SOCKET _socket) {
	username = _username;
	password = _password;

	if (_socket != 0) {
		socket = socket;
	}
}

user::~user() {

}