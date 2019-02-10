
#ifdef _DEBUG
#define SOKOL_DEBUG
#endif

#define SOKOL_IMPL
//Select implementation
//#define SOKOL_GLCORE33
//#define SOKOL_D3D11

#ifdef SOKOL_GLCORE33
	#include <windows.h>
	#include "flextGL.h"
#endif //SOKOL_GLCORE33

#ifdef SOKOL_D3D11
	#define SOKOL_D3D11_SHADER_COMPILER
#endif //SOKOL_D3D11

#include "ImwPlatformWindowSokol.h"
#include "ImwWindowManagerSokol.h"

using namespace ImWindow;

const int ImwPlatformWindowSokol::c_iDefaultVertexCount = (1 << 16); //Should be enough 
const int ImwPlatformWindowSokol::c_iDefaultIndexCount = c_iDefaultVertexCount * 3;

ImwPlatformWindowSokol::UniformBlock::UniformBlock()
{
	m_pData = NULL;
	m_iSize = 0;
}

ImwPlatformWindowSokol::ImwPlatformWindowSokol(EPlatformWindowType eType, bool bCreateState)
	: ImwPlatformWindowEasyWindow(eType, bCreateState)
	, m_hFontTexture()
	, m_hShader()
	, m_hPipeline()
	, m_hVertexBuffer()
	, m_hIndexBuffer()
	, m_oPassAction()
	, m_oDrawState()
{
	m_pVertexBuffer		= NULL;
	m_pIndexBuffer		= NULL;
	m_iVerticesCapacity	= 0;
	m_iIndicesCapacity	= 0;

	memset(m_oApiData, 0, sizeof(m_oApiData));
}

ImwPlatformWindowSokol::~ImwPlatformWindowSokol()
{
	bool bMain = GetType() == E_PLATFORM_WINDOW_TYPE_MAIN;
	
	if (bMain)
	{
		sg_destroy_image(m_hFontTexture);
	}

	sg_destroy_shader(m_hShader);
	sg_destroy_pipeline(m_hPipeline);

	sg_destroy_buffer(m_hVertexBuffer);
	sg_destroy_buffer(m_hIndexBuffer);

	ImwFree(m_pVertexBuffer);
	ImwFree(m_pIndexBuffer);

	ShutdownSokol(bMain);
}

bool ImwPlatformWindowSokol::Init(ImwPlatformWindow* pMain)
{
	if (ImwPlatformWindowEasyWindow::Init(pMain))
	{
		sg_desc& oSokolDesc = ((ImwWindowManagerSokol*)ImwWindowManager::GetInstance())->m_oSokolDesc;
		sg_pipeline_desc oPipelineDesc = { 0 };
		sg_shader_desc oShaderDesc = { 0 };
		if (SetupSokol(pMain, &oSokolDesc, &oPipelineDesc, &oShaderDesc) == false)
		{
			return false;
		}

		ImGuiIO& io = GetContext()->IO;

		if (pMain != NULL)
		{
			//Copy texture id
			m_hFontTexture = ((ImwPlatformWindowSokol*)pMain)->m_hFontTexture;
		}
		else
		{
			unsigned char* pPixels;
			int iFontWidth;
			int iFontHeight;
			io.Fonts->AddFontDefault();
			io.Fonts->GetTexDataAsRGBA32(&pPixels, &iFontWidth, &iFontHeight);

			sg_image_desc img_desc = {};
			img_desc.width = iFontWidth;
			img_desc.height = iFontHeight;
			img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
			img_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
			img_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
			img_desc.content.subimage[0][0].ptr = pPixels;
			img_desc.content.subimage[0][0].size = iFontWidth * iFontHeight * 4;
			m_hFontTexture = sg_make_image(&img_desc);

			// Store our identifier
			io.Fonts->TexID = (void *)m_hFontTexture.id;
		}

		ResizeVertexBuffer(c_iDefaultVertexCount);
		ResizeIndexBuffer(c_iDefaultIndexCount);

		m_hShader = sg_make_shader(&oShaderDesc);

		oPipelineDesc.layout.buffers[0].stride = sizeof(ImDrawVert);
		
		oPipelineDesc.shader = m_hShader;
		oPipelineDesc.index_type = (sizeof(ImDrawIdx) == 2) ? SG_INDEXTYPE_UINT16 : SG_INDEXTYPE_UINT32;
		oPipelineDesc.blend.enabled = true;
		oPipelineDesc.blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
		oPipelineDesc.blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		oPipelineDesc.blend.color_write_mask = SG_COLORMASK_RGB;
		m_hPipeline = sg_make_pipeline(&oPipelineDesc);
		m_oDrawState.pipeline = m_hPipeline;

		m_oPassAction.colors[0].action = SG_ACTION_CLEAR;
		m_oPassAction.colors[0].val[0] = 0.0f;
		m_oPassAction.colors[0].val[1] = 0.5f;
		m_oPassAction.colors[0].val[2] = 0.7f;
		m_oPassAction.colors[0].val[3] = 1.0f;

		return true;
	}
	return false;
}

char* ImwPlatformWindowSokol::GetApiData()
{
	return m_oApiData;
}

void ImwPlatformWindowSokol::SetPipeline(sg_pipeline hPipeline)
{
	ImwPlatformWindowSokol* pCurrentPlatformWindow = (ImwPlatformWindowSokol*)ImwWindowManager::GetInstance()->GetCurrentPlatformWindow();
	IM_ASSERT(pCurrentPlatformWindow != NULL);

	ImGui::GetWindowDrawList()->AddCallback(CallbackSetPipeline, (void*)hPipeline.id);
}

void ImwPlatformWindowSokol::RestorePipeline()
{
	sg_pipeline hPipeline = { 0 };
	SetPipeline(hPipeline);
}

void ImwPlatformWindowSokol::SetUniformBlock(sg_shader_stage eStage, int iIndex, void* pData, int iSize)
{
	IM_ASSERT((eStage == SG_SHADERSTAGE_FS && iIndex >= 0 && iIndex < SG_MAX_SHADERSTAGE_UBS)
		|| (eStage == SG_SHADERSTAGE_VS && iIndex > 0 && iIndex < SG_MAX_SHADERSTAGE_UBS));

	uint32_t iStageIndexSize = 
		(eStage & 0x1) | 
		(iIndex & 0x3) << 1 |
		(iSize & 0xFFFF) << 3;

	ImGui::GetWindowDrawList()->AddCallback(CallbackSetUniformBlockShaderStageIndexSize, (void*)iStageIndexSize);
	ImGui::GetWindowDrawList()->AddCallback(CallbackSetUniformBlockData, (void*)pData);
}

void ImwPlatformWindowSokol::ReleaseUniformBlock(sg_shader_stage eStage, int iIndex)
{
	SetUniformBlock(eStage, iIndex, NULL, 0);
}

void ImwPlatformWindowSokol::CallbackSetPipeline(const ImDrawList* pParentList, const ImDrawCmd* pCmd)
{
	ImwPlatformWindowSokol* pCurrentPlatformWindow = (ImwPlatformWindowSokol*)ImwWindowManager::GetInstance()->GetCurrentPlatformWindow();
	IM_ASSERT(pCurrentPlatformWindow != NULL);

	if (pCurrentPlatformWindow != NULL)
	{
		uint32_t iPipelineId = (uint32_t)pCmd->UserCallbackData;
		if (iPipelineId == 0)
		{
			pCurrentPlatformWindow->m_oDrawState.pipeline = pCurrentPlatformWindow->m_hPipeline;
		}
		else
		{
			pCurrentPlatformWindow->m_oDrawState.pipeline.id = iPipelineId;
		}
	}
}

void ImwPlatformWindowSokol::CallbackSetUniformBlockShaderStageIndexSize(const ImDrawList* pParentList, const ImDrawCmd* pCmd)
{
	ImwPlatformWindowSokol* pCurrentPlatformWindow = (ImwPlatformWindowSokol*)ImwWindowManager::GetInstance()->GetCurrentPlatformWindow();
	IM_ASSERT(pCurrentPlatformWindow != NULL);

	if (pCurrentPlatformWindow != NULL)
	{
		uint32_t iStageIndexSize = (uint32_t)pCmd->UserCallbackData;
		int eStage = (iStageIndexSize & 0x1);
		int iIndex = (iStageIndexSize >> 1) & 0x3;
		int iSize = (iStageIndexSize >> 3) & 0xFFFF;

		pCurrentPlatformWindow->m_eUniformBlockCurrentShaderStage = (sg_shader_stage)eStage;
		pCurrentPlatformWindow->m_iUniformBlockCurrentIndex = iIndex;
		pCurrentPlatformWindow->m_oUniformBlock[eStage][iIndex].m_iSize = iSize;
	}
}

void ImwPlatformWindowSokol::CallbackSetUniformBlockData(const ImDrawList* pParentList, const ImDrawCmd* pCmd)
{
	ImwPlatformWindowSokol* pCurrentPlatformWindow = (ImwPlatformWindowSokol*)ImwWindowManager::GetInstance()->GetCurrentPlatformWindow();
	IM_ASSERT(pCurrentPlatformWindow != NULL);

	if (pCurrentPlatformWindow != NULL)
	{
		int iShaderStage = pCurrentPlatformWindow->m_eUniformBlockCurrentShaderStage;
		int iBlockIndex = pCurrentPlatformWindow->m_iUniformBlockCurrentIndex;
		pCurrentPlatformWindow->m_oUniformBlock[iShaderStage][iBlockIndex].m_pData = pCmd->UserCallbackData;
	}
}

void ImwPlatformWindowSokol::OnClientSize(int iClientWidth, int iClientHeight)
{
	OnSizeSokol(iClientWidth, iClientHeight);
}

void ImwPlatformWindowSokol::RenderDrawLists(ImDrawData* pDrawData)
{
	if (PreRenderDrawListsSokol() == false)
		return;

	ImVec2 vDisplaySize = ImGui::GetIO().DisplaySize;
	sg_begin_default_pass(&m_oPassAction, (int)vDisplaySize.x, (int)vDisplaySize.y);
	if (pDrawData->CmdListsCount > 0)
	{
		{ // Resize buffers if they are not big enough
			int iNeededVertices = 0;
			int iNeededIndices = 0;
			for (int iNumCmdlists = 0; iNumCmdlists < pDrawData->CmdListsCount; iNumCmdlists++)
			{
				const ImDrawList* cl = pDrawData->CmdLists[iNumCmdlists];
				iNeededVertices += cl->VtxBuffer.size();
				iNeededIndices	+= cl->IdxBuffer.size();
			}
			ResizeVertexBuffer(iNeededVertices);
			ResizeIndexBuffer(iNeededIndices);
		}
		// copy vertices and indices
		int iNumVertices = 0;
		int iNumIndices = 0;
		int iNumCmdlists = 0;
		for (iNumCmdlists = 0; iNumCmdlists < pDrawData->CmdListsCount; iNumCmdlists++)
		{
			const ImDrawList* cl = pDrawData->CmdLists[iNumCmdlists];
			const int iCmdNumVertices = cl->VtxBuffer.size();
			const int iCmdNumIndices = cl->IdxBuffer.size();

			// copy vertices
			memcpy(&m_pVertexBuffer[iNumVertices], &cl->VtxBuffer.front(), iCmdNumVertices * sizeof(ImDrawVert));

			// copy indices, need to 'rebase' indices to start of global vertex buffer
			const ImDrawIdx* pSrcIndex = &cl->IdxBuffer.front();
			const uint32_t iBaseVertexIndex = iNumVertices;
			ImDrawIdx* pDstIndex = (ImDrawIdx*)m_pIndexBuffer;
			for (int i = 0; i < iCmdNumIndices; i++)
			{
				pDstIndex[iNumIndices++] = pSrcIndex[i] + iBaseVertexIndex;
			}
			iNumVertices += iCmdNumVertices;
		}

		// update vertex and index buffers
		const int iVertexDataSize = iNumVertices * sizeof(ImDrawVert);
		const int iIndexDataSize = iNumIndices * sizeof(ImDrawIdx);
		sg_update_buffer(m_oDrawState.vertex_buffers[0], m_pVertexBuffer, iVertexDataSize);
		sg_update_buffer(m_oDrawState.index_buffer, m_pIndexBuffer, iIndexDataSize);

		// render the command list
		int iBaseElement = 0;
		for (int iCmdIndex = 0; iCmdIndex < iNumCmdlists; iCmdIndex++)
		{
			const ImDrawList* pCmdList = pDrawData->CmdLists[iCmdIndex];
			for (const ImDrawCmd& oCmd : pCmdList->CmdBuffer)
			{
				if (oCmd.UserCallback)
				{
					oCmd.UserCallback(pCmdList, &oCmd);
				}
				else
				{
					m_oDrawState.fs_images[0].id = (uint32_t)oCmd.TextureId;
					sg_apply_draw_state(&m_oDrawState);
					sg_apply_uniform_block(SG_SHADERSTAGE_VS, 0, &vDisplaySize, sizeof(vDisplaySize));

					//Start at 1 because 0 is reversed by DisplaySize
					for (int iVertexIndex = 1; iVertexIndex < SG_MAX_SHADERSTAGE_UBS; ++iVertexIndex)
					{
						const UniformBlock& oUniformBlock = m_oUniformBlock[0][iVertexIndex];
						if (oUniformBlock.m_pData != NULL)
							sg_apply_uniform_block(SG_SHADERSTAGE_VS, iVertexIndex, oUniformBlock.m_pData, oUniformBlock.m_iSize);
					}

					for (int iFragmentIndex = 0; iFragmentIndex < SG_MAX_SHADERSTAGE_UBS; ++iFragmentIndex)
					{
						const UniformBlock& oUniformBlock = m_oUniformBlock[1][iFragmentIndex];
						if (oUniformBlock.m_pData != NULL)
							sg_apply_uniform_block(SG_SHADERSTAGE_FS, iFragmentIndex, oUniformBlock.m_pData, oUniformBlock.m_iSize);
					}
					
					const int sx = (int)oCmd.ClipRect.x;
					const int sy = (int)oCmd.ClipRect.y;
					const int sw = (int)(oCmd.ClipRect.z - oCmd.ClipRect.x);
					const int sh = (int)(oCmd.ClipRect.w - oCmd.ClipRect.y);
					sg_apply_scissor_rect(sx, sy, sw, sh, true);
					sg_draw(iBaseElement, oCmd.ElemCount, 1);
				}
				iBaseElement += oCmd.ElemCount;
			}
		}
	}

	sg_end_pass();
	sg_commit();

	PostRenderDrawListsSokol();
}

void ImwPlatformWindowSokol::ResizeVertexBuffer(int iNewCapacity)
{
	if (iNewCapacity > m_iVerticesCapacity)
	{
		if (m_iVerticesCapacity > 0)
		{
			ImwFree(m_pVertexBuffer);
			sg_destroy_buffer(m_hVertexBuffer);
		}

		m_iVerticesCapacity = iNewCapacity;
		m_pVertexBuffer = (ImDrawVert*)ImwMalloc(sizeof(ImDrawVert) * m_iVerticesCapacity);
		sg_buffer_desc oVertexBufferDesc = { 0 };
		oVertexBufferDesc.type = SG_BUFFERTYPE_VERTEXBUFFER;
		oVertexBufferDesc.usage = SG_USAGE_STREAM;
		oVertexBufferDesc.size = sizeof(ImDrawVert) * m_iVerticesCapacity;
		m_hVertexBuffer = sg_make_buffer(&oVertexBufferDesc);
		m_oDrawState.vertex_buffers[ 0 ] = m_hVertexBuffer;
	}
}

void ImwPlatformWindowSokol::ResizeIndexBuffer(int iNewCapacity)
{
	if (iNewCapacity > m_iIndicesCapacity)
	{
		if (m_iIndicesCapacity > 0)
		{
			ImwFree(m_pIndexBuffer);
			sg_destroy_buffer(m_hIndexBuffer);
		}
		m_iIndicesCapacity = iNewCapacity;

		m_pIndexBuffer = ImwMalloc(sizeof(ImDrawIdx) * m_iIndicesCapacity);
		sg_buffer_desc oIndexBufferDesc = { 0 };
		oIndexBufferDesc.type = SG_BUFFERTYPE_INDEXBUFFER;
		oIndexBufferDesc.usage = SG_USAGE_STREAM;
		oIndexBufferDesc.size = sizeof(ImDrawIdx) * m_iIndicesCapacity;
		m_hIndexBuffer = sg_make_buffer(&oIndexBufferDesc);
		m_oDrawState.index_buffer = m_hIndexBuffer;
	}
}

/////////////////////////////////////////
// OpenGL backend
/////////////////////////////////////////

#ifdef SOKOL_GLCORE33 

typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
HGLRC WINAPI wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList);

struct SpecificDataOpenGL
{
	HDC									m_hDC;
	HGLRC								m_hGLRC;
};

bool ImwPlatformWindowSokol::SetupSokol(ImwPlatformWindow* pMain, sg_desc* pSokolDesc, sg_pipeline_desc* pPipelineDesc, sg_shader_desc* pShaderDesc)
{

	SpecificDataOpenGL& oApiData = *(SpecificDataOpenGL*)m_oApiData;

	PIXELFORMATDESCRIPTOR oPixelFormatDescriptor;
	memset(&oPixelFormatDescriptor, 0, sizeof(oPixelFormatDescriptor));
	oPixelFormatDescriptor.nVersion = 1;
	oPixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	oPixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	oPixelFormatDescriptor.cColorBits = 32;
	oPixelFormatDescriptor.cDepthBits = 16;
	oPixelFormatDescriptor.cStencilBits = 0;
	oPixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;

	oApiData.m_hDC = GetDC((HWND)m_pWindow->GetHandle());
	if (oApiData.m_hDC == 0)
	{
		printf("Error: Can't Create A GL Device Context.");
		return false;
	}

	GLuint iPixelFormat = ChoosePixelFormat(oApiData.m_hDC, &oPixelFormatDescriptor);
	if (iPixelFormat == 0)
	{
		printf("Error: Can't Find A Suitable PixelFormat.");
		return false;
	}

	if (SetPixelFormat(oApiData.m_hDC, iPixelFormat, &oPixelFormatDescriptor) == false)
	{
		printf("Error: Can't Set The PixelFormat.");
		return false;
	}

	if (pMain != NULL)
	{
		oApiData.m_hGLRC = ((SpecificDataOpenGL*)((ImwPlatformWindowSokol*)pMain)->m_oApiData)->m_hGLRC;
	}
	else
	{
		oApiData.m_hGLRC = wglCreateContext(oApiData.m_hDC);

		if (oApiData.m_hGLRC == 0)
		{
			printf("Error: Can't create the OpenGL context.");
			return false;
		}

		if (!wglMakeCurrent(oApiData.m_hDC, oApiData.m_hGLRC))
		{
			printf("Error: Can't enable OpenGL context.");
			return false;
		}

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092

		const int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			0 };

		if (wglCreateContextAttribsARB == NULL)
		{
			printf("Error: Can't retrieve wglCreateContextAttribsARB");
			return false;
		}

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(oApiData.m_hGLRC);

		oApiData.m_hGLRC = wglCreateContextAttribsARB(oApiData.m_hDC, 0, attribs);
		if (oApiData.m_hGLRC == 0)
		{
			printf("Error: Can't create OpenGL context.");
			return false;
		}

		if (!wglMakeCurrent(oApiData.m_hDC, oApiData.m_hGLRC))
		{
			printf("Error: Can't enable OpenGL context.");
			return false;
		}
	}

	if (pMain == NULL)
	{
		flextInit();

		sg_setup(pSokolDesc);
		if (sg_isvalid() == false)
		{
			printf("Error: Can't setup Sokol.");
			return false;
		}
	}

	//Fill Shader desc
	pShaderDesc->vs.uniform_blocks[0].size = sizeof(ImVec2);
	pShaderDesc->vs.uniform_blocks[0].uniforms[0].name = "display_size";
	pShaderDesc->vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT2;
	pShaderDesc->vs.source =
		"#version 330\n"
		"uniform vec2 display_size;\n"
		"in vec2 vertex_pos;\n"
		"in vec2 vertex_uv;\n"
		"in vec4 vertex_color;\n"
		"out vec2 uv;\n"
		"out vec4 color;\n"
		"void main() {\n"
		"    gl_Position = vec4(((vertex_pos/display_size)-0.5)*vec2(2.0,-2.0), 0.5, 1.0);\n"
		"    uv = vertex_uv;\n"
		"    color = vertex_color;\n"
		"}\n";
	pShaderDesc->fs.images[0].name = "tex";
	pShaderDesc->fs.images[0].type = SG_IMAGETYPE_2D;
	pShaderDesc->fs.source =
		"#version 330\n"
		"uniform sampler2D tex;\n"
		"in vec2 uv;\n"
		"in vec4 color;\n"
		"void main() {\n"
		"    gl_FragColor = texture(tex, uv) * color;\n"
		"}\n";

	// Fill pipieline desc for vertex layout of previous shader
	pPipelineDesc->layout.attrs[0].name = "vertex_pos";
	pPipelineDesc->layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2;
	pPipelineDesc->layout.attrs[1].name = "vertex_uv";
	pPipelineDesc->layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
	pPipelineDesc->layout.attrs[2].name = "vertex_color";
	pPipelineDesc->layout.attrs[2].format = SG_VERTEXFORMAT_UBYTE4N;

	return true;
}

void ImwPlatformWindowSokol::ShutdownSokol(bool bMain)
{
	SpecificDataOpenGL& oApiData = *(SpecificDataOpenGL*)m_oApiData;
	if (bMain)
	{
		sg_shutdown();
		wglDeleteContext(oApiData.m_hGLRC);
	}

	ReleaseDC((HWND)m_pWindow->GetHandle(), oApiData.m_hDC);
}

void ImwPlatformWindowSokol::OnSizeSokol(int /*iClientWidth*/, int /*iClientHeight*/)
{
	//Nothing to do with OpenGL
}

bool ImwPlatformWindowSokol::PreRenderDrawListsSokol()
{
	SpecificDataOpenGL& oApiData = *(SpecificDataOpenGL*)m_oApiData;
	if (!wglMakeCurrent(oApiData.m_hDC, oApiData.m_hGLRC))
	{
		printf("Error: Can't enable OpenGL econtext.");
		return false;
	}

	return true;
}

void ImwPlatformWindowSokol::PostRenderDrawListsSokol()
{
	SpecificDataOpenGL& oApiData = *(SpecificDataOpenGL*)m_oApiData;
	SwapBuffers( oApiData.m_hDC );
}

#endif //SOKOL_GLCORE33



/////////////////////////////////////////
// DirectX 11 backend
/////////////////////////////////////////

#ifdef SOKOL_D3D11

struct SpecificDataD3D11
{
	//Shared
	IDXGIFactory*						m_pDXGIFactory;
	ID3D11Device*						m_pDX11Device;
	ID3D11DeviceContext*				m_pDX11DeviceContext;

	//Unique
	DXGI_SWAP_CHAIN_DESC				m_oSwapChainDesc;

	IDXGISwapChain*						m_pDXGISwapChain;
	ID3D11RenderTargetView*				m_pDX11RenderTargetView;

	ID3D11Texture2D*					m_pDX11DepthStencilBuffer;
	ID3D11DepthStencilView*				m_pDX11DepthStencilView;
};

const void* GetD3D11RenderTargetViewCb()
{
	ImwPlatformWindowSokol* pCurrentPlatform = (ImwPlatformWindowSokol*)ImwWindowManager::GetInstance()->GetCurrentPlatformWindow();
	IM_ASSERT(pCurrentPlatform != NULL);
	if (pCurrentPlatform != NULL)
	{
		return ( (SpecificDataD3D11*)pCurrentPlatform->GetApiData() )->m_pDX11RenderTargetView;
	}
	return NULL;
}

const void* GetD3D11DepthStencilViewCb()
{
	ImwPlatformWindowSokol* pCurrentPlatform = (ImwPlatformWindowSokol*)ImwWindowManager::GetInstance()->GetCurrentPlatformWindow();
	IM_ASSERT( pCurrentPlatform != NULL );
	if( pCurrentPlatform != NULL )
	{
		return ( (SpecificDataD3D11*)pCurrentPlatform->GetApiData() )->m_pDX11DepthStencilView;
	}
	return NULL;
}

bool ImwPlatformWindowSokol::SetupSokol( ImwPlatformWindow* pMain, sg_desc* pSokolDesc, sg_pipeline_desc* pPipelineDesc, sg_shader_desc* pShaderDesc )
{
	SpecificDataD3D11& oApiData = *(SpecificDataD3D11*)m_oApiData;
	
	if( pMain == NULL )
	{
		int iResult = CreateDXGIFactory( IID_IDXGIFactory, (void**)&oApiData.m_pDXGIFactory );
		if( FAILED( iResult ) )
		{
			CHAR pMsg[ 1024 ] = { 0 };
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
			MessageBox( NULL, pMsg, TEXT( "Can't create FXGI factory" ), MB_ICONERROR | MB_OK );
			return false;
		}

		iResult = D3D11CreateDevice( NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&oApiData.m_pDX11Device,
			NULL,
			&oApiData.m_pDX11DeviceContext );

		if( FAILED( iResult ) )
		{
			CHAR pMsg[ 1024 ] = { 0 };
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
			MessageBox( NULL, pMsg, TEXT( "Can't create DX11 device and device context" ), MB_ICONERROR | MB_OK );
			return false;
		}

		sg_desc oSokolDesc = *pSokolDesc;
		oSokolDesc.d3d11_device = oApiData.m_pDX11Device;
		oSokolDesc.d3d11_device_context = oApiData.m_pDX11DeviceContext;
		oSokolDesc.d3d11_render_target_view_cb = GetD3D11RenderTargetViewCb;
		oSokolDesc.d3d11_depth_stencil_view_cb = GetD3D11DepthStencilViewCb;
		sg_setup(&oSokolDesc);
		if( sg_isvalid() == false )
		{
			printf( "Error: Can't setup Sokol." );
			return false;
		}
	}
	else
	{
		oApiData.m_pDXGIFactory = ( (SpecificDataD3D11*)( (ImwPlatformWindowSokol*)pMain )->m_oApiData )->m_pDXGIFactory;
		oApiData.m_pDX11Device = ( (SpecificDataD3D11*)( (ImwPlatformWindowSokol*)pMain )->m_oApiData )->m_pDX11Device;
		oApiData.m_pDX11DeviceContext = ( (SpecificDataD3D11*)( (ImwPlatformWindowSokol*)pMain )->m_oApiData )->m_pDX11DeviceContext;
	} 

	// Create swap chain

	
	ZeroMemory( &oApiData.m_oSwapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );
	oApiData.m_oSwapChainDesc.BufferCount = 1;
	oApiData.m_oSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	oApiData.m_oSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	oApiData.m_oSwapChainDesc.OutputWindow = (HWND)m_pWindow->GetHandle();
	oApiData.m_oSwapChainDesc.SampleDesc.Count = 1;
	oApiData.m_oSwapChainDesc.Windowed = true;

	HRESULT iResult;
	
	iResult = IDXGIFactory_CreateSwapChain( oApiData.m_pDXGIFactory, (IUnknown*)oApiData.m_pDX11Device, &oApiData.m_oSwapChainDesc, &oApiData.m_pDXGISwapChain );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error: Can't create swap chain" ), MB_ICONERROR | MB_OK );
		return false;
	}

	iResult = IDXGIFactory_MakeWindowAssociation( oApiData.m_pDXGIFactory, (HWND)m_pWindow->GetHandle(), DXGI_MWA_NO_ALT_ENTER );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : DXGI MakeWindowAssociation failed!" ), MB_ICONERROR | MB_OK );
		//return false;
	}

	//Create our BackBuffer
	ID3D11Texture2D* pBackBuffer;
	iResult = IDXGISwapChain_GetBuffer( oApiData.m_pDXGISwapChain, 0, IID_ID3D11Texture2D, (void**)&pBackBuffer );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't get Buffer of swapchain" ), MB_ICONERROR | MB_OK );
		return false;
	}

	//Create our Render Target
	iResult = ID3D11Device_CreateRenderTargetView( oApiData.m_pDX11Device, (ID3D11Resource*)pBackBuffer, NULL, &oApiData.m_pDX11RenderTargetView );
	ID3D11Texture2D_Release(pBackBuffer);
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't create RenderTargetView" ), MB_ICONERROR | MB_OK );
		return false;
	}

	int iClientWidth, iClientHeight;
	m_pWindow->GetClientSize( &iClientWidth, &iClientHeight );

	D3D11_TEXTURE2D_DESC oDepthStencilDesc;
	ZeroMemory( &oDepthStencilDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
	oDepthStencilDesc.Width = iClientWidth;
	oDepthStencilDesc.Height = iClientHeight;
	oDepthStencilDesc.MipLevels = 1;
	oDepthStencilDesc.ArraySize = 1;
	oDepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	oDepthStencilDesc.SampleDesc = oApiData.m_oSwapChainDesc.SampleDesc;
	oDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	oDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	iResult = ID3D11Device_CreateTexture2D( oApiData.m_pDX11Device, &oDepthStencilDesc, NULL, &oApiData.m_pDX11DepthStencilBuffer );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't create DepthStencil texture" ), MB_ICONERROR | MB_OK );
		return false;
	}

	
	D3D11_DEPTH_STENCIL_VIEW_DESC oDepthStencilViewDesc;
	ZeroMemory( &oDepthStencilViewDesc, sizeof( D3D11_DEPTH_STENCIL_VIEW_DESC ) );
	oDepthStencilViewDesc.Format = oDepthStencilDesc.Format;
	oDepthStencilViewDesc.ViewDimension = oApiData.m_oSwapChainDesc.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

	iResult = ID3D11Device_CreateDepthStencilView( oApiData.m_pDX11Device, (ID3D11Resource*)oApiData.m_pDX11DepthStencilBuffer, &oDepthStencilViewDesc, &oApiData.m_pDX11DepthStencilView );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't create DepthStencil view" ), MB_ICONERROR | MB_OK );
		return false;
	}

	//Set our Render Target
	//ID3D11DeviceContext_OMSetRenderTargets( oApiData.m_pDX11DeviceContext, 1, &oApiData.m_pDX11RenderTargetView, NULL );

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)
	//Fill Shader desc
	pShaderDesc->vs.uniform_blocks[ 0 ].size = sizeof( ImVec2 );
	pShaderDesc->vs.source =
		//"#line " STRINGIFY(__LINE__) " \"" __FILE__ "\"\n"
		"cbuffer params {\n"
		"  float2 disp_size;\n"
		"};\n"
		"struct vs_in {\n"
		"  float2 pos: POSITION;\n"
		"  float2 uv: TEXCOORD0;\n"
		"  float4 color: COLOR0;\n"
		"};\n"
		"struct vs_out {\n"
		"  float2 uv: TEXCOORD0;\n"
		"  float4 color: COLOR0;\n"
		"  float4 pos: SV_Position;\n"
		"};\n"
		"vs_out main(vs_in inp) {\n"
		"  vs_out outp;\n"
		"  outp.pos = float4(((inp.pos/disp_size)-0.5)*float2(2.0,-2.0), 0.5, 1.0);\n"
		"  outp.uv = inp.uv;\n"
		"  outp.color = inp.color;\n"
		"  return outp;\n"
		"}\n";

	pShaderDesc->fs.images[ 0 ].type = SG_IMAGETYPE_2D;
	pShaderDesc->fs.source =
		//"#line " STRINGIFY( __LINE__ ) " \"" __FILE__ "\"\n"
		"Texture2D<float4> tex: register(t0);\n"
		"sampler smp: register(s0);\n"
		"float4 main(float2 uv: TEXCOORD0, float4 color: COLOR0): SV_Target0 {\n"
		"  return tex.Sample(smp, uv) * color;\n"
		"}\n";

	// Fill pipieline desc for vertex layout of previous shader
	pPipelineDesc->layout.attrs[ 0 ].sem_name = "POSITION";
	pPipelineDesc->layout.attrs[ 0 ].offset = offsetof( ImDrawVert, pos );
	pPipelineDesc->layout.attrs[ 0 ].format = SG_VERTEXFORMAT_FLOAT2;
	pPipelineDesc->layout.attrs[ 1 ].sem_name = "TEXCOORD";
	pPipelineDesc->layout.attrs[ 1 ].offset = offsetof( ImDrawVert, uv );
	pPipelineDesc->layout.attrs[ 1 ].format = SG_VERTEXFORMAT_FLOAT2;
	pPipelineDesc->layout.attrs[ 2 ].sem_name = "COLOR";
	pPipelineDesc->layout.attrs[ 2 ].offset = offsetof( ImDrawVert, col );
	pPipelineDesc->layout.attrs[ 2 ].format = SG_VERTEXFORMAT_UBYTE4N;

	return true;
}

void ImwPlatformWindowSokol::ShutdownSokol( bool bMain )
{
	SpecificDataD3D11& oApiData = *(SpecificDataD3D11*)m_oApiData;
	if( bMain )
	{
		sg_shutdown();
		//wglDeleteContext( oApiData.m_hGLRC );
	}

	//ReleaseDC( (HWND)m_pWindow->GetHandle(), oApiData.m_hDC );
}

void ImwPlatformWindowSokol::OnSizeSokol( int iClientWidth, int iClientHeight )
{
	SpecificDataD3D11& oApiData = *(SpecificDataD3D11*)m_oApiData;

	ID3D11RenderTargetView_Release( oApiData.m_pDX11RenderTargetView );
	ID3D11RenderTargetView_Release( oApiData.m_pDX11DepthStencilView );
	ID3D11Texture2D_Release( oApiData.m_pDX11DepthStencilBuffer );

	HRESULT iResult;
	iResult = IDXGISwapChain_ResizeBuffers( oApiData.m_pDXGISwapChain, 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't resize swap chain" ), MB_ICONERROR | MB_OK );
		return;
	}

	// Get buffer and create a render-target-view.
	ID3D11Texture2D* pBuffer;
	IDXGISwapChain_GetBuffer(oApiData.m_pDXGISwapChain, 0, IID_ID3D11Texture2D, (void**)&pBuffer );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't get swap chain buffer" ), MB_ICONERROR | MB_OK );
		return;
	}

	iResult = ID3D11Device_CreateRenderTargetView( oApiData.m_pDX11Device, ( ID3D11Resource*)pBuffer, NULL, &oApiData.m_pDX11RenderTargetView );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't create RenderTarget view" ), MB_ICONERROR | MB_OK );
		return;
	}

	ID3D11Texture2D_Release( pBuffer );

	D3D11_TEXTURE2D_DESC oDepthStencilDesc;
	ZeroMemory( &oDepthStencilDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
	oDepthStencilDesc.Width = iClientWidth;
	oDepthStencilDesc.Height = iClientHeight;
	oDepthStencilDesc.MipLevels = 1;
	oDepthStencilDesc.ArraySize = 1;
	oDepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	oDepthStencilDesc.SampleDesc = oApiData.m_oSwapChainDesc.SampleDesc;
	oDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	oDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	iResult = ID3D11Device_CreateTexture2D( oApiData.m_pDX11Device, &oDepthStencilDesc, NULL, &oApiData.m_pDX11DepthStencilBuffer );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't create DepthStencil texture" ), MB_ICONERROR | MB_OK );
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC oDepthStencilViewDesc;
	ZeroMemory( &oDepthStencilViewDesc, sizeof( D3D11_DEPTH_STENCIL_VIEW_DESC ) );
	oDepthStencilViewDesc.Format = oDepthStencilDesc.Format;
	oDepthStencilViewDesc.ViewDimension = oApiData.m_oSwapChainDesc.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

	iResult = ID3D11Device_CreateDepthStencilView( oApiData.m_pDX11Device, (ID3D11Resource*)oApiData.m_pDX11DepthStencilBuffer, &oDepthStencilViewDesc, &oApiData.m_pDX11DepthStencilView );
	if( FAILED( iResult ) )
	{
		CHAR pMsg[ 1024 ] = { 0 };
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), pMsg, 512, NULL );
		MessageBox( NULL, pMsg, TEXT( "Error : Can't create DepthStencil view" ), MB_ICONERROR | MB_OK );
		return;
	}
}

bool ImwPlatformWindowSokol::PreRenderDrawListsSokol()
{
	SpecificDataD3D11& oApiData = *(SpecificDataD3D11*)m_oApiData;
	

	return true;
}

void ImwPlatformWindowSokol::PostRenderDrawListsSokol()
{
	SpecificDataD3D11& oApiData = *(SpecificDataD3D11*)m_oApiData;

	IDXGISwapChain_Present( oApiData.m_pDXGISwapChain, 0, 0 );
}

#endif //SOKOL_D3D11