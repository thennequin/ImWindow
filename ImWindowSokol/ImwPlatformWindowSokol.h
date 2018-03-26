
#ifndef __IM_PLATFORM_WINDOW_SOKOL_H__
#define __IM_PLATFORM_WINDOW_SOKOL_H__

#include "ImwConfig.h"
#include "ImwPlatformWindowEasyWindow.h"
#include "EasyWindow.h"

#include "windows.h"
#include "sokol_gfx.h"

namespace ImWindow
{
	class ImwPlatformWindowSokol : ImwPlatformWindowEasyWindow
	{
		friend class ImwWindowManagerOGL;
	public:
											ImwPlatformWindowSokol(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindowSokol();

		virtual bool						Init(ImwPlatformWindow* pMain);

	protected:
		virtual void						RenderDrawLists(ImDrawData* pDrawData);

		virtual void						OnClientSize(int iClientWidth, int iClientHeight);

		//API specific functions
		virtual bool						SetupSokol(ImwPlatformWindow* pMain, sg_pipeline_desc* pPipelineDesc, sg_shader_desc* pShaderDesc);
		virtual void						ShutdownSokol(bool bMain);
		virtual void						OnSizeSokol(int iClientWidth, int iClientHeight);
		virtual bool						PreRenderDrawListsSokol();
		virtual void						PostRenderDrawListsSokol();

		sg_image							m_hFontTexture;
		sg_shader							m_hShader;
		sg_pipeline							m_hPipeline;
		sg_buffer							m_hVertexBuffer;
		sg_buffer							m_hIndexBuffer;
		sg_pass_action						m_oPassAction;
		sg_draw_state						m_oDrawState;

		ImDrawVert*							m_pVertexBuffer;
		uint32_t*							m_pIndexBuffer;

		static const int					c_iMaxVertices;
		static const int					c_iMaxIndices;

		char								m_oApiData[512];
	};
}

#endif // __IM_PLATFORM_WINDOW_SOKOL_H__