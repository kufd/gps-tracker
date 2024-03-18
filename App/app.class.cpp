#include "app.class.h"
#include "gps_record_synchronizer.class.h"
#include "ui.events.h"
#include "ui.sync_gps.screen.class.h"
#include "ui.menu.screen.class.h"

App::App(UART_HandleTypeDef* huartWifi, UiEventDispatcher *uiEventDispatcher)
{
	this->huartWifi = huartWifi;
	this->uiEventDispatcher = uiEventDispatcher;
}
App::~App()
{
	stop();
}
void App::start()
{
	mountFileSystem();
	logger.openFile();
	config.read();
}
void App::stop()
{
	unmountFileSystem();
	logger.closeFile();
}
void App::mountFileSystem()
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
}
void App::unmountFileSystem()
{
	FRESULT res = f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
	if(res != FR_OK)
	{
		throw "Cannot unmount storage file system";
	}
}
void App::onGpsDataChange(GpsData &gpsData)
{
	if (!gpsDataRecordingStarted)
	{
		return;
	}

	if (gpsRecord == NULL)
	{
		char recordName[20];
		char time[7] = {gpsData.timeUtc[0], gpsData.timeUtc[1], gpsData.timeUtc[3], gpsData.timeUtc[4], gpsData.timeUtc[6], gpsData.timeUtc[7], '\0'};
		sprintf(recordName, "%sT%s", gpsData.dateUtc, time);
		gpsRecord = new GpsRecord(recordName);
	}


	gpsRecord->write(gpsData);

	GpsDataChangedUiEvent gpsDataChangedUiEvent(gpsData);
	this->uiEventDispatcher->dispatch(gpsDataChangedUiEvent);
}
bool App::isGpsDataRecordingStarted()
{
	return gpsDataRecordingStarted;
}
void App::startGpsDataRecording()
{
	gpsDataRecordingStarted = true;
}
void App::stopGpsDataRecording()
{
	gpsDataRecordingStarted = false;

	if (gpsRecord != NULL)
	{
		delete gpsRecord;
	}
}
void App::syncGpsRecords()
{
	Wifi wifi;
	this->wifi = &wifi;
//	wifi.init(huartWifi, config.getWifiSsid(), config.getWifiPassword());

	while(true)
	{
		char recordName[21];
		if (!GpsRecord::findRecordToSync(recordName))
		{
			break;
		}

		SyncGpsRecordStartedUiEvent syncGpsRecordStartedUiEvent(recordName);
		uiEventDispatcher->dispatch(syncGpsRecordStartedUiEvent);

		UiEventDispatcher *uiEventDispatcherClosureParam = uiEventDispatcher;

		GpsRecordSynchronizer gpsRecordSynchronizer(
			recordName,
			[&wifi, &uiEventDispatcherClosureParam](const char* packet, uint32_t totalBytes, uint32_t progressBytes){
				//TODO maybe rename method
				wifi.sendPost(packet);

				SyncGpsRecordProgressChangedUiEvent syncGpsRecordProgressChangedUiEvent((uint8_t)(100*(double)progressBytes/(double)totalBytes));
				uiEventDispatcherClosureParam->dispatch(syncGpsRecordProgressChangedUiEvent);
			}
		);
		gpsRecordSynchronizer.syncRecord();
	}

	this->wifi = NULL;
}

void App::onReceivedDataFromWifi(const char* data, size_t size)
{
	if (wifi == NULL)
	{
		return;
	}

	wifi->onReceivedDataFromHuart(data, size);
}

void App::onCircularBufferDataReceived(const char *name, const char *data, uint16_t dataSize)
{
	if (strcmp(name, "wifi") == 0)
	{
		onReceivedDataFromWifi(data, dataSize);
	}
}

SDCardStatus App::getSDCardStatus()
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

	uint64_t totalMBytes = totalSectors * SDFatFS.ssize/(1024*1024);
	uint64_t freeMBytes = freeSectors * SDFatFS.ssize/(1024*1024);
	uint64_t usedMBytes = totalMBytes - freeMBytes;

	SDCardStatus sdCardStatus(totalMBytes, freeMBytes, usedMBytes);

	return sdCardStatus;
}

void App::onUiEvent(UiEvent &event)
{
	if (event.matchName(UiEventName::StopGpsDataRecordingUiEventName))
	{
		stopGpsDataRecording();
	}

	if (event.matchName(UiEventName::StartGpsDataRecordingUiEventName))
	{
		startGpsDataRecording();
	}

	if (event.matchName(UiEventName::RefreshSDCardStatusUiEventName))
	{
		SDCardStatus sdCardStatus = getSDCardStatus();
		SDCardStatusUiEvent sdCardStatusUiEvent(sdCardStatus);
		uiEventDispatcher->dispatch(sdCardStatusUiEvent);
	}
}
