
#ifndef __IM_WINDOW_H__
#define __IM_WINDOW_H__

#include "ImwConfig.h"

#include "ImwId.h"
#include <imgui/imgui.h>

namespace ImWindow
{
	class ImwWindow
	{
		friend class ImwWindowManager;
		friend class ImwContainer;
	protected:
		ImwWindow();
		virtual					~ImwWindow();
	public:
		virtual void			OnGui() = 0;
		virtual void			OnMenu() {};

		const char*				GetId() const { return m_oId.GetStr(); }

		void					Destroy();

		void					SetTitle(const char* pTitle);
		const char*				GetTitle() const;

		const ImVec2&			GetLastPosition() const;
		const ImVec2&			GetLastSize() const;
	protected:

		char*					m_pTitle;
		ImwId					m_oId;

		ImVec2					m_oLastPosition;
		ImVec2					m_oLastSize;
	};

	typedef ImwList<ImwWindow*> ImwWindowList;
}



#endif // __IM_WINDOW_H__