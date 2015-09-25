
#ifndef __IM_WINDOW_MANAGER_DX11_H__
#define __IM_WINDOW_MANAGER_DX11_H__

#include "ImwConfig.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
	class ImwWindowManagerDX11 : public ImwWindowManager
	{
	public:
		ImwWindowManagerDX11();
		virtual							~ImwWindowManagerDX11();
	protected:
		virtual ImwPlatformWindow*		CreatePlatformWindow(bool bMain, ImwPlatformWindow* pParent, bool bDragWindow);

		virtual void					LogFormatted(const char* pStr);

		virtual void					InternalRun();
		virtual ImVec2					GetCursorPos();
	};
}

#endif //__IM_WINDOW_MANAGER_DX11_H__