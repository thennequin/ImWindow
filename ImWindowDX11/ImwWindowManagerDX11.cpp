#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImwWindowManagerDX11.h"
#include "ImwPlatformWindowDX11.h"

#include "windows.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxerr.lib")

#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DxErr.h>
int ( WINAPIV * __vsnprintf )( char *, size_t, const char*, va_list ) = _vsnprintf;

using namespace ImWindow;

ImwWindowManagerDX11::ImwWindowManagerDX11(bool bCustomFrame)
	: m_pDXGIFactory( NULL )
	, m_pDX11Device( NULL )
	, m_pDX11DeviceContext( NULL )
	, m_bCustomFrame(bCustomFrame)
{
}

ImwWindowManagerDX11::~ImwWindowManagerDX11()
{
}

bool ImwWindowManagerDX11::InternalInit()
{
	int iResult = CreateDXGIFactory( __uuidof( IDXGIFactory ), (void**)&m_pDXGIFactory );
	if( FAILED( iResult ) )
	{
		MessageBox( NULL, DXGetErrorDescription( iResult ), TEXT( "Can't create FXGI factory" ), MB_ICONERROR | MB_OK );
		return false;
	}

	iResult = D3D11CreateDevice( NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&m_pDX11Device,
		NULL,
		&m_pDX11DeviceContext );

	if( FAILED( iResult ) )
	{
		MessageBox( NULL, DXGetErrorDescription( iResult ), TEXT( "Can't create DX11 device and device context" ), MB_ICONERROR | MB_OK );
		return false;
	}

	return true;
}

void ImwWindowManagerDX11::InternalDestroy()
{
	ImwSafeRelease( m_pDXGIFactory );
	ImwSafeRelease( m_pDX11Device );
	ImwSafeRelease( m_pDX11DeviceContext );
}

ImwPlatformWindow* ImwWindowManagerDX11::CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent)
{
	IM_ASSERT(m_pCurrentPlatformWindow == NULL);
	ImwPlatformWindowDX11* pWindow = new ImwPlatformWindowDX11(eType, CanCreateMultipleWindow());
	if (pWindow->Init(pParent))
	{
		return (ImwPlatformWindow*)pWindow;
	}
	else
	{
		delete pWindow;
		return NULL;
	}
}

ImVec2 ImwWindowManagerDX11::GetCursorPos()
{
	POINT oPoint;
	::GetCursorPos(&oPoint);
	return ImVec2((float)oPoint.x, (float)oPoint.y);
}

bool ImwWindowManagerDX11::IsLeftClickDown()
{
	return GetAsyncKeyState(VK_LBUTTON) != 0;
}

bool ImwWindowManagerDX11::IsUsingCustomFrame() const
{
	return m_bCustomFrame;
}

const float c_fIconSize = 20.f;
float ImwWindowManagerDX11::GetTitleBarHeight() const
{
	ImGuiContext* pContext = m_pMainPlatformWindow->GetContext();
	float fContentSize = pContext->Style.FramePadding.y * 2.f + pContext->FontSize;
	if ((c_fIconSize +1.f)> fContentSize)
		fContentSize = c_fIconSize + 1.f;
	return pContext->Style.WindowPadding.y + fContentSize;
}

void ImwWindowManagerDX11::PaintTitleBar(ImwPlatformWindow* pPlatformWindow)
{
	//Draw simple icon in title bar
	ImGui::Dummy(ImVec2(c_fIconSize, c_fIconSize));
	ImRect oRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	const ImVec2 oCenter = oRect.GetCenter();
	const ImVec2 oSize = oRect.GetSize();
	const ImVec2 c_oSpace = ImVec2(1.f, 1.f);

	ImU32 iColor = ImGui::GetColorU32(ImGuiCol_Text);
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	pDrawList->AddRectFilled(oRect.Min, oCenter - c_oSpace, iColor);
	pDrawList->AddRectFilled(oCenter + c_oSpace, oRect.Max, iColor);
	pDrawList->AddLine(oRect.GetBL(), oRect.GetTR(), iColor);

	ImGui::SameLine();
	ImwWindowManager::PaintTitleBar(pPlatformWindow);
}