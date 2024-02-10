#include <Application/gps_parser.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GPS_BUFFER_SIZE 128
uint8_t GPS_BUFFER[GPS_BUFFER_SIZE];

class GpsParser
{
	private:
		uint8_t maxBufferSize = 254;
		char buffer[254]; //TODO
		uint8_t bufferSize = 0;

		char timeUtc[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		double latitude = 0;
		double longitude = 0;
		uint8_t satellitesNumber = 0;
		uint8_t fixQuality = 0;
	public:
		void addData(uint8_t *data, uint8_t size)
		{
			for (int i = 0; i < size; i++)
			{
				if (data[i] == '$') {
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
	private:
		void parseDataNmeaSentence()
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
					memcpy(nmeaSentenceSection, parseBufferStart, nmeaSentenceSectionLen);

					if (nmeaSentenceSectionNumber == 1 && nmeaSentenceSectionLen > 0)
					{
						size_t len = strlen(nmeaSentenceSection);
						if (len == 8) {
							strncpy(this->timeUtc, nmeaSentenceSection, 8);
						}
					}
					if (nmeaSentenceSectionNumber == 2 && nmeaSentenceSectionLen > 0)
					{
						latitude = atof(nmeaSentenceSection);
					}
					if (nmeaSentenceSectionNumber == 4 && nmeaSentenceSectionLen > 0)
					{
						longitude = atof(nmeaSentenceSection);
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
};

void applicationMain(UART_HandleTypeDef huart1)
{
	GpsParser gpsParser;

	while (1)
	{
		memset(GPS_BUFFER, 0, GPS_BUFFER_SIZE);
		HAL_UART_Receive(&huart1, GPS_BUFFER, GPS_BUFFER_SIZE, 100);

		//TODO refactor
		uint8_t size = 0;
		while (size < GPS_BUFFER_SIZE && GPS_BUFFER[size] != 0) {
			++size;
		}

		gpsParser.addData(GPS_BUFFER, size);
	}
}



