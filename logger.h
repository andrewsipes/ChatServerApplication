#pragma once

#include <vector>
#include <string>
#include <fstream>

class logger
{
	std::vector<std::string>log;
	std::fstream fLog;

public:
	void logEntry(std::string _logString, std::string _logPath);
	void logEntryNoVerbose(std::string _logString, std::string _logPath);

};

