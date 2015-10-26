
#ifndef __IM_PLATFORM_WINDOW_H__
#define __IM_PLATFORM_WINDOW_H__

#include "ImwConfig.h"

#include "ImwContainer.h"

namespace ImWindow
{
	class IMW_API ImwPlatformWindow
	{
		friend class ImwWindowManager;
	public:
											ImwPlatformWindow(bool bMainWindow, bool bIsDragWindow);
		virtual								~ImwPlatformWindow();

		virtual bool						Init(ImwPlatformWindow* pParent) = 0;

		virtual const ImVec2&				GetPosition() const = 0;
		virtual const ImVec2&				GetSize() const = 0;

		virtual void						Show() = 0;
		virtual void						Hide() = 0;
		virtual void						SetSize(int iWidth, int iHeight) = 0;
		virtual void						SetPosition(int iX, int iY) = 0;
		virtual void						SetTitle(const char* pTtile) = 0;

		bool								IsMain();

		void								Dock(ImwWindow* pWindow);
		bool								UnDock(ImwWindow* pWindow);

		ImwContainer*						GetContainer();
		ImwContainer*						HasWindow(ImwWindow* pWindow);
		bool								IsStateSet();
	protected:
		void								SetState();
		void								RestoreState();
		void								OnLoseFocus();
		virtual void						PreUpdate() = 0;
		virtual void						Paint();
		virtual void						Destroy() = 0;
		virtual void						StartDrag() = 0;
		virtual void						StopDrag() = 0;
		virtual bool						IsDraging() = 0;

		void								PaintContainer();
		void								OnClose();

		bool								m_bMain;
		bool								m_bIsDragWindow;
		ImwContainer*						m_pContainer;
		void*								m_pState;
		void*								m_pPreviousState;
	};

	typedef ImwList<ImwPlatformWindow*> ImwPlatformWindowList;
}

#endif // __IM_PLATFORM_WINDOW_H__