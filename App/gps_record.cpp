#include "gps_record.h"
#include <string.h>
#include <stdio.h>
#include <functional>

GpsRecord::GpsRecord(const char* recordName)
{
	if (strlen(recordName) > MAX_GPS_RECORD_NAME_LEN)
	{
		throw "Too long recordName";
	}

	strcpy(this->recordName, recordName);
	sprintf(this->fileName, "rec.%s.csv", recordName);

	FRESULT res = f_open(&this->file, this->fileName, FA_OPEN_APPEND | FA_WRITE);
	if(res != FR_OK)
	{
		throw "Cannot open gps record file";
	}
}

GpsRecord::~GpsRecord()
{
	if (isRemoved)
	{
		return;
	}

	closeFile();
}

void GpsRecord::write(GpsData &gpsData)
{
	if (isRemoved)
	{
		throw "record is removed";
	}

	uint32_t bytesWritten = f_printf(
		&file,
		"%s,%s,%f,%c,%f,%c\n",
		gpsData.dateUtc,
		gpsData.timeUtc,
		gpsData.latitude,
		gpsData.latitudeNorS,
		gpsData.longitude,
		gpsData.longitudeEorW
	);

	if(bytesWritten < 0)
	{
		throw "Cannot write data to gps record file";
	}

	if(++writeGpsDataCounter % 10 == 0)
	{
		FRESULT res = f_sync(&file);
		if(res != FR_OK)
		{
			throw "Cannot sync gps record file";
		}
	}
}

uint32_t GpsRecord::read(char *data, uint32_t dataSize, uint32_t offset)
{
	if (isRemoved)
	{
		throw "record is removed";
	}

	FRESULT res = f_lseek(&file, offset);
	if(res != FR_OK)
	{
		throw "Cannot move gps record file read/write pointer";
	}

	uint32_t readDataSize;
	res = f_read(&file, data, dataSize, (UINT*) &readDataSize);
	if(res != FR_OK)
	{
		throw "Cannot read gps record file";
	}

	return readDataSize;
}

void GpsRecord::remove()
{
	if (isRemoved)
	{
		throw "record is removed";
	}

	closeFile();

	char filenameToDelete[MAX_FILE_NAME_LEN+5];
	sprintf(filenameToDelete, "/%s", fileName);

	FRESULT res = f_unlink(filenameToDelete);
	if(res != FR_OK)
	{
		throw "Cannot delete record file";
	}

	isRemoved = true;
}

uint32_t GpsRecord::size()
{
	return f_size(&file);
}

void GpsRecord::closeFile()
{
	FRESULT res = f_close(&file);
	if(res != FR_OK)
	{
		throw "Cannot close gps record file";
	}
}


//find the first record
bool GpsRecord::findRecordToSync(char* recordName)
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
		}
	}
	f_closedir(&dir);

	return result;
}
