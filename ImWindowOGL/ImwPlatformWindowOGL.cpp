
#include "ImwPlatformWindowOGL.h"

using namespace ImWindow;

ImwPlatformWindowOGL::ImwPlatformWindowOGL(EPlatformWindowType eType, bool bCreateState)
	: ImwPlatformWindowEasyWindow(eType, bCreateState)
	, m_hDC( NULL )
	, m_hRC( NULL )
	, m_iTextureID( 0 )
{
}

ImwPlatformWindowOGL::~ImwPlatformWindowOGL()
{
	if (m_eType == E_PLATFORM_WINDOW_TYPE_MAIN)
	{
		if (m_iTextureID != 0)
		{
			glDeleteTextures(1, &m_iTextureID);
			m_iTextureID = 0;
		}

		if (m_hRC != NULL)
		{
			if (wglMakeCurrent(NULL, NULL) != TRUE)
			{
				MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}

			if (wglDeleteContext(m_hRC) != TRUE)
			{
				MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}
		}
	}

	if (m_hDC != NULL)
	{
		if (ReleaseDC((HWND)m_pWindow->GetHandle(), m_hDC) != 1)
		{
			MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
	}
}

bool ImwPlatformWindowOGL::Init(ImwPlatformWindow* pMain)
{
	if (ImwPlatformWindowEasyWindow::Init(pMain))
	{

		static PIXELFORMATDESCRIPTOR oPFD =			// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),			// Size Of This Pixel Format Descriptor
			1,										// Version Number
			PFD_DRAW_TO_WINDOW |					// Format Must Support Window
			PFD_SUPPORT_OPENGL |					// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,						// Must Support Double Buffering
			PFD_TYPE_RGBA,							// Request An RGBA Format
			32,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,						// Color Bits Ignored
			0,										// No Alpha Buffer
			0,										// Shift Bit Ignored
			0,										// No Accumulation Buffer
			0, 0, 0, 0,								// Accumulation Bits Ignored
			16,										// 16Bit Z-Buffer (Depth Buffer)
			0,										// No Stencil Buffer
			0,										// No Auxiliary Buffer
			PFD_MAIN_PLANE,							// Main Drawing Layer
			0,										// Reserved
			0, 0, 0									// Layer Masks Ignored
		};

		m_hDC = GetDC((HWND)m_pWindow->GetHandle());
		if (m_hDC == NULL)
		{
			MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		// Did Windows Find A Matching Pixel Format?
		GLuint iPixelFormat = ChoosePixelFormat(m_hDC, &oPFD);
		if (iPixelFormat == NULL)
		{
			MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!SetPixelFormat(m_hDC, iPixelFormat, &oPFD))               // Are We Able To Set The Pixel Format?
		{
			MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}

		if (pMain != NULL)
		{
			m_hRC = ((ImwPlatformWindowOGL*)pMain)->m_hRC;
		}
		else
		{
			m_hRC = wglCreateContext(m_hDC);
		}

		if (m_hRC == NULL)
		{
			MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!wglMakeCurrent(m_hDC, m_hRC))
		{
			MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGuiIO& io = GetContext()->IO;;

		if (pMain != NULL)
		{
			//Copy texture reference
			m_iTextureID = ((ImwPlatformWindowOGL*)pMain)->m_iTextureID;
		}
		else
		{
			unsigned char* pPixels;
			int iWidth;
			int iHeight;
			io.Fonts->AddFontDefault();
			io.Fonts->GetTexDataAsAlpha8(&pPixels, &iWidth, &iHeight);

			// Upload texture to graphics system
			glEnable(GL_TEXTURE_2D);
			m_iTextureID = 0;
			glGenTextures(1, &m_iTextureID);
			glBindTexture(GL_TEXTURE_2D, m_iTextureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (GLint)iWidth, (GLint)iHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)pPixels);

			// Store our identifier
			io.Fonts->TexID = (void *)(intptr_t)m_iTextureID;
		}

		return true;
	}
	return false;
}

void ImwPlatformWindowOGL::OnClientSize(int iClientWidth, int iClientHeight)
{
	if (m_hDC != NULL && m_hRC != NULL)
	{
		wglMakeCurrent(m_hDC, m_hRC);
		glViewport(0, 0, iClientWidth, iClientHeight);
	}
}

void ImwPlatformWindowOGL::RenderDrawLists(ImDrawData* pDrawData)
{
	if (m_hDC != NULL && m_hRC != NULL)
	{
		wglMakeCurrent(m_hDC, m_hRC);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		ImGuiIO& io = ImGui::GetIO();
		int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if (fb_width == 0 || fb_height == 0)
			return;
		pDrawData->ScaleClipRects(io.DisplayFramebufferScale);

		// We are using the OpenGL fixed pipeline to make the example code simpler to read!
		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
		GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

		// Setup viewport, orthographic projection matrix
		glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Render command lists
	#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
		for (int n = 0; n < pDrawData->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = pDrawData->CmdLists[n];
			const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->VtxBuffer.front();
			const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
			glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
			glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
				}
				idx_buffer += pcmd->ElemCount;
			}
		}
	#undef OFFSETOF

		// Restore modified state
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();

		SwapBuffers(m_hDC);
	}
}