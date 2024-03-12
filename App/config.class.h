#ifndef __CONFIG_CLASS_H
#define __CONFIG_CLASS_H

#include "main.h"

class Config
{
	private:
		constexpr static uint8_t maxValueLen = 30;

		char fileName[17] = "gps-tracker.conf";
		char wifiSsid[maxValueLen+1];
		char wifiPassword[maxValueLen+1];
	public:
		void read();
		const char* getWifiSsid();
		const char* getWifiPassword();
	private:
		void initConfig();
};

#endif /* __CONFIG_CLASS_H */
