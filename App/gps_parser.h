#ifndef __GPS_PARSER_H
#define __GPS_PARSER_H

#include "main.h"

class GpsParser
{
	private:
		constexpr static uint8_t maxBufferSize = 254;
		constexpr static uint8_t latitudeAndLongitudeMaxSize = 20;

		char buffer[maxBufferSize];
		uint8_t bufferSize = 0;

		char timeUtc[9] = "";
		char latitude[latitudeAndLongitudeMaxSize] = "";
		char latitudeNorS;
		char longitude[latitudeAndLongitudeMaxSize] = "";
		char longitudeEorW;
		uint8_t satellitesNumber = 0;
		uint8_t fixQuality = 0;
		bool isReceivingData = false; //TODO expire after some time
	public:
		void addData(uint8_t *data, uint8_t size);
		bool isContaisValidData();
		bool getIsReceivingData();
		const char* getTimeUtc();
		uint8_t getSatellitesNumber();
		const char* getLocation();
	private:
		void parseDataNmeaSentence();
};

#endif /* __GPS_PARSER_H */
