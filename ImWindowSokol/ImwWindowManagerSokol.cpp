#include "ImwWindowManagerSokol.h"
#include "ImwPlatformWindowSokol.h"

#include "windows.h"

using namespace ImWindow;

ImwWindowManagerSokol::ImwWindowManagerSokol(sg_desc* pSokolDesc)
{
	IM_ASSERT(pSokolDesc != NULL);
	m_oSokolDesc = *pSokolDesc;
}

ImwWindowManagerSokol::~ImwWindowManagerSokol()
{
	Destroy();
}

bool ImwWindowManagerSokol::InternalInit()
{
	return true;
}

void ImwWindowManagerSokol::InternalDestroy()
{

}

ImwPlatformWindow* ImwWindowManagerSokol::CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent)
{
	IM_ASSERT(m_pCurrentPlatformWindow == NULL);
	ImwPlatformWindowSokol* pWindow = new ImwPlatformWindowSokol(eType, CanCreateMultipleWindow());
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
