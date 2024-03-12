#ifndef __APP_CLASS_H
#define __APP_CLASS_H

#include "gps_record.h"
#include "logger.h"
#include "app.h"
#include "gps_parser.h"
#include "wifi.h"
#include <string.h>
#include <stdexcept>
#include <exception>
#include <functional>
#include "sdcard_status.h"
#include "fatfs.h"
#include "circular_buffer.class.h"
#include "config.class.h"

class App: public GpsDataChangeListener, public CircularBufferSubscriber
{
	private:
		GpsRecord *gpsRecord = NULL;
		bool gpsDataRecordingStarted = false;
		UART_HandleTypeDef* huartWifi;
		Wifi *wifi = NULL;
		Config config;
	public:
		App(UART_HandleTypeDef* huartWifi);
		~App();
		void start();
		void stop();
		void mountFileSystem();
		void unmountFileSystem();
		void onGpsDataChange(GpsData &gpsData);
		bool isGpsDataRecordingStarted();
		void startGpsDataRecording();
		void stopGpsDataRecording();
		void syncGpsRecords(
			std::function<void(const char* recordName)> onRecordChanged,
			std::function<void(uint8_t progressPercent)> onProgressChanged,
			std::function<void()> onSyncCompleted
		);
		void onReceivedDataFromWifi(const char* data, size_t size);
		void onCircularBufferDataReceived(const char *name, const char *data, uint16_t dataSize);
		SDCardStatus getSDCardStatus();
};

#endif /* __APP_CLASS_H */
