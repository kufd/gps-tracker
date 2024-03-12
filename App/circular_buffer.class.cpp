#include "circular_buffer.class.h"
#include <string.h>
#include <stdlib.h>

CircularBuffer::CircularBuffer(const char* name, uint16_t bufferSize)
{
	if (strlen(name) > nameMaxLen)
	{
		throw "too long CircularBuffer name";
	}

	strcpy(this->name, name);
	this->bufferSize = bufferSize;
	buffer = (char*) malloc(this->bufferSize);
}

CircularBuffer::~CircularBuffer()
{
	free(buffer);
}

char* CircularBuffer::getBuffer()
{
	return buffer;
}

uint16_t CircularBuffer::getBufferSize()
{
	return bufferSize;
}

void CircularBuffer::subscribe(CircularBufferSubscriber* subscriber)
{
	if (subscribersNumber == subscribersMaxNumber)
	{
		throw "Reached subscribersMaxNumber";
	}

	subscribers[subscribersNumber++] = subscriber;
}

void CircularBuffer::notifySubscribers(const char *data, uint16_t dataSize)
{
	for (uint8_t i = 0; i < subscribersNumber; i++)
	{
		subscribers[i]->onCircularBufferDataReceived(name, data, dataSize);
	}
}

void CircularBuffer::onDataReceived(uint16_t size)
{
	//no data received since the last call
	if (prevSize == size)
	{
		return;
	}

	if (size > prevSize)
	{
		uint16_t dataSize = size - prevSize;
		char data[dataSize];
		memcpy(data, buffer+prevSize, dataSize);

		notifySubscribers(data, dataSize);
	}
	else
	{
		uint16_t firstPartSize = bufferSize - prevSize;
		uint16_t secondPartSize = size;
		uint16_t dataSize = firstPartSize + secondPartSize;
		char data[dataSize];
		if (firstPartSize > 0)
		{
			memcpy(data, buffer+prevSize, firstPartSize);
		}
		if (secondPartSize > 0)
		{
			memcpy(data+firstPartSize, buffer, secondPartSize);
		}

		notifySubscribers(data, dataSize);
	}

	prevSize = size;
}
