
#ifndef __IM_WINDOW_H__
#define __IM_WINDOW_H__

#include "ImwConfig.h"
#include "JsonValue.h"

namespace ImWindow
{
//SFF_BEGIN
	enum EDockOrientation
	{
		E_DOCK_ORIENTATION_CENTER,
		//E_DOCK_ORIENTATION_TABBED = E_DOCK_ORIENTATION_CENTER,
		E_DOCK_ORIENTATION_TOP,
		E_DOCK_ORIENTATION_LEFT,
		E_DOCK_ORIENTATION_RIGHT,
		E_DOCK_ORIENTATION_BOTTOM,
	};

#ifdef IMW_CUSTOM_BEFORE_DECLARE_IMWWINDOW
	IMW_CUSTOM_BEFORE_DECLARE_IMWWINDOW
#endif //IMW_CUSTOM_BEFORE_DECLARE_IMWWINDOW

#ifdef IMW_INHERITED_BY_IMWWINDOW
	class IMGUI_API ImwWindow : public IMW_INHERITED_BY_IMWWINDOW
#else
	class IMGUI_API ImwWindow
#endif //IMW_INHERITED_BY_IMWWINDOW
	{
		friend class ImwWindowManager;
		friend class ImwContainer;
	protected:
		ImwWindow();
		virtual					~ImwWindow();
	public:
		virtual void			OnGui() = 0;
		virtual void			OnContextMenu();

		virtual void			GetParameters(JsonValue& oOutParameters);
		virtual void			SetParameters(const JsonValue& oParameters);

		ImU32					GetId() const;
		const ImwChar*			GetIdStr() const;

		void					Destroy();

		void					SetTitle(const ImwChar* pTitle);
		const ImwChar*			GetTitle() const;

		void					SetClosable( bool bClosable );
		bool					IsClosable() const;

		void					SetAlone( bool bAlone );
		bool					IsAlone() const;

		const ImVec2&			GetLastPosition() const;
		const ImVec2&			GetLastSize() const;

#ifdef IMW_CUSTOM_DECLARE_IMWWINDOW
		IMW_CUSTOM_DECLARE_IMWWINDOW
#endif //IMW_CUSTOM_DECLARE_IMWWINDOW
	protected:
		ImwChar*				m_pTitle;
		ImU32					m_iId;
		ImwChar					m_pId[11];
		bool					m_bClosable;
		bool					m_bAlone;
		ImVec2					m_oLastPosition;
		ImVec2					m_oLastSize;

		static int				s_iNextId;
	};

	typedef ImwList<ImwWindow*> ImwWindowList;
//SFF_END
}

#endif // __IM_WINDOW_H__