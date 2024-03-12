#include "config.class.h"
#include "fatfs.h"
#include <string.h>

const char* Config::getWifiSsid()
{
	return wifiSsid;
}

const char* Config::getWifiPassword()
{
	return wifiPassword;
}

void Config::read()
{
	initConfig();

	FIL file;
	FRESULT res = f_open(&file, fileName, FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		throw "Cannot open config file";
	}

	UINT readBufferSize = 512;
	char readBuffer[readBufferSize];
	UINT readBufferUsedSize;

	res = f_read(&file, readBuffer, readBufferSize, &readBufferUsedSize);
	if(res != FR_OK)
	{
		throw "Cannot read config file";
	}

	if (readBufferUsedSize == readBufferSize)
	{
		throw "too big config file";
	}

	readBuffer[readBufferUsedSize] = '\0';

	const char* readCursor = readBuffer;
	const char* lastNewLineChar = strrchr(readCursor, '\n');
	if (lastNewLineChar == NULL)
	{
		throw "corrupted config";
	}
	for (;;)
	{
		char* firstNewLineChar = strchr(readCursor, '\n');
		if (firstNewLineChar == NULL)
		{
			throw "corrupted config";
		}

		char* firstDelimiterChar = strchr(readCursor, '=');
		if (firstDelimiterChar > firstNewLineChar)
		{
			throw "corrupted config";
		}

		UINT nameLen = firstDelimiterChar - readCursor;
		UINT valueLen = firstNewLineChar - firstDelimiterChar - 1;

		if (valueLen > maxValueLen)
		{
			throw "too long option value";
		}

		char name[nameLen+1];
		char value[valueLen+1];
		strncpy(name, readCursor, nameLen);
		name[nameLen] = '\0';
		strncpy(value, firstDelimiterChar+1, valueLen);
		value[valueLen] = '\0';

		if (strcmp(name, "wifiSsid") == 0)
		{
			strcpy(wifiSsid, value);
		}
		if (strcmp(name, "wifiPassword") == 0)
		{
			strcpy(wifiPassword, value);
		}

		//all data parsed
		if (lastNewLineChar == firstNewLineChar)
		{
			break;
		}

		readCursor = firstNewLineChar+1;
	}




	res = f_close(&file);
	if(res != FR_OK)
	{
		throw "Cannot close config file";
	}
}

void Config::initConfig()
{
	//	FRESULT res = f_unlink("gps-tracker.conf");
	//	if(res != FR_OK)
	//	{
	//		throw "Cannot delete record sync log file";
	//	}
	//
	//	FIL file;
	//
	//	FRESULT res = f_open(&file, fileName, FA_WRITE | FA_CREATE_ALWAYS);
	//	if(res != FR_OK)
	//	{
	//		throw "Cannot open storage gps data file";
	//	}
	//
	//	char data[] = "wifiSsid=\nwifiPassword=\n";
	//	UINT bytesWritten;
	//
	//	res = f_write(&file, data, strlen(data), &bytesWritten);
	//	if((bytesWritten == 0) || (res != FR_OK))
	//	{
	//		throw "Cannot write to recordSyncLogFile";
	//	}
	//	res = f_close(&file);
	//	if(res != FR_OK)
	//	{
	//		throw "Cannot close config file";
	//	}
}



