#include "ImwWindowManagerDX11.h"
#include "ImwPlatformWindowDX11.h"

#include "windows.h"

using namespace ImWindow;

ImwWindowManagerDX11::ImwWindowManagerDX11()
{
}

ImwWindowManagerDX11::~ImwWindowManagerDX11()
{
}

ImwPlatformWindow* ImwWindowManagerDX11::CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent)
{
	IM_ASSERT(m_pCurrentPlatformWindow == NULL);
	ImwPlatformWindowDX11* pWindow = new ImwPlatformWindowDX11(eType, CanCreateMultipleWindow());
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

ImVec2 ImwWindowManagerDX11::GetCursorPos()
{
	POINT oPoint;
	::GetCursorPos(&oPoint);
	return ImVec2((float)oPoint.x, (float)oPoint.y);
}

bool ImwWindowManagerDX11::IsLeftClickDown()
{
	return GetAsyncKeyState(VK_LBUTTON) != 0;
}
