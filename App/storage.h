#ifndef __STORAGE_H
#define __STORAGE_H

#include "main.h"
#include "fatfs.h"

class Storage
{
	private:
		char dataFileName[9] = "data.csv";
		char logFileName[8] = "app.log";
		FIL dataFile;
		FIL logFile;
		uint8_t writeDataCounter = 0;
		uint8_t writeLogCounter = 0;
		uint64_t totalMBytes = 0;
		uint64_t freeMBytes = 0;
		uint64_t usedMBytes = 0;

		void refreshStorageStatus();
	public:
		void open();
		void close();
		void writeData(const char* data);
		void writeLog(const char* data);
		uint32_t getTotalMBytes();
		uint32_t getFreeMBytes();
		uint32_t getUsedMBytes();
		uint32_t getTotalGBytes();
		uint32_t getFreeGBytes();
		uint32_t getUsedGBytes();
};

#endif /* __STORAGE_H */
