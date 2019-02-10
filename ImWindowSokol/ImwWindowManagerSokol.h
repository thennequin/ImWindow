
#ifndef __IM_WINDOW_MANAGER_SOKOL_H__
#define __IM_WINDOW_MANAGER_SOKOL_H__

#include "ImwConfig.h"

#include "ImwWindowManagerEasyWindow.h"

#include "sokol_gfx.h"

namespace ImWindow
{
	class ImwWindowManagerSokol : public ImwWindowManagerEasyWindow
	{
		friend class ImwPlatformWindowSokol;
	public:
		ImwWindowManagerSokol(sg_desc* pSokolDesc);
		virtual							~ImwWindowManagerSokol();

		virtual bool					InternalInit();
		virtual void					InternalDestroy();
	protected:
		virtual bool					CanCreateMultipleWindow() { return true; }
		virtual ImwPlatformWindow*		CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);

		sg_desc							m_oSokolDesc;
	};
}

#endif //__IM_WINDOW_MANAGER_SOKOL_H__