#ifndef __LOGGER_H
#define __LOGGER_H

#include "fatfs.h"

class Logger
{
	private:
		char logFileName[8] = "app.log";
		FIL logFile;
		bool isLogFileOpened = false;

		uint8_t writeLogCounter = 0;
		UART_HandleTypeDef* huartDebugConsole;
	public:
		Logger(UART_HandleTypeDef* huartDebugConsole);
		~Logger();
		void error(const char* message);
		void error(const char* message, const char* contex);
		void debug(const char* message);
		void debug(const char* message, const char* contex);
		void info(const char* message);
		void info(const char* message, const char* contex);
		void openLogFile();
	private:
		void write(const char* level, const char* message, const char* contex);
};


extern Logger *logger;

#endif /* __LOGGER_H */
