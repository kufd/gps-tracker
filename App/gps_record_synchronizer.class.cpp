#include "gps_record_synchronizer.class.h"
#include "logger.h"
#include "gps_record.h"
#include <string.h>
#include <stdio.h>

GpsRecordSynchronizer::GpsRecordSynchronizer(
	const char* recordName,
	std::function<void(const char* packet, uint32_t totalBytes, uint32_t progressBytes)> sendPacketToRemoteFunc
)
{
	if (strlen(recordName) > MAX_GPS_RECORD_NAME_LEN)
	{
		throw "too long gps record name";
	}

	strcpy(this->recordName, recordName);
	this->sendPacketToRemoteFunc = sendPacketToRemoteFunc;

	sprintf(recordSyncLogFileName, "rec.%s.sync", recordName);

	FRESULT res = f_open(&recordSyncLogFile, recordSyncLogFileName, FA_OPEN_APPEND | FA_WRITE | FA_READ);
	if(res != FR_OK)
	{
		throw "Cannot open record sync log file for synchronization";
	}

	readSyncProgressFromSyncLog();
}

GpsRecordSynchronizer::~GpsRecordSynchronizer()
{
	closeFile();
}

void GpsRecordSynchronizer::syncRecord()
{
	GpsRecord gpsRecord(recordName);

	syncTotalBytes = gpsRecord.size();

	for (;;)
	{
		UINT readBufferSize = 1024;
		char readBuffer[readBufferSize];
		UINT readBufferUsedSize = gpsRecord.read(readBuffer, readBufferSize, syncProgressBytes);

		if (readBufferUsedSize == 0)
		{
			break;
		}

		//build packet
		for (UINT i=0; i<readBufferUsedSize; i++)
		{
			syncBuffer[syncBufferUsedSize] = readBuffer[i];
			syncBufferUsedSize++;
			syncBuffer[syncBufferUsedSize] = '\0'; //to have valid null terminated string

			if (syncBufferUsedSize >= syncBufferFlushThreshold && readBuffer[i] == '\n')
			{
				sendRecordPackage(syncBuffer);
			}

			if (syncBufferUsedSize >= syncBufferSize)
			{
				throw "GpsRecordSynchronizer: syncBuffer overflow";
			}
		}

		if (readBufferUsedSize < readBufferSize)
		{
			if (syncBufferUsedSize > 0 && syncBuffer[syncBufferUsedSize-1] != '\n')
			{
				throw "End of record file but sync buffer not complete";
			}

			if (syncBufferUsedSize > 0)
			{
				sendRecordPackage(syncBuffer);
			}

			break;
		}
	}

	gpsRecord.remove();

	closeFile();

	char filenameToDelete[MAX_FILE_NAME_LEN+5];
	sprintf(filenameToDelete, "/%s", recordSyncLogFileName);
	FRESULT res = f_unlink(filenameToDelete);
	if(res != FR_OK)
	{
		throw "Cannot delete record sync log file";
	}
}

void GpsRecordSynchronizer::closeFile()
{
	if (isFileClosed)
	{
		return;
	}

	FRESULT res = f_close(&recordSyncLogFile);
	if(res != FR_OK)
	{
		throw "Cannot close recordSyncLogFile";
	}
	isFileClosed = true;
}

void GpsRecordSynchronizer::readSyncProgressFromSyncLog()
{
	FSIZE_t recordSyncLogFileSize = f_size(&recordSyncLogFile);
	if (recordSyncLogFileSize == 0)
	{
		return;
	}

	UINT recordSyncLogFileReadBufferSize = 30;
	char recordSyncLogFileReadBuffer[recordSyncLogFileReadBufferSize+1];
	UINT recordSyncLogFileReadBufferActualSize;
	char storedSyncProgressBytes[21];
	char storedSyncProgressPackets[11];

	FSIZE_t recordSyncLogFileReadPointer = 0;
	if (recordSyncLogFileSize > recordSyncLogFileReadBufferSize)
	{
		recordSyncLogFileReadPointer = recordSyncLogFileSize - recordSyncLogFileReadBufferSize;
	}

	//read from the end with a margin. We do not know exact size of a line
	FRESULT res = f_lseek(&recordSyncLogFile, recordSyncLogFileReadPointer);
	if(res != FR_OK)
	{
		throw "Cannot move file read/write pointer";
	}

	res = f_read(&recordSyncLogFile, recordSyncLogFileReadBuffer, recordSyncLogFileReadBufferSize, &recordSyncLogFileReadBufferActualSize);
	if(res != FR_OK)
	{
		throw "Cannot read recordSyncLogFile";
	}

	recordSyncLogFileReadBuffer[recordSyncLogFileReadBufferActualSize] = '\0';

	char* logLineStart = recordSyncLogFileReadBuffer;
	const char* lastNewLineChar = strrchr(recordSyncLogFileReadBuffer, '\n');
	if (lastNewLineChar == NULL)
	{
		throw "Corrupted recordSyncLogFile";
	}

	for (;;)
	{
		char* firstNewLineChar = strchr(logLineStart, '\n');
		if (firstNewLineChar == NULL)
		{
			throw "Corrupted recordSyncLogFile";
		}

		if (lastNewLineChar == firstNewLineChar)
		{
			break;
		}

		logLineStart = firstNewLineChar+1;

		if (logLineStart > lastNewLineChar)
		{
			throw "Corrupted recordSyncLogFile";
		}
	}

	char* comma1 = strrchr(logLineStart, ',');
	if (comma1 == NULL)
	{
		throw "Corrupted recordSyncLogFile";
	}

	strncpy(storedSyncProgressPackets, logLineStart, comma1-logLineStart);
	strncpy(storedSyncProgressBytes, comma1+1, lastNewLineChar-comma1-1);

	syncProgressBytes = atoi(storedSyncProgressBytes);
	syncProgressPackets = atoi(storedSyncProgressPackets);

	res = f_lseek(&recordSyncLogFile, recordSyncLogFileSize);
	if(res != FR_OK)
	{
		throw "Cannot move recordSyncLogFile read/write pointer";
	}
}

void GpsRecordSynchronizer::sendRecordPackage(const char* syncBuffer)
{
	syncProgressPackets++;
	syncProgressBytes += strlen(syncBuffer);
	syncBufferUsedSize = 0;

	char syncPacket[strlen(syncBuffer)+100];
	sprintf(syncPacket, "%s,%d,%ld,%ld\n%s", recordName, syncProgressPackets, syncProgressBytes, syncTotalBytes, syncBuffer);

	sendPacketToRemoteFunc(syncPacket, syncTotalBytes, syncProgressBytes);

	saveSyncProgress();
}

void GpsRecordSynchronizer::saveSyncProgress()
{
	uint32_t bytesWritten;
	char syncLogRecord[50];
	sprintf(syncLogRecord, "%d,%ld\n", syncProgressPackets, syncProgressBytes);

	FRESULT res = f_write(&recordSyncLogFile, syncLogRecord, strlen(syncLogRecord), (UINT *)&bytesWritten);
	if((bytesWritten == 0) || (res != FR_OK))
	{
		throw "Cannot write to recordSyncLogFile";
	}

	res = f_sync(&recordSyncLogFile);
	if(res != FR_OK)
	{
		throw "Cannot sync recordSyncLogFile";
	}
}
