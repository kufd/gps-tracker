#include "ui.record_gps.screen.class.h"
#include "ui.menu.screen.class.h"

UiRecordGpsScreen::UiRecordGpsScreen(UiEventDispatcher *uiEventDispatcher)
{
	this->uiEventDispatcher = uiEventDispatcher;
}

bool UiRecordGpsScreen::matchName(const char *nameToMatch)
{
	return strcmp(UiRecordGpsScreen::name, nameToMatch) == 0;
}

void UiRecordGpsScreen::selectButtonPressed()
{

}

void UiRecordGpsScreen::moveButtonPressed()
{
	if (++exitScreenPushButtonCounter < 3) {
		refreshGpsRecordingScreenStopButton();
		return;
	}

	exitScreenPushButtonCounter = 0;

	StopGpsDataRecordingUiEvent stopGpsDataRecordingUiEvent;
	uiEventDispatcher->dispatch(stopGpsDataRecordingUiEvent);

	ShowScreenUiEvent showScreenUiEvent(UiMenuScreen::name);
	uiEventDispatcher->dispatch(showScreenUiEvent);
}

void UiRecordGpsScreen::show()
{
	refreshGpsRecordingScreenStopButton();

	StartGpsDataRecordingUiEvent startGpsDataRecordingUiEvent;
	uiEventDispatcher->dispatch(startGpsDataRecordingUiEvent);
}

void UiRecordGpsScreen::onUiEvent(UiEvent &event)
{
	if (event.matchName(UiEventName::GpsDataChangedUiEventName))
	{
		GpsDataChangedUiEvent *gpsDataChangedUiEvent = (GpsDataChangedUiEvent*) &event;
		refreshGpsData(gpsDataChangedUiEvent->getGpsData());
	}

	if (event.matchName(UiEventName::GpsStatusUpdateUiEventName))
	{
		GpsStatusUpdateUiEvent *gpsStatusUpdateUiEvent = (GpsStatusUpdateUiEvent*) &event;
		refreshGpsStatus(gpsStatusUpdateUiEvent->getGpsStatus());
	}
}




void UiRecordGpsScreen::refreshGpsRecordingScreenStopButton()
{
	char stopLabel[7] = "STOP";
	if (exitScreenPushButtonCounter == 1) {
		strcpy(stopLabel, "STOP?");
	}
	if (exitScreenPushButtonCounter == 2) {
		strcpy(stopLabel, "STOP??");
	}
	Paint_DrawString_EN (5, 210, stopLabel, &Font24, BLACK, YELLOW);
}

void UiRecordGpsScreen::refreshGpsStatus(GpsStatus gpsStatus)
{
	static GpsStatus prevGpsStatus = GpsStatus::NO_DATA;

	if (gpsStatus == GpsStatus::VALID_DATA && prevGpsStatus == gpsStatus)
	{
		return;
	}

	if (gpsStatus == GpsStatus::VALID_DATA)
	{
    	Paint_DrawString_EN (260, 0, "GPS", &Font24, BLACK, GREEN);
		return;
	}

	if (++searchGpsProgressCounter % 2 != 0)
	{
		Paint_DrawString_EN (260, 0, "GPS", &Font24, BLACK, gpsStatus == GpsStatus::INVALID_DATA ? CYAN : RED);
	}
	else
	{
		Paint_DrawString_EN (260, 0, "GPS", &Font24, BLACK, BLACK);
	}

	prevGpsStatus = gpsStatus;
}

void UiRecordGpsScreen::refreshGpsData(GpsData &gpsData)
{
	char outputBuffer[50];

	Paint_DrawString_EN (5, 54, gpsData.dateUtc, &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (155, 54, gpsData.timeUtc, &Font20, BLACK, YELLOW);

	Paint_DrawString_EN (5, 78, "long:", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 102, "lat:", &Font20, BLACK, YELLOW);

	sprintf(outputBuffer, "%f    ", gpsData.longitude);
	Paint_DrawString_EN (80, 78, outputBuffer, &Font20, BLACK, YELLOW);

	sprintf(outputBuffer, "%f    ", gpsData.latitude);
	Paint_DrawString_EN (80, 102, outputBuffer, &Font20, BLACK, YELLOW);
}


