
#ifndef __IM_WINDOW_MANAGER_BGFX_H__
#define __IM_WINDOW_MANAGER_BGFX_H__

#include "ImwConfig.h"

#include "ImwWindowManagerEasyWindow.h"

#include "bgfx/bgfx.h"

namespace ImWindow
{
	class ImwWindowManagerBGFX : public ImwWindowManagerEasyWindow
	{
	public:
		ImwWindowManagerBGFX(bgfx::RendererType::Enum eRenderer);
		virtual							~ImwWindowManagerBGFX();

		virtual bool					InternalInit();
		virtual void					InternalDestroy();

	protected:
		virtual bool					CanCreateMultipleWindow() { return true; }
		virtual ImwPlatformWindow*		CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);
		virtual void					PostRender();

		bgfx::RendererType::Enum		m_eRenderer;
	};
}

#endif //__IM_WINDOW_MANAGER_BGFX_H__