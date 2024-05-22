#pragma once

#include <vector>
#include <string>

class logger
{
	std::vector<std::string>log;

public:
	void logEntry(std::string _logString);
	void logEntryNoVerbose(std::string _logString);


};

