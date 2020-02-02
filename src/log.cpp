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
		text = "[MESSAGE] ";
		break;
	case Log::Type::INFO:
		text = "   [INFO] ";
		break;
	case Log::Type::WARNING:
		text = "[WARNING] ";
		break;
	case Log::Type::ERROR:
		text = "  [ERROR] ";
		break;
	}

	return text;
}

void Log::log(Log::Type type, std::string message, bool file, bool date)
{
	if (static_cast<unsigned int>(type) >= LOG_LEVEL) {
		std::string dateText = getCurrentDateTime();
		if (date) {
			std::cout << dateText << ": ";
		}
		std::cout << typeText(type) << message << "\n";
		if (file) {
			logToFile(message, date);
		}
	}
}
