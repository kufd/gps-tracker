#include "ui.h"
#include <string.h>

UI::UI(
	std::function<void()> startGpsDataRecordingFunc,
	std::function<void()> stopGpsDataRecordingFunc,
	std::function<void()> startSyncGpsRecordsFunc
)
{
	this->startGpsDataRecordingFunc = startGpsDataRecordingFunc;
	this->stopGpsDataRecordingFunc = stopGpsDataRecordingFunc;
	this->startSyncGpsRecordsFunc = startSyncGpsRecordsFunc;
}

void UI::init()
{
	DEV_Module_Init();
	LCD_2IN_SetBackLight(100);
	LCD_2IN_Init();
	LCD_2IN_Clear(BLACK);

	Paint_NewImage(LCD_2IN_WIDTH,LCD_2IN_HEIGHT, ROTATE_90, WHITE);

	Paint_SetClearFuntion(LCD_2IN_Clear);
	Paint_SetDisplayFuntion(LCD_2IN_DrawPaint);
}

void UI::selectButtonPressed()
{
	if (activeScreenNumber == SCREEN_MENU)
	{
		if (activeMenuItemNumber == 0)
		{
			startGpsDataRecordingFunc();

			activeScreenNumber = SCREEN_RECORD_GPS;
			showGpsRecordingScreen();
		}

		if (activeMenuItemNumber == 1)
		{
			activeScreenNumber = SCREEN_SYNC_GPS;
			showSyncGpsRecordsScreen();

			startSyncGpsRecordsFunc();
		}
	}


}

void UI::moveButtonPressed()
{
	if (activeScreenNumber == SCREEN_MENU)
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

	if (activeScreenNumber == SCREEN_RECORD_GPS)
	{
		exitScreenPushButtonCounter++;

		if (exitScreenPushButtonCounter >= 3)
		{
			stopGpsDataRecordingFunc();

			exitScreenPushButtonCounter = 0;
			activeScreenNumber = SCREEN_MENU;
			showMenuScreen();

			return;
		}

		refreshGpsRecordingScreenStopButton();
	}

	if (activeScreenNumber == SCREEN_SYNC_GPS_FINISH)
	{
		showMenuScreen();
		activeScreenNumber = SCREEN_MENU; //TODO set in showMenuScreen ??

		return;
	}
}

void UI::showMenuScreen()
{
	Paint_Clear(BLACK);
	for (uint8_t i = 0; i < menuItemsNumber; i++) {
		showMenuItem(i);
	}
}

void UI::showMenuItem(uint8_t menuItemNumber)
{
	uint8_t y = (menuItemNumber+1)*34;
	uint8_t x = (320 - strlen(menuItems[menuItemNumber])*17)/2;
	Paint_DrawString_EN (x, y, menuItems[menuItemNumber], &Font24, BLACK, menuItemNumber == activeMenuItemNumber ? GREEN : WHITE);
}


//========GpsRecordingScreen============
void UI::showGpsRecordingScreen()
{
	Paint_Clear(BLACK);

	refreshGpsRecordingScreenStopButton();
}

void UI::refreshGpsRecordingScreenStopButton()
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

void UI::refreshGpsStatus(GpsStatus gpsStatus)
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

void UI::refreshGpsData(GpsData* gpsData)
{
	char outputBuffer[50];

	Paint_DrawString_EN (5, 54, gpsData->dateUtc, &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (155, 54, gpsData->timeUtc, &Font20, BLACK, YELLOW);

	Paint_DrawString_EN (5, 78, "long:", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 102, "lat:", &Font20, BLACK, YELLOW);

	sprintf(outputBuffer, "%f    ", gpsData->longitude);
	Paint_DrawString_EN (80, 78, outputBuffer, &Font20, BLACK, YELLOW);

	sprintf(outputBuffer, "%f    ", gpsData->latitude);
	Paint_DrawString_EN (80, 102, outputBuffer, &Font20, BLACK, YELLOW);
}

void UI::onGpsDataChange(GpsData* gpsData)
{
	refreshGpsData(gpsData);
}
//========GpsRecordingScreen============



//========SyncGpsRecordsScreen============
void UI::showSyncGpsRecordsScreen()
{
	Paint_Clear(BLACK);
	Paint_DrawString_EN (60, 34, "Syncing record", &Font20, BLACK, YELLOW);
}
void UI::syncGpsRecordsScreenRefreshRecordName(const char* recordName)
{
	Paint_DrawString_EN (40, 68, recordName, &Font20, BLACK, YELLOW);
}
void UI::syncGpsRecordsScreenRefreshProgress(uint8_t progress)
{
	char outputBuffer[50];
	sprintf(outputBuffer, "%d%%   ", progress);

	Paint_DrawString_EN (140, 116, outputBuffer, &Font24, BLACK, YELLOW);
}
//========SyncGpsRecordsScreen============

//========SyncGpsRecordsScreenFinishScreen============
void UI::showSyncGpsRecordsScreenFinishScreen()
{
	activeScreenNumber = SCREEN_SYNC_GPS_FINISH;

	Paint_Clear(BLACK);
	Paint_DrawString_EN ((320-4*17)/2, 68, "DONE", &Font24, BLACK, YELLOW);
	Paint_DrawString_EN ((320-18*11)/2, 116, "no records to sync", &Font16, BLACK, YELLOW);

	Paint_DrawString_EN (5, 210, "MENU", &Font24, BLACK, YELLOW);
}
//========SyncGpsRecordsScreenFinishScreen============

void UI::printError(const char* error)
{
	Paint_DrawString_EN(5, 210, error, &Font12, RED, WHITE);
}

void UI::refreshStorageData(SDCardStatus* sdCardStatus)
{
	char outputBuffer[50] = "";
	char totalUnit[2] = "G";
	char freeUnit[2] = "G";
	char usedUnit[2] = "G";
	uint32_t total = sdCardStatus->getTotalGBytes();
	uint32_t free = sdCardStatus->getFreeGBytes();
	uint32_t used = sdCardStatus->getUsedGBytes();

	if (total == 0) {
		total = sdCardStatus->getTotalMBytes();
		totalUnit[0] = 'M';
	}
	if (free == 0) {
		free = sdCardStatus->getFreeMBytes();
		freeUnit[0] = 'M';
	}
	if (used == 0) {
		used = sdCardStatus->getUsedMBytes();
		usedUnit[0] = 'M';
	}

	sprintf(outputBuffer, "%d%s/%d%s/%d%s", total, totalUnit, free, freeUnit, used, usedUnit);
	Paint_DrawString_EN (5, 34, outputBuffer, &Font20, BLACK, YELLOW);
}
