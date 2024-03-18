#ifndef __UI_EVENTS_DISPATCHER_H
#define __UI_EVENTS_DISPATCHER_H

#include "ui.events.h"

class UiEventsListener
{
	public:
		virtual void onUiEvent(UiEvent &uiEvent) = 0;
};

class UiEventDispatcher
{
	private:
		static const uint8_t maxListenersNum = 5;
		uint8_t listenersNum = 0;
		UiEventsListener* listeners[maxListenersNum];
	public:
		void subscribe(UiEventsListener* listener)
		{
			if (listenersNum == maxListenersNum)
			{
				throw "reached maxListenersNum";
			}

			listeners[listenersNum++] = listener;
		}
		void dispatch(UiEvent &uiEvent)
		{
			for (uint8_t i=0; i<listenersNum; i++)
			{
				listeners[i]->onUiEvent(uiEvent);
			}
		}
};

#endif
