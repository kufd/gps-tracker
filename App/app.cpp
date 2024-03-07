#include "logger.h"
#include "app.h"
#include "gps_parser.h"
#include "ui.h"
#include "storage.h"
#include "wifi.h"
#include <string.h>
#include <stdexcept>
#include <exception>
#include <functional>
#include "sdcard_status.h"
#include "fatfs.h"

#define GPS_BUFFER_SIZE 256
char GPS_BUFFER[GPS_BUFFER_SIZE];

#define WIFI_BUFFER_SIZE 512
char WIFI_BUFFER[WIFI_BUFFER_SIZE];

UART_HandleTypeDef* huart2_wifi;
DMA_HandleTypeDef* hdma_usart2_rx_wifi;

class App: public GpsDataChangeListener
{
	private:
		Storage* storage;
		UI* ui;
		bool gpsDataRecordingStarted = false;
		bool isOpenedGpsDateStorage = false;
		Wifi* wifi = NULL;
	public:
		App(Storage* storage)
		{
			this->storage = storage;
		}
		~App()
		{
			stop();
		}
		void setUI(UI* ui)
		{
			this->ui = ui;
		}
		void start()
		{
			mountFileSystem();
			logger.openFile();
		}
		void stop()
		{
			unmountFileSystem();
			logger.closeFile();
		}
		void mountFileSystem()
		{
			FRESULT res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
			if(res != FR_OK)
			{
				throw "Cannot mount storage file system";
			}

			//TODO add option late
			//	if(f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, rtext, sizeof(rtext)) != FR_OK)
			//	{
			//		throw "Cannot make storage file system";
			//	}
		}
		void unmountFileSystem()
		{
			FRESULT res = f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
			if(res != FR_OK)
			{
				throw "Cannot unmount storage file system";
			}
		}
		void onGpsDataChange(GpsData* gpsData)
		{
			if (!gpsDataRecordingStarted)
			{
				return;
			}

			if (!isOpenedGpsDateStorage)
			{
				char recordName[20];
				char time[7] = {gpsData->timeUtc[0], gpsData->timeUtc[1], gpsData->timeUtc[3], gpsData->timeUtc[4], gpsData->timeUtc[6], gpsData->timeUtc[7]};
				sprintf(recordName, "%sT%s", gpsData->dateUtc, time);
				storage->openGpsDateStorage(recordName);
				isOpenedGpsDateStorage = true;
			}

			char dataToSave[256] = "";

			sprintf(
				dataToSave,
				"%s,%s,%f,%c,%f,%c\n",
				gpsData->dateUtc,
				gpsData->timeUtc,
				gpsData->latitude,
				gpsData->latitudeNorS,
				gpsData->longitude,
				gpsData->longitudeEorW
			);

			storage->writeGpsData(dataToSave);
		}
		bool isGpsDataRecordingStarted()
		{
			return gpsDataRecordingStarted;
		}
		void startGpsDataRecording()
		{
			gpsDataRecordingStarted = true;
		}
		void stopGpsDataRecording()
		{
			gpsDataRecordingStarted = false;

			if (isOpenedGpsDateStorage)
			{
				storage->closeGpsDateStorage();
			}
		}
		void syncGpsRecords()
		{
			//add progress on UI
			Wifi wifi;
			this->wifi = &wifi;
			wifi.init(huart2_wifi, "", ""); //TODO keep secrets on SD card

			UI* uiLocal = this->ui;

			while(true)
			{
				char recordName[21];
				if (!this->storage->findRecordToSync(recordName))
				{
					break;
				}

				ui->syncGpsRecordsScreenRefreshRecordName(recordName);
				this->storage->syncRecord(
					recordName,
					[&wifi, uiLocal](const char* packet, uint32_t totalBytes, uint32_t progressBytes){
						//TODO maybe rename method
						wifi.sendPost(packet);
						uiLocal->syncGpsRecordsScreenRefreshProgress((uint8_t)(100*(double)progressBytes/(double)totalBytes));
					}
				);
			}


			this->wifi = NULL;
			ui->showSyncGpsRecordsScreenFinishScreen();
		}
		void onReceivedDataFromWifi(const char* data, size_t size)
		{
			if (wifi == NULL)
			{
				return;
			}

			wifi->onReceivedDataFromHuart(data, size);
		}
		SDCardStatus getSDCardStatus()
		{
			DWORD freeClusters;
			FATFS* fatFsPointer = &SDFatFS;

			/* Get volume information and free clusters of drive 1 */
			FRESULT res = f_getfree((TCHAR const*)SDPath, &freeClusters, &fatFsPointer);
			if(res != FR_OK)
			{
				throw "Cannot get information about storage usage";
			}

			/* Get total sectors and free sectors */
			uint64_t totalSectors = (SDFatFS.n_fatent - 2) * SDFatFS.csize;
			uint64_t freeSectors = freeClusters * SDFatFS.csize;

			uint64_t totalMBytes = totalSectors * SDFatFS.ssize/(1024*1024);
			uint64_t freeMBytes = freeSectors * SDFatFS.ssize/(1024*1024);
			uint64_t usedMBytes = totalMBytes - freeMBytes;

			SDCardStatus sdCardStatus(totalMBytes, freeMBytes, usedMBytes);

			return sdCardStatus;
		}
};

Logger logger;
Storage storage;
App app(&storage);

//TODO add log and debug console
void applicationMain(UART_HandleTypeDef* huart1, UART_HandleTypeDef* huart2, DMA_HandleTypeDef* hdma_usart2_rx)
{
	huart2_wifi = huart2;
	hdma_usart2_rx_wifi = hdma_usart2_rx;

	UI ui(
		[&app](){
			app.startGpsDataRecording();
		},
		[&app](){
			app.stopGpsDataRecording();
		},
		[&app](){
			app.syncGpsRecords();
		}
	);

	try {

		GpsParser gpsParser;

		app.start();
		ui.init();
		app.setUI(&ui);

//INIT GPS RECORD
//		storage.openGpsDateStorage("2024-27-02T100000");
//		char dataToSave[256] = "";
//
//		for (int i =0; i<10000; i++)
//		{
//			sprintf(
//				dataToSave,
//				"%s,%s,%f,%c,%f,%c\n",
//				"2024-27-02",
//				"10:00:00",
//				50.4235,
//				'N',
//				45.5432543,
//				'E'
//			);
//
//			storage.writeGpsData(dataToSave);
//		}



		if (HAL_UARTEx_ReceiveToIdle_DMA(huart2_wifi, (uint8_t*)WIFI_BUFFER, WIFI_BUFFER_SIZE) != HAL_OK)
		{
			throw "cannot start WIFI DMA";
		}


//		app.syncData();

		gpsParser.addGpsDataChangeListener(&ui);
		gpsParser.addGpsDataChangeListener(&app);

		ui.showMenuScreen();

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


//			//TODO use PWM late
//			if (mainCounter % 100 == 0) {
//				SDCardStatus sdCardStatus = app.getSDCardStatus();
//				ui.refreshStorageData(&sdCardStatus);
//			}
//

			if (app.isGpsDataRecordingStarted())
			{
				memset(GPS_BUFFER, 0, GPS_BUFFER_SIZE);
				HAL_UART_Receive(huart1, (uint8_t *) GPS_BUFFER, GPS_BUFFER_SIZE-1, 100);
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
	static uint16_t lastPost;

	if(huart == huart2_wifi)
	{
		//no data received since the last call
		if (lastPost == size)
		{
			return;
		}

		if (size > lastPost)
		{
			uint16_t newDataSize = size - lastPost;
			char data[newDataSize];
			memcpy(data, WIFI_BUFFER+lastPost, newDataSize);

			app.onReceivedDataFromWifi(data, newDataSize);
		}
		else
		{
			uint16_t firstPartSize = WIFI_BUFFER_SIZE - lastPost;
			uint16_t secondPartSize = size;
			uint16_t newDataSize = firstPartSize + secondPartSize;
			char data[newDataSize];
			if (firstPartSize > 0)
			{
				memcpy(data, WIFI_BUFFER+lastPost, firstPartSize);
			}
			if (secondPartSize > 0)
			{
				memcpy(data+firstPartSize, WIFI_BUFFER, secondPartSize);
			}

			app.onReceivedDataFromWifi(data, newDataSize);
		}

		lastPost = size;
	}
}

void applicationErrorHandler()
{

}

