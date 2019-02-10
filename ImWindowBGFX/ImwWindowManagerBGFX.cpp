#include "ImwWindowManagerBGFX.h"
#include "ImwPlatformWindowBGFX.h"

#include "windows.h"

using namespace ImWindow;

ImwWindowManagerBGFX::ImwWindowManagerBGFX(bgfx::RendererType::Enum eRenderer)
{
	m_eRenderer = eRenderer;
}

ImwWindowManagerBGFX::~ImwWindowManagerBGFX()
{
	
}

bool ImwWindowManagerBGFX::InternalInit()
{
	return true;
}

void ImwWindowManagerBGFX::InternalDestroy()
{
	bgfx::shutdown();
}

ImwPlatformWindow* ImwWindowManagerBGFX::CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent)
{
	IM_ASSERT(m_pCurrentPlatformWindow == NULL);
	ImwPlatformWindowBGFX* pWindow = new ImwPlatformWindowBGFX(eType, CanCreateMultipleWindow(), m_eRenderer);
	if (pWindow->Init(pParent))
	{
		return (ImwPlatformWindow*)pWindow;
	}
	else
	{
		delete pWindow;
		return NULL;
	}
}

void ImwWindowManagerBGFX::PostRender()
{
	bgfx::frame();
}

