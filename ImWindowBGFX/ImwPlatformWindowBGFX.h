
#ifndef __IM_PLATFORM_WINDOW_BGFX_H__
#define __IM_PLATFORM_WINDOW_BGFX_H__

#include "ImwConfig.h"
#include "ImwPlatformWindow.h"
#include "EasyWindow.h"

#include "bgfx/bgfx.h"

namespace ImWindow
{
	class ImwPlatformWindowBGFX : ImwPlatformWindow
	{
		friend class ImwWindowManagerBGFX;
	public:
											ImwPlatformWindowBGFX(EPlatformWindowType eType, bool bCreateState, bgfx::RendererType::Enum eRenderer);
		virtual								~ImwPlatformWindowBGFX();

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


		bgfx::RendererType::Enum			m_eRenderer;

		EasyWindow*							m_pWindow;
		bgfx::FrameBufferHandle				m_hFrameBufferHandle;

		bgfx::TextureHandle					m_hTexture;
		bgfx::VertexDecl					m_oVertexDecl;
		bgfx::ProgramHandle					m_hProgram;
		bgfx::UniformHandle					m_hUniformTexture;

		bgfx::DynamicVertexBufferHandle		m_hVertexBuffer;
		bgfx::DynamicIndexBufferHandle		m_hIndexBuffer;
	};
}

#endif // __IM_PLATFORM_WINDOW_BGFX_H__