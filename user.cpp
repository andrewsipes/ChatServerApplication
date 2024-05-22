#include "user.h"

user::user(std::string _username, std::string _password, SOCKET _socket) {
	username = _username;
	password = _password;
	connected = false;

	if (_socket != 0) {
		socket = _socket;
	}
}

user::user(SOCKET _socket) {
	username = "";
	password = "";
	connected = false;

	if (_socket != 0) {
		socket = _socket;
	}
}

user::~user() {

}
