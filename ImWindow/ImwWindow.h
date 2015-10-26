
#ifndef __IM_WINDOW_H__
#define __IM_WINDOW_H__

#include "ImwConfig.h"

#include "ImwId.h"

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

		ImU32					GetId() const;
		const char*				GetIdStr() const;

		void					Destroy();

		void					SetTitle(const char* pTitle);
		const char*				GetTitle() const;

		void					SetClosable( bool bClosable );
		bool					IsClosable() const;

		const ImVec2&			GetLastPosition() const;
		const ImVec2&			GetLastSize() const;

	protected:

		char*					m_pTitle;
		ImU32					m_iId;
		char					m_pId[11];
		bool					m_bClosable;

		ImVec2					m_oLastPosition;
		ImVec2					m_oLastSize;

		static int				s_iNextId;
	};

	typedef ImwList<ImwWindow*> ImwWindowList;
}



#endif // __IM_WINDOW_H__