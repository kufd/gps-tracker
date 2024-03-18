#include "ui.sync_gps.screen.class.h"
#include "ui.sync_gps_finish.screen.class.h"

UiSyncGpsScreen::UiSyncGpsScreen(UiEventDispatcher *uiEventDispatcher)
{
	this->uiEventDispatcher = uiEventDispatcher;
}

bool UiSyncGpsScreen::matchName(const char *nameToMatch)
{
	return strcmp(UiSyncGpsScreen::name, nameToMatch) == 0;
}

void UiSyncGpsScreen::selectButtonPressed()
{

}

void UiSyncGpsScreen::moveButtonPressed()
{
}

void UiSyncGpsScreen::show()
{
	Paint_DrawString_EN (60, 34, "Syncing record", &Font20, BLACK, YELLOW);

	StartSyncGpsRecordsUiEvent startSyncGpsRecords;
	uiEventDispatcher->dispatch(startSyncGpsRecords);

	ShowScreenUiEvent showScreenUiEvent(UiSyncGpsFinishScreen::name);
	uiEventDispatcher->dispatch(showScreenUiEvent);
}

void UiSyncGpsScreen::onUiEvent(UiEvent &event)
{
	if (event.matchName(UiEventName::SyncGpsRecordStartedUiEventName))
	{
		SyncGpsRecordStartedUiEvent *syncGpsRecordStartedUiEvent = (SyncGpsRecordStartedUiEvent*) &event;
		refreshRecordName(syncGpsRecordStartedUiEvent->getRecordName());
	}

	if (event.matchName(UiEventName::SyncGpsRecordProgressChangedUiEventName))
	{
		SyncGpsRecordProgressChangedUiEvent *syncGpsRecordProgressChangedUiEvent = (SyncGpsRecordProgressChangedUiEvent*) &event;
		syncGpsRecordsScreenRefreshProgress(syncGpsRecordProgressChangedUiEvent->getProgress());
	}
}

void UiSyncGpsScreen::refreshRecordName(const char* recordName)
{
	Paint_DrawString_EN (40, 68, recordName, &Font20, BLACK, YELLOW);
}

void UiSyncGpsScreen::syncGpsRecordsScreenRefreshProgress(uint8_t progress)
{
	char outputBuffer[50];
	sprintf(outputBuffer, "%d%%   ", progress);

	Paint_DrawString_EN (140, 116, outputBuffer, &Font24, BLACK, YELLOW);
}
