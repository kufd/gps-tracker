#ifndef __UI_SCREEN_CLASS_H
#define __UI_SCREEN_CLASS_H

#include "ui.events.h"

class UiScreen
{
	public:
		virtual void selectButtonPressed() = 0;
		virtual void moveButtonPressed() = 0;
		virtual void show() = 0;
		virtual bool matchName(const char *name) = 0;
		virtual void onUiEvent(UiEvent &event) = 0;
};

#endif
