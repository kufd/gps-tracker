#include "storage.h"
#include <string.h>
#include "fatfs.h"

void Storage::refreshStorageStatus()
{
	DWORD freeClusters;
	FATFS* fatFsPointer = &SDFatFS;

	/* Get volume information and free clusters of drive 1 */
	FRESULT res = f_getfree((TCHAR const*)SDPath, &freeClusters, &fatFsPointer);
	if(res != FR_OK)
	{
		throw "Cannot get information about storage usage";
	}

	/* Get total sectors and free sectors */
	uint64_t totalSectors = (SDFatFS.n_fatent - 2) * SDFatFS.csize;
	uint64_t freeSectors = freeClusters * SDFatFS.csize;

	totalMBytes = totalSectors * SDFatFS.ssize/(1024*1024);
	freeMBytes = freeSectors * SDFatFS.ssize/(1024*1024);
	usedMBytes = totalMBytes - freeMBytes;
}

uint32_t Storage::getTotalMBytes()
{
	return totalMBytes;
}

uint32_t Storage::getFreeMBytes()
{
	return freeMBytes;
}

uint32_t Storage::getUsedMBytes()
{
	return usedMBytes;
}

uint32_t Storage::getTotalGBytes()
{
	return totalMBytes/1024;
}

uint32_t Storage::getFreeGBytes()
{
	return freeMBytes/1024;
}

uint32_t Storage::getUsedGBytes()
{
	return usedMBytes/1024;
}

void  Storage::open()
{
	FRESULT res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
	if(res != FR_OK)
	{
		throw "Cannot mount storage file system";
	}

//TODO add option late
//	if(f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, rtext, sizeof(rtext)) != FR_OK)
//	{
//		throw "Cannot make storage file system";
//	}


	res = f_open(&dataFile, dataFileName, FA_OPEN_APPEND | FA_WRITE);
	if(res != FR_OK)
	{
		throw "Cannot open storage data file";
	}

	res = f_open(&logFile, logFileName, FA_OPEN_APPEND | FA_WRITE);
	if(res != FR_OK)
	{
		throw "Cannot open storage log file";
	}

	refreshStorageStatus();
}

void  Storage::close()
{
	FRESULT res = f_close(&dataFile);
	if(res != FR_OK)
	{
		throw "Cannot close storage data file";
	}

	res = f_close(&logFile);
	if(res != FR_OK)
	{
		throw "Cannot close storage log file";
	}

	res = f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
	if(res != FR_OK)
	{
		throw "Cannot unmount storage file system";
	}
}

void Storage::writeData(const char* data)
{
	uint32_t bytesWritten;
	FRESULT res = f_write(&dataFile, data, strlen(data), (UINT *)&bytesWritten);

	if((bytesWritten == 0) || (res != FR_OK))
	{
		throw "Cannot write data to storage";
	}

	if(++writeDataCounter % 10 == 0)
	{
		res = f_sync(&dataFile);
		if(res != FR_OK)
		{
			throw "Cannot sync storage file to SD Card";
		}

		refreshStorageStatus();
	}
}

void Storage::writeLog(const char* data)
{
	uint32_t bytesWritten;
	FRESULT res = f_write(&logFile, data, strlen(data), (UINT *)&bytesWritten);

	if((bytesWritten == 0) || (res != FR_OK))
	{
		throw "Cannot write log to storage";
	}

	if(++writeLogCounter % 10 == 0)
	{
		res = f_sync(&logFile);
		if(res != FR_OK)
		{
			throw "Cannot sync log file to SD Card";
		}

		refreshStorageStatus();
	}
}

