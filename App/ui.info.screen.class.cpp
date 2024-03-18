#include "ui.info.screen.class.h"

UiInfoScreen::UiInfoScreen(UiEventDispatcher *uiEventDispatcher)
{
	this->uiEventDispatcher = uiEventDispatcher;
}

bool UiInfoScreen::matchName(const char *nameToMatch)
{
	return strcmp(UiInfoScreen::name, nameToMatch) == 0;
}

void UiInfoScreen::show()
{
	Paint_DrawString_EN (5, 10, "GPS tracker v0.01", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 30, "SD Card", &Font20, BLACK, YELLOW);

	Paint_DrawString_EN (5, 210, "MENU", &Font24, BLACK, YELLOW);

	RefreshSDCardStatusUiEvent refreshSDCardStatusUiEvent;
	uiEventDispatcher->dispatch(refreshSDCardStatusUiEvent);
}

void UiInfoScreen::onUiEvent(UiEvent &event)
{
	if (event.matchName(UiEventName::SDCardStatusUiEventName))
	{
		SDCardStatusUiEvent *sdCardStatusUiEvent = (SDCardStatusUiEvent*) &event;
		refreshSDCardStatus(sdCardStatusUiEvent->getSDCardStatus());
	}
}

void UiInfoScreen::refreshSDCardStatus(SDCardStatus* sdCardStatus)
{
	char outputBuffer[40];
	char size[20];

	sdCardStatus->getTotal(size);
	sprintf(outputBuffer, "Total: %s", size);
	Paint_DrawString_EN (30, 50, outputBuffer, &Font20, BLACK, YELLOW);

	sdCardStatus->getUsed(size);
	sprintf(outputBuffer, "Used: %s", size);
	Paint_DrawString_EN (30, 70, outputBuffer, &Font20, BLACK, YELLOW);

	sdCardStatus->getFree(size);
	sprintf(outputBuffer, "Free: %s", size);
	Paint_DrawString_EN (30, 90, outputBuffer, &Font20, BLACK, YELLOW);
}

void UiInfoScreen::selectButtonPressed()
{

}

void UiInfoScreen::moveButtonPressed()
{
	ShowScreenUiEvent showScreenUiEvent(UiMenuScreen::name);
	uiEventDispatcher->dispatch(showScreenUiEvent);
}
