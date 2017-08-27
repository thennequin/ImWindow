
#ifndef __IM_WINDOW_MANAGER_BGFX_H__
#define __IM_WINDOW_MANAGER_BGFX_H__

#include "ImwConfig.h"

#include "ImwWindowManager.h"

#include "bgfx/bgfx.h"

namespace ImWindow
{
	class ImwWindowManagerBGFX : public ImwWindowManager
	{
	public:
		ImwWindowManagerBGFX(bgfx::RendererType::Enum eRenderer);
		virtual							~ImwWindowManagerBGFX();
	protected:
		virtual bool					CanCreateMultipleWindow() { return true; }
		virtual ImwPlatformWindow*		CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);

		virtual ImVec2					GetCursorPos();
		virtual bool					IsLeftClickDown();

		bgfx::RendererType::Enum		m_eRenderer;
	};
}

#endif //__IM_WINDOW_MANAGER_BGFX_H__