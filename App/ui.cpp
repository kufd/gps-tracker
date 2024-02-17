#include "ui.h"


void UI::init()
{
	DEV_Module_Init();
	LCD_2IN_SetBackLight(100);
	LCD_2IN_Init();
	LCD_2IN_Clear(GREEN);

	Paint_NewImage(LCD_2IN_WIDTH,LCD_2IN_HEIGHT, ROTATE_90, WHITE);

	Paint_SetClearFuntion(LCD_2IN_Clear);
	Paint_SetDisplayFuntion(LCD_2IN_DrawPaint);

	Paint_Clear(BLACK);
	Paint_SetRotate(ROTATE_270);

	Paint_DrawString_EN (5, 10, "time:", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 34, "ST t/f/u:", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 58, "sat.:", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 82, "long:", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 106, "lat:", &Font20, BLACK, YELLOW);

}

void UI::onGpsDataChange(GpsParser *gpsParser)
{
	printGpsData(gpsParser);
}

void UI::printError(const char* error)
{
	Paint_DrawString_EN(5, 210, error, &Font24, RED, WHITE);
}

void UI::refreshGpsDataRecordingEnabledIndicator(bool gpsDataRecordingEnabled)
{
	if (gpsDataRecordingEnabled)
	{
		Paint_DrawString_EN (260, 210, "rec", &Font24, BLACK, GREEN);
		return;
	}

	Paint_DrawString_EN (260, 210, "rec", &Font24, BLACK, BLACK);
}

void UI::refreshSearchGpsIndicator(GpsParser *gpsParser)
{
	if (gpsParser->isContaisValidData()) {
		//TODO update screen only when there is changes
		Paint_DrawString_EN (260, 0, "GPS", &Font24, BLACK, GREEN);
		return;
	}

	if (++searchGpsProgressCounter % 2 != 0) {
		Paint_DrawString_EN (260, 0, "GPS", &Font24, BLACK, gpsParser->getIsReceivingData() ? CYAN : RED);
	}
	else
	{
		Paint_DrawString_EN (260, 0, "GPS", &Font24, BLACK, BLACK);
	}
}

void UI::printGpsData(GpsParser *gpsParser)
{
	char outputBuffer[50];
	char longitude[30];
	char latitude[30];

	gpsParser->getLongitude(longitude);
	gpsParser->getLatitude(latitude);

	Paint_DrawString_EN (80, 10, gpsParser->getTimeUtc(), &Font20, BLACK, YELLOW);

	sprintf(outputBuffer, "%d    ", gpsParser->getSatellitesNumber());
	Paint_DrawString_EN (80, 58, outputBuffer, &Font20, BLACK, YELLOW);

	sprintf(outputBuffer, "%s    ", longitude);
	Paint_DrawString_EN (80, 82, outputBuffer, &Font20, BLACK, YELLOW);

	sprintf(outputBuffer, "%s    ", latitude);
	Paint_DrawString_EN (80, 104, outputBuffer, &Font20, BLACK, YELLOW);
}

void UI::refreshStorageData(Storage *storage)
{
	char outputBuffer[50] = "";
	char totalUnit[2] = "G";
	char freeUnit[2] = "G";
	char usedUnit[2] = "G";
	uint32_t total = storage->getTotalGBytes();
	uint32_t free = storage->getFreeGBytes();
	uint32_t used = storage->getUsedGBytes();

	if (total == 0) {
		total = storage->getTotalMBytes();
		totalUnit[0] = 'M';
	}
	if (free == 0) {
		free = storage->getFreeMBytes();
		freeUnit[0] = 'M';
	}
	if (used == 0) {
		used = storage->getUsedMBytes();
		usedUnit[0] = 'M';
	}

	sprintf(outputBuffer, "%d%s/%d%s/%d%s", total, totalUnit, free, freeUnit, used, usedUnit);
	Paint_DrawString_EN (140, 34, outputBuffer, &Font20, BLACK, YELLOW);
}
