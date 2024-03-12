#ifndef __UI_H
#define __UI_H

#include "main.h"
#include "LcdDriver/LCD_2inch.h"
#include "LcdDriver/DEV_Config.h"
#include "LcdDriver/GUI_Paint.h"
#include "LcdDriver/fonts.h"
#include "gps_parser.h"
#include "sdcard_status.h"
#include <functional>
#include "app.class.h"

class UI: public GpsDataChangeListener
{
	private:
		const uint8_t SCREEN_MENU = 0;
		const uint8_t SCREEN_SYNC_GPS = 1;
		const uint8_t SCREEN_RECORD_GPS = 2;
		const uint8_t SCREEN_SYNC_GPS_FINISH = 3;
		const uint8_t SCREEN_INFO = 4;

		uint8_t searchGpsProgressCounter = 0;

		uint8_t screens[5] = {SCREEN_MENU, SCREEN_SYNC_GPS, SCREEN_RECORD_GPS, SCREEN_SYNC_GPS_FINISH, SCREEN_INFO};
		uint8_t screensNumber = 5;
		uint8_t activeScreenNumber = 0;

		char menuItems[4][18] = {"Record GPS", "Sync GPS", "Record env", "Info"};
		uint8_t menuItemsNumber = 4;
		uint8_t activeMenuItemNumber = 0;

		uint8_t exitScreenPushButtonCounter = 0;

		App* app;
	public:
		UI(App* app);
		void start();
		void printError(const char* error);
		void onGpsDataChange(GpsData &gpsData);
		void refreshGpsStatus(GpsStatus gpsStatus);
		void showGpsRecordingScreen();
		void showMenuScreen();
		void moveButtonPressed();
		void selectButtonPressed();

		void showSyncGpsRecordsScreen();
		void syncGpsRecordsScreenRefreshRecordName(const char* recordName);
		void syncGpsRecordsScreenRefreshProgress(uint8_t progress);
		void showSyncGpsRecordsScreenFinishScreen();
	private:
		void showMenuItem(uint8_t menuItemNumber);
		void refreshGpsData(GpsData &gpsData);
		void refreshGpsRecordingScreenStopButton();
		void showInfoScreen(SDCardStatus sdCardStatus);
};

#endif /* __UI_H */
