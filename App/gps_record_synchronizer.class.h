#ifndef __GPS_RECORD_SYNCHRONIZER_CLASS_H
#define __GPS_RECORD_SYNCHRONIZER_CLASS_H

#include "common.h"
#include "fatfs.h"
#include <functional>

class GpsRecordSynchronizer
{
	private:
		char recordName[MAX_GPS_RECORD_NAME_LEN+1];
		char recordSyncLogFileName[MAX_FILE_NAME_LEN+1];
		FIL recordSyncLogFile;

		uint32_t syncTotalBytes = 0;
		uint32_t syncProgressBytes = 0;
		uint16_t syncProgressPackets = 0;

		constexpr static uint16_t syncBufferFlushThreshold = 1024;
		constexpr static uint16_t syncBufferSize = syncBufferFlushThreshold+200;
		uint16_t syncBufferUsedSize = 0;
		char syncBuffer[syncBufferSize+1]; //0 terminated string

		std::function<void(const char* packet, uint32_t totalBytes, uint32_t progressBytes)> sendPacketToRemoteFunc;

		bool isFileClosed = false;
	public:
		GpsRecordSynchronizer(const char* recordName, std::function<void(const char* packet, uint32_t totalBytes, uint32_t progressBytes)> sendPacketToRemoteFunc);
		~GpsRecordSynchronizer();
		void syncRecord();
	private:
		void readSyncProgressFromSyncLog();
		void sendRecordPackage(const char* syncBuffer);
		void saveSyncProgress();
		void closeFile();
};

#endif
