#include "ui.menu.screen.class.h"
#include "ui.info.screen.class.h"
#include "ui.sync_gps.screen.class.h"
#include "ui.record_gps.screen.class.h"

UiMenuScreen::UiMenuScreen(UiEventDispatcher *uiEventDispatcher)
{
	this->uiEventDispatcher = uiEventDispatcher;
}

bool UiMenuScreen::matchName(const char *nameToMatch)
{
	return strcmp(UiMenuScreen::name, nameToMatch) == 0;
}

void UiMenuScreen::selectButtonPressed()
{
	if (activeMenuItemNumber == MENU_ITEM_RECORD_GPS)
	{
		ShowScreenUiEvent showScreenUiEvent(UiRecordGpsScreen::name);
		uiEventDispatcher->dispatch(showScreenUiEvent);
	}

	if (activeMenuItemNumber == MENU_ITEM_SYNC_GPS)
	{
		ShowScreenUiEvent showScreenUiEvent(UiSyncGpsScreen::name);
		uiEventDispatcher->dispatch(showScreenUiEvent);
	}

	if (activeMenuItemNumber == MENU_ITEM_INFO)
	{
		ShowScreenUiEvent showScreenUiEvent(UiInfoScreen::name);
		uiEventDispatcher->dispatch(showScreenUiEvent);
	}
}

void UiMenuScreen::moveButtonPressed()
{
	uint8_t activeMenuItemNumberPrev = activeMenuItemNumber;

	++activeMenuItemNumber;
	if (activeMenuItemNumber >= menuItemsNumber)
	{
		activeMenuItemNumber = 0;
	}

	showMenuItem(activeMenuItemNumberPrev);
	showMenuItem(activeMenuItemNumber);
}

void UiMenuScreen::onUiEvent(UiEvent &event)
{

}

void UiMenuScreen::show()
{
	for (uint8_t i = 0; i < menuItemsNumber; i++) {
		showMenuItem(i);
	}
}

void UiMenuScreen::showMenuItem(uint8_t menuItemNumber)
{
	uint8_t y = (menuItemNumber+1)*34;
	uint8_t x = (320 - strlen(menuItems[menuItemNumber])*17)/2;
	Paint_DrawString_EN (x, y, menuItems[menuItemNumber], &Font24, BLACK, menuItemNumber == activeMenuItemNumber ? GREEN : WHITE);
}
