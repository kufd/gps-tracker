#include "ui.sync_gps_finish.screen.class.h"

UiSyncGpsFinishScreen::UiSyncGpsFinishScreen(UiEventDispatcher *uiEventDispatcher)
{
	this->uiEventDispatcher = uiEventDispatcher;
}

bool UiSyncGpsFinishScreen::matchName(const char *nameToMatch)
{
	return strcmp(UiSyncGpsFinishScreen::name, nameToMatch) == 0;
}

void UiSyncGpsFinishScreen::show()
{
		Paint_DrawString_EN ((320-4*17)/2, 68, "DONE", &Font24, BLACK, YELLOW);
		Paint_DrawString_EN ((320-18*11)/2, 116, "no records to sync", &Font16, BLACK, YELLOW);

		Paint_DrawString_EN (5, 210, "MENU", &Font24, BLACK, YELLOW);
}

void UiSyncGpsFinishScreen::onUiEvent(UiEvent &event)
{

}

void UiSyncGpsFinishScreen::selectButtonPressed()
{

}

void UiSyncGpsFinishScreen::moveButtonPressed()
{
	ShowScreenUiEvent showScreenUiEvent(UiMenuScreen::name);
	uiEventDispatcher->dispatch(showScreenUiEvent);
}
