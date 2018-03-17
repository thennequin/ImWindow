
#ifndef __IM_PLATFORM_WINDOW_OGL_H__
#define __IM_PLATFORM_WINDOW_OGL_H__

#include "ImwConfig.h"
#include "ImwPlatformWindowEasyWindow.h"
#include "EasyWindow.h"

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

namespace ImWindow
{
	class ImwPlatformWindowOGL : ImwPlatformWindowEasyWindow
	{
		friend class ImwWindowManagerOGL;
	public:
											ImwPlatformWindowOGL(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindowOGL();

		virtual bool						Init(ImwPlatformWindow* pMain);

	protected:
		virtual void						RenderDrawLists(ImDrawData* pDrawData);

		void								OnClientSize(int iClientWidth, int iClientHeight);

		HDC									m_hDC;
		HGLRC								m_hRC;
		GLuint								m_iTextureID;
	};
}

#endif // __IM_PLATFORM_WINDOW_OGL_H__