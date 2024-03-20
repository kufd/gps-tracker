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
#include "ui.events.dispatcher.h"

class App: public GpsDataChangeListener, public CircularBufferSubscriber, public UiEventsListener
{
	private:
		GpsRecord *gpsRecord = NULL;
		bool gpsDataRecordingStarted = false;
		UART_HandleTypeDef* huartWifi;
		Wifi *wifi = NULL;
		Config config;
		UiEventDispatcher *uiEventDispatcher;
		GpsParser gpsParser;
	public:
		App(UART_HandleTypeDef* huartWifi, UiEventDispatcher *uiEventDispatcher);
		~App();
		void start();
		void stop();
		void mountFileSystem();
		void unmountFileSystem();
		void onGpsDataChange(GpsData &gpsData);
		bool isGpsDataRecordingStarted();
		void startGpsDataRecording();
		void stopGpsDataRecording();
		void syncGpsRecords();
		void onReceivedDataFromWifi(const char* data, size_t size);
		void onCircularBufferDataReceived(const char *name, const char *data, uint16_t dataSize);
		void onUiEvent(UiEvent &event);
	private:
		SDCardStatus getSDCardStatus();
};

#endif /* __APP_CLASS_H */
