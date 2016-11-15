
#ifndef __IM_MENU_H__
#define __IM_MENU_H__

#include "ImwConfig.h"

namespace ImWindow
{
//SFF_BEGIN
	class IMGUI_API ImwMenu
	{
	public:
		ImwMenu(int iHorizontalPriority = 0, bool bAutoDeleted = true);
		ImwMenu(const ImwMenu& oStatusBar);
		virtual						~ImwMenu();

		virtual void				OnMenu() = 0;

		int							GetHorizontalPriority() const;
		bool						IsAutoDeleted();
	private:
		int							m_iHorizontalPriority;
		bool						m_bAutoDeleted;
	};
	typedef ImwList<ImwMenu*> ImwMenuList;
//SFF_END
}


#endif // __IM_MENU_H__