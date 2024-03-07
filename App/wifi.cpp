#include "wifi.h"
#include "logger.h"
#include <string.h>
#include <stdio.h>

class ResponserWaiter
{
	private:
		constexpr static uint16_t waitForStrMaxSize = 128;
		char waitForStr[waitForStrMaxSize+1];
		uint8_t waitForStrCursor = 0;
	public:
		ResponserWaiter(const char* waitForStr)
		{
			if (strlen(waitForStr) > waitForStrMaxSize)
			{
				throw "too long string";
			}

			strcpy(this->waitForStr, waitForStr);
		}
		bool addChar(const char oneChar)
		{
			if (waitForStr[waitForStrCursor] == oneChar)
			{
				waitForStrCursor++;
				if (waitForStr[waitForStrCursor] == '\0')
				{
					return true;
				}
			}
			else
			{
				waitForStrCursor = 0;
			}

			return false;
		}
};

//==================

HttpResponse::~ HttpResponse()
{
	if (responseSize > 0)
	{
		free(response);
	}
}

int HttpResponse::getResponseStatus()
{
	if (!isValid())
	{
		throw "invalid response";
	}

	char statusStr[4] = "";
	strncpy(statusStr, response+9, 3);

	//TODO test with invalid string
	return atoi(statusStr);
}

bool HttpResponse::isComplete()
{
	return responseCompleted;
}

bool HttpResponse::isValid()
{
	if (!isComplete())
	{
		return false;
	}

	if (responseSize < 20)
	{
		return false;
	}

	return true;
}

void HttpResponse::readChar(const char oneChar)
{
	if (!ipdTokenFound)
	{
		if (ipdToken[ipdTokenSearchCursor] == oneChar)
		{
			ipdTokenSearchCursor++;
			if (ipdToken[ipdTokenSearchCursor] == '\0')
			{
				ipdTokenFound = true;
			}
		}
		else
		{
			ipdTokenSearchCursor = 0;
		}
	}

	else if (ipdTokenFound && !responseSizeFound)
	{
		if (':' == oneChar)
		{
			responseSizeFound = true;
			responseSizeToken[responseSizeTokenCursor] = '\0';
			responseSize = atoi(responseSizeToken);
			response = (char*) malloc(responseSize+1);
		}
		else
		{
			responseSizeToken[responseSizeTokenCursor++] =  oneChar;
		}
	}

	else if (ipdTokenFound && responseSizeFound && !responseCompleted)
	{
		response[responseWriteCursor++] = oneChar;

		//-1 to consider symbol ":" which was read in previous section
		if (responseWriteCursor == responseSize -1)
		{
			responseCompleted = true;
			response[responseSize] = '\0';
		}
	}
}

//==================

void Wifi::onReceivedDataFromHuart(const char* data, size_t size)
{
	if(communicationCounter < 10)
	{
		memset(communication[communicationCounter], 0, 1024);
		memcpy(communication[communicationCounter], data, size);
	}
	communicationCounter++;

	for (size_t i=0; i<size; i++)
	{
		circleBuffer[head] = data[i];
		head++;
		if (head == circleBufferSize)
		{
			head = 0;
		}
	}
}

void Wifi::resetCircleBuffer()
{
	head = 0;
	tail = 0;
}

void Wifi::init (UART_HandleTypeDef* huart, const char *ssid, const char *password)
{
	this->huart = huart;

	//TODO does not work debug further
	//maybe need to do HAL_UARTEx_ReceiveToIdle_DMA
	//however i do not need reset
	/********* AT+RST **********/
//	writeToUart("AT+RST\r\n");
//	if(!waitFor("OK\r\n"))
//	{
//		throw "cannot execute AT+RST";
//	}

	/********* AT **********/
	writeToUart("AT\r\n");
	if(!waitFor("OK\r\n"))
	{
		throw "cannot execute AT";
	}

	/********* AT+CWMODE=1 **********/
	writeToUart("AT+CWMODE=1\r\n");
	if(!waitFor("OK\r\n"))
	{
		throw "cannot execute AT+CWMODE=1";
	}


	/********* AT+CWJAP="SSID","PASSWD" **********/
	char joinApCommand[80];
	sprintf(joinApCommand, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
	writeToUart(joinApCommand);
	if(!waitFor("OK\r\n")) //TODO there can be more text
	{
		throw "cannot execute AT+CWJAP";
	}

	/********* AT+CIPMUX=0 **********/
	writeToUart("AT+CIPMUX=0\r\n");
	if(!waitFor("OK\r\n"))
	{
		throw "cannot execute AT+CIPMUX=0";
	}
}

//TODO add timeout as parameter
//currently max delay is 10 seconds
bool Wifi::waitFor(const char* expectedData)
{
	ResponserWaiter responserWaiter(expectedData);

	for (int i = 0; i < 5000; i++)
	{
		if (tail == head)
		{
			i++;
			HAL_Delay(10);
		}
		else
		{
			char symbol = circleBuffer[tail];
			++tail;
			if (tail == circleBufferSize)
			{
				tail = 0;
			}

			if (responserWaiter.addChar(symbol))
			{
				return true;
			}
		}
	}

	return false;
}

//currently max delay is 10 seconds
bool Wifi::sendPost(const char* data)
{
	writeToUart("AT+CIPSTART=\"TCP\",\"18.173.206.151\",80\r\n");
	if(!waitFor("OK\r\n"))
	{
		throw "cannot execute AT+CIPSTART";
	}

	uint16_t dataSize = strlen(data);
	char method[64] = "POST /prod/export-chunk HTTP/1.1\r\n";
	char hostHeader[64] = "Host: d1fur4uvofyjrb.cloudfront.net\r\n";
	char contentTypeHeader[64] = "Content-Type: text/csv\r\n";
	char authorizationHeader[128] = "Authorization: Z3BzVHJhY2tlclN5bmM6a2tra2FhYWRkZGtmZHNqa2FoZmdqaEpIR0dISEpKSEhKSEdHSDc2NDczOGtKVUlm\r\n";
	char contentLengthHeader[30];
	sprintf(contentLengthHeader, "Content-Length: %d\r\n", dataSize);
	char headersTermination[3] = "\r\n";
	uint16_t requestSize = strlen(method) + strlen(hostHeader) + strlen(contentTypeHeader)
			+ strlen(authorizationHeader) + strlen(contentLengthHeader) + strlen(headersTermination)
			+ dataSize;

	char command[32];
	sprintf(command, "AT+CIPSEND=%d\r\n", requestSize);

	writeToUart(command);

	if(!waitFor("OK\r\n"))
	{
		throw "cannot execute AT+CIPSEND";
	}

	writeToUart(method);
	writeToUart(hostHeader);
	writeToUart(contentTypeHeader);
	writeToUart(authorizationHeader);
	writeToUart(contentLengthHeader);
	writeToUart(headersTermination);
	writeToUart(data);

	HttpResponse httpResponse;
	readHttpResponse(httpResponse);

	writeToUart("AT+CIPCLOSE\r\n");
	if(!waitFor("OK\r\n"))
	{
		throw "cannot execute AT+CIPCLOSE";
	}

	if (!httpResponse.isValid())
	{
		throw "http response incomplete";
	}

	if (httpResponse.getResponseStatus() != 200)
	{
		throw "http error response received";
	}
}

//TODO add timeout as parameter
//currently max delay is 10 seconds
//TODO check TIMER for HAL_Delay whether it is in milliseconds
void Wifi::readHttpResponse(HttpResponse &httpResponse)
{
	for (int i = 0; i < 5000; i++)
	{
		if (tail == head)
		{
			i++;
			HAL_Delay(10);
		}
		else
		{
			char symbol = circleBuffer[tail];
			++tail;
			if (tail == circleBufferSize)
			{
				tail = 0;
			}

			httpResponse.readChar(symbol);

			if (httpResponse.isComplete())
			{
				break;
			}
		}
	}

	if (!httpResponse.isComplete())
	{
		throw "Http response is not received";
	}
}

void Wifi::writeToUart(const char* data)
{
	if(communicationCounter < 10)
	{
		memset(communication[communicationCounter], 0, 1024);
		memcpy(communication[communicationCounter], data, strlen(data));
	}
	communicationCounter++;

	logger.debug("Sent to WIFI module", data);

	HAL_StatusTypeDef res = HAL_UART_Transmit(huart, (uint8_t*) data, strlen(data), 500);

	if (res != HAL_OK)
	{
		throw "cannot send data to WIFI module";
	}
}

