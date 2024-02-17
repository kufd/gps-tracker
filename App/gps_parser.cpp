#include "logger.h"
#include "gps_parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool GpsParser::isContaisValidData()
{
	return fixQuality > 0;
}

const char* GpsParser::getTimeUtc()
{
	return timeUtc;
}

uint8_t GpsParser::getSatellitesNumber()
{
	return satellitesNumber;
}

bool GpsParser::getIsReceivingData()
{
	return isReceivingData;
}

void GpsParser::addGpsDataChangeListener(GpsDataChangeListener* gpsDataChangeListener)
{
	if (this->numberGpsDataChangeListeners == GpsParser::maxGpsDataChangeListeners) {
		throw "reached max number of gpsDataChangeListeners";
	}

	this->gpsDataChangeListeners[this->numberGpsDataChangeListeners++] = gpsDataChangeListener;
}

void GpsParser::getLocation(char* location)
{
	char longitudeEorWString[2] = {longitudeEorW, 0};
	char latitudeNorSString[2] = {latitudeNorS, 0};

	sprintf(location, "%s%s,%s%s", longitude, longitudeEorWString, latitude, latitudeNorSString);
}

void GpsParser::getLongitude(char* longitude)
{
	char longitudeEorWString[2] = {longitudeEorW, 0};

	sprintf(longitude, "%s%s", this->longitude, longitudeEorWString);
}

void GpsParser::getLatitude(char* latitude)
{
	char latitudeNorSString[2] = {latitudeNorS, 0};

	sprintf(latitude, "%s%s", this->latitude, latitudeNorSString);
}

void GpsParser::addData(const char* data)
{
	for (size_t i = 0; i < strlen(data); i++)
	{
		if (data[i] == '$') {
			isReceivingData = true;

			parseDataNmeaSentence(buffer);

			bufferSize = 0;
			memset(buffer, 0, maxBufferSize);
		}

		buffer[bufferSize] = data[i];
		++bufferSize;

		//buffer overflow should not happen. If will then clear buffer
		if (bufferSize >= maxBufferSize) {
			bufferSize = 0;
			memset(buffer, 0, maxBufferSize);
		}
	}
}


void GpsParser::parseDataNmeaSentence(const char* nmeaSentence)
{
	if (!nmeaSentenceChecksumCompare(buffer)) {
		logger.debug("NMEA sentence received with invalid checksum", buffer);
		return;
	}

	logger.debug("NMEA sentence received", buffer);

	char nmeaSentenceName[7] = {0, 0, 0, 0, 0, 0, 0};
	strncpy(nmeaSentenceName, nmeaSentence, 6);

	if (strcmp(nmeaSentenceName, "$GPGGA") == 0)
	{
		const char* parseBufferStart = nmeaSentence;
		uint8_t nmeaSentenceSectionNumber = 0;

		while (1)
		{
			char* delimiterPosition = strchr(parseBufferStart, ',');

			const size_t nmeaSentenceSectionLen = delimiterPosition != NULL ? delimiterPosition - parseBufferStart : strlen(parseBufferStart);
			char nmeaSentenceSection[nmeaSentenceSectionLen+1];
			nmeaSentenceSection[nmeaSentenceSectionLen] = '\0';
			memcpy(nmeaSentenceSection, parseBufferStart, nmeaSentenceSectionLen);

			if (nmeaSentenceSectionNumber == 1 && nmeaSentenceSectionLen >= 8)
			{
				strncpy(this->timeUtc, nmeaSentenceSection, 2);
				strncpy((this->timeUtc+2), ":", 2);
				strncpy((this->timeUtc+3), nmeaSentenceSection+2, 2);
				strncpy((this->timeUtc+5), ":", 2);
				strncpy((this->timeUtc+6), nmeaSentenceSection+4, 2);
			}
			if (nmeaSentenceSectionNumber == 2 && nmeaSentenceSectionLen > 0 && nmeaSentenceSectionLen < this->latitudeAndLongitudeMaxSize)
			{
				strncpy(this->latitude, nmeaSentenceSection, nmeaSentenceSectionLen);
			}
			if (nmeaSentenceSectionNumber == 3 && nmeaSentenceSectionLen == 1)
			{
				this->latitudeNorS = nmeaSentenceSection[0];
			}
			if (nmeaSentenceSectionNumber == 4 && nmeaSentenceSectionLen > 0 && nmeaSentenceSectionLen < this->latitudeAndLongitudeMaxSize)
			{
				strncpy(this->longitude, nmeaSentenceSection, nmeaSentenceSectionLen);
			}
			if (nmeaSentenceSectionNumber == 5 && nmeaSentenceSectionLen == 1)
			{
				this->longitudeEorW = nmeaSentenceSection[0];
			}
			if (nmeaSentenceSectionNumber == 6 && nmeaSentenceSectionLen > 0)
			{
				this->fixQuality = atoi(nmeaSentenceSection);
			}
			if (nmeaSentenceSectionNumber == 7 && nmeaSentenceSectionLen > 0)
			{
				this->satellitesNumber = atoi(nmeaSentenceSection);
				if (this->satellitesNumber > 10) {
					this->satellitesNumber = atoi(nmeaSentenceSection);
				}
			}

			if (delimiterPosition == NULL)
			{
				break;
			}

			nmeaSentenceSectionNumber++;
			parseBufferStart += nmeaSentenceSectionLen + 1;
		}

		if (isContaisValidData()) {
			for (int i = 0; i < this->numberGpsDataChangeListeners; i++) {
				this->gpsDataChangeListeners[i]->onGpsDataChange(this);
			}
		}
	}
}

bool GpsParser::nmeaSentenceChecksumCompare(const char* nmeaSentence)
{
	uint8_t nmeaSentenceLen = strlen(nmeaSentence);
	uint8_t calculatedChecksum = 0;

	if (nmeaSentenceLen < 10)
	{
		return false;
	}

	for(uint8_t i = 1; i < nmeaSentenceLen-5; ++i) //Loop from 1 to nmeaSentenceLen-5 because the checksum is calculated between $ and *
	{
		calculatedChecksum = calculatedChecksum^nmeaSentence[i];
	}

	char receivedChecksumStr[3] = {nmeaSentence[nmeaSentenceLen-4], nmeaSentence[nmeaSentenceLen-3], '\0'};
	uint8_t receivedChecksum = (uint8_t)strtol(receivedChecksumStr, NULL, 16);

	return calculatedChecksum == receivedChecksum;
}



