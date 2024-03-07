#ifndef __STORAGE_H
#define __STORAGE_H

#include "main.h"
#include <functional>
#include "fatfs.h"

class Storage
{
	public:
		constexpr static uint8_t maxFileNameLen = 30;
		constexpr static uint8_t maxRecordNameLen = 20;
	private:
		char gpsDataFileName[maxFileNameLen+1] = "";
		FIL gpsDataFile;
		uint8_t writeGpsDataCounter = 0;
	public:
		void writeGpsData(const char* data);
		void openGpsDateStorage(const char* gpsDataFileName);
		void closeGpsDateStorage();
		bool findRecordToSync(char* recordName);
		void syncRecord(const char* recordName, std::function<void(const char* packet, uint32_t totalBytes, uint32_t progressBytes)> sendPacketToRemoteFunc);
};

#endif /* __STORAGE_H */
