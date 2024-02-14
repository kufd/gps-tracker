#ifndef __UI_H
#define __UI_H

#include "main.h"
#include "LcdDriver/LCD_2inch.h"
#include "LcdDriver/DEV_Config.h"
#include "LcdDriver/GUI_Paint.h"
#include "LcdDriver/fonts.h"
#include "gps_parser.h"

class UI
{
	private:
		uint8_t searchGpsProgressCounter = 0;
	public:
		void init();
		void refresh(GpsParser gpsParser);
	private:
		void printGpsData(GpsParser gpsParser);
		void printStorageData();

};

#endif /* __UI_H */
