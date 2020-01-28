#include <iostream>
#include <fstream>

#include "log.hpp"
#include "constants.hpp"

std::string Log::getCurrentDateTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[86];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return std::string(buf);
};

void Log::logToFile(std::string message, bool date)
{
	std::string filePath = PATH_LOG_FILE;
	std::string now = getCurrentDateTime();
	std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
	if (date) {
		ofs << now << '\t';
	}
	ofs << message << '\n';
	ofs.close();
}

std::string Log::typeText(Log::Type type)
{
	std::string text;
	switch (type) {
	case Log::Type::NONE:
		text = "";
		break;
	case Log::Type::MESSAGE:
        text = "[";
        text += "MESSAGE";
		text += "] ";
		break;
	case Log::Type::INFO:
        text = "   [";
        text += "INFO";
		text += "] ";
		break;
	case Log::Type::WARNING:
        text = "[";
        text += "WARNING";
		text += "] ";
		break;
	case Log::Type::ERROR:
        text = "  [";
        text += "ERROR";
		text += "] ";
		break;
	}
	return text;
}

void Log::log(Log::Type type, std::string message, bool file, bool date)
{
	std::string dateText = getCurrentDateTime();
	if (date) {
		std::cout << dateText << ": ";
	}
	std::cout << typeText(type) << message << "\n";
	if (file) {
		logToFile(message, date);
	}
}
