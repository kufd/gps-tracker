#include <App/logger.h>
#include <string.h>
#include <stdio.h>
#include "fatfs.h"
#include "main.h"

Logger::Logger(UART_HandleTypeDef* huartDebugConsole)
{
	this->huartDebugConsole = huartDebugConsole;
}

Logger::~Logger()
{
	if (!isLogFileOpened)
	{
		return;
	}

	FRESULT res = f_close(&logFile);
	if(res != FR_OK)
	{
		throw "Cannot close storage log file";
	}
}

void Logger::openLogFile()
{
	FRESULT res = f_open(&logFile, logFileName, FA_OPEN_APPEND | FA_WRITE);
	if(res != FR_OK)
	{
		throw "Cannot open storage log file";
	}
	isLogFileOpened = true;
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
	uint16_t debugConsoleMessageLen = strlen(level) + strlen(message) + strlen(context) + 8;
	char debugConsoleMessage[debugConsoleMessageLen+1];
	sprintf(debugConsoleMessage, "%s - %s - %s\r\n", level, message, context);
	//TODO DMA???
	HAL_UART_Transmit(huartDebugConsole, (uint8_t *) debugConsoleMessage, debugConsoleMessageLen, 100);

	if (!isLogFileOpened)
	{
		return;
	}

	int bytesWritten = f_printf(&logFile, "%s - %s - %s\n", level, message, context);
	if(bytesWritten <= 0)
	{
		throw "Cannot write log to storage";
	}
	if(++writeLogCounter % 10 == 0)
	{
		FRESULT res = f_sync(&logFile);
		if(res != FR_OK)
		{
			throw "Cannot sync log file to SD Card";
		}
	}
}
