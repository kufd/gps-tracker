#ifndef __WIFI_H
#define __WIFI_H

#include "main.h"

class HttpResponse
{
	private:
		const char ipdToken[6] = "+IPD,";
		uint8_t ipdTokenSearchCursor = 0;
		bool ipdTokenFound = false;

		uint16_t responseSize = 0;
		char responseSizeToken[6];
		uint8_t responseSizeTokenCursor = 0;
		bool responseSizeFound = false;

		char* response;
		uint16_t responseWriteCursor = 0;
		bool responseCompleted = false;
	public:
		~ HttpResponse();
		int getResponseStatus();
		bool isComplete();
		bool isValid();
		void readChar(const char oneChar);
};

class Wifi
{
	private:
		constexpr static uint16_t circleBufferSize = 512;
		volatile char circleBuffer[circleBufferSize];
		volatile uint16_t tail = 0;
		volatile uint16_t head = 0;

		UART_HandleTypeDef* huart;

		//for debug
		uint16_t communicationCounter = 0;
		char communication[10][1024];

	public:
		void init (UART_HandleTypeDef* huart, const char *ssid, const char *password);
		bool sendPost(const char* data);
		void onReceivedDataFromHuart(const char* data, size_t size);

	private:
		bool waitFor(const char* expectedData);
		void writeToUart(const char* data);
		void resetCircleBuffer();
		void readHttpResponse(HttpResponse &httpResponse);
};

#endif /* __WIFI_H */
