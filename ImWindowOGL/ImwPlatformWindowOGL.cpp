
#include "ImwPlatformWindowOGL.h"

using namespace ImWindow;

ImwPlatformWindowOGL::ImwPlatformWindowOGL(EPlatformWindowType eType, bool bCreateState)
	: ImwPlatformWindow(eType, bCreateState)
	, m_pWindow( NULL )
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
	
	//Release opengl
	ImwSafeDelete(m_pWindow);
}

bool ImwPlatformWindowOGL::Init(ImwPlatformWindow* pMain)
{
	ImwPlatformWindowOGL* pMainOGL = ((ImwPlatformWindowOGL*)pMain);

	EasyWindow::EWindowStyle eStyle = EasyWindow::E_STYLE_NORMAL;
	if (m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW)
		eStyle = EasyWindow::E_STYLE_POPUP;

	m_pWindow = EasyWindow::Create("ImwPlatformWindowOGL", 800, 600, false, pMain != NULL ? pMainOGL->m_pWindow : NULL, eStyle);
	m_pWindow->OnClose.Set(this, &ImwPlatformWindowOGL::OnClose);
	m_pWindow->OnFocus.Set(this, &ImwPlatformWindowOGL::OnFocus);
	m_pWindow->OnSize.Set(this, &ImwPlatformWindowOGL::OnSize);
	m_pWindow->OnMouseButton.Set(this, &ImwPlatformWindowOGL::OnMouseButton);
	m_pWindow->OnMouseMove.Set(this, &ImwPlatformWindowOGL::OnMouseMove);
	m_pWindow->OnMouseWheel.Set(this, &ImwPlatformWindowOGL::OnMouseWheel);
	m_pWindow->OnKey.Set(this, &ImwPlatformWindowOGL::OnKey);
	m_pWindow->OnChar.Set(this, &ImwPlatformWindowOGL::OnChar);

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
	if(m_hDC == NULL)
	{
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	// Did Windows Find A Matching Pixel Format?
	GLuint iPixelFormat = ChoosePixelFormat(m_hDC, &oPFD);
	if (iPixelFormat == NULL )
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

	if (m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW)
		m_pWindow->SetAlpha(128);

	SetState();
	ImGuiIO& io = ImGui::GetIO();
	
	if (pMainOGL != NULL)
	{
		//Copy texture reference
		m_iTextureID = pMainOGL->m_iTextureID;
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

	io.KeyMap[ImGuiKey_Tab] = EasyWindow::KEY_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = EasyWindow::KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = EasyWindow::KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = EasyWindow::KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = EasyWindow::KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = EasyWindow::KEY_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = EasyWindow::KEY_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = EasyWindow::KEY_HOME;
	io.KeyMap[ImGuiKey_End] = EasyWindow::KEY_END;
	io.KeyMap[ImGuiKey_Delete] = EasyWindow::KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = EasyWindow::KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = EasyWindow::KEY_RETURN;
	io.KeyMap[ImGuiKey_Escape] = EasyWindow::KEY_ESC;
	io.KeyMap[ImGuiKey_A] = EasyWindow::KEY_A;
	io.KeyMap[ImGuiKey_C] = EasyWindow::KEY_C;
	io.KeyMap[ImGuiKey_V] = EasyWindow::KEY_V;
	io.KeyMap[ImGuiKey_X] = EasyWindow::KEY_X;
	io.KeyMap[ImGuiKey_Y] = EasyWindow::KEY_Y;
	io.KeyMap[ImGuiKey_Z] = EasyWindow::KEY_Z;

	io.RenderDrawListsFn = NULL;
	io.ImeWindowHandle = m_pWindow->GetHandle();

	RestoreState();

	return true;
}

ImVec2 ImwPlatformWindowOGL::GetPosition() const
{
	return ImVec2(float(m_pWindow->GetClientPositionX()), float(m_pWindow->GetClientPositionY()));
}

ImVec2 ImwPlatformWindowOGL::GetSize() const
{
	return ImVec2(float(m_pWindow->GetClientWidth()), float(m_pWindow->GetClientHeight()));
}

bool ImwPlatformWindowOGL::IsWindowMaximized() const
{
	return m_pWindow->IsMaximized();
}

bool ImwPlatformWindowOGL::IsWindowMinimized() const
{
	return m_pWindow->IsMinimized();
}

void ImwPlatformWindowOGL::Show(bool bShow)
{
	m_pWindow->Show(bShow);
}

void ImwPlatformWindowOGL::SetSize(int iWidth, int iHeight)
{
	m_pWindow->SetSize(iWidth, iHeight, true);
}

void ImwPlatformWindowOGL::SetPosition(int iX, int iY)
{
	m_pWindow->SetPosition(iX, iY, true);
}

void ImwPlatformWindowOGL::SetWindowMaximized(bool bMaximized)
{
	if (bMaximized)
		m_pWindow->SetMaximized();
	else
		m_pWindow->SetRestored();
}

void ImwPlatformWindowOGL::SetWindowMinimized()
{
	m_pWindow->SetMinimized();
}

void ImwPlatformWindowOGL::SetTitle(const ImwChar* pTitle)
{
	m_pWindow->SetTitle(pTitle);
}

void ImwPlatformWindowOGL::PreUpdate()
{
	m_pWindow->Update();
	ImGuiIO& oIO = ((ImGuiState*)m_pState)->IO;
	oIO.KeyCtrl = m_pWindow->IsKeyCtrlDown();
	oIO.KeyShift = m_pWindow->IsKeyShiftDown();
	oIO.KeyAlt = m_pWindow->IsKeyAltDown();
	oIO.KeySuper = false;

	if (oIO.MouseDrawCursor)
	{
		m_pWindow->SetCursor(EasyWindow::E_CURSOR_NONE);
	}
	else if (oIO.MousePos.x != -1.f && oIO.MousePos.y != -1.f)
	{
		switch (((ImGuiState*)m_pState)->MouseCursor)
		{
		case ImGuiMouseCursor_Arrow:
			m_pWindow->SetCursor(EasyWindow::E_CURSOR_ARROW);
			break;
		case ImGuiMouseCursor_TextInput:         // When hovering over InputText, etc.
			m_pWindow->SetCursor(EasyWindow::E_CURSOR_TEXT_INPUT);
			break;
		case ImGuiMouseCursor_Move:              // Unused
			m_pWindow->SetCursor(EasyWindow::E_CURSOR_HAND);
			break;
		case ImGuiMouseCursor_ResizeNS:          // Unused
			m_pWindow->SetCursor(EasyWindow::E_CURSOR_RESIZE_NS);
			break;
		case ImGuiMouseCursor_ResizeEW:          // When hovering over a column
			m_pWindow->SetCursor(EasyWindow::E_CURSOR_RESIZE_EW);
			break;
		case ImGuiMouseCursor_ResizeNESW:        // Unused
			m_pWindow->SetCursor(EasyWindow::E_CURSOR_RESIZE_NESW);
			break;
		case ImGuiMouseCursor_ResizeNWSE:        // When hovering over the bottom-right corner of a window
			m_pWindow->SetCursor(EasyWindow::E_CURSOR_RESIZE_NWSE);
			break;
		}
	}
}

void ImwPlatformWindowOGL::Render()
{
	if (!m_bNeedRender)
		return;

	if (m_hDC != NULL && m_hRC != NULL)
	{
		wglMakeCurrent(m_hDC, m_hRC);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SetState();	
		ImVec2 oSize = ImVec2(float(m_pWindow->GetClientWidth()), float(m_pWindow->GetClientHeight()));
		ImGui::GetIO().DisplaySize = oSize;

		ImGui::Render();
		RenderDrawList(ImGui::GetDrawData());

		SwapBuffers(m_hDC);

		RestoreState();
	}
}

bool ImwPlatformWindowOGL::OnClose()
{
	ImwPlatformWindow::OnClose();
	return true;
}

void ImwPlatformWindowOGL::OnFocus(bool bHasFocus)
{
	if (!bHasFocus)
		OnLoseFocus();
}

void ImwPlatformWindowOGL::OnSize(int iWidth, int iHeight)
{
	if (m_hDC != NULL && m_hRC != NULL)
	{
		wglMakeCurrent(m_hDC, m_hRC);
		glViewport(0, 0, iWidth, iHeight);
	}
}

void ImwPlatformWindowOGL::OnMouseButton(int iButton, bool bDown)
{
	((ImGuiState*)m_pState)->IO.MouseDown[iButton] = bDown;
}

void ImwPlatformWindowOGL::OnMouseMove(int iX, int iY)
{
	((ImGuiState*)m_pState)->IO.MousePos = ImVec2((float)iX, (float)iY);
}

void ImwPlatformWindowOGL::OnMouseWheel( int iStep )
{
	( ( ImGuiState* )m_pState )->IO.MouseWheel += iStep;
}

void ImwPlatformWindowOGL::OnKey(EasyWindow::EKey eKey, bool bDown)
{
	((ImGuiState*)m_pState)->IO.KeysDown[eKey] = bDown;
}

void ImwPlatformWindowOGL::OnChar(int iChar)
{
	((ImGuiState*)m_pState)->IO.AddInputCharacter((ImwChar)iChar);
}

void ImwPlatformWindowOGL::RenderDrawList(ImDrawData* pDrawData)
{
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
}