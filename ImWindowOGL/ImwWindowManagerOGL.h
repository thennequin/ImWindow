
#ifndef __IM_WINDOW_MANAGER_OGL_H__
#define __IM_WINDOW_MANAGER_OGL_H__

#include "ImwConfig.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
	class ImwWindowManagerOGL : public ImwWindowManager
	{
	public:
		ImwWindowManagerOGL();
		virtual							~ImwWindowManagerOGL();
	protected:
		virtual bool					CanCreateMultipleWindow() { return true; }
		virtual ImwPlatformWindow*		CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);

		virtual ImVec2					GetCursorPos();
		virtual bool					IsLeftClickDown();
	};
}

#endif //__IM_WINDOW_MANAGER_OGL_H__