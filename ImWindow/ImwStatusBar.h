
#ifndef __IM_STATUS_BAR_H__
#define __IM_STATUS_BAR_H__

#include "ImwConfig.h"

namespace ImWindow
{
//SFF_BEGIN
	class IMGUI_API ImwStatusBar
	{
	public:
		ImwStatusBar(int iHorizontalPriority = 0, bool bAutoDeleted = true);
		ImwStatusBar(const ImwStatusBar& oStatusBar);
		virtual						~ImwStatusBar();

		virtual void				OnStatusBar();

		int							GetHorizontalPriority() const;
		bool						IsAutoDeleted();
	private:
		int							m_iHorizontalPriority;
		bool						m_bAutoDeleted;
	};
	typedef ImwList<ImwStatusBar*> ImwStatusBarList;
//SFF_END
}


#endif // __IM_STATUS_BAR_H__