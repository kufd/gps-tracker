#ifndef __GPS_RECORD_H
#define __GPS_RECORD_H

#include "main.h"
#include "fatfs.h"
#include "common.h"
#include <functional>
#include "gps_parser.h"

class GpsRecord
{
	private:
		char fileName[MAX_FILE_NAME_LEN+1];
		char recordName[MAX_GPS_RECORD_NAME_LEN+1];
		FIL file;
		uint8_t writeGpsDataCounter = 0;
		bool isRemoved = false;
	public:
		GpsRecord(const char* recordName);
		~GpsRecord();
		void write(GpsData &gpsData);
		uint32_t read(char *data, uint32_t dataSize, uint32_t offset);
		void remove();
		uint32_t size();

		static bool findRecordToSync(char* recordName);

	private:
		void closeFile();
};

#endif /* __STORAGE_H */
