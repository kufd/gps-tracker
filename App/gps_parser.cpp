#include "logger.h"
#include "gps_parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool GpsData::isValid()
{
	return strlen(timeUtc) > 0
		&& strlen(dateUtc) > 0
		&& latitude < initialCoordinatesValue
		&& latitudeNorS != '\0'
		&& longitude < initialCoordinatesValue
		&& longitudeEorW != '\0'
		&& positionStatus == positionStatusValid;
}
void GpsData::parseTimeUtc(char* nmeaSentenceSection)
{
	if (strlen(nmeaSentenceSection) < 8)
	{
		return;
	}

	strncpy(timeUtc, nmeaSentenceSection, 2);
	strncpy((timeUtc+2), ":", 1);
	strncpy((timeUtc+3), nmeaSentenceSection+2, 2);
	strncpy((timeUtc+5), ":", 1);
	strncpy((timeUtc+6), nmeaSentenceSection+4, 2);
	timeUtc[8] = '\0';
}
void GpsData::parseDateUtc(char* nmeaSentenceSection)
{
	if (strlen(nmeaSentenceSection) != 6)
	{
		return;
	}

	strncpy(dateUtc, "20", 2);
	strncpy(dateUtc+2, nmeaSentenceSection+4, 2);
	strncpy((dateUtc+4), "-", 1);
	strncpy((dateUtc+5), nmeaSentenceSection+2, 2);
	strncpy((dateUtc+7), "-", 1);
	strncpy((dateUtc+8), nmeaSentenceSection, 2);
	dateUtc[10] = '\0';
}
void GpsData::parseLatitude(char* nmeaSentenceSection)
{
	size_t nmeaSentenceSectionLen = strlen(nmeaSentenceSection);

	if (nmeaSentenceSectionLen <= 4 || nmeaSentenceSectionLen > latitudeAndLongitudeMaxSize)
	{
		return;
	}

	char buffer[latitudeAndLongitudeMaxSize+1] = "";
	strncpy(buffer, nmeaSentenceSection, 2);
	double degree = atof(buffer);

	strncpy(buffer, nmeaSentenceSection+2, nmeaSentenceSectionLen-2);
	double minutes = atof(buffer);

	latitude = degree + minutes/60;
}
void GpsData::parseLatitudeNorS(char* nmeaSentenceSection)
{
	if (strlen(nmeaSentenceSection) != 1)
	{
		return;
	}

	latitudeNorS = nmeaSentenceSection[0];
}
void GpsData::parseLongitude(char* nmeaSentenceSection)
{
	size_t nmeaSentenceSectionLen = strlen(nmeaSentenceSection);

	if (nmeaSentenceSectionLen <= 5 || nmeaSentenceSectionLen > latitudeAndLongitudeMaxSize)
	{
		return;
	}

	char buffer[GpsData::latitudeAndLongitudeMaxSize+1] = "";
	strncpy(buffer, nmeaSentenceSection, 3);
	double degree = atof(buffer);

	strncpy(buffer, nmeaSentenceSection+3, nmeaSentenceSectionLen-2);
	double minutes = atof(buffer);

	longitude = degree + minutes/60;
}
void GpsData::parseLongitudeEorW(char* nmeaSentenceSection)
{
	if (strlen(nmeaSentenceSection) != 1)
	{
		return;
	}

	longitudeEorW = nmeaSentenceSection[0];
}
void GpsData::parsePositionStatus(char* nmeaSentenceSection)
{
	if (strlen(nmeaSentenceSection) != 1)
	{
		return;
	}

	positionStatus = nmeaSentenceSection[0];
}



GpsStatus GpsParser::getGpsStatus()
{
	if (isReceivingValidData)
	{
		return GpsStatus::VALID_DATA;
	}

	if (!isReceivingData)
	{
		return GpsStatus::NO_DATA;
	}

	return GpsStatus::INVALID_DATA;
}
bool GpsParser::getIsReceivingValidData()
{
	return isReceivingValidData;
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

void GpsParser::addData(const char* data, uint16_t dataSize)
{
	for (size_t i = 0; i < dataSize; i++)
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

			logger->debug("Happened buffer overflow in GpsParser");
		}
	}
}


void GpsParser::parseDataNmeaSentence(const char* nmeaSentence)
{
	//TODO add valid sentences percent calculation and log it
	if (!nmeaSentenceChecksumCompare(buffer)) {
		logger->debug("NMEA sentence received with invalid checksum", buffer);
		return;
	}

	logger->debug("NMEA sentence received", buffer);

	char nmeaSentenceName[7] = {0, 0, 0, 0, 0, 0, 0};
	strncpy(nmeaSentenceName, nmeaSentence, 6);

	//https://docs.novatel.com/OEM7/Content/Logs/GPRMC.htm
	if (strcmp(nmeaSentenceName, "$GPRMC") == 0)
	{
		GpsData gpsData;
		const char* nmeaSentenceCursor = nmeaSentence;
		uint8_t nmeaSentenceSectionNumber = 0;

		while (1)
		{
			char* delimiterPosition = strchr(nmeaSentenceCursor, ',');

			const size_t nmeaSentenceSectionLen = delimiterPosition != NULL ? delimiterPosition - nmeaSentenceCursor : strlen(nmeaSentenceCursor);
			char nmeaSentenceSection[nmeaSentenceSectionLen+1];
			nmeaSentenceSection[nmeaSentenceSectionLen] = '\0';
			memcpy(nmeaSentenceSection, nmeaSentenceCursor, nmeaSentenceSectionLen);

			switch(nmeaSentenceSectionNumber) {
				case 1:
					gpsData.parseTimeUtc(nmeaSentenceSection);
				break;
				case 2:
					gpsData.parsePositionStatus(nmeaSentenceSection);
				break;
				case 3:
					gpsData.parseLatitude(nmeaSentenceSection);
				break;
				case 4:
					gpsData.parseLatitudeNorS(nmeaSentenceSection);
				break;
				case 5:
					gpsData.parseLongitude(nmeaSentenceSection);
				break;
				case 6:
					gpsData.parseLongitudeEorW(nmeaSentenceSection);
				break;
				case 9:
					gpsData.parseDateUtc(nmeaSentenceSection);
				break;
			}


			if (delimiterPosition == NULL)
			{
				break;
			}

			nmeaSentenceSectionNumber++;
			nmeaSentenceCursor += nmeaSentenceSectionLen + 1;
		}

		if (gpsData.isValid()) {
			isReceivingValidData = true;

			for (int i = 0; i < this->numberGpsDataChangeListeners; i++) {
				this->gpsDataChangeListeners[i]->onGpsDataChange(gpsData);
			}
		}
		else
		{
			isReceivingValidData = false;
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



