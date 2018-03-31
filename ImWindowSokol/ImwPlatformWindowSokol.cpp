
#ifdef _DEBUG
#define SOKOL_DEBUG
#endif

#define SOKOL_IMPL
#define SOKOL_GLCORE33
//#define SOKOL_D3D11 //Todo

#ifdef SOKOL_GLCORE33
#include "flextGL.h"
#endif

#include "ImwPlatformWindowSokol.h"

using namespace ImWindow;

const int ImwPlatformWindowSokol::c_iMaxVertices = (1 << 16); //Should be enough
const int ImwPlatformWindowSokol::c_iMaxIndices = c_iMaxVertices * 3;

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
	m_pVertexBuffer = (ImDrawVert*)ImwMalloc(sizeof(ImDrawVert) * c_iMaxVertices);
	m_pIndexBuffer = (uint32_t*)ImwMalloc(sizeof(uint32_t) * c_iMaxIndices);

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
		sg_pipeline_desc oPipelineDesc = { 0 };
		sg_shader_desc oShaderDesc = { 0 };
		if (SetupSokol(pMain, &oPipelineDesc, &oShaderDesc) == false)
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

		//Create vertex buffer
		sg_buffer_desc oVertexBufferDesc = {0};
		oVertexBufferDesc.usage = SG_USAGE_STREAM;
		oVertexBufferDesc.size = sizeof(ImDrawVert) * c_iMaxVertices;
		m_hVertexBuffer = sg_make_buffer(&oVertexBufferDesc);
		m_oDrawState.vertex_buffers[0] = m_hVertexBuffer;

		//Create index buffer
		sg_buffer_desc oIndexBufferDesc = {0};
		oIndexBufferDesc.type = SG_BUFFERTYPE_INDEXBUFFER;
		oIndexBufferDesc.usage = SG_USAGE_STREAM;
		oIndexBufferDesc.size = sizeof(uint32_t) * c_iMaxIndices;
		m_hIndexBuffer = sg_make_buffer(&oIndexBufferDesc);
		m_oDrawState.index_buffer = m_hIndexBuffer;

		m_hShader = sg_make_shader(&oShaderDesc);

		oPipelineDesc.layout.buffers[0].stride = sizeof(ImDrawVert);
		
		oPipelineDesc.shader = m_hShader;
		oPipelineDesc.index_type = SG_INDEXTYPE_UINT32;
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
		// copy vertices and indices
		int iNumVertices = 0;
		int iNumIndices = 0;
		int iNumCmdlists = 0;
		for (iNumCmdlists = 0; iNumCmdlists < pDrawData->CmdListsCount; iNumCmdlists++)
		{
			const ImDrawList* cl = pDrawData->CmdLists[iNumCmdlists];
			const int iCmdNumVertices = cl->VtxBuffer.size();
			const int iCmdNumIndices = cl->IdxBuffer.size();

			// overflow check
			if ((iNumVertices + iCmdNumVertices) > c_iMaxVertices)
			{
				break;
			}
			if ((iNumIndices + iCmdNumIndices) > c_iMaxIndices)
			{
				break;
			}

			// copy vertices
			memcpy(&m_pVertexBuffer[iNumVertices], &cl->VtxBuffer.front(), iCmdNumVertices * sizeof(ImDrawVert));

			// copy indices, need to 'rebase' indices to start of global vertex buffer
			const ImDrawIdx* pSrcIndex = &cl->IdxBuffer.front();
			const uint32_t iBaseVertexIndex = iNumVertices;
			for (int i = 0; i < iCmdNumIndices; i++)
			{
				m_pIndexBuffer[iNumIndices++] = pSrcIndex[i] + iBaseVertexIndex;
			}
			iNumVertices += iCmdNumVertices;
		}

		// update vertex and index buffers
		const int iVertexDataSize = iNumVertices * sizeof(ImDrawVert);
		const int iIndexDataSize = iNumIndices * sizeof(uint32_t);
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

bool ImwPlatformWindowSokol::SetupSokol(ImwPlatformWindow* pMain, sg_pipeline_desc* pPipelineDesc, sg_shader_desc* pShaderDesc)
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

		sg_desc oSokolDesc = {};
		sg_setup(&oSokolDesc);
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
	SwapBuffers(oApiData.m_hDC);
}

#endif //SOKOL_GLCORE33