#ifndef __UI_EVENTS_H
#define __UI_EVENTS_H

#include "common.h"
#include <string.h>
#include "gps_parser.h"
#include "sdcard_status.h"

enum UiEventName
{
	SyncGpsRecordStartedUiEventName,
	SyncGpsRecordProgressChangedUiEventName,
	GpsDataChangedUiEventName,
	GpsStatusUpdateUiEventName,
	ShowScreenUiEventName,
	StopGpsDataRecordingUiEventName,
	StartGpsDataRecordingUiEventName,
	StartSyncGpsRecordsUiEventName,
	RefreshSDCardStatusUiEventName,
	SDCardStatusUiEventName
};

class UiEvent
{
	protected:
		UiEventName name;
	public:
		bool matchName(UiEventName name)
		{
			return this->name == name;
		}
};

class SyncGpsRecordStartedUiEvent: public UiEvent
{
	private:
		char recordName[MAX_GPS_RECORD_NAME_LEN+1];
	public:
		SyncGpsRecordStartedUiEvent(const char *recordName)
		{
			this->name = UiEventName::SyncGpsRecordStartedUiEventName;

			if (strlen(recordName) > MAX_GPS_RECORD_NAME_LEN)
			{
				throw "too long recordName";
			}

			strcpy(this->recordName, recordName);
		}
		const char *getRecordName()
		{
			return recordName;
		}
};

class SyncGpsRecordProgressChangedUiEvent: public UiEvent
{
	private:
		uint8_t progress;
	public:
		SyncGpsRecordProgressChangedUiEvent(uint8_t progress)
		{
			this->name = UiEventName::SyncGpsRecordProgressChangedUiEventName;
			this->progress = progress;
		}
		uint8_t getProgress()
		{
			return progress;
		}
};

class GpsDataChangedUiEvent: public UiEvent
{
	private:
		GpsData gpsData;
	public:
		GpsDataChangedUiEvent(GpsData &gpsData)
		{
			this->name = UiEventName::GpsDataChangedUiEventName;
			this->gpsData = gpsData;
		}
		GpsData& getGpsData()
		{
			return gpsData;
		}
};

class GpsStatusUpdateUiEvent: public UiEvent
{
	private:
		GpsStatus gpsStatus;
	public:
		GpsStatusUpdateUiEvent(GpsStatus gpsStatus)
		{
			this->name = UiEventName::GpsStatusUpdateUiEventName;
			this->gpsStatus = gpsStatus;
		}
		GpsStatus getGpsStatus()
		{
			return gpsStatus;
		}
};

class ShowScreenUiEvent: public UiEvent
{
	private:
		const static uint8_t screenNameMaxLen = 30;
		char screenName[screenNameMaxLen+1];
	public:
		ShowScreenUiEvent(const char *screenName)
		{
			this->name = UiEventName::ShowScreenUiEventName;

			if (strlen(screenName) > screenNameMaxLen)
			{
				throw "too long screenName";
			}

			strcpy(this->screenName, screenName);
		}
		const char *getScreenName()
		{
			return screenName;
		}
};


class StopGpsDataRecordingUiEvent: public UiEvent
{
	public:
		StopGpsDataRecordingUiEvent()
		{
			this->name = UiEventName::StopGpsDataRecordingUiEventName;
		}
};

class StartGpsDataRecordingUiEvent: public UiEvent
{
	public:
		StartGpsDataRecordingUiEvent()
		{
			this->name = UiEventName::StartGpsDataRecordingUiEventName;
		}
};

class StartSyncGpsRecordsUiEvent: public UiEvent
{
	public:
		StartSyncGpsRecordsUiEvent()
		{
			this->name = UiEventName::StartSyncGpsRecordsUiEventName;
		}
};

class RefreshSDCardStatusUiEvent: public UiEvent
{
	public:
		RefreshSDCardStatusUiEvent()
		{
			this->name = UiEventName::RefreshSDCardStatusUiEventName;
		}
};

class SDCardStatusUiEvent: public UiEvent
{
	private:
		SDCardStatus* sdCardStatus;
	public:
		SDCardStatusUiEvent(SDCardStatus &sdCardStatus)
		{
			this->name = UiEventName::SDCardStatusUiEventName;
			this->sdCardStatus = &sdCardStatus;
		}
		SDCardStatus* getSDCardStatus()
		{
			return sdCardStatus;
		}
};

#endif

