
#ifndef __IM_PLATFORM_WINDOW_BGFX_H__
#define __IM_PLATFORM_WINDOW_BGFX_H__

#include "ImwConfig.h"
#include "ImwPlatformWindowEasyWindow.h"
#include "EasyWindow.h"

#include "bgfx/bgfx.h"

namespace ImWindow
{
	class ImwPlatformWindowBGFX : ImwPlatformWindowEasyWindow
	{
		friend class ImwWindowManagerBGFX;
	public:
											ImwPlatformWindowBGFX(EPlatformWindowType eType, bool bCreateState, bgfx::RendererType::Enum eRenderer);
		virtual								~ImwPlatformWindowBGFX();

		virtual bool						Init(ImwPlatformWindow* pMain);

	protected:

		virtual void						RenderDrawLists(ImDrawData* pDrawData);

		void								OnClientSize(int iClientWidth, int iClientHeight);

		bgfx::RendererType::Enum			m_eRenderer;

		bgfx::ViewId						m_iViewId;
		bgfx::FrameBufferHandle				m_hFrameBufferHandle;

		bgfx::TextureHandle					m_hTexture;
		bgfx::VertexDecl					m_oVertexDecl;
		bgfx::ProgramHandle					m_hProgram;
		bgfx::UniformHandle					m_hUniformTexture;

		bgfx::DynamicVertexBufferHandle		m_hVertexBuffer;
		bgfx::DynamicIndexBufferHandle		m_hIndexBuffer;

		static bgfx::ViewId					s_iCurrentViewId;
	};
}

#endif // __IM_PLATFORM_WINDOW_BGFX_H__