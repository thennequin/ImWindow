
#include "ImwPlatformWindowEasyWindow.h"
#include "ImwWindowManager.h"

using namespace ImWindow;

ImGuiKey s_iTranslateEasyWindowKeyToImGuiKey[256];

ImwPlatformWindowEasyWindow::ImwPlatformWindowEasyWindow(EPlatformWindowType eType, bool bCreateState)
	: ImwPlatformWindow(eType, bCreateState)
	, m_pWindow( NULL )
{
	if (eType == E_PLATFORM_WINDOW_TYPE_MAIN)
	{
		memset(s_iTranslateEasyWindowKeyToImGuiKey, 0, sizeof(s_iTranslateEasyWindowKeyToImGuiKey));

		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_ESC] = ImGuiKey_Escape;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_RETURN] = ImGuiKey_Enter;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_TAB] = ImGuiKey_Tab;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_BACKSPACE] = ImGuiKey_Backspace;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_SPACE] = ImGuiKey_Space;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_UP] = ImGuiKey_UpArrow;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_DOWN] = ImGuiKey_DownArrow;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_LEFT] = ImGuiKey_LeftArrow;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_RIGHT] = ImGuiKey_RightArrow;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_INSERT] = ImGuiKey_Insert;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_DELETE] = ImGuiKey_Delete;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_HOME] = ImGuiKey_Home;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_END] = ImGuiKey_End;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_PAGEUP] = ImGuiKey_PageUp;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_PAGEDOWN] = ImGuiKey_PageDown;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_PRINT] = ImGuiKey_PrintScreen;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_PLUS] = ImGuiKey_Equal;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_MINUS] = ImGuiKey_Minus;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_LEFTBRACKET] = ImGuiKey_LeftBracket;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_RIGHTBRACKET] = ImGuiKey_RightBracket;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_SEMICOLON] = ImGuiKey_Semicolon;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_QUOTE] = ImGuiKey_Apostrophe;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_COMMA] = ImGuiKey_Comma;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_PERIOD] = ImGuiKey_Period;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_PERIOD] = ImGuiKey_KeypadDecimal;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_SLASH] = ImGuiKey_Slash;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_BACKSLASH] = ImGuiKey_Backslash;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_TILDE] = ImGuiKey_GraveAccent;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F1] = ImGuiKey_F1;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F2] = ImGuiKey_F2;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F3] = ImGuiKey_F3;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F4] = ImGuiKey_F4;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F5] = ImGuiKey_F5;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F6] = ImGuiKey_F6;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F7] = ImGuiKey_F7;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F8] = ImGuiKey_F8;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F9] = ImGuiKey_F9;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F10] = ImGuiKey_F10;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F11] = ImGuiKey_F11;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F12] = ImGuiKey_F12;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD0] = ImGuiKey_Keypad0;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD1] = ImGuiKey_Keypad1;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD2] = ImGuiKey_Keypad2;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD3] = ImGuiKey_Keypad3;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD4] = ImGuiKey_Keypad4;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD5] = ImGuiKey_Keypad5;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD6] = ImGuiKey_Keypad6;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD7] = ImGuiKey_Keypad7;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD8] = ImGuiKey_Keypad8;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_NUMPAD9] = ImGuiKey_Keypad9;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_0] = ImGuiKey_0;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_1] = ImGuiKey_1;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_2] = ImGuiKey_2;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_3] = ImGuiKey_3;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_4] = ImGuiKey_4;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_5] = ImGuiKey_5;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_6] = ImGuiKey_6;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_7] = ImGuiKey_7;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_8] = ImGuiKey_8;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_9] = ImGuiKey_9;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_A] = ImGuiKey_A;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_B] = ImGuiKey_B;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_C] = ImGuiKey_C;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_D] = ImGuiKey_D;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_E] = ImGuiKey_E;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_F] = ImGuiKey_F;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_G] = ImGuiKey_G;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_H] = ImGuiKey_H;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_I] = ImGuiKey_I;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_J] = ImGuiKey_J;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_K] = ImGuiKey_K;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_L] = ImGuiKey_L;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_M] = ImGuiKey_M;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_N] = ImGuiKey_N;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_O] = ImGuiKey_O;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_P] = ImGuiKey_P;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_Q] = ImGuiKey_Q;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_R] = ImGuiKey_R;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_S] = ImGuiKey_S;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_T] = ImGuiKey_T;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_U] = ImGuiKey_U;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_V] = ImGuiKey_V;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_W] = ImGuiKey_W;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_X] = ImGuiKey_X;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_Y] = ImGuiKey_Y;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_Z] = ImGuiKey_Z;

		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_LEFTALT] = ImGuiKey_LeftAlt;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_RIGHTALT] = ImGuiKey_RightAlt;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_LEFTCTRL] = ImGuiKey_LeftCtrl;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_RIGHTCTRL] = ImGuiKey_RightCtrl;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_LEFTSHIFT] = ImGuiKey_LeftShift;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_RIGHTSHIFT] = ImGuiKey_RightShift;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_LEFTMETA] = ImGuiKey_LeftSuper;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_RIGHTMETA] = ImGuiKey_RightSuper;

		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_ALT] = ImGuiMod_Alt;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_CTRL] = ImGuiMod_Ctrl;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_SHIFT] = ImGuiMod_Shift;
		s_iTranslateEasyWindowKeyToImGuiKey[EasyWindow::EKey::KEY_META] = ImGuiMod_Super;
	}
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

	ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();
	if (pWindowManager->IsUsingCustomFrame())
		eStyle = EasyWindow::E_STYLE_BORDERLESS_RESIZABLE;

	if (m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW)
		eStyle = EasyWindow::E_STYLE_POPUP;

	m_pWindow = EasyWindow::Create("ImwPlatformWindowEasyWindow", 800, 600, false, pMain != NULL ? pMainWindow->m_pWindow : NULL, eStyle, EasyWindow::E_FLAG_CATCH_ALT_KEY | EasyWindow::E_FLAG_ACCEPT_FILES_DROP | EasyWindow::E_FLAG_ALWAYS_CAPTURE_MOUSE_ON_CLICK);
	m_pWindow->OnSize.Set(this, &ImwPlatformWindowEasyWindow::OnSize);
	m_pWindow->OnClose.Set(this, &ImwPlatformWindowEasyWindow::OnClose);
	m_pWindow->OnFocus.Set(this, &ImwPlatformWindowEasyWindow::OnFocus);
	m_pWindow->OnMouseButton.Set(this, &ImwPlatformWindowEasyWindow::OnMouseButton);
	m_pWindow->OnMouseMove.Set(this, &ImwPlatformWindowEasyWindow::OnMouseMove);
	m_pWindow->OnMouseWheel.Set(this, &ImwPlatformWindowEasyWindow::OnMouseWheel);
	m_pWindow->OnKey.Set(this, &ImwPlatformWindowEasyWindow::OnKey);
	m_pWindow->OnChar.Set(this, &ImwPlatformWindowEasyWindow::OnChar);
	m_pWindow->OnDropFiles.Set(this, &ImwPlatformWindowEasyWindow::OnDropFiles);
	m_pWindow->BorderlessHoveredArea.Set(this, &ImwPlatformWindowEasyWindow::GetHoveredArea);

	if (m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW)
		m_pWindow->SetAlpha(128);

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

void ImwPlatformWindowEasyWindow::SetTitle(const char* pTitle)
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
		case ImGuiMouseCursor_Hand:              // Unused
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

void ImwPlatformWindowEasyWindow::OnOverlay()
{
	if (ImwWindowManager::GetInstance()->IsUsingCustomFrame())
	{
		ImDrawList* pDrawList = ImGui::GetForegroundDrawList();
		ImU32 iBorderColor = ImGui::GetColorU32(ImGuiCol_Border);
		pDrawList->AddRect(ImVec2(0.f, 0.f), GetSize(), iBorderColor);
	}
}

bool ImwPlatformWindowEasyWindow::OnClose(const EasyWindow* /*pWindow*/)
{
	ImwPlatformWindow::OnClose();
	return true;
}

void ImwPlatformWindowEasyWindow::OnSize(const EasyWindow* /*pWindow*/, int /*iWidth*/, int /*iHeight*/)
{
	int iClientWidth, iClientHeight;
	m_pWindow->GetClientSize(&iClientWidth, &iClientHeight);
	OnClientSize(iClientWidth, iClientHeight);
}

void ImwPlatformWindowEasyWindow::OnFocus(const EasyWindow* /*pWindow*/, bool bHasFocus)
{
	ImwPlatformWindow::OnFocus(bHasFocus);
}

void ImwPlatformWindowEasyWindow::OnMouseButton(const EasyWindow* /*pWindow*/, int iButton, bool bDown)
{
	GetContext()->IO.AddMouseButtonEvent(iButton, bDown);
}

void ImwPlatformWindowEasyWindow::OnMouseMove(const EasyWindow* /*pWindow*/, int iX, int iY)
{
	GetContext()->IO.AddMousePosEvent((float)iX, (float)iY);
}

void ImwPlatformWindowEasyWindow::OnMouseWheel(const EasyWindow* /*pWindow*/, float fStep)
{
	GetContext()->IO.AddMouseWheelEvent(0.f, fStep);
}

void ImwPlatformWindowEasyWindow::OnKey(const EasyWindow* /*pWindow*/, EasyWindow::EKey eKey, bool bDown)
{
	if (eKey >= 0 && eKey < 256)
	{
		ImGuiKey eImGuiKey = s_iTranslateEasyWindowKeyToImGuiKey[eKey];
		if (eImGuiKey != ImGuiKey_None)
		{
			GetContext()->IO.AddKeyEvent(eImGuiKey, bDown);
		}
	}
}

void ImwPlatformWindowEasyWindow::OnChar(const EasyWindow* /*pWindow*/, unsigned long iChar)
{
	GetContext()->IO.AddInputCharacter((ImWchar)iChar);
}

void ImwPlatformWindowEasyWindow::OnDropFiles(const EasyWindow* /*pWindow*/, const EasyWindow::DropFiles& oFiles)
{
	ImVec2 oPos((float)oFiles.oPosition.x, (float)oFiles.oPosition.y);
	ImwPlatformWindow::OnDropFiles(oFiles.iCount, oFiles.pFiles, oPos);
}

EasyWindow::EHoveredArea ImwPlatformWindowEasyWindow::GetHoveredArea(const EasyWindow* /*pWindow*/, int /*iX*/, int /*iY*/)
{
	switch (m_eHoveredArea)
	{
	break; case E_PLATFORMWINDOWHOVEREDAREA_NONE:
	default:
		return EasyWindow::E_HOVEREDAREA_NONE;
	break; case E_PLATFORMWINDOWHOVEREDAREA_MENU:
		return EasyWindow::E_HOVEREDAREA_MENU;
	break; case E_PLATFORMWINDOWHOVEREDAREA_CAPTION:
		return EasyWindow::E_HOVEREDAREA_CAPTION;
	break; case E_PLATFORMWINDOWHOVEREDAREA_MINIMIZE:
		return EasyWindow::E_HOVEREDAREA_MINIMIZE;
	break; case E_PLATFORMWINDOWHOVEREDAREA_MAXIMIZE:
		return EasyWindow::E_HOVEREDAREA_MAXIMIZE;
	break; case E_PLATFORMWINDOWHOVEREDAREA_CLOSE:
		return EasyWindow::E_HOVEREDAREA_CLOSE;
	}
}
