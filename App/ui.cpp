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
	Paint_DrawString_EN (5, 34, "loc.:", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 58, "sat.:", &Font20, BLACK, YELLOW);
	Paint_DrawString_EN (5, 82, "storage:", &Font20, BLACK, YELLOW);
}

void UI::refresh(GpsParser gpsParser)
{
	printGpsData(gpsParser);
	printStorageData();
}

void UI::printGpsData(GpsParser gpsParser)
{
	char outputBuffer[50];

	if (!gpsParser.isContaisValidData()) {
		if (++searchGpsProgressCounter % 2 != 0) {
			Paint_DrawString_EN (260, 10, "GPS", &Font24, BLACK, gpsParser.getIsReceivingData() ? CYAN : RED);
		}
		else
		{
			Paint_DrawString_EN (260, 10, "GPS", &Font24, BLACK, BLACK);
		}

		return;
	}

	//TODO update screen only when there is changes
	Paint_DrawString_EN (260, 10, "GPS", &Font24, BLACK, GREEN);

	Paint_DrawString_EN (80, 10, gpsParser.getTimeUtc(), &Font20, BLACK, YELLOW);

	Paint_DrawString_EN (80, 34, gpsParser.getLocation(), &Font20, BLACK, YELLOW);

	sprintf(outputBuffer, "%d", gpsParser.getSatellitesNumber());
	Paint_DrawString_EN (80, 58, outputBuffer, &Font20, BLACK, YELLOW);
}

void UI::printStorageData()
{
	char outputBuffer[50];

	Paint_DrawString_EN (120, 82, "10GB/1GB", &Font20, BLACK, YELLOW);
}
