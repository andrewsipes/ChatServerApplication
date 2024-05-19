#include "user.h"

user::user(char* _username, char* _password) {
	username = _username;
	password = _password;
}

user::~user() {
	delete username;
	delete password;
}