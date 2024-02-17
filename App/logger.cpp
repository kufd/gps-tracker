#include <App/logger.h>
#include <string.h>
#include <stdio.h>

Logger::Logger(Storage* storage)
{
	this->storage = storage;
}

void Logger::error(const char* message)
{
	write("ERROR", message, "");
}

void Logger::error(const char* message, const char* context)
{
	write("ERROR", message, context);
}

void Logger::debug(const char* message)
{
	write("DEBUG", message, "");
}

void Logger::debug(const char* message, const char* context)
{
	write("DEBUG", message, context);
}

void Logger::info(const char* message)
{
	write("INFO", message, "");
}

void Logger::info(const char* message, const char* context)
{
	write("INFO", message, context);
}

void Logger::write(const char* level, const char* message, const char* context)
{
	size_t logSize = strlen(message) + strlen(context) + 20;
	char preparedLog[logSize];
	sprintf(preparedLog, "%s - %s - %s\n", level, message, context);

	storage->writeLog(preparedLog);
}
