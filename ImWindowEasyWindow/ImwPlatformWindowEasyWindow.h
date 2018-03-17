
#ifndef __IM_PLATFORM_WINDOW_EASYWINDOW_H__
#define __IM_PLATFORM_WINDOW_EASYWINDOW_H__

#include "ImwConfig.h"
#include "ImwPlatformWindow.h"
#include "EasyWindow.h"

class IDXGIFactory;

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
		virtual void						SetTitle(const ImwChar* pTtile);

	protected:
		virtual void						PreUpdate();
		virtual void						RenderDrawLists(ImDrawData* pDrawData) = 0;
		virtual void						OnClientSize(int iClientWidth, int iClientHeight) = 0;

		bool								OnClose();
		void								OnSize(int iWidth, int iHeight);
		void								OnFocus(bool bHasFocus);
		void								OnMouseButton(int iButton, bool bDown);
		void								OnMouseMove(int iX, int iY);
		void								OnMouseWheel( int iStep );
		void								OnKey(EasyWindow::EKey eKey, bool bDown);
		void								OnChar(int iChar);
		void								OnDropFiles(const EasyWindow::DropFiles& oFiles);

		EasyWindow*							m_pWindow;
	};
}

#endif // __IM_PLATFORM_WINDOW_EASYWINDOW_H__