#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImwWindowManagerDX11.h"
#include "ImwPlatformWindowDX11.h"

#include "windows.h"

#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dx11.lib")
//#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxguid.lib")

#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>

using namespace ImWindow;

ImwWindowManagerDX11::ImwWindowManagerDX11(bool bCustomFrame)
	: ImwWindowManagerEasyWindow(bCustomFrame)
	, m_pDXGIFactory( NULL )
	, m_pDX11Device( NULL )
	, m_pDX11DeviceContext( NULL )
{
}

ImwWindowManagerDX11::~ImwWindowManagerDX11()
{
}

bool ImwWindowManagerDX11::InternalInit()
{
	HRESULT iResult = CreateDXGIFactory( __uuidof( IDXGIFactory ), (void**)&m_pDXGIFactory );
	if( FAILED( iResult ) )
	{
		char pErrorMessage[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pErrorMessage, 1024, NULL);
		MessageBox( NULL, pErrorMessage, TEXT( "Can't create FXGI factory" ), MB_ICONERROR | MB_OK );
		return false;
	}

	iResult = D3D11CreateDevice( NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
#else
		NULL,
#endif
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&m_pDX11Device,
		NULL,
		&m_pDX11DeviceContext );

	if( FAILED( iResult ) )
	{
		char pErrorMessage[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, iResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pErrorMessage, 1024, NULL);
		MessageBox( NULL, pErrorMessage, TEXT( "Can't create DX11 device and device context" ), MB_ICONERROR | MB_OK );
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
const float c_fIconSize = 20.f;
float ImwWindowManagerDX11::GetTitleBarHeight() const
{
	ImGuiContext* pContext = m_pMainPlatformWindow->GetContext();
	float fContentSize = pContext->FontSize;
	if ((c_fIconSize + 1.f)> fContentSize)
		fContentSize = c_fIconSize + 1.f;
	return pContext->Style.WindowPadding.y + pContext->Style.FramePadding.y * 2.f + fContentSize;
}

void ImwWindowManagerDX11::PaintTitleBar(ImwPlatformWindow* pPlatformWindow, bool bDrawTitle)
{
	//Draw simple icon in title bar
	ImGui::Dummy(ImVec2(c_fIconSize, c_fIconSize));
	ImRect oRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	const ImVec2 oCenter = oRect.GetCenter();
	const ImVec2 oSize = oRect.GetSize();
	const ImVec2 c_oSpace = ImVec2(1.f, 1.f);

	if (ImGui::IsMouseHoveringRect(oRect.Min, oRect.Max))
	{
		((ImwPlatformWindowDX11*)pPlatformWindow)->m_eHoveredArea = E_PLATFORMWINDOWHOVEREDAREA_MENU;
	}

	ImU32 iColor = ImGui::GetColorU32(ImGuiCol_Text);
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	pDrawList->AddRectFilled(oRect.Min, oCenter - c_oSpace, iColor);
	pDrawList->AddRectFilled(oCenter + c_oSpace, oRect.Max, iColor);
	pDrawList->AddLine(oRect.GetBL(), oRect.GetTR(), iColor);

	ImGui::SameLine();
	ImwWindowManager::PaintTitleBar(pPlatformWindow, true);
}
