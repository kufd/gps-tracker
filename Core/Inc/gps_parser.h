#ifndef GPS_PARSER_H
#define GPS_PARSER_H

#include "stm32f4xx_hal.h"

#define GPS_PARSER_BUFFER_SIZE 512

class GpsParser {
		uint8_t GPS_BUFFER[GPS_PARSER_BUFFER_SIZE];
	public:
		GpsParser();
		~GpsParser();

		void addData(uint8_t *data);
};

#endif /* GPS_PARSER_H */
