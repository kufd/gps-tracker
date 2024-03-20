#include <App/gps_record.h>
#include "logger.h"
#include "app.h"
#include "ui.class.h"
#include "wifi.h"
#include <string.h>
#include <stdexcept>
#include <exception>
#include <functional>
#include "sdcard_status.h"
#include "fatfs.h"
#include "app.class.h"
#include "circular_buffer.class.h"

UART_HandleTypeDef* huartWifiGLobal;
UART_HandleTypeDef* huartGpsGlobal;

CircularBuffer wifiCircularBuffer("wifi", 512);
CircularBuffer gpsCircularBuffer("gps", 256);

Logger *logger = NULL;
UI *uiGlobal = NULL;

void applicationMain(UART_HandleTypeDef* huartGps, UART_HandleTypeDef* huartWifi, UART_HandleTypeDef* huartDebugConsole)
{
	Logger loggerInstance(huartDebugConsole);
	logger = &loggerInstance;

	huartWifiGLobal = huartWifi;
	huartGpsGlobal = huartGps;

	UiEventDispatcher uiEventDispatcher;
	App app(huartWifi, &uiEventDispatcher);
	UI ui(&uiEventDispatcher);
	uiGlobal = &ui;


	uiEventDispatcher.subscribe(&app);
	uiEventDispatcher.subscribe(&ui);

	wifiCircularBuffer.subscribe(&app);
	gpsCircularBuffer.subscribe(&app);

	try {
		app.start();
		ui.start();

		HAL_StatusTypeDef startGpsDmaResult = HAL_UARTEx_ReceiveToIdle_DMA(huartGpsGlobal, (uint8_t*)gpsCircularBuffer.getBuffer(), gpsCircularBuffer.getBufferSize());
		if (startGpsDmaResult != HAL_OK)
		{
			throw "cannot start GPS DMA";
		}

		HAL_StatusTypeDef startWifiDmaResult = HAL_UARTEx_ReceiveToIdle_DMA(huartWifiGLobal, (uint8_t*)wifiCircularBuffer.getBuffer(), wifiCircularBuffer.getBufferSize());
		if (startWifiDmaResult != HAL_OK)
		{
			throw "cannot start WIFI DMA";
		}



		for(uint8_t mainCounter = 0; ; mainCounter++)
		{

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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (uiGlobal == NULL || logger == NULL)
	{
		return;
	}

	//TODO HAL_GPIO_EXTI_Callback is called a few times when do one push ?????
    if(GPIO_Pin == BTN_MOVE_Pin)
    {
    	static bool btnMovePressedPrev = false;

		bool btnMovePressed = HAL_GPIO_ReadPin(BTN_MOVE_GPIO_Port, BTN_MOVE_Pin) == 1;

		logger->debug("btnMovePressed", btnMovePressed ? "true" : "false");

		if (btnMovePressedPrev == btnMovePressed)
		{
			return;
		}

		btnMovePressedPrev = btnMovePressed;
		if (btnMovePressed)
		{
			uiGlobal->moveButtonPressed();
		}
    }

    if(GPIO_Pin == BTN_SELECT_Pin)
    {
    	static bool btnSelectPressedPrev = false;

		bool btnSelectPressed = HAL_GPIO_ReadPin(BTN_SELECT_GPIO_Port, BTN_SELECT_Pin) == 1;

		logger->debug("btnSelectPressed", btnSelectPressed ? "true" : "false");

		if (btnSelectPressedPrev == btnSelectPressed)
		{
			return;
		}

		btnSelectPressedPrev = btnSelectPressed;
		if (btnSelectPressed)
		{
			uiGlobal->selectButtonPressed();
		}
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size){
	if(huart == huartWifiGLobal)
	{
		wifiCircularBuffer.onDataReceived(size);
		return;
	}

	if(huart == huartGpsGlobal)
	{
		gpsCircularBuffer.onDataReceived(size);
		return;
	}
}

void applicationErrorHandler()
{

}

