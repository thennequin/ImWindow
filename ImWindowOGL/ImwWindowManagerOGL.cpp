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

bool ImwWindowManagerOGL::InternalInit()
{
	return true;
}

void ImwWindowManagerOGL::InternalDestroy()
{

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
