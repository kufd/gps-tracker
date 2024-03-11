#include "storage.h"
#include <string.h>
#include <stdio.h>
#include <functional>

class RecordSynchronizer
{
	private:
		constexpr static uint16_t syncBufferFlushThreshold = 1024;
		constexpr static uint16_t syncBufferSize = syncBufferFlushThreshold+200;

		const char* recordName;
		char recordFileName[Storage::maxFileNameLen+1];
		char recordSyncLogFileName[Storage::maxFileNameLen+1];
		FIL recordFile;
		FIL recordSyncLogFile;

		uint32_t syncTotalBytes = 0;
		uint32_t syncProgressBytes = 0;
		uint16_t syncProgressPackets = 0;

		char syncBuffer[syncBufferSize+1]; //0 terminated string
		uint16_t syncBufferUsedSize = 0;

		std::function<void(const char* packet, uint32_t totalBytes, uint32_t progressBytes)> sendPacketToRemoteFunc;

		bool isFinishedSuccessfully = false;
	public:
		RecordSynchronizer(const char* recordName, std::function<void(const char* packet, uint32_t totalBytes, uint32_t progressBytes)> sendPacketToRemoteFunc)
		{
			this->recordName = recordName;
			this->sendPacketToRemoteFunc = sendPacketToRemoteFunc;

			sprintf(recordFileName, "rec.%s.csv", recordName);
			sprintf(recordSyncLogFileName, "rec.%s.sync", recordName);

			FRESULT res = f_open(&recordFile, recordFileName, FA_OPEN_EXISTING | FA_READ);
			if(res != FR_OK)
			{
				throw "Cannot open record file for synchronization";
			}

			res = f_open(&recordSyncLogFile, recordSyncLogFileName, FA_OPEN_APPEND | FA_WRITE | FA_READ);
			if(res != FR_OK)
			{
				throw "Cannot open record sync log file for synchronization";
			}

			syncTotalBytes = f_size(&recordFile);

			readSyncProgressFromSyncLog();
		}
		~RecordSynchronizer()
		{
			//close and remove both files after sync
			FRESULT res = f_close(&recordFile);
			if(res != FR_OK)
			{
				throw "Cannot close recordFile";
			}

			res = f_close(&recordSyncLogFile);
			if(res != FR_OK)
			{
				throw "Cannot close recordSyncLogFile";
			}

			if (isFinishedSuccessfully)
			{
				char filenameToDelete[50];

				sprintf(filenameToDelete, "/%s", recordFileName);
				res = f_unlink(filenameToDelete);
				if(res != FR_OK)
				{
					throw "Cannot delete record file";
				}

				sprintf(filenameToDelete, "/%s", recordSyncLogFileName);
				res = f_unlink(filenameToDelete);
				if(res != FR_OK)
				{
					throw "Cannot delete record sync log file";
				}
			}

			//ADD LOGS
		}
		void syncRecord()
		{
			FRESULT res = f_lseek(&recordFile, syncProgressBytes);
			if(res != FR_OK)
			{
				throw "Cannot move recordFile read/write pointer";
			}

			for (;;)
			{
				UINT readBufferSize = 1024;
				char readBuffer[readBufferSize];
				UINT readBufferUsedSize;

				res = f_read(&recordFile, readBuffer, readBufferSize, &readBufferUsedSize);
				if(res != FR_OK)
				{
					throw "Cannot read record file";
				}

				if (readBufferUsedSize == 0)
				{
					break;
				}

				//TODO handle overflow buffers
				//build packet
				for (UINT i=0; i<readBufferUsedSize; i++)
				{
					syncBuffer[syncBufferUsedSize] = readBuffer[i];
					syncBufferUsedSize++;
					syncBuffer[syncBufferUsedSize] = '\0'; //ho have valid null terminated string

					if (syncBufferUsedSize >= syncBufferFlushThreshold && readBuffer[i] == '\n')
					{
						sendRecordPackage(syncBuffer);
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

			isFinishedSuccessfully = true;
		}
	private:
		void readSyncProgressFromSyncLog()
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

			recordSyncLogFileReadBuffer[recordSyncLogFileReadBufferActualSize] = '\0'; //TODO need???

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
		void sendRecordPackage(const char* syncBuffer)
		{
			syncProgressPackets++;
			syncProgressBytes += strlen(syncBuffer);
			syncBufferUsedSize = 0;

			char syncPacket[strlen(syncBuffer)+100];
			sprintf(syncPacket, "%s,%d,%ld,%ld\n%s", recordName, syncProgressPackets, syncProgressBytes, syncTotalBytes, syncBuffer);

			//send packet
			sendPacketToRemoteFunc(syncPacket, syncTotalBytes, syncProgressBytes);

			//write sync log file
			saveSyncProgress();
		}
		void saveSyncProgress()
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
};

void  Storage::openGpsDateStorage(const char* recordName)
{
	if (strlen(gpsDataFileName) > maxRecordNameLen)
	{
		throw "Too long recordName";
	}

	sprintf(this->gpsDataFileName, "rec.%s.csv", recordName);

	FRESULT res = f_open(&this->gpsDataFile, this->gpsDataFileName, FA_OPEN_APPEND | FA_WRITE);
	if(res != FR_OK)
	{
		throw "Cannot open storage gps data file";
	}
}

void  Storage::closeGpsDateStorage()
{
	strcpy(gpsDataFileName, "");
	FRESULT res = f_close(&gpsDataFile);
	if(res != FR_OK)
	{
		throw "Cannot close storage gps data file";
	}
}

void Storage::writeGpsData(const char* data)
{
	uint32_t bytesWritten;
	FRESULT res = f_write(&gpsDataFile, data, strlen(data), (UINT *)&bytesWritten);

	if((bytesWritten == 0) || (res != FR_OK))
	{
		throw "Cannot write data to storage";
	}

	if(++writeGpsDataCounter % 10 == 0)
	{
		res = f_sync(&gpsDataFile);
		if(res != FR_OK)
		{
			throw "Cannot sync storage file to SD Card";
		}
	}
}

void Storage::syncRecord(const char* recordName, std::function<void(const char* packet, uint32_t totalBytes, uint32_t progressBytes)> sendPacketToRemoteFunc)
{
	RecordSynchronizer recordSynchronizer(recordName, sendPacketToRemoteFunc);

	recordSynchronizer.syncRecord();
}


//find the first record
bool Storage::findRecordToSync(char* recordName)
{
	FRESULT res;
	DIR dir;
	static FILINFO fileinfo;
	bool result = false;

	res = f_opendir(&dir, "/"); /* Open the directory */
	if (res != FR_OK) {
		return result;
	}

	for (;;) {
		res = f_readdir(&dir, &fileinfo); /* Read a directory item */

		if (res != FR_OK || fileinfo.fname[0] == 0) /* Break on error or end of dir */
		{
			break;
		}

		if (!(fileinfo.fattrib & AM_DIR)) /* It is file */
		{
			uint8_t fileNameLength = strlen(fileinfo.fname);
			if (
				fileNameLength > 8
				&& fileinfo.fname[0] == 'r' && fileinfo.fname[1] == 'e' && fileinfo.fname[2] == 'c' && fileinfo.fname[3] == '.'
				&& fileinfo.fname[fileNameLength-4] == '.' && fileinfo.fname[fileNameLength-3] == 'c' && fileinfo.fname[fileNameLength-2] == 's' && fileinfo.fname[fileNameLength-1] == 'v'
			)
			{
				strncpy(recordName, (fileinfo.fname)+4, fileNameLength - 8);
				recordName[fileNameLength - 8] = '\0';
				result = true;
				break;
			}

//				char filenameToDelete[50];
//				sprintf(filenameToDelete, "/%s", fileName);
//				f_unlink(filenameToDelete);

		}
	}
	f_closedir(&dir);

	return result;
}
