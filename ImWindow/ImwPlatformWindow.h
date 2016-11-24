
#ifndef __IM_PLATFORM_WINDOW_H__
#define __IM_PLATFORM_WINDOW_H__

#include "ImwConfig.h"

#include "ImwContainer.h"
#include "JsonValue.h"

namespace ImWindow
{
//SFF_BEGIN
	class IMGUI_API ImwPlatformWindow
	{
		friend class ImwWindowManager;
	public:
											ImwPlatformWindow(bool bMainWindow, bool bIsDragWindow, bool bCreateState);
		virtual								~ImwPlatformWindow();

		virtual bool						Init(ImwPlatformWindow* pParent);

		virtual ImVec2						GetPosition() const;
		virtual ImVec2						GetSize() const;
		virtual bool						IsWindowMaximized() const;

		virtual void						Show();
		virtual void						Hide();
		virtual void						SetSize(int iWidth, int iHeight);
		virtual void						SetPosition(int iX, int iY);
		virtual void						SetWindowMaximized(bool bMaximized);
		virtual void						SetTitle(const char* pTtile);
		bool								IsShowContent() const;
		void								SetShowContent(bool bShow);

		bool								IsMain();

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
		virtual void						Destroy();
		virtual void						StartDrag();
		virtual void						StopDrag();
		virtual bool						IsDraging();

		void								PaintContainer();
		void								OnClose();

		bool								Save(JsonValue& oJson);
		bool								Load(const JsonValue& oJson, bool bJustCheck);

		bool								m_bMain;
		bool								m_bIsDragWindow;
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