
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)

#include "EasyWindow.h"

#include <windows.h>

#define EW_OVERRIDE override

class EasyWindowWin32 : public EasyWindow
{
public:
	EasyWindowWin32(const char* pTitle, int iWidth, int iHeight, bool bClientSize, EasyWindow* pParent, EWindowStyle eStyle)
		: m_bSizing(false)
	{
		if (!s_bClassInitialized)
		{
			s_bClassInitialized = true;
			WNDCLASSEX wc;
			ZeroMemory(&wc, sizeof(WNDCLASSEX));
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = (WNDPROC)EasyWindowWin32::Proc;
			//wc.hInstance = hInstance;
			wc.hInstance = GetModuleHandle(NULL);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
			wc.lpszClassName = "EasyWindowWin32";
			RegisterClassEx(&wc);

			for (int i = 0; i < 256; ++i)
				s_iTranslateKeys[i] = KEY_NONE;

			s_iTranslateKeys[VK_ESCAPE]     = EKey::KEY_ESC;
			s_iTranslateKeys[VK_RETURN]     = EKey::KEY_RETURN;
			s_iTranslateKeys[VK_TAB]        = EKey::KEY_TAB;
			s_iTranslateKeys[VK_BACK]       = EKey::KEY_BACKSPACE;
			s_iTranslateKeys[VK_SPACE]      = EKey::KEY_SPACE;
			s_iTranslateKeys[VK_UP]         = EKey::KEY_UP;
			s_iTranslateKeys[VK_DOWN]       = EKey::KEY_DOWN;
			s_iTranslateKeys[VK_LEFT]       = EKey::KEY_LEFT;
			s_iTranslateKeys[VK_RIGHT]      = EKey::KEY_RIGHT;
			s_iTranslateKeys[VK_INSERT]     = EKey::KEY_INSERT;
			s_iTranslateKeys[VK_DELETE]     = EKey::KEY_DELETE;
			s_iTranslateKeys[VK_HOME]       = EKey::KEY_HOME;
			s_iTranslateKeys[VK_END]        = EKey::KEY_END;
			s_iTranslateKeys[VK_PRIOR]      = EKey::KEY_PAGEUP;
			s_iTranslateKeys[VK_NEXT]       = EKey::KEY_PAGEDOWN;
			s_iTranslateKeys[VK_SNAPSHOT]   = EKey::KEY_PRINT;
			s_iTranslateKeys[VK_OEM_PLUS]   = EKey::KEY_PLUS;
			s_iTranslateKeys[VK_OEM_MINUS]  = EKey::KEY_MINUS;
			s_iTranslateKeys[VK_OEM_4]      = EKey::KEY_LEFTBRACKET;
			s_iTranslateKeys[VK_OEM_6]      = EKey::KEY_RIGHTBRACKET;
			s_iTranslateKeys[VK_OEM_1]      = EKey::KEY_SEMICOLON;
			s_iTranslateKeys[VK_OEM_7]      = EKey::KEY_QUOTE;
			s_iTranslateKeys[VK_OEM_COMMA]  = EKey::KEY_COMMA;
			s_iTranslateKeys[VK_OEM_PERIOD] = EKey::KEY_PERIOD;
			s_iTranslateKeys[VK_DECIMAL]    = EKey::KEY_PERIOD;
			s_iTranslateKeys[VK_OEM_2]      = EKey::KEY_SLASH;
			s_iTranslateKeys[VK_OEM_5]      = EKey::KEY_BACKSLASH;
			s_iTranslateKeys[VK_OEM_3]      = EKey::KEY_TILDE;
			s_iTranslateKeys[VK_F1]         = EKey::KEY_F1;
			s_iTranslateKeys[VK_F2]         = EKey::KEY_F2;
			s_iTranslateKeys[VK_F3]         = EKey::KEY_F3;
			s_iTranslateKeys[VK_F4]         = EKey::KEY_F4;
			s_iTranslateKeys[VK_F5]         = EKey::KEY_F5;
			s_iTranslateKeys[VK_F6]         = EKey::KEY_F6;
			s_iTranslateKeys[VK_F7]         = EKey::KEY_F7;
			s_iTranslateKeys[VK_F8]         = EKey::KEY_F8;
			s_iTranslateKeys[VK_F9]         = EKey::KEY_F9;
			s_iTranslateKeys[VK_F10]        = EKey::KEY_F10;
			s_iTranslateKeys[VK_F11]        = EKey::KEY_F11;
			s_iTranslateKeys[VK_F12]        = EKey::KEY_F12;
			s_iTranslateKeys[VK_NUMPAD0]    = EKey::KEY_NUMPAD0;
			s_iTranslateKeys[VK_NUMPAD1]    = EKey::KEY_NUMPAD1;
			s_iTranslateKeys[VK_NUMPAD2]    = EKey::KEY_NUMPAD2;
			s_iTranslateKeys[VK_NUMPAD3]    = EKey::KEY_NUMPAD3;
			s_iTranslateKeys[VK_NUMPAD4]    = EKey::KEY_NUMPAD4;
			s_iTranslateKeys[VK_NUMPAD5]    = EKey::KEY_NUMPAD5;
			s_iTranslateKeys[VK_NUMPAD6]    = EKey::KEY_NUMPAD6;
			s_iTranslateKeys[VK_NUMPAD7]    = EKey::KEY_NUMPAD7;
			s_iTranslateKeys[VK_NUMPAD8]    = EKey::KEY_NUMPAD8;
			s_iTranslateKeys[VK_NUMPAD9]    = EKey::KEY_NUMPAD9;
			s_iTranslateKeys['0']           = EKey::KEY_0;
			s_iTranslateKeys['1']           = EKey::KEY_1;
			s_iTranslateKeys['2']           = EKey::KEY_2;
			s_iTranslateKeys['3']           = EKey::KEY_3;
			s_iTranslateKeys['4']           = EKey::KEY_4;
			s_iTranslateKeys['5']           = EKey::KEY_5;
			s_iTranslateKeys['6']           = EKey::KEY_6;
			s_iTranslateKeys['7']           = EKey::KEY_7;
			s_iTranslateKeys['8']           = EKey::KEY_8;
			s_iTranslateKeys['9']           = EKey::KEY_9;
			s_iTranslateKeys['A']           = EKey::KEY_A;
			s_iTranslateKeys['B']           = EKey::KEY_B;
			s_iTranslateKeys['C']           = EKey::KEY_C;
			s_iTranslateKeys['D']           = EKey::KEY_D;
			s_iTranslateKeys['E']           = EKey::KEY_E;
			s_iTranslateKeys['F']           = EKey::KEY_F;
			s_iTranslateKeys['G']           = EKey::KEY_G;
			s_iTranslateKeys['H']           = EKey::KEY_H;
			s_iTranslateKeys['I']           = EKey::KEY_I;
			s_iTranslateKeys['J']           = EKey::KEY_J;
			s_iTranslateKeys['K']           = EKey::KEY_K;
			s_iTranslateKeys['L']           = EKey::KEY_L;
			s_iTranslateKeys['M']           = EKey::KEY_M;
			s_iTranslateKeys['N']           = EKey::KEY_N;
			s_iTranslateKeys['O']           = EKey::KEY_O;
			s_iTranslateKeys['P']           = EKey::KEY_P;
			s_iTranslateKeys['Q']           = EKey::KEY_Q;
			s_iTranslateKeys['R']           = EKey::KEY_R;
			s_iTranslateKeys['S']           = EKey::KEY_S;
			s_iTranslateKeys['T']           = EKey::KEY_T;
			s_iTranslateKeys['U']           = EKey::KEY_U;
			s_iTranslateKeys['V']           = EKey::KEY_V;
			s_iTranslateKeys['W']           = EKey::KEY_W;
			s_iTranslateKeys['X']           = EKey::KEY_X;
			s_iTranslateKeys['Y']           = EKey::KEY_Y;
			s_iTranslateKeys['Z']           = EKey::KEY_Z;

			s_iTranslateKeys[VK_LMENU]      = EKey::KEY_LEFTALT;
			s_iTranslateKeys[VK_RMENU]      = EKey::KEY_RIGHTALT;
			s_iTranslateKeys[VK_LCONTROL]   = EKey::KEY_LEFTCTRL;
			s_iTranslateKeys[VK_RCONTROL]   = EKey::KEY_RIGHTCTRL;
			s_iTranslateKeys[VK_LSHIFT]     = EKey::KEY_LEFTSHIFT;
			s_iTranslateKeys[VK_RSHIFT]     = EKey::KEY_RIGHTSHIFT;
			s_iTranslateKeys[VK_LWIN]       = EKey::KEY_LEFTMETA;
			s_iTranslateKeys[VK_RWIN]       = EKey::KEY_RIGHTMETA;
		}

		DWORD iWindowStyle = WS_OVERLAPPEDWINDOW; // E_NORMAL
		
		if (eStyle == E_BORDERLESS)
		{
			iWindowStyle = WS_POPUP | WS_SYSMENU;
		}
		else if (eStyle == E_BORDERLESS_RESIZABLE)
		{
			iWindowStyle = WS_POPUP | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
		}
		else if (eStyle == E_POPUP)
		{
			iWindowStyle = WS_POPUP;
		}

		m_bManualSizing = eStyle == E_BORDERLESS_RESIZABLE;

		RECT wr = { 0, 0, iWidth, iHeight };
		if (bClientSize)
		{
			AdjustWindowRect(&wr, iWindowStyle, FALSE);
		}

		m_pHandle = CreateWindowEx(
			NULL,
			"EasyWindowWin32",
			pTitle != NULL ? pTitle : "",
			iWindowStyle,
			300,
			300,
			wr.right - wr.left,
			wr.bottom - wr.top,
			pParent != NULL ? ((EasyWindowWin32*)pParent)->m_pHandle : NULL,
			NULL,
			GetModuleHandle(NULL),
			NULL);

		if (m_bManualSizing)
		{
			SetRectEmpty(&m_oBorderThickness);
			if (iWindowStyle & WS_THICKFRAME)
			{
				AdjustWindowRectEx(&m_oBorderThickness, iWindowStyle & ~WS_CAPTION, FALSE, NULL);
				m_oBorderThickness.left *= -1;
				m_oBorderThickness.top *= -1;
			}
			else if (iWindowStyle & WS_BORDER)
			{
				SetRect(&m_oBorderThickness, 1, 1, 1, 1);
			}
		}

		SetWindowLongPtr(m_pHandle, GWLP_USERDATA, (LONG_PTR)this);
	}

	virtual							~EasyWindowWin32()
	{
		if (m_pHandle != NULL)
			DestroyWindow(m_pHandle);
	}

	virtual bool					Update() EW_OVERRIDE
	{
		MSG oMsg;
		int iCount = 0;
		while (iCount < 10 && PeekMessage(&oMsg, m_pHandle, 0, 0, PM_NOREMOVE)) // Max 10 messages
		{
			if (GetMessage(&oMsg, m_pHandle, 0, 0))
			{
				TranslateMessage(&oMsg);
				DispatchMessage(&oMsg);
				if (m_bSizing)
					break;
			}
			++iCount;
		}
		return true;
	}


	virtual void					Show(bool bShow) EW_OVERRIDE
	{
		ShowWindow(m_pHandle, bShow ? SW_SHOW : SW_HIDE);
	}

	virtual void					SetSize(int iWidth, int iHeight, bool bClientSize) EW_OVERRIDE
	{
		RECT oRect = { 0, 0, iWidth, iHeight };
		if (bClientSize)
		{
			DWORD iWindowStyle = GetWindowLong(m_pHandle, GWL_STYLE);
			AdjustWindowRect(&oRect, iWindowStyle, FALSE);
		}

		SetWindowPos(m_pHandle, NULL, 0, 0, oRect.right - oRect.left, oRect.bottom - oRect.top, SWP_NOMOVE);
	}

	virtual void					SetPosition(int iPosX, int iPosY) EW_OVERRIDE
	{
		SetWindowPos(m_pHandle, NULL, iPosX, iPosY, 0, 0, SWP_NOSIZE);
	}

	virtual void					SetMaximized() EW_OVERRIDE
	{
		ShowWindow(m_pHandle, SW_MAXIMIZE);
	}

	virtual void					SetMinimized() EW_OVERRIDE
	{
		ShowWindow(m_pHandle, SW_MINIMIZE);
	}

	virtual void					SetRestored() EW_OVERRIDE
	{
		ShowWindow(m_pHandle, SW_RESTORE);
	}

	virtual void					SetTitle(const char* pTitle) EW_OVERRIDE
	{
		SetWindowText(m_pHandle, pTitle);
	}

	virtual void					SetAlpha(unsigned char iAlpha) EW_OVERRIDE
	{
		if (iAlpha < 255)
		{
			SetWindowLong(m_pHandle, GWL_EXSTYLE, GetWindowLong(m_pHandle, GWL_EXSTYLE) | WS_EX_LAYERED);
		}
		else
		{
			SetWindowLong(m_pHandle, GWL_EXSTYLE, GetWindowLong(m_pHandle, GWL_EXSTYLE) & ~WS_EX_LAYERED);
		}
		SetLayeredWindowAttributes(m_pHandle, RGB(0, 0, 0), iAlpha, LWA_ALPHA);
	}

	virtual int						GetWidth() EW_OVERRIDE
	{
		RECT oRect;
		GetWindowRect(m_pHandle, &oRect);
		return oRect.right - oRect.left;
	}
	virtual int						GetHeight() EW_OVERRIDE
	{
		RECT oRect;
		GetWindowRect(m_pHandle, &oRect);
		return oRect.bottom - oRect.top;
	}
	virtual int						GetClientWidth() EW_OVERRIDE
	{
		RECT oRect;
		GetClientRect(m_pHandle, &oRect);
		return oRect.right - oRect.left;
	}
	virtual int						GetClientHeight() EW_OVERRIDE
	{
		RECT oRect;
		GetClientRect(m_pHandle, &oRect);
		return oRect.bottom - oRect.top;
	}

	virtual int						GetPositionX() EW_OVERRIDE
	{
		RECT oRect;
		GetWindowRect(m_pHandle, &oRect);
		return oRect.left;
	}

	virtual int						GetPositionY() EW_OVERRIDE
	{
		RECT oRect;
		GetWindowRect(m_pHandle, &oRect);
		return oRect.top;
	}

	virtual int						GetClientPositionX() EW_OVERRIDE
	{
		RECT oRectClient;
		GetClientRect(m_pHandle, &oRectClient);
		ClientToScreen(m_pHandle, reinterpret_cast<POINT*>(&oRectClient.left)); // convert top-left
		return oRectClient.left;
	}

	virtual int						GetClientPositionY() EW_OVERRIDE
	{
		RECT oRectClient;
		GetClientRect(m_pHandle, &oRectClient);
		ClientToScreen(m_pHandle, reinterpret_cast<POINT*>(&oRectClient.left)); // convert top-left
		return oRectClient.top;
	}

	virtual bool						IsMaximized() EW_OVERRIDE
	{
		return IsZoomed(m_pHandle);
	}

	virtual bool						IsMinimized() EW_OVERRIDE
	{
		return IsIconic(m_pHandle);
	}

	virtual bool						IsKeyCtrlDown() EW_OVERRIDE
	{
		return (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	}

	virtual bool						IsKeyAltDown() EW_OVERRIDE
	{
		return (GetKeyState(VK_MENU) & 0x8000) != 0;
	}

	virtual bool						IsKeyShiftDown()
	{
		return (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	}

	virtual void*					GetHandle() EW_OVERRIDE
	{
		return (void*)m_pHandle;
	}
protected:
	HWND							m_pHandle;
	bool							m_bManualSizing;
	RECT							m_oBorderThickness;
	bool							m_bSizing;
	LONG_PTR						m_iSizingMode;

	static bool						s_bClassInitialized;
	static EKey						s_iTranslateKeys[256];

	static LRESULT					CALLBACK Proc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		EasyWindowWin32* pThis = (EasyWindowWin32*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		if (pThis != NULL)
		{
			switch (iMsg)
			{
			case WM_ERASEBKGND:
				return 1;
				break;
			case WM_PAINT:
				return 1;
				break;
			case WM_NCPAINT:
				//return 0;
				break;
			case WM_SIZE:
				pThis->OnSize(LOWORD(lParam), HIWORD(lParam));
				break;
			case WM_MOVE:
				pThis->OnMove(LOWORD(lParam), HIWORD(lParam));
				break;
			case WM_SETFOCUS:
				pThis->OnFocus(true);
				break;
			case WM_KILLFOCUS:
				pThis->OnFocus(false);
				break;
			case WM_CLOSE:
				if (pThis->OnClose())
					return 1;
				break;
			case WM_LBUTTONDOWN:
				if (!pThis->m_bSizing)
					pThis->OnMouseButton(0, true);
				break;
			case WM_LBUTTONUP:
				if (pThis->m_bSizing)
				{
					ReleaseCapture();
					pThis->m_bSizing = false;
				}
				else
				{
					pThis->OnMouseButton(0, false);
				}
				break;
			case WM_RBUTTONDOWN:
				pThis->OnMouseButton(1, true);
				break;
			case WM_RBUTTONUP:
				pThis->OnMouseButton(1, false);
				break;
			case WM_MBUTTONDOWN:
				pThis->OnMouseButton(2, true);
				break;
			case WM_MBUTTONUP:
				pThis->OnMouseButton(2, false);
				break;
			case WM_XBUTTONDOWN:
				pThis->OnMouseButton(3 + GET_XBUTTON_WPARAM(wParam), false);
				break;
			case WM_XBUTTONUP:
				pThis->OnMouseButton(3 + GET_XBUTTON_WPARAM(wParam), false);
				break;
			case WM_MOUSEMOVE:
				/*if (!m_bMouseTracking)
				{
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = m_hWnd;
					if (TrackMouseEvent(&tme))
					{
						m_bMouseTracking = true;
					}
				}*/
				POINT oCursorPos;
				GetCursorPos(&oCursorPos);
				if (pThis->m_bSizing)
				{
					RECT oRect;
					GetWindowRect(pThis->m_pHandle, &oRect);
					switch (pThis->m_iSizingMode)
					{
					case HTLEFT:
						oRect.left = oCursorPos.x;
						break;
					case HTTOPLEFT:
						oRect.top = oCursorPos.y;
						oRect.left = oCursorPos.x;
						break;
					case HTBOTTOMLEFT:
						oRect.bottom = oCursorPos.y;
						oRect.left = oCursorPos.x;
						break;
					case HTRIGHT:
						oRect.right = oCursorPos.x;
						break;
					case HTTOPRIGHT:
						oRect.top = oCursorPos.y;
						oRect.right = oCursorPos.x;
						break;
					case HTBOTTOMRIGHT:
						oRect.bottom = oCursorPos.y;
						oRect.right = oCursorPos.x;
						break;
					case HTTOP:
						oRect.top = oCursorPos.y;
						break;
					case HTBOTTOM:
						oRect.bottom = oCursorPos.y;
						break;
					}
					SetWindowPos(pThis->m_pHandle, NULL, oRect.left, oRect.top, oRect.right - oRect.left, oRect.bottom - oRect.top, 0);
				}
				else
				{
					pThis->OnMouseMove((signed short)(lParam), (signed short)(lParam >> 16));
				}
				break;
			case WM_MOUSEWHEEL:
				pThis->OnMouseWheel( GET_WHEEL_DELTA_WPARAM( wParam ) / WHEEL_DELTA );
				break;
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
				return 1;
			case WM_KEYDOWN:
				if (wParam >= 0 && wParam < 256 && (HIWORD(lParam) & KF_REPEAT) == 0)
				{
					EKey eKey = (EKey)s_iTranslateKeys[wParam];
					pThis->OnKey(eKey, true);
				}
				break;
			case WM_KEYUP:
				if (wParam >= 0 && wParam < 256)
				{
					EKey eKey = (EKey)s_iTranslateKeys[wParam];
					pThis->OnKey(eKey, false);
				}
				break;
			case WM_CHAR:
				pThis->OnChar(wParam);
				break;

			case WM_NCCALCSIZE:
				if (pThis->m_bManualSizing && lParam)
				{
					//NCCALCSIZE_PARAMS* pSizeParams = (NCCALCSIZE_PARAMS*)lParam;
					//pSizeParams->rgrc[0].left += pThis->m_oBorderThickness.left;
					//pSizeParams->rgrc[0].right -= pThis->m_oBorderThickness.right;
					//pSizeParams->rgrc[0].bottom -= pThis->m_oBorderThickness.bottom;
					return 0;
				}
				break;
			case WM_NCHITTEST:
				if (pThis->m_bManualSizing)
				{
					RECT oWindowRect;
					const RECT& oBorder = pThis->m_oBorderThickness;

					GetWindowRect(pThis->m_pHandle, &oWindowRect);
					int x = LOWORD(lParam) - oWindowRect.left;
					int y = HIWORD(lParam) - oWindowRect.top;

					//return HTCLIENT;

					if (x < oBorder.left && y < oBorder.top)
						return HTTOPLEFT;
					else if (x > oWindowRect.right - oWindowRect.left - oBorder.right && y < oBorder.top)
						return HTTOPRIGHT;
					else if (x > oWindowRect.right - oWindowRect.left - oBorder.right && y > oWindowRect.bottom - oWindowRect.top - oBorder.bottom)
						return HTBOTTOMRIGHT;
					else if (x < oBorder.left && y > oWindowRect.bottom - oWindowRect.top - oBorder.bottom)
						return HTBOTTOMLEFT;
					else if (x < oBorder.left)
						return HTLEFT;
					else if (y < oBorder.top)
						return HTTOP;
					else if (x > oWindowRect.right - oWindowRect.left - oBorder.right)
						return HTRIGHT;
					else if (y > oWindowRect.bottom - oWindowRect.top - oBorder.bottom)
						return HTBOTTOM;
					//else if (x >= m_oCaptionArea.left && x <= m_oCaptionArea.right && y >= m_oCaptionArea.top && y <= m_oCaptionArea.bottom)
						//return HTCAPTION;
					else
						return HTCLIENT;
				}
				break;
			case WM_NCLBUTTONDOWN:
				if (pThis->m_bManualSizing)
				{
					switch (wParam)
					{
					case HTLEFT:
					case HTTOPLEFT:
					case HTBOTTOMLEFT:
					case HTRIGHT:
					case HTTOPRIGHT:
					case HTBOTTOMRIGHT:
					case HTTOP:
					case HTBOTTOM:
						SetCapture(hWnd);
						pThis->m_bSizing = true;
						pThis->m_iSizingMode = wParam;
						return 0;
					}
				}
				break;
			case WM_NCLBUTTONUP:
				if (pThis->m_bSizing)
				{
					ReleaseCapture();
					pThis->m_bSizing = false;
				}
				break;
			}
		}
		return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
};

bool EasyWindowWin32::s_bClassInitialized = false;
EasyWindowWin32::EKey EasyWindowWin32::s_iTranslateKeys[256];

EasyWindow* EasyWindow::Create(const char* pTitle, int iWidth, int iHeight, bool bClientSize, EasyWindow* pParent, EWindowStyle eStyle)
{
	return new EasyWindowWin32(pTitle, iWidth, iHeight, bClientSize, pParent, eStyle);
}

#endif //_WIN32 || _WIN64 || _MSC_VER