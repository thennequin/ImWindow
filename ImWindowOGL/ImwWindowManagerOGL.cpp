#include "ImwWindowManagerOGL.h"
#include "ImwPlatformWindowOGL.h"

#include "windows.h"

using namespace ImWindow;

ImwWindowManagerOGL::ImwWindowManagerOGL()
{
}

ImwWindowManagerOGL::~ImwWindowManagerOGL()
{
	Destroy();
}

ImwPlatformWindow* ImwWindowManagerOGL::CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent)
{
	IM_ASSERT(m_pCurrentPlatformWindow == NULL);
	ImwPlatformWindowOGL* pWindow = new ImwPlatformWindowOGL(eType, CanCreateMultipleWindow());
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

ImVec2 ImwWindowManagerOGL::GetCursorPos()
{
	POINT oPoint;
	::GetCursorPos(&oPoint);
	return ImVec2(oPoint.x, oPoint.y);
}

bool ImwWindowManagerOGL::IsLeftClickDown()
{
	return GetAsyncKeyState(VK_LBUTTON);
}
