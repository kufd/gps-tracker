#ifndef __CIRCULAR_BUFFER_CLASS_H
#define __CIRCULAR_BUFFER_CLASS_H

#include "main.h"

class CircularBufferSubscriber
{
	public:
		virtual void onCircularBufferDataReceived(const char *name, const char *data, uint16_t dataSize) = 0;
};

class CircularBuffer
{
	private:
		uint16_t bufferSize;
		char* buffer;

		constexpr static uint8_t nameMaxLen = 10;
		char name[nameMaxLen+1];

		constexpr static uint8_t subscribersMaxNumber = 1;
		uint8_t subscribersNumber = 0;
		CircularBufferSubscriber* subscribers[subscribersMaxNumber];

		uint16_t prevSize = 0;
	public:
		CircularBuffer(const char* name, uint16_t bufferSize);
		~CircularBuffer();
		void subscribe(CircularBufferSubscriber* subscriber);
		void onDataReceived(uint16_t size);
		char* getBuffer();
		uint16_t getBufferSize();
	private:
		void notifySubscribers(const char *data, uint16_t dataSize);
};

#endif /* __CIRCULAR_BUFFER_CLASS_H */
