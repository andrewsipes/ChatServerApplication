// ChatServerApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "chatServer.h"
#include <iostream>

int main(){
    chatServer server;

    int result = server.init();
    if (result != SUCCESS) {

        std::cout << "ERROR CODE:" << result;
    }

    while (server.run()) {

    }
  
}
