
#ifndef __IM_WINDOW_MANAGER_SOKOL_H__
#define __IM_WINDOW_MANAGER_SOKOL_H__

#include "ImwConfig.h"

#include "ImwWindowManagerEasyWindow.h"

namespace ImWindow
{
	class ImwWindowManagerSokol : public ImwWindowManagerEasyWindow
	{
	public:
		ImwWindowManagerSokol();
		virtual							~ImwWindowManagerSokol();

		virtual bool					InternalInit();
		virtual void					InternalDestroy();
	protected:
		virtual bool					CanCreateMultipleWindow() { return true; }
		virtual ImwPlatformWindow*		CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);
	};
}

#endif //__IM_WINDOW_MANAGER_SOKOL_H__