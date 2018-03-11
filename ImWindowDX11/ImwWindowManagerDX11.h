
#ifndef __IM_WINDOW_MANAGER_DX11_H__
#define __IM_WINDOW_MANAGER_DX11_H__

#include "ImwConfig.h"

#include "ImwWindowManager.h"

class IDXGIFactory;
class IDXGISwapChain;
class ID3D11Device;
class ID3D11DeviceContext;

namespace ImWindow
{
	class ImwWindowManagerDX11 : public ImwWindowManager
	{
	public:
		ImwWindowManagerDX11(bool bCustomFrame);
		virtual							~ImwWindowManagerDX11();

		IDXGIFactory*					GetDXGIFactory() const { return m_pDXGIFactory; }
		ID3D11Device*					GetDX11Device() const { return m_pDX11Device; }
		ID3D11DeviceContext*			GetDX11DeviceContext() const { return m_pDX11DeviceContext; }

		virtual bool					IsUsingCustomFrame() const;
	protected:
		virtual bool					InternalInit();
		virtual void					InternalDestroy();
		virtual bool					CanCreateMultipleWindow() { return true; }
		virtual ImwPlatformWindow*		CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);

		virtual ImVec2					GetCursorPos();
		virtual bool					IsLeftClickDown();

		virtual float					GetTitleBarHeight() const;
		virtual void					PaintTitleBar(ImwPlatformWindow* pPlatformWindow);
	protected:
		IDXGIFactory*					m_pDXGIFactory;
		ID3D11Device*					m_pDX11Device;
		ID3D11DeviceContext*			m_pDX11DeviceContext;

		const bool						m_bCustomFrame;
	};
}

#endif //__IM_WINDOW_MANAGER_DX11_H__