
#ifndef __IM_PLATFORM_WINDOW_DX11_H__
#define __IM_PLATFORM_WINDOW_DX11_H__

#include "ImwConfig.h"
#include "ImwPlatformWindowEasyWindow.h"

class IDXGIFactory;
class IDXGISwapChain;
class ID3D11Device;
class ID3D11DeviceContext;
class ID3D11RenderTargetView;
class ID3D11Texture2D;
class ID3D11SamplerState;
class ID3D11ShaderResourceView;
class ID3D11VertexShader;
class ID3D11PixelShader;
class ID3D11InputLayout;
class ID3D11Buffer;
class ID3D11RasterizerState;
class ID3D11BlendState;

namespace ImWindow
{
	class ImwPlatformWindowDX11 : public ImwPlatformWindowEasyWindow
	{
	public:
											ImwPlatformWindowDX11(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindowDX11();

		virtual bool						Init(ImwPlatformWindow* pMain);
	protected:
		virtual void						RenderDrawLists(ImDrawData* pDrawData);

		virtual void						OnClientSize(int iClientWidth, int iClientHeight);
		
		IDXGISwapChain*						m_pDXGISwapChain;
		ID3D11RenderTargetView*				m_pDX11RenderTargetView;

		ID3D11Buffer*						m_pDX11VertexBuffer;
		int									m_iVertexBufferSize;
		ID3D11Buffer*						m_pDX11IndexBuffer;
		int									m_iIndexBufferSize;

		// Shared
		IDXGIFactory*						m_pDXGIFactory;
		ID3D11Device*						m_pDX11Device;
		ID3D11DeviceContext*				m_pDX11DeviceContext;

		ID3D11Texture2D*					m_pDX11FontTexture;
		ID3D11SamplerState*					m_pDX11FontSampler;
		ID3D11ShaderResourceView*			m_pDX11FontTextureView;

		ID3D11VertexShader*					m_pDX11VertexShader;
		ID3D11PixelShader*					m_pDX11PixelShader;
		ID3D11InputLayout*					m_pDX11InputLayout;
		ID3D11Buffer*						m_pDX11VertexConstantBuffer;

		ID3D11BlendState*					m_pDX11BlendState;
		ID3D11RasterizerState*				m_pDX11RasterizerState;
	};
}

#endif // __IM_PLATFORM_WINDOW_DX11_H__