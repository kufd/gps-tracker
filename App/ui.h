#ifndef __UI_H
#define __UI_H

#include "main.h"
#include "LcdDriver/LCD_2inch.h"
#include "LcdDriver/DEV_Config.h"
#include "LcdDriver/GUI_Paint.h"
#include "LcdDriver/fonts.h"
#include "gps_parser.h"
#include "storage.h"

class UI: public GpsDataChangeListener
{
	private:
		uint8_t searchGpsProgressCounter = 0;
	public:
		void init();
		void printError(const char* error);
		void onGpsDataChange(GpsParser*);
		void refreshSearchGpsIndicator(GpsParser* gpsParser);
		void refreshStorageData(Storage* storage);
		void refreshGpsDataRecordingEnabledIndicator(bool gpsDataRecordingEnabled);
	private:
		void printGpsData(GpsParser* gpsParser);
};

#endif /* __UI_H */
