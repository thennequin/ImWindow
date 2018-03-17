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

ImwWindowManagerDX11::ImwWindowManagerDX11()
	: m_pDXGIFactory( NULL )
	, m_pDX11Device( NULL )
	, m_pDX11DeviceContext( NULL )
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
