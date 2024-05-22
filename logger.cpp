#include "logger.h"
#include <string>
#include <iostream>
#include "user.h"

void logger::logEntry(std::string _logString)
{
	std::cout << _logString;
	log.push_back(_logString);
}

void logger::logEntryNoVerbose(std::string _logString){
	log.push_back(_logString);
}




