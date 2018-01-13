
#ifndef __IM_PLATFORM_WINDOW_OGL_H__
#define __IM_PLATFORM_WINDOW_OGL_H__

#include "ImwConfig.h"
#include "ImwPlatformWindow.h"
#include "EasyWindow.h"

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

namespace ImWindow
{
	class ImwPlatformWindowOGL : ImwPlatformWindow
	{
		friend class ImwWindowManagerOGL;
	public:
											ImwPlatformWindowOGL(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindowOGL();

		virtual bool						Init(ImwPlatformWindow* pMain);

		virtual ImVec2						GetPosition() const;
		virtual ImVec2						GetSize() const;
		virtual bool						IsWindowMaximized() const;
		virtual bool						IsWindowMinimized() const;

		virtual void						Show(bool bShow);
		virtual void						SetSize(int iWidth, int iHeight);
		virtual void						SetPosition(int iX, int iY);
		virtual void						SetWindowMaximized(bool bMaximized);
		virtual void						SetWindowMinimized();
		virtual void						SetTitle(const ImwChar* pTtile);

	protected:
		virtual void						PreUpdate();
		virtual void						RenderDrawLists(ImDrawData* pDrawData);

		bool								OnClose();
		void								OnFocus(bool bHasFocus);
		void								OnSize(int iWidth, int iHeight);
		void								OnMouseButton(int iButton, bool bDown);
		void								OnMouseMove(int iX, int iY);
		void								OnMouseWheel( int iStep );
		void								OnKey(EasyWindow::EKey eKey, bool bDown);
		void								OnChar(int iChar);

		EasyWindow*							m_pWindow;
		HDC									m_hDC;
		HGLRC								m_hRC;
		GLuint								m_iTextureID;
	};
}

#endif // __IM_PLATFORM_WINDOW_OGL_H__