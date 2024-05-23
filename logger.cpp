#include "logger.h"
#include <string>
#include <iostream>
#include "user.h"

void logger::logEntry(std::string _logString, std::string _logPath)
{
	std::cout << _logString;
	log.push_back(_logString);

	fLog.open(_logPath.c_str(), std::ios::app);

	if (fLog.is_open()) {
		fLog << _logString;
		fLog.close();
	}


}

void logger::logEntryNoVerbose(std::string _logString, std::string _logPath){
	log.push_back(_logString);

	fLog.open(_logPath.c_str(), std::ios::app);

	if (fLog.is_open()) {
		fLog << _logString;
		fLog.close();
	}
}




