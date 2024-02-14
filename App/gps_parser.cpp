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

const char* GpsParser::getLocation()
{
	char location[latitudeAndLongitudeMaxSize*2+2+10]; //TODO is it bad return this variable outside????
	char longitudeEorWString[2] = {longitudeEorW, 0};
	char latitudeNorSString[2] = {latitudeNorS, 0};

	sprintf(location, "%s%s,%s%s", longitude, longitudeEorWString, latitude, latitudeNorSString);

	return "4532583";
}

void GpsParser::addData(uint8_t *data, uint8_t size)
{
	for (int i = 0; i < size; i++)
	{
		if (data[i] == '$') {
			isReceivingData = true;
			parseDataNmeaSentence();
			bufferSize = 0;
			memset(buffer, 0, maxBufferSize);
		}

		buffer[bufferSize] = data[i];
		++bufferSize;

		//buffer overflow should not happen. If will then clear buffer
		if (bufferSize >= maxBufferSize) {
			bufferSize = 0;
		}
	}
}


void GpsParser::parseDataNmeaSentence()
{
	char nmeaSentenceName[7] = {0, 0, 0, 0, 0, 0, 0};
	strncpy(nmeaSentenceName, (const char *)buffer, 6);

	if (strcmp(nmeaSentenceName, "$GPGGA") == 0)
	{
		char parseBuffer[bufferSize + 1];
		strncpy(parseBuffer, (const char *)buffer, bufferSize);
		parseBuffer[bufferSize] = '\0';

		char* parseBufferStart = parseBuffer;
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
			if (nmeaSentenceSectionNumber == 2 && nmeaSentenceSectionLen > 0 && nmeaSentenceSectionLen < latitudeAndLongitudeMaxSize)
			{
				strncpy(this->latitude, nmeaSentenceSection, nmeaSentenceSectionLen);
			}
			if (nmeaSentenceSectionNumber == 3 && nmeaSentenceSectionLen == 1)
			{
				this->latitudeNorS = nmeaSentenceSection[0];
			}
			if (nmeaSentenceSectionNumber == 4 && nmeaSentenceSectionLen > 0 && nmeaSentenceSectionLen < latitudeAndLongitudeMaxSize)
			{
				strncpy(this->longitude, nmeaSentenceSection, nmeaSentenceSectionLen);
			}
			if (nmeaSentenceSectionNumber == 5 && nmeaSentenceSectionLen == 1)
			{
				this->longitudeEorW = nmeaSentenceSection[0];
			}
			if (nmeaSentenceSectionNumber == 6 && nmeaSentenceSectionLen > 0)
			{
				fixQuality = atoi(nmeaSentenceSection);
			}
			if (nmeaSentenceSectionNumber == 7 && nmeaSentenceSectionLen > 0)
			{
				satellitesNumber = atoi(nmeaSentenceSection);
			}

			if (delimiterPosition == NULL)
			{
				break;
			}

			nmeaSentenceSectionNumber++;
			parseBufferStart += nmeaSentenceSectionLen + 1;
		}
	}
}



