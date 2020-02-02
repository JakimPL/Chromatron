#ifndef LOG_H
#define LOG_H

#include <string>

class Log
{
private:
	static std::string getCurrentDateTime();
	static void logToFile(std::string message, bool date = true);
public:
	enum class Type;
	static std::string typeText(Log::Type type);
	static void log(Log::Type type, std::string message, bool file = true, bool date = true);
};

enum class Log::Type {
	NONE,
	MESSAGE,
	INFO,
	WARNING,
	ERROR
};

#endif