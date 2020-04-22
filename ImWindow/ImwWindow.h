
#ifndef __IM_WINDOW_H__
#define __IM_WINDOW_H__

#include "ImwConfig.h"

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

	enum EWindowMode
	{
		E_WINDOW_MODE_NORMAL,
		//Specials
		E_WINDOW_MODE_ALONE,
		E_WINDOW_MODE_PLACEHOLDER
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
		ImwWindow(EWindowMode eMode = E_WINDOW_MODE_NORMAL);
		virtual					~ImwWindow();
	public:
		virtual void			OnGui() = 0;
		virtual void			OnContextMenu();
		virtual void			OnDropFiles(int iCount, char** pFiles, const ImVec2& oPos);

		bool					IsFillingSpace() const;
		void					SetFillingSpace(bool bFilling);

#ifdef IMW_USE_LAYOUT_SERIALIZATION
		virtual void			GetParameters(JsonStthm::JsonValue& oOutParameters);
		virtual void			SetParameters(const JsonStthm::JsonValue& oParameters);
#endif //IMW_USE_LAYOUT_SERIALIZATION

		ImU32					GetId() const;
		const char*				GetIdStr() const;

		void					Destroy();

		void					SetTitle(const char* pTitle);
		const char*				GetTitle() const;

		void					SetClosable( bool bClosable );
		bool					IsClosable() const;

		EWindowMode				GetWindowMode() const;

		const ImVec2&			GetLastPosition() const;
		const ImVec2&			GetLastSize() const;

#ifdef IMW_CUSTOM_DECLARE_IMWWINDOW
		IMW_CUSTOM_DECLARE_IMWWINDOW
#endif //IMW_CUSTOM_DECLARE_IMWWINDOW
	private:
		char*					m_pTitle;
		ImU32					m_iId;
		char					m_pId[11];
		bool					m_bClosable;
		EWindowMode				m_eMode;
		bool					m_bFillingSpace;
		ImVec2					m_oLastPosition;
		ImVec2					m_oLastSize;

		static int				s_iNextId;
	};

	typedef ImVector<ImwWindow*> ImwWindowVector;
//SFF_END
}

#endif // __IM_WINDOW_H__