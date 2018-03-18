
#include "ImwPlatformWindowEasyWindow.h"

//#include "ImwPlatformWindowEasyWindow.h"

using namespace ImWindow;

ImwPlatformWindowEasyWindow::ImwPlatformWindowEasyWindow(EPlatformWindowType eType, bool bCreateState)
	: ImwPlatformWindow(eType, bCreateState)
	, m_pWindow( NULL )
{
}

ImwPlatformWindowEasyWindow::~ImwPlatformWindowEasyWindow()
{
	ImwSafeDelete(m_pWindow);
}

bool ImwPlatformWindowEasyWindow::Init(ImwPlatformWindow* pMain)
{
	//Create window
	ImwPlatformWindowEasyWindow* pMainWindow = ((ImwPlatformWindowEasyWindow*)pMain);

	EasyWindow::EWindowStyle eStyle = EasyWindow::E_STYLE_NORMAL;
	if (m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW)
		eStyle = EasyWindow::E_STYLE_POPUP;

	m_pWindow = EasyWindow::Create("ImwPlatformWindowEasyWindow", 800, 600, false, pMain != NULL ? pMainWindow->m_pWindow : NULL, eStyle, EasyWindow::E_FLAG_CATCH_ALT_KEY | EasyWindow::E_FLAG_ACCEPT_FILES_DROP);
	m_pWindow->OnSize.Set(this, &ImwPlatformWindowEasyWindow::OnSize);
	m_pWindow->OnClose.Set(this, &ImwPlatformWindowEasyWindow::OnClose);
	m_pWindow->OnFocus.Set(this, &ImwPlatformWindowEasyWindow::OnFocus);
	m_pWindow->OnMouseButton.Set(this, &ImwPlatformWindowEasyWindow::OnMouseButton);
	m_pWindow->OnMouseMove.Set(this, &ImwPlatformWindowEasyWindow::OnMouseMove);
	m_pWindow->OnMouseWheel.Set(this, &ImwPlatformWindowEasyWindow::OnMouseWheel);
	m_pWindow->OnKey.Set(this, &ImwPlatformWindowEasyWindow::OnKey);
	m_pWindow->OnChar.Set(this, &ImwPlatformWindowEasyWindow::OnChar);
	m_pWindow->OnDropFiles.Set(this, &ImwPlatformWindowEasyWindow::OnDropFiles);

	if (m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW)
		m_pWindow->SetAlpha(128);

	//Setup IO
	ImGuiIO& io = GetContext()->IO;
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

	io.ImeWindowHandle = m_pWindow->GetHandle();

	io.Fonts->AddFontDefault();
	
	return true;
}

ImVec2 ImwPlatformWindowEasyWindow::GetPosition() const
{
	int iX, iY;
	m_pWindow->GetClientPosition(&iX, &iY);
	return ImVec2((float)iX, (float)iY);
}

ImVec2 ImwPlatformWindowEasyWindow::GetSize() const
{
	int iWidth, iHeight;
	m_pWindow->GetClientSize(&iWidth, &iHeight);
	return ImVec2((float)iWidth, (float)iHeight);
}

bool ImwPlatformWindowEasyWindow::IsWindowMaximized() const
{
	return m_pWindow->IsMaximized();
}

bool ImwPlatformWindowEasyWindow::IsWindowMinimized() const
{
	return m_pWindow->IsMinimized();
}

void ImwPlatformWindowEasyWindow::Show(bool bShow)
{
	m_pWindow->Show(bShow);
}

void ImwPlatformWindowEasyWindow::SetSize(int iWidth, int iHeight)
{
	m_pWindow->SetSize(iWidth, iHeight, true);
}

void ImwPlatformWindowEasyWindow::SetPosition(int iX, int iY)
{
	m_pWindow->SetPosition(iX, iY, true);
}

void ImwPlatformWindowEasyWindow::SetWindowMaximized(bool bMaximized)
{
	m_pWindow->SetMaximized(bMaximized);
}

void ImwPlatformWindowEasyWindow::SetWindowMinimized(bool bMinimized)
{
	m_pWindow->SetMinimized(bMinimized);
}

void ImwPlatformWindowEasyWindow::SetTitle(const ImwChar* pTitle)
{
	m_pWindow->SetTitle(pTitle);
}

void ImwPlatformWindowEasyWindow::PreUpdate()
{
	m_pWindow->Update();
	ImGuiIO& oIO = GetContext()->IO;
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
		switch (GetContext()->MouseCursor)
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

bool ImwPlatformWindowEasyWindow::OnClose()
{
	ImwPlatformWindow::OnClose();
	return true;
}

void ImwPlatformWindowEasyWindow::OnSize(int /*iWidth*/, int /*iHeight*/)
{
	int iClientWidth, iClientHeight;
	m_pWindow->GetClientSize(&iClientWidth, &iClientHeight);
	OnClientSize(iClientWidth, iClientHeight);
}

void ImwPlatformWindowEasyWindow::OnFocus(bool bHasFocus)
{
	ImwPlatformWindow::OnFocus(bHasFocus);
}

void ImwPlatformWindowEasyWindow::OnMouseButton(int iButton, bool bDown)
{
	GetContext()->IO.MouseDown[iButton] = bDown;
}

void ImwPlatformWindowEasyWindow::OnMouseMove(int iX, int iY)
{
	GetContext()->IO.MousePos = ImVec2((float)iX, (float)iY);
}

void ImwPlatformWindowEasyWindow::OnMouseWheel( int iStep )
{
	GetContext()->IO.MouseWheel += iStep;
}

void ImwPlatformWindowEasyWindow::OnKey(EasyWindow::EKey eKey, bool bDown)
{
	GetContext()->IO.KeysDown[eKey] = bDown;
}

void ImwPlatformWindowEasyWindow::OnChar(int iChar)
{
	GetContext()->IO.AddInputCharacter((ImwChar)iChar);
}

void ImwPlatformWindowEasyWindow::OnDropFiles(const EasyWindow::DropFiles& oFiles)
{
	ImVec2 oPos((float)oFiles.oPosition.x, (float)oFiles.oPosition.y);
	ImwPlatformWindow::OnDropFiles(oFiles.iCount, oFiles.pFiles, oPos);
}
