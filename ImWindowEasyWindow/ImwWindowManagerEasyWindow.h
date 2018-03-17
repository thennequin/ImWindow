
#ifndef __IM_WINDOW_MANAGER_EASYWINDOW_H__
#define __IM_WINDOW_MANAGER_EASYWINDOW_H__

#include "ImwConfig.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
	class ImwWindowManagerEasyWindow : public ImwWindowManager
	{
	public:
		ImwWindowManagerEasyWindow();
		virtual							~ImwWindowManagerEasyWindow();
	protected:
		virtual bool					InternalInit() = 0;
		virtual void					InternalDestroy() = 0;

		virtual ImVec2					GetCursorPos();
		virtual bool					IsLeftClickDown();
	protected:
	};
}

#endif //__IM_WINDOW_MANAGER_EASYWINDOW_H__