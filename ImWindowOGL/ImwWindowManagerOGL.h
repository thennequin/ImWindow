
#ifndef __IM_WINDOW_MANAGER_OGL_H__
#define __IM_WINDOW_MANAGER_OGL_H__

#include "ImwConfig.h"

#include "ImwWindowManagerEasyWindow.h"

namespace ImWindow
{
	class ImwWindowManagerOGL : public ImwWindowManagerEasyWindow
	{
	public:
		ImwWindowManagerOGL();
		virtual							~ImwWindowManagerOGL();

		virtual bool					InternalInit();
		virtual void					InternalDestroy();
	protected:
		virtual bool					CanCreateMultipleWindow() { return true; }
		virtual ImwPlatformWindow*		CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);
	};
}

#endif //__IM_WINDOW_MANAGER_OGL_H__