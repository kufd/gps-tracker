#include "ui.class.h"
#include "ui.menu.screen.class.h"
#include "ui.menu.screen.class.h"
#include "ui.info.screen.class.h"
#include "ui.sync_gps.screen.class.h"
#include "ui.sync_gps_finish.screen.class.h"
#include "ui.record_gps.screen.class.h"

UI::UI(UiEventDispatcher *uiEventDispatcher)
{
	this->uiEventDispatcher = uiEventDispatcher;

	screens[0] = new UiMenuScreen(uiEventDispatcher);
	screens[1] = new UiInfoScreen(uiEventDispatcher);
	screens[2] = new UiSyncGpsScreen(uiEventDispatcher);
	screens[3] = new UiSyncGpsFinishScreen(uiEventDispatcher);
	screens[4] = new UiRecordGpsScreen(uiEventDispatcher);
}

UI::~UI()
{
	for (uint8_t i=0; i<screensNumber; i++)
	{
		delete screens[i];
	}
}

void UI::show(const char *screenName)
{
	for (uint8_t i = 0; i < screensNumber; i++) {
		if(screens[i]->matchName(screenName))
		{
			clear();
			currentScreen = screens[i];
			currentScreen->show();

			return;
		}
	}

	throw "Screen not found";
}

void UI::selectButtonPressed()
{
	currentScreen->selectButtonPressed();
}

void UI::moveButtonPressed()
{
	currentScreen->moveButtonPressed();
}

void UI::start()
{
	DEV_Module_Init();
	LCD_2IN_SetBackLight(100);
	LCD_2IN_Init();
	LCD_2IN_Clear(BLACK);

	Paint_NewImage(LCD_2IN_WIDTH,LCD_2IN_HEIGHT, ROTATE_90, WHITE);

	Paint_SetClearFuntion(LCD_2IN_Clear);
	Paint_SetDisplayFuntion(LCD_2IN_DrawPaint);

	show(UiMenuScreen::name);
}

void UI::clear()
{
	Paint_Clear(BLACK);
}

void UI::printError(const char* error)
{
	Paint_DrawString_EN(5, 210, error, &Font12, RED, WHITE);
}

void UI::onUiEvent(UiEvent &event)
{
	if (event.matchName(UiEventName::ShowScreenUiEventName))
	{
		ShowScreenUiEvent *showScreenUiEvent = (ShowScreenUiEvent*) &event;
		show(showScreenUiEvent->getScreenName());
	}

	currentScreen->onUiEvent(event);
}

