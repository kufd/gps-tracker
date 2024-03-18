#ifndef __UI_RECORD_GPS_SCREEN_CLASS_H
#define __UI_RECORD_GPS_SCREEN_CLASS_H

#include "main.h"
#include "LcdDriver/LCD_2inch.h"
#include "LcdDriver/DEV_Config.h"
#include "LcdDriver/GUI_Paint.h"
#include "LcdDriver/fonts.h"
#include "app.class.h"
#include "ui.screen.class.h"
#include "ui.menu.screen.class.h"
#include "ui.events.h"
#include "gps_parser.h"

class UiRecordGpsScreen: public UiScreen
{
	public:
		static constexpr const char* const name {"UiRecordGpsScreen"};
	private:
		UiEventDispatcher *uiEventDispatcher;
		uint8_t exitScreenPushButtonCounter = 0;
		uint8_t searchGpsProgressCounter = 0;
	public:
		UiRecordGpsScreen(UiEventDispatcher *uiEventDispatcher);
		void selectButtonPressed();
		void moveButtonPressed();
		void show();
		bool matchName(const char *nameToMatch);
		void onUiEvent(UiEvent &event);
	private:
		void refreshGpsRecordingScreenStopButton();
		void refreshGpsStatus(GpsStatus gpsStatus);
		void refreshGpsData(GpsData &gpsData);
};

#endif
