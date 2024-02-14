#include "app.h"
#include "gps_parser.h"
#include "ui.h"
#include <string.h>

#define GPS_BUFFER_SIZE 128
uint8_t GPS_BUFFER[GPS_BUFFER_SIZE];

void applicationMain(UART_HandleTypeDef huart1)
{
	GpsParser gpsParser;
	UI ui;
	ui.init();

	while(1) {
		memset(GPS_BUFFER, 0, GPS_BUFFER_SIZE);
		HAL_UART_Receive(&huart1, GPS_BUFFER, GPS_BUFFER_SIZE, 100);

		//TODO refactor
		uint8_t size = 0;
		while (size < GPS_BUFFER_SIZE && GPS_BUFFER[size] != 0) {
			++size;
		}

		gpsParser.addData(GPS_BUFFER, size);

		ui.refresh(gpsParser);
	}
}

