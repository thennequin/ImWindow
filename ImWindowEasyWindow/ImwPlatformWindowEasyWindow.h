
#ifndef __IM_PLATFORM_WINDOW_EASYWINDOW_H__
#define __IM_PLATFORM_WINDOW_EASYWINDOW_H__

#include "ImwConfig.h"
#include "ImwPlatformWindow.h"
#include "EasyWindow.h"

namespace ImWindow
{
	class ImwPlatformWindowEasyWindow : public ImwPlatformWindow
	{
	public:
											ImwPlatformWindowEasyWindow(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindowEasyWindow();

		virtual bool						Init(ImwPlatformWindow* pMain);

		virtual ImVec2						GetPosition() const;
		virtual ImVec2						GetSize() const;
		virtual bool						IsWindowMaximized() const;
		virtual bool						IsWindowMinimized() const;

		virtual void						Show(bool bShow);
		virtual void						SetSize(int iWidth, int iHeight);
		virtual void						SetPosition(int iX, int iY);
		virtual void						SetWindowMaximized(bool bMaximized);
		virtual void						SetWindowMinimized(bool bMinimized);
		virtual void						SetTitle(const char* pTtile);

	protected:
		virtual void						PreUpdate();
		virtual void						OnOverlay();
		virtual void						RenderDrawLists(ImDrawData* pDrawData) = 0;
		virtual void						OnClientSize(int iClientWidth, int iClientHeight) = 0;

		bool								OnClose(const EasyWindow* pWindow);
		void								OnSize(const EasyWindow* pWindow, int iWidth, int iHeight);
		void								OnFocus(const EasyWindow* pWindow, bool bHasFocus);
		void								OnMouseButton(const EasyWindow* pWindow, int iButton, bool bDown);
		void								OnMouseMove(const EasyWindow* pWindow, int iX, int iY);
		void								OnMouseWheel(const EasyWindow* pWindow, int iStep);
		void								OnKey(const EasyWindow* pWindow, EasyWindow::EKey eKey, bool bDown);
		void								OnChar(const EasyWindow* pWindow, int iChar);
		void								OnDropFiles(const EasyWindow* pWindow, const EasyWindow::DropFiles& oFiles);

		EasyWindow::EHoveredArea			GetHoveredArea(const EasyWindow* pWindow, int iX, int iY);

		EasyWindow*							m_pWindow;
	};
}

#endif // __IM_PLATFORM_WINDOW_EASYWINDOW_H__
