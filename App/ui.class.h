#ifndef __UI_CLASS_H
#define __UI_CLASS_H

#include "main.h"
#include "LcdDriver/LCD_2inch.h"
#include "LcdDriver/DEV_Config.h"
#include "LcdDriver/GUI_Paint.h"
#include "LcdDriver/fonts.h"
#include "gps_parser.h"
#include "sdcard_status.h"
#include <functional>
#include "ui.screen.class.h"
#include "ui.events.h"
#include "ui.events.dispatcher.h"


class UI: public UiEventsListener
{
	private:
		const static uint8_t screensNumber = 5;
		UiScreen *screens[screensNumber];


		UiScreen *currentScreen;

		UiEventDispatcher *uiEventDispatcher;
	public:
		UI(UiEventDispatcher *uiEventDispatcher);
		~UI();
		void show(const char *screenName);
		void selectButtonPressed();
		void moveButtonPressed();
		void start();
		void printError(const char* error);
		void onUiEvent(UiEvent &event);
	private:
		void clear();
};

#endif
