
#ifndef __IM_PLATFORM_WINDOW_SOKOL_H__
#define __IM_PLATFORM_WINDOW_SOKOL_H__

#include "sokol_gfx.h"

#include "ImwConfig.h"
#include "ImwPlatformWindowEasyWindow.h"

namespace ImWindow
{
	class ImwPlatformWindowSokol : ImwPlatformWindowEasyWindow
	{
		friend class ImwWindowManagerSokol;
	public:
											ImwPlatformWindowSokol(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindowSokol();

		virtual bool						Init(ImwPlatformWindow* pMain);

		char*								GetApiData();

		static void							SetPipeline(sg_pipeline hPipeline);
		static void							RestorePipeline();
		// First uniform block for vertex shader is reserved for display size parameter
		static void							SetUniformBlock(sg_shader_stage eStage, int iIndex, void* pData, int iSize);
		static void							ReleaseUniformBlock(sg_shader_stage eStage, int iIndex);
	protected:
		virtual void						RenderDrawLists(ImDrawData* pDrawData);

		virtual void						OnClientSize(int iClientWidth, int iClientHeight);

		//API specific functions
		virtual bool						SetupSokol(ImwPlatformWindow* pMain, sg_desc* pSokolDesc, sg_pipeline_desc* pPipelineDesc, sg_shader_desc* pShaderDesc);
		virtual void						ShutdownSokol(bool bMain);
		virtual void						OnSizeSokol(int iClientWidth, int iClientHeight);
		virtual bool						PreRenderDrawListsSokol();
		virtual void						PostRenderDrawListsSokol();

		void								ResizeVertexBuffer( int iNewCapacity );
		void								ResizeIndexBuffer( int iNewCapacity );

		static void							CallbackSetPipeline(const ImDrawList* pParentList, const ImDrawCmd* pCmd);
		static void							CallbackSetUniformBlockShaderStageIndexSize(const ImDrawList* pParentList, const ImDrawCmd* pCmd);
		static void							CallbackSetUniformBlockData(const ImDrawList* pParentList, const ImDrawCmd* pCmd);

		sg_image							m_hFontTexture;
		sg_shader							m_hShader;
		sg_pipeline							m_hPipeline;
		sg_buffer							m_hVertexBuffer;
		sg_buffer							m_hIndexBuffer;
		sg_pass_action						m_oPassAction;
		sg_draw_state						m_oDrawState;

		ImDrawVert*							m_pVertexBuffer;
		void*								m_pIndexBuffer;
		int									m_iVerticesCapacity;
		int									m_iIndicesCapacity;

		static const int					c_iDefaultVertexCount;
		static const int					c_iDefaultIndexCount;

		char								m_oApiData[512];

		struct UniformBlock
		{
			UniformBlock();
			void*							m_pData;
			int								m_iSize;
		};

		UniformBlock						m_oUniformBlock[2][SG_MAX_SHADERSTAGE_UBS];
		sg_shader_stage						m_eUniformBlockCurrentShaderStage;
		int									m_iUniformBlockCurrentIndex;
	};
}

#endif // __IM_PLATFORM_WINDOW_SOKOL_H__