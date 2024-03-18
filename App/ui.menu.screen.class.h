#ifndef __UI_MENU_SCREEN_CLASS_H
#define __UI_MENU_SCREEN_CLASS_H

#include "main.h"
#include "LcdDriver/LCD_2inch.h"
#include "LcdDriver/DEV_Config.h"
#include "LcdDriver/GUI_Paint.h"
#include "LcdDriver/fonts.h"
#include "ui.screen.class.h"
#include "ui.events.h"
#include "ui.events.dispatcher.h"

class UiMenuScreen: public UiScreen
{
	public:
		static constexpr const char* const name {"UiMenuScreen"};
	private:
		const uint8_t MENU_ITEM_RECORD_GPS = 0;
		const uint8_t MENU_ITEM_SYNC_GPS = 1;
		const uint8_t MENU_ITEM_RECORD_ENV = 2;
		const uint8_t MENU_ITEM_INFO = 3;

		static const uint8_t menuItemsNumber = 4;
		char menuItems[menuItemsNumber][18] {"Record GPS", "Sync GPS", "Record env", "Info"};
		uint8_t activeMenuItemNumber = 0;

		UiEventDispatcher *uiEventDispatcher;
	public:
		UiMenuScreen(UiEventDispatcher *uiEventDispatcher);
		void selectButtonPressed();
		void moveButtonPressed();
		void show();
		bool matchName(const char *nameToMatch);
		void onUiEvent(UiEvent &event);
	private:
		void showMenuItem(uint8_t menuItemNumber);
};

#endif
