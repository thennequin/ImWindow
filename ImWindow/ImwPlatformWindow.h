
#ifndef __IM_PLATFORM_WINDOW_H__
#define __IM_PLATFORM_WINDOW_H__

#include "ImwConfig.h"

#include "ImwContainer.h"
#include "JsonValue.h"

namespace ImWindow
{
//SFF_BEGIN
	enum EPlatformWindowType
	{
		E_PLATFORM_WINDOW_TYPE_MAIN,
		E_PLATFORM_WINDOW_TYPE_SECONDARY,
		E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW,
		E_PLATFORM_WINDOW_TYPE_DIALOG_BOX
	};

	class IMGUI_API ImwPlatformWindow
	{
		friend class ImwWindowManager;
	public:
											ImwPlatformWindow(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindow();

		virtual bool						Init(ImwPlatformWindow* pParent);

		EPlatformWindowType					GetType() const;
		virtual ImVec2						GetPosition() const;
		virtual ImVec2						GetSize() const;
		virtual bool						IsWindowMaximized() const;
		virtual bool						IsWindowMinimized() const;

		virtual void						Show(bool bShow);
		virtual void						SetSize(int iWidth, int iHeight);
		virtual void						SetPosition(int iX, int iY);
		virtual void						SetWindowMaximized(bool bMaximized);
		virtual void						SetWindowMinimized();
		virtual void						SetTitle(const char* pTtile);
		bool								IsShowContent() const;
		void								SetShowContent(bool bShow);

		void								Dock(ImwWindow* pWindow);
		bool								UnDock(ImwWindow* pWindow);

		ImwContainer*						GetContainer();
		ImwContainer*						HasWindow(ImwWindow* pWindow);
		bool								FocusWindow(ImwWindow* pWindow);

		bool								HasState() const;
		void								SetState();
		void								RestoreState();
		static bool							IsStateSet();
	protected:
		void								OnLoseFocus();
		virtual void						PreUpdate();
		virtual void						Render();

		void								PaintContainer();
		void								OnClose();

		bool								Save(JsonValue& oJson);
		bool								Load(const JsonValue& oJson, bool bJustCheck);

		EPlatformWindowType					m_eType;
		ImwContainer*						m_pContainer;
		void*								m_pState;
		void*								m_pPreviousState;
		bool								m_bNeedRender;
		bool								m_bShowContent;
	};

	typedef ImwList<ImwPlatformWindow*> ImwPlatformWindowList;
//SFF_END
}

#endif // __IM_PLATFORM_WINDOW_H__