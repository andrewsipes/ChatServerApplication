// ChatServerApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "chatServer.h"
#include <iostream>

int main(){
    chatServer server;
    bool exit = true;

    int result = server.init();
    if (result != SUCCESS) {
        server.errorVerbose(result);
    }

    while (exit) {

       exit = server.run();

    }
  
}
