#include "logger.h"
#include "app.h"
#include "gps_parser.h"
#include "ui.h"
#include "storage.h"
#include <string.h>
#include <stdexcept>
#include <exception>


#define GPS_BUFFER_SIZE 255
char GPS_BUFFER[GPS_BUFFER_SIZE];

UI ui;
GpsParser gpsParser;
Storage storage;
Logger logger(&storage);

class App: public GpsDataChangeListener
{
	private:
		Storage* storage;
		bool gpsDataRecordingEnabled = true;
		bool syncData = false;
	public:
		App(Storage* storage)
		{
			this->storage = storage;
		}
		void onGpsDataChange(GpsParser* gpsParser)
		{
			if (!gpsDataRecordingEnabled) {
				return;
			}

			char dataToSave[256] = "";
			char location[50] = "";
			gpsParser->getLocation(location);

			sprintf(
				dataToSave,
				"%s,%d,\"%s\"\n",
				gpsParser->getTimeUtc(),
				gpsParser->getSatellitesNumber(),
				location
			);

			storage->writeData(dataToSave);
		}
		bool getGpsDataRecordingEnabled()
		{
			return gpsDataRecordingEnabled;
		}

};

//TODO add log and debug console
void applicationMain(UART_HandleTypeDef huart1)
{
	try {
		ui.init();
		storage.open();

		App app(&storage);

		gpsParser.addGpsDataChangeListener(&ui);
		gpsParser.addGpsDataChangeListener(&app);

		ui.refreshStorageData(&storage);

		uint8_t mainCounter = 0;
		while(1) {
			memset(GPS_BUFFER, 0, GPS_BUFFER_SIZE);
			HAL_UART_Receive(&huart1, (uint8_t *) GPS_BUFFER, GPS_BUFFER_SIZE-1, 100);

			gpsParser.addData(GPS_BUFFER);

			ui.refreshSearchGpsIndicator(&gpsParser);
			ui.refreshGpsDataRecordingEnabledIndicator(app.getGpsDataRecordingEnabled());

			//TODO use PWM late
			if (mainCounter % 100 == 0) {
				ui.refreshStorageData(&storage);
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

void applicationErrorHandler()
{

}

