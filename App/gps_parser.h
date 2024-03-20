#ifndef __GPS_PARSER_H
#define __GPS_PARSER_H

#include "main.h"

class GpsParser;
class GpsDataChangeListener;

enum GpsStatus {
	NO_DATA,
	INVALID_DATA,
	VALID_DATA,
};

struct GpsData {
	public:
		constexpr static uint8_t latitudeAndLongitudeMaxSize = 20;
		constexpr static uint8_t timeUtcMaxSize = 8;
		constexpr static uint8_t dateUtcMaxSize = 10;
		constexpr static double initialCoordinatesValue = 1000;
		constexpr static char positionStatusValid = 'A';

		char timeUtc[timeUtcMaxSize+1] = "";
		char dateUtc[dateUtcMaxSize+1] = "";
		double latitude = initialCoordinatesValue;
		char latitudeNorS = '\0';
		double longitude = initialCoordinatesValue;
		char longitudeEorW = '\0';
		char positionStatus = '\0';

		bool isValid();
		void parseTimeUtc(char* nmeaSentenceSection);
		void parseDateUtc(char* nmeaSentenceSection);
		void parseLatitude(char* nmeaSentenceSection);
		void parseLatitudeNorS(char* nmeaSentenceSection);
		void parseLongitude(char* nmeaSentenceSection);
		void parseLongitudeEorW(char* nmeaSentenceSection);
		void parsePositionStatus(char* nmeaSentenceSection);
};

class GpsDataChangeListener
{
	public:
		virtual void onGpsDataChange(GpsData &gpsData) = 0;
};

class GpsParser
{
	private:
		constexpr static uint8_t maxBufferSize = 255;
		constexpr static uint8_t maxGpsDataChangeListeners = 5;

		char buffer[maxBufferSize];
		uint8_t bufferSize = 0;

		bool isReceivingData = false; //TODO expire after some time
		bool isReceivingValidData = false; //TODO expire after some time

		GpsDataChangeListener* gpsDataChangeListeners[maxGpsDataChangeListeners];
		uint8_t numberGpsDataChangeListeners = 0;
	public:
		void addData(const char* data, uint16_t dataSize);
		bool getIsReceivingValidData();
		bool getIsReceivingData();
		void addGpsDataChangeListener(GpsDataChangeListener* gpsDataChangeListener);
		GpsStatus getGpsStatus();
	private:
		void parseDataNmeaSentence(const char* nmeaSentence);
		bool nmeaSentenceChecksumCompare(const char* nmeaSentence);
};

#endif /* __GPS_PARSER_H */
