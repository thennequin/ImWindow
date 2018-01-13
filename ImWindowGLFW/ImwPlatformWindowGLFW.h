
#ifndef __IM_PLATFORM_WINDOW_GLFW_H__
#define __IM_PLATFORM_WINDOW_GLFW_H__

#include "ImwConfig.h"
#include "ImwPlatformWindow.h"

#include "GLFW/glfw3.h"

namespace ImWindow
{
	class ImwPlatformWindowGLFW : ImwPlatformWindow
	{
		friend class ImwWindowManagerGLFW;
	public:
											ImwPlatformWindowGLFW(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindowGLFW();

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

		static void							OnClose(GLFWwindow* pWindow);
		static void							OnFocus(GLFWwindow* pWindow, int iFocus);
		static void							OnSize(GLFWwindow* pWindow, int iWidth, int iHeight);
		static void							OnMouseButton(GLFWwindow* pWindow, int iButton, int iAction, int iMods);
		static void							OnMouseMove(GLFWwindow* pWindow, double fPosX, double fPosY);
		static void							OnMouseWheel(GLFWwindow* pWindow, double fOffsetX, double fOffsetY);
		static void							OnKey(GLFWwindow* pWindow, int iKey, int iScanCode, int iAction, int iMods);
		static void							OnChar(GLFWwindow* pWindow, unsigned int iCodepoint);

		GLFWwindow*							m_pWindow;
		GLFWcursor*							m_pCursorArrow;
		GLFWcursor*							m_pCursorCrosshair;
		GLFWcursor*							m_pCursorHand;
		GLFWcursor*							m_pCursorIBeam;
		GLFWcursor*							m_pCursorHResize;
		GLFWcursor*							m_pCursorVResize;
		int									m_iLastMods;
		GLuint								m_iTextureID;
	};
}

#endif // __IM_PLATFORM_WINDOW_GLFW_H__