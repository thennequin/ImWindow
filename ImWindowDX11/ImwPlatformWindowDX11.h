
#ifndef __IM_PLATFORM_WINDOW_DX11_H__
#define __IM_PLATFORM_WINDOW_DX11_H__

#include "ImwConfig.h"
#include "ImwPlatformWindowEasyWindow.h"

struct IDXGIFactory;
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11RasterizerState;
struct ID3D11BlendState;

namespace ImWindow
{
	class ImwPlatformWindowDX11 : public ImwPlatformWindowEasyWindow
	{
		friend class ImwWindowManagerDX11;
	public:
											ImwPlatformWindowDX11(EPlatformWindowType eType, bool bCreateState);
		virtual								~ImwPlatformWindowDX11();

		virtual bool						Init(ImwPlatformWindow* pMain);
		virtual void						RegenFontTexture(ImwPlatformWindow* pMain);
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
