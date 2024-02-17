#ifndef __GPS_PARSER_H
#define __GPS_PARSER_H

#include "main.h"

class GpsParser;
class GpsDataChangeListener;

class GpsDataChangeListener
{
	public:
		virtual void onGpsDataChange(GpsParser *gpsParser) = 0;
};

class GpsParser
{
	private:
		constexpr static uint8_t maxBufferSize = 255;
		constexpr static uint8_t latitudeAndLongitudeMaxSize = 20;
		constexpr static uint8_t maxGpsDataChangeListeners = 5;

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

		GpsDataChangeListener* gpsDataChangeListeners[maxGpsDataChangeListeners];
		uint8_t numberGpsDataChangeListeners = 0;
	public:
		void addData(const char* data);
		bool isContaisValidData();
		bool getIsReceivingData();
		const char* getTimeUtc();
		uint8_t getSatellitesNumber();
		void getLocation(char* location);
		void getLongitude(char* longitude);
		void getLatitude(char* latitude);
		void addGpsDataChangeListener(GpsDataChangeListener* gpsDataChangeListener);
	private:
		void parseDataNmeaSentence(const char* nmeaSentence);
		bool nmeaSentenceChecksumCompare(const char* nmeaSentence);
};

#endif /* __GPS_PARSER_H */
