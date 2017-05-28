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
	Destroy();
	bgfx::shutdown();
}

ImwPlatformWindow* ImwWindowManagerBGFX::CreatePlatformWindow(bool bMain, ImwPlatformWindow* pParent, bool bDragWindow)
{
	IM_ASSERT(m_pCurrentPlatformWindow == NULL);
	ImwPlatformWindowBGFX* pWindow = new ImwPlatformWindowBGFX(bMain, bDragWindow, CanCreateMultipleWindow(), m_eRenderer);
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

ImVec2 ImwWindowManagerBGFX::GetCursorPos()
{
	POINT oPoint;
	::GetCursorPos(&oPoint);
	return ImVec2(oPoint.x, oPoint.y);
}

bool ImwWindowManagerBGFX::IsLeftClickDown()
{
	return GetAsyncKeyState(VK_LBUTTON);
}
