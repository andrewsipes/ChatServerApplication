#include "user.h"

user::user(std::string _username, std::string _password, SOCKET _socket) {
	username = _username;
	password = _password;
	connected = false;

	//logFilepath = "logs/" + username + ".txt";
	
	if (_socket != 0) {
		socket = _socket;
	}
}

user::user(SOCKET _socket) {
	username = "";
	password = "";
	connected = false;

	//logFilepath = "logs/Client " + std::to_string(_socket) + ".txt";

	if (_socket != 0) {
		socket = _socket;
	}

}

user::~user() {
	username = "";
	password = "";
	socket = 0;
	connected = false;

	std::string logFilePath = "logs/Client " + std::to_string(socket) + ".txt";


}
