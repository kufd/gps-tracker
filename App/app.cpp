#include <App/gps_record.h>
#include "logger.h"
#include "app.h"
#include "gps_parser.h"
#include "ui.h"
#include "wifi.h"
#include <string.h>
#include <stdexcept>
#include <exception>
#include <functional>
#include "sdcard_status.h"
#include "fatfs.h"
#include "app.class.h"
#include "circular_buffer.class.h"

#define GPS_BUFFER_SIZE 256
char GPS_BUFFER[GPS_BUFFER_SIZE];

UART_HandleTypeDef* huartWifiGLobal;
DMA_HandleTypeDef* hdmaUsartRxWifiGlobal;

CircularBuffer wifiCircularBuffer("wifi", 512);

Logger logger;

//TODO add log and debug console
void applicationMain(UART_HandleTypeDef* huartGps, UART_HandleTypeDef* huartWifi, DMA_HandleTypeDef* hdmaUsartRxWifi)
{
	huartWifiGLobal = huartWifi;
	hdmaUsartRxWifiGlobal = hdmaUsartRxWifi;

	App app(huartWifi);
	UI ui(&app);

	wifiCircularBuffer.subscribe(&app);

	try {
		GpsParser gpsParser;
		gpsParser.addGpsDataChangeListener(&ui);
		gpsParser.addGpsDataChangeListener(&app);

		app.start();
		ui.start();

		if (HAL_UARTEx_ReceiveToIdle_DMA(huartWifiGLobal, (uint8_t*)wifiCircularBuffer.getBuffer(), wifiCircularBuffer.getBufferSize()) != HAL_OK)
		{
			throw "cannot start WIFI DMA";
		}

		bool btnMovePressedPrev = false;
		bool btnSelectPressedPrev = false;
		for(uint8_t mainCounter = 0; ; mainCounter++)
		{
			//TODO add interruption
			bool btnMovePressed = HAL_GPIO_ReadPin(BTN_MOVE_GPIO_Port, BTN_MOVE_Pin) == 1;
			if (btnMovePressedPrev != btnMovePressed)
			{
				btnMovePressedPrev = btnMovePressed;

				if (btnMovePressed)
				{
					ui.moveButtonPressed();
				}
			}

			bool btnSelectPressed = HAL_GPIO_ReadPin(BTN_SELECT_GPIO_Port, BTN_SELECT_Pin) == 1;
			if (btnSelectPressedPrev != btnSelectPressed)
			{
				btnSelectPressedPrev = btnSelectPressed;

				if (btnSelectPressed)
				{
					ui.selectButtonPressed();
				}
			}

			//TODO DMA or interruption
			if (app.isGpsDataRecordingStarted())
			{
				memset(GPS_BUFFER, 0, GPS_BUFFER_SIZE);
				HAL_UART_Receive(huartGps, (uint8_t *) GPS_BUFFER, GPS_BUFFER_SIZE-1, 100);
				gpsParser.addData(GPS_BUFFER);

				ui.refreshGpsStatus(gpsParser.getGpsStatus());
			}
		}
	}
	catch (const char* error)
	{
		ui.printError(error);
	}
	catch (...)
	{
		ui.printError("Unknown error");
	}
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size){
	if(huart == huartWifiGLobal)
	{
		wifiCircularBuffer.onDataReceived(size);
	}
}

void applicationErrorHandler()
{

}

