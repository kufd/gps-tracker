#ifndef __UI_SYNC_GPS_CLASS_H
#define __UI_SYNC_GPS_CLASS_H

#include "main.h"
#include "LcdDriver/LCD_2inch.h"
#include "LcdDriver/DEV_Config.h"
#include "LcdDriver/GUI_Paint.h"
#include "LcdDriver/fonts.h"
#include "app.class.h"
#include "ui.screen.class.h"
#include "ui.menu.screen.class.h"
#include "ui.events.h"

class UiSyncGpsScreen: public UiScreen
{
	public:
		static constexpr const char* const name {"UiSyncGpsScreen"};
	private:
		UiEventDispatcher *uiEventDispatcher;
	public:
		UiSyncGpsScreen(UiEventDispatcher *uiEventDispatcher);
		void selectButtonPressed();
		void moveButtonPressed();
		void show();
		bool matchName(const char *nameToMatch);
		void onUiEvent(UiEvent &event);
	private:
		void refreshRecordName(const char* recordName);
		void syncGpsRecordsScreenRefreshProgress(uint8_t progress);
};

#endif
