#include "ImwWindowManagerGLFW.h"
#include "ImwPlatformWindowGLFW.h"

#include "windows.h"

using namespace ImWindow;

ImwWindowManagerGLFW::ImwWindowManagerGLFW()
{
	glfwInit();
}

ImwWindowManagerGLFW::~ImwWindowManagerGLFW()
{
	Destroy();
	glfwTerminate();
}

ImwPlatformWindow* ImwWindowManagerGLFW::CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent)
{
	IM_ASSERT(m_pCurrentPlatformWindow == NULL);
	ImwPlatformWindowGLFW* pWindow = new ImwPlatformWindowGLFW(eType, CanCreateMultipleWindow());
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

ImVec2 ImwWindowManagerGLFW::GetCursorPos()
{
	//TODO Make ImwWindowManagerGLFW::GetCursorPos multiplatform
	POINT oPoint;
	::GetCursorPos(&oPoint);
	return ImVec2(oPoint.x, oPoint.y);
}

bool ImwWindowManagerGLFW::IsLeftClickDown()
{
	//TODO Make ImwWindowManagerGLFW::IsLeftClickDown multiplatform
	return GetAsyncKeyState(VK_LBUTTON);
}
