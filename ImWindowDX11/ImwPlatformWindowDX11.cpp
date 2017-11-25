
#include "ImwPlatformWindowDX11.h"

#include "ImwWindowManagerDX11.h"

#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DxErr.h>
extern int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list);

struct VERTEX_CONSTANT_BUFFER
{
	float        mvp[4][4];
};

// Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
struct BACKUP_DX11_STATE
{
	UINT                        ScissorRectsCount, ViewportsCount;
	D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	ID3D11RasterizerState*      RS;
	ID3D11BlendState*           BlendState;
	FLOAT                       BlendFactor[4];
	UINT                        SampleMask;
	ID3D11ShaderResourceView*   PSShaderResource;
	ID3D11SamplerState*         PSSampler;
	ID3D11PixelShader*          PS;
	ID3D11VertexShader*         VS;
	UINT                        PSInstancesCount, VSInstancesCount;
	ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
	D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
	ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
	UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
	DXGI_FORMAT                 IndexBufferFormat;
	ID3D11InputLayout*          InputLayout;

	void						Backup(ID3D11DeviceContext* pDeviceContext)
	{
		ScissorRectsCount = ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		pDeviceContext->RSGetScissorRects(&ScissorRectsCount, ScissorRects);
		pDeviceContext->RSGetViewports(&ViewportsCount, Viewports);
		pDeviceContext->RSGetState(&RS);
		pDeviceContext->OMGetBlendState(&BlendState, BlendFactor, &SampleMask);
		pDeviceContext->PSGetShaderResources(0, 1, &PSShaderResource);
		pDeviceContext->PSGetSamplers(0, 1, &PSSampler);
		PSInstancesCount = VSInstancesCount = 256;
		pDeviceContext->PSGetShader(&PS, PSInstances, &PSInstancesCount);
		pDeviceContext->VSGetShader(&VS, VSInstances, &VSInstancesCount);
		pDeviceContext->VSGetConstantBuffers(0, 1, &VSConstantBuffer);
		pDeviceContext->IAGetPrimitiveTopology(&PrimitiveTopology);
		pDeviceContext->IAGetIndexBuffer(&IndexBuffer, &IndexBufferFormat, &IndexBufferOffset);
		pDeviceContext->IAGetVertexBuffers(0, 1, &VertexBuffer, &VertexBufferStride, &VertexBufferOffset);
		pDeviceContext->IAGetInputLayout(&InputLayout);
	}

	void						Restore(ID3D11DeviceContext* pDeviceContext)
	{
		pDeviceContext->RSSetScissorRects(ScissorRectsCount, ScissorRects);
		pDeviceContext->RSSetViewports(ViewportsCount, Viewports);
		pDeviceContext->RSSetState(RS); if (RS) RS->Release();
		pDeviceContext->OMSetBlendState(BlendState, BlendFactor, SampleMask); if (BlendState) BlendState->Release();
		pDeviceContext->PSSetShaderResources(0, 1, &PSShaderResource); if (PSShaderResource) PSShaderResource->Release();
		pDeviceContext->PSSetSamplers(0, 1, &PSSampler); if (PSSampler) PSSampler->Release();
		pDeviceContext->PSSetShader(PS, PSInstances, PSInstancesCount); if (PS) PS->Release();
		for (UINT i = 0; i < PSInstancesCount; i++) if (PSInstances[i]) PSInstances[i]->Release();
		pDeviceContext->VSSetShader(VS, VSInstances, VSInstancesCount); if (VS) VS->Release();
		pDeviceContext->VSSetConstantBuffers(0, 1, &VSConstantBuffer); if (VSConstantBuffer) VSConstantBuffer->Release();
		for (UINT i = 0; i < VSInstancesCount; i++) if (VSInstances[i]) VSInstances[i]->Release();
		pDeviceContext->IASetPrimitiveTopology(PrimitiveTopology);
		pDeviceContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset); if (IndexBuffer) IndexBuffer->Release();
		pDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexBufferStride, &VertexBufferOffset); if (VertexBuffer) VertexBuffer->Release();
		pDeviceContext->IASetInputLayout(InputLayout); if (InputLayout) InputLayout->Release();
	}
};

#define SHADER_SOURCE(...) #__VA_ARGS__

static const char* c_pVertexShader = SHADER_SOURCE(
	cbuffer vertexBuffer : register(b0)
	{
		float4x4 ProjectionMatrix;
	};

	struct VS_INPUT
	{
		float2 pos : POSITION;
		float4 col : COLOR0;
		float2 uv  : TEXCOORD0;
	};

	struct PS_INPUT
	{
		float4 pos : SV_POSITION;
		float4 col : COLOR0;
		float2 uv  : TEXCOORD0;
	};

	PS_INPUT main(VS_INPUT input)
	{
		PS_INPUT output;
		output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
		output.col = input.col;
		output.uv  = input.uv;
		return output;
	}
);

static const char* c_pPixelShader = SHADER_SOURCE(
	struct PS_INPUT
	{
		float4 pos : SV_POSITION;
		float4 col : COLOR0;
		float2 uv  : TEXCOORD0;
	};

	sampler sampler0;
	Texture2D texture0;

	float4 main(PS_INPUT input) : SV_Target
	{
		float4 out_col = input.col * texture0.Sample(sampler0, input.uv); 
		return out_col; 
	}
);

using namespace ImWindow;

ImwPlatformWindowDX11::ImwPlatformWindowDX11(EPlatformWindowType eType, bool bCreateState)
	: ImwPlatformWindow(eType, bCreateState)
	, m_pWindow( NULL )
	, m_pDXGISwapChain(NULL)
	, m_pDX11RenderTargetView(NULL)
	// Shared
	, m_pDXGIFactory( NULL )
	, m_pDX11Device( NULL )
	, m_pDX11DeviceContext( NULL )
	, m_pDX11FontTexture( NULL )
	, m_pDX11FontSampler( NULL )
	, m_pDX11FontTextureView( NULL )
	, m_pDX11VertexShader( NULL )
	, m_pDX11PixelShader( NULL )
	, m_pDX11InputLayout( NULL )
	, m_pDX11VertexConstantBuffer( NULL )
	, m_pDX11BlendState( NULL )
	, m_pDX11RasterizerState( NULL )
	, m_pDX11VertexBuffer( NULL )
	, m_iVertexBufferSize( 0 )
	, m_pDX11IndexBuffer( NULL )
	, m_iIndexBufferSize( 0 )
{
}

ImwPlatformWindowDX11::~ImwPlatformWindowDX11()
{
	if (m_eType == E_PLATFORM_WINDOW_TYPE_MAIN)
	{
		// Shared
		ImwSafeRelease(m_pDX11FontTexture);
		ImwSafeRelease(m_pDX11FontSampler);
		ImwSafeRelease(m_pDX11FontTextureView);

		ImwSafeRelease(m_pDX11VertexShader);
		ImwSafeRelease(m_pDX11PixelShader);
		ImwSafeRelease(m_pDX11InputLayout);

		ImwSafeRelease(m_pDX11BlendState);
		ImwSafeRelease(m_pDX11RasterizerState);
	}

	ImwSafeRelease(m_pDXGISwapChain);
	ImwSafeRelease(m_pDX11RenderTargetView);
	
	ImwSafeRelease(m_pDX11VertexBuffer);
	ImwSafeRelease(m_pDX11IndexBuffer);

	ImwSafeDelete(m_pWindow);
}

bool ImwPlatformWindowDX11::Init(ImwPlatformWindow* pMain)
{
	ImwPlatformWindowDX11* pMainWindow = ((ImwPlatformWindowDX11*)pMain);

	EasyWindow::EWindowStyle eStyle = EasyWindow::E_STYLE_NORMAL;
	if (m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW)
		eStyle = EasyWindow::E_STYLE_POPUP;

	m_pWindow = EasyWindow::Create("ImwPlatformWindowDX11", 800, 600, false, pMain != NULL ? pMainWindow->m_pWindow : NULL, eStyle);
	m_pWindow->OnClose.Set(this, &ImwPlatformWindowDX11::OnClose);
	m_pWindow->OnFocus.Set(this, &ImwPlatformWindowDX11::OnFocus);
	m_pWindow->OnSize.Set(this, &ImwPlatformWindowDX11::OnSize);
	m_pWindow->OnMouseButton.Set(this, &ImwPlatformWindowDX11::OnMouseButton);
	m_pWindow->OnMouseMove.Set(this, &ImwPlatformWindowDX11::OnMouseMove);
	m_pWindow->OnMouseWheel.Set(this, &ImwPlatformWindowDX11::OnMouseWheel);
	m_pWindow->OnKey.Set(this, &ImwPlatformWindowDX11::OnKey);
	m_pWindow->OnChar.Set(this, &ImwPlatformWindowDX11::OnChar);

	if (m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW)
		m_pWindow->SetAlpha(128);

	ImwWindowManagerDX11* pWindowManagerDX11 = (ImwWindowManagerDX11*)ImWindow::ImwWindowManager::GetInstance();

	m_pDXGIFactory = pWindowManagerDX11->GetDXGIFactory();
	m_pDX11Device = pWindowManagerDX11->GetDX11Device();
	m_pDX11DeviceContext = pWindowManagerDX11->GetDX11DeviceContext();

	////
	HRESULT iResult;

	if (NULL == pMainWindow)
	{
		// Create the vertex shader
		{
			ID3D10Blob* pVertexShaderBlob = NULL;
			iResult = D3DCompile(c_pVertexShader, strlen(c_pVertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &pVertexShaderBlob, NULL);
			
			if (pVertexShaderBlob == NULL) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
			{
				MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : Can't compile vertex shader"), MB_ICONERROR | MB_OK);
				return false;
			}

			iResult = m_pDX11Device->CreateVertexShader((DWORD*)pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pDX11VertexShader);
			if (FAILED(iResult))
			{
				MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : Can't create vertex shader"), MB_ICONERROR | MB_OK);
				return false;
			}

			// Create the input layout
			D3D11_INPUT_ELEMENT_DESC local_layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			iResult = m_pDX11Device->CreateInputLayout(local_layout, 3, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &m_pDX11InputLayout);
			if (FAILED(iResult))
			{
				MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : Can't create input layout for vertex shader"), MB_ICONERROR | MB_OK);
				return false;
			}

			// Create the constant buffer
			{
				D3D11_BUFFER_DESC desc;
				desc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;
				iResult = m_pDX11Device->CreateBuffer(&desc, NULL, &m_pDX11VertexConstantBuffer);
				if (FAILED(iResult))
				{
					MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : Can't create constant buffer"), MB_ICONERROR | MB_OK);
					return false;
				}
			}

			ImwSafeRelease(pVertexShaderBlob);
		}

		// Create the pixel shader
		{
			ID3D10Blob* pPixelShaderBlob = NULL;
			iResult = D3DCompile(c_pPixelShader, strlen(c_pPixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &pPixelShaderBlob, NULL);
			if (pPixelShaderBlob == NULL)  // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
			{
				MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : Can't compile pixel shader"), MB_ICONERROR | MB_OK);
				return false;
			}

			iResult = m_pDX11Device->CreatePixelShader((DWORD*)pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pDX11PixelShader);
			if (FAILED(iResult))
			{
				MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : Can't create pixel shader"), MB_ICONERROR | MB_OK);
				return false;
			}
		}

		// Create the blending setup
		{
			D3D11_BLEND_DESC oBlendDesc;
			ZeroMemory(&oBlendDesc, sizeof(oBlendDesc));
			oBlendDesc.AlphaToCoverageEnable = false;
			oBlendDesc.RenderTarget[0].BlendEnable = true;
			oBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			oBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			oBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			oBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			oBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			oBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			oBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			m_pDX11Device->CreateBlendState(&oBlendDesc, &m_pDX11BlendState);
		}

		// Create the rasterizer state
		{
			D3D11_RASTERIZER_DESC oRasterizerDesc;
			ZeroMemory(&oRasterizerDesc, sizeof(oRasterizerDesc));
			oRasterizerDesc.FillMode = D3D11_FILL_SOLID;
			oRasterizerDesc.CullMode = D3D11_CULL_NONE;
			oRasterizerDesc.ScissorEnable = true;
			oRasterizerDesc.DepthClipEnable = true;
			m_pDX11Device->CreateRasterizerState(&oRasterizerDesc, &m_pDX11RasterizerState);
		}
	}
	else
	{
		m_pDXGIFactory = pMainWindow->m_pDXGIFactory;
		m_pDX11Device = pMainWindow->m_pDX11Device;
		m_pDX11DeviceContext = pMainWindow->m_pDX11DeviceContext;
		m_pDX11FontTexture = pMainWindow->m_pDX11FontTexture;
		m_pDX11FontSampler = pMainWindow->m_pDX11FontSampler;

		m_pDX11VertexShader = pMainWindow->m_pDX11VertexShader;
		m_pDX11PixelShader = pMainWindow->m_pDX11PixelShader;
		m_pDX11InputLayout = pMainWindow->m_pDX11InputLayout;
		m_pDX11VertexConstantBuffer = pMainWindow->m_pDX11VertexConstantBuffer;

		m_pDX11BlendState = pMainWindow->m_pDX11BlendState;
		m_pDX11RasterizerState = pMainWindow->m_pDX11RasterizerState;
	}

	DXGI_SWAP_CHAIN_DESC oSwapChainDesc;
	ZeroMemory(&oSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	oSwapChainDesc.BufferCount = 1;
	oSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	oSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	oSwapChainDesc.OutputWindow = (HWND)m_pWindow->GetHandle();
	oSwapChainDesc.SampleDesc.Count = 4;
	oSwapChainDesc.Windowed = true;

	iResult = m_pDXGIFactory->CreateSwapChain(m_pDX11Device, &oSwapChainDesc, &m_pDXGISwapChain);

	if (FAILED(iResult))
	{
		MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error: Can't create swap chain"), MB_ICONERROR | MB_OK);
		return false;
	}

	iResult = m_pDXGIFactory->MakeWindowAssociation((HWND)m_pWindow->GetHandle(), DXGI_MWA_NO_ALT_ENTER);

	if (FAILED(iResult))
	{
		MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : DXGI MakeWindowAssociation failed!"), MB_ICONERROR | MB_OK);
		//return false;
	}

	//Create our BackBuffer
	ID3D11Texture2D* pBackBuffer;
	iResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(iResult))
	{
		MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : Can't get Buffer of swapchain"), MB_ICONERROR | MB_OK);
		return false;
	}

	//Create our Render Target
	iResult = m_pDX11Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pDX11RenderTargetView);
	pBackBuffer->Release();
	if (FAILED(iResult))
	{
		MessageBox(NULL, DXGetErrorDescription(iResult), TEXT("Error : Can't create RenderTargetView"), MB_ICONERROR | MB_OK);
		return false;
	}

	//Set our Render Target
	m_pDX11DeviceContext->OMSetRenderTargets(1, &m_pDX11RenderTargetView, NULL);

	SetState();
	ImGuiIO& io = ImGui::GetIO();

	if (NULL == pMainWindow)
	{
		unsigned char* pPixels;
		int iWidth;
		int iHeight;
		io.Fonts->AddFontDefault();
		io.Fonts->GetTexDataAsRGBA32(&pPixels, &iWidth, &iHeight);

		D3D11_TEXTURE2D_DESC oTextureDesc;
		ZeroMemory(&oTextureDesc, sizeof(oTextureDesc));
		oTextureDesc.Width = iWidth;
		oTextureDesc.Height = iHeight;
		oTextureDesc.MipLevels = 1;
		oTextureDesc.ArraySize = 1;
		oTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		oTextureDesc.SampleDesc.Count = 1;
		oTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		oTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		oTextureDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA oSubResource;
		oSubResource.pSysMem = pPixels;
		oSubResource.SysMemPitch = oTextureDesc.Width * 4;
		oSubResource.SysMemSlicePitch = 0;
		m_pDX11Device->CreateTexture2D(&oTextureDesc, &oSubResource, &m_pDX11FontTexture);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC oShaderResViewDesc;
		ZeroMemory(&oShaderResViewDesc, sizeof(oShaderResViewDesc));
		oShaderResViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		oShaderResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		oShaderResViewDesc.Texture2D.MipLevels = oTextureDesc.MipLevels;
		oShaderResViewDesc.Texture2D.MostDetailedMip = 0;
		m_pDX11Device->CreateShaderResourceView(m_pDX11FontTexture, &oShaderResViewDesc, &m_pDX11FontTextureView);


		// Create texture sampler
		D3D11_SAMPLER_DESC oSamplerDesc;
		ZeroMemory(&oSamplerDesc, sizeof(oSamplerDesc));
		oSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		oSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		oSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		oSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		oSamplerDesc.MipLODBias = 0.f;
		oSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		oSamplerDesc.MinLOD = 0.f;
		oSamplerDesc.MaxLOD = 0.f;
		m_pDX11Device->CreateSamplerState(&oSamplerDesc, &m_pDX11FontSampler);
		// Store our identifier
		io.Fonts->TexID = (void *)(intptr_t)m_pDX11FontTextureView;
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

ImVec2 ImwPlatformWindowDX11::GetPosition() const
{
	return ImVec2(float(m_pWindow->GetClientPositionX()), float(m_pWindow->GetClientPositionY()));
}

ImVec2 ImwPlatformWindowDX11::GetSize() const
{
	return ImVec2(float(m_pWindow->GetClientWidth()), float(m_pWindow->GetClientHeight()));
}

bool ImwPlatformWindowDX11::IsWindowMaximized() const
{
	return m_pWindow->IsMaximized();
}

bool ImwPlatformWindowDX11::IsWindowMinimized() const
{
	return m_pWindow->IsMinimized();
}

void ImwPlatformWindowDX11::Show(bool bShow)
{
	m_pWindow->Show(bShow);
}

void ImwPlatformWindowDX11::SetSize(int iWidth, int iHeight)
{
	m_pWindow->SetSize(iWidth, iHeight, false);
}

void ImwPlatformWindowDX11::SetPosition(int iX, int iY)
{
	m_pWindow->SetPosition(iX, iY);
}

void ImwPlatformWindowDX11::SetWindowMaximized(bool bMaximized)
{
	if (bMaximized)
		m_pWindow->SetMaximized();
	else
		m_pWindow->SetRestored();
}

void ImwPlatformWindowDX11::SetWindowMinimized()
{
	m_pWindow->SetMinimized();
}

void ImwPlatformWindowDX11::SetTitle(const ImwChar* pTitle)
{
	m_pWindow->SetTitle(pTitle);
}

void ImwPlatformWindowDX11::PreUpdate()
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

void ImwPlatformWindowDX11::Render()
{
	if (!m_bNeedRender)
		return;

	if (NULL != m_pDXGISwapChain)
	{
		float fBgColor[4] = { 0.4f, 0.4f, 0.4f, 1.0f };

		m_pDX11DeviceContext->OMSetRenderTargets(1, &m_pDX11RenderTargetView, NULL);

		ImwIsSafe(m_pDX11DeviceContext)->ClearRenderTargetView(m_pDX11RenderTargetView, fBgColor);

		SetState();

		ImVec2 oSize = ImVec2(float(m_pWindow->GetClientWidth()), float(m_pWindow->GetClientHeight()));
		ImGui::GetIO().DisplaySize = oSize;

		ImGui::Render();
		RenderDrawList(ImGui::GetDrawData());

		RestoreState();

		//Present the backbuffer to the screen
		m_pDXGISwapChain->Present(0, 0);
	}
}

bool ImwPlatformWindowDX11::OnClose()
{
	ImwPlatformWindow::OnClose();
	return true;
}

void ImwPlatformWindowDX11::OnFocus(bool bHasFocus)
{
	if (!bHasFocus)
		OnLoseFocus();
}

void ImwPlatformWindowDX11::OnSize(int iWidth, int iHeight)
{
	if (NULL != m_pDXGISwapChain)
	{
		m_pDX11DeviceContext->OMSetRenderTargets(0, 0, 0);

		m_pDX11RenderTargetView->Release();

		HRESULT iResult;
		iResult = m_pDXGISwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		// Perform error handling here!

		iResult = m_pDXGIFactory->MakeWindowAssociation((HWND)m_pWindow->GetHandle(), DXGI_MWA_NO_ALT_ENTER);
		// Perform error handling here!

		// Get buffer and create a render-target-view.
		ID3D11Texture2D* pBuffer;
		iResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
		// Perform error handling here!

		iResult = m_pDX11Device->CreateRenderTargetView(pBuffer, NULL, &m_pDX11RenderTargetView);
		// Perform error handling here!
		pBuffer->Release();

		m_pDX11DeviceContext->OMSetRenderTargets(1, &m_pDX11RenderTargetView, NULL);

		// Set up the viewport.
		D3D11_VIEWPORT oViewport;
		oViewport.Width = m_pWindow->GetClientWidth();
		oViewport.Height = m_pWindow->GetClientHeight();
		oViewport.MinDepth = 0.0f;
		oViewport.MaxDepth = 1.0f;
		oViewport.TopLeftX = 0;
		oViewport.TopLeftY = 0;
		m_pDX11DeviceContext->RSSetViewports(1, &oViewport);
	}
}

void ImwPlatformWindowDX11::OnMouseButton(int iButton, bool bDown)
{
	((ImGuiState*)m_pState)->IO.MouseDown[iButton] = bDown;
}

void ImwPlatformWindowDX11::OnMouseMove(int iX, int iY)
{
	((ImGuiState*)m_pState)->IO.MousePos = ImVec2((float)iX, (float)iY);
}

void ImwPlatformWindowDX11::OnMouseWheel( int iStep )
{
	( ( ImGuiState* )m_pState )->IO.MouseWheel += iStep;
}

void ImwPlatformWindowDX11::OnKey(EasyWindow::EKey eKey, bool bDown)
{
	((ImGuiState*)m_pState)->IO.KeysDown[eKey] = bDown;
}

void ImwPlatformWindowDX11::OnChar(int iChar)
{
	((ImGuiState*)m_pState)->IO.AddInputCharacter((ImwChar)iChar);
}

void ImwPlatformWindowDX11::RenderDrawList(ImDrawData* pDrawData)
{
	if ( m_pDX11VertexBuffer == NULL || m_iVertexBufferSize < pDrawData->TotalVtxCount)
	{
		ImwSafeRelease(m_pDX11VertexBuffer);
		m_iVertexBufferSize = pDrawData->TotalVtxCount + 5000;
		D3D11_BUFFER_DESC oBufferDesc;
		memset(&oBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		oBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		oBufferDesc.ByteWidth = m_iVertexBufferSize * sizeof(ImDrawVert);
		oBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		oBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		oBufferDesc.MiscFlags = 0;
		if (m_pDX11Device->CreateBuffer(&oBufferDesc, NULL, &m_pDX11VertexBuffer) < 0)
			return;
	}

	if (m_pDX11IndexBuffer == NULL || m_iIndexBufferSize < pDrawData->TotalIdxCount)
	{
		ImwSafeRelease(m_pDX11IndexBuffer);
		D3D11_BUFFER_DESC oBufferDesc;
		m_iIndexBufferSize = pDrawData->TotalIdxCount + 10000;
		memset(&oBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		oBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		oBufferDesc.ByteWidth = m_iIndexBufferSize * sizeof(ImDrawIdx);
		oBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		oBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		if (m_pDX11Device->CreateBuffer(&oBufferDesc, NULL, &m_pDX11IndexBuffer) < 0)
			return;
	}

	// Copy and convert all vertices into a single contiguous buffer
	D3D11_MAPPED_SUBRESOURCE oMappedVertexBuffer, oMappedIndexBuffer;
	if (m_pDX11DeviceContext->Map(m_pDX11VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedVertexBuffer) != S_OK)
		return;
	if (m_pDX11DeviceContext->Map(m_pDX11IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedIndexBuffer) != S_OK)
		return;
	ImDrawVert* pVertexBufferData = (ImDrawVert*)oMappedVertexBuffer.pData;
	ImDrawIdx* pIndexBufferData = (ImDrawIdx*)oMappedIndexBuffer.pData;
	for (int n = 0; n < pDrawData->CmdListsCount; n++)
	{
		const ImDrawList* pCmdList = pDrawData->CmdLists[n];
		memcpy(pVertexBufferData, &pCmdList->VtxBuffer[0], pCmdList->VtxBuffer.size() * sizeof(ImDrawVert));
		memcpy(pIndexBufferData, &pCmdList->IdxBuffer[0], pCmdList->IdxBuffer.size() * sizeof(ImDrawIdx));
		pVertexBufferData += pCmdList->VtxBuffer.size();
		pIndexBufferData += pCmdList->IdxBuffer.size();
	}
	m_pDX11DeviceContext->Unmap(m_pDX11VertexBuffer, 0);
	m_pDX11DeviceContext->Unmap(m_pDX11IndexBuffer, 0);

	// Setup orthographic projection matrix into our constant buffer
	{
		D3D11_MAPPED_SUBRESOURCE oMappedConstantBuffer;
		if (m_pDX11DeviceContext->Map(m_pDX11VertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedConstantBuffer) != S_OK)
			return;
		VERTEX_CONSTANT_BUFFER* pConstantBufferData = (VERTEX_CONSTANT_BUFFER*)oMappedConstantBuffer.pData;
		float fL = 0.0f;
		float fR = ImGui::GetIO().DisplaySize.x;
		float fB = ImGui::GetIO().DisplaySize.y;
		float vT = 0.0f;
		float fMVP[4][4] =
		{
			{ 2.0f / (fR - fL),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f / (vT - fB),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (fR + fL) / (fL - fR),  (vT + fB) / (fB - vT),    0.5f,       1.0f },
		};
		memcpy(&pConstantBufferData->mvp, fMVP, sizeof(fMVP));
		m_pDX11DeviceContext->Unmap(m_pDX11VertexConstantBuffer, 0);
	}

	BACKUP_DX11_STATE oDeviceContextBackupState;
	oDeviceContextBackupState.Backup(m_pDX11DeviceContext);

	// Setup viewport
	D3D11_VIEWPORT oViewport;
	memset(&oViewport, 0, sizeof(D3D11_VIEWPORT));
	oViewport.Width = ImGui::GetIO().DisplaySize.x;
	oViewport.Height = ImGui::GetIO().DisplaySize.y;
	oViewport.MinDepth = 0.0f;
	oViewport.MaxDepth = 1.0f;
	oViewport.TopLeftX = oViewport.TopLeftY = 0.0f;
	m_pDX11DeviceContext->RSSetViewports(1, &oViewport);

	// Bind shader and vertex buffers
	unsigned int iStride = sizeof(ImDrawVert);
	unsigned int iOffset = 0;
	m_pDX11DeviceContext->IASetInputLayout(m_pDX11InputLayout);
	m_pDX11DeviceContext->IASetVertexBuffers(0, 1, &m_pDX11VertexBuffer, &iStride, &iOffset);
	m_pDX11DeviceContext->IASetIndexBuffer(m_pDX11IndexBuffer, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_pDX11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDX11DeviceContext->VSSetShader(m_pDX11VertexShader, NULL, 0);
	m_pDX11DeviceContext->VSSetConstantBuffers(0, 1, &m_pDX11VertexConstantBuffer);
	m_pDX11DeviceContext->PSSetShader(m_pDX11PixelShader, NULL, 0);
	m_pDX11DeviceContext->PSSetSamplers(0, 1, &m_pDX11FontSampler);

	// Setup render state
	const float fBlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_pDX11DeviceContext->OMSetBlendState(m_pDX11BlendState, fBlendFactor, 0xffffffff);
	m_pDX11DeviceContext->RSSetState(m_pDX11RasterizerState);

	// Render command lists
	int iVertexOffset = 0;
	int iIndexOffset = 0;
	for (int iCommandListIndex = 0; iCommandListIndex < pDrawData->CmdListsCount; iCommandListIndex++)
	{
		const ImDrawList* pCmdList = pDrawData->CmdLists[iCommandListIndex];
		for (int iCommandIndex = 0; iCommandIndex < pCmdList->CmdBuffer.size(); iCommandIndex++)
		{
			const ImDrawCmd* pCommand = &pCmdList->CmdBuffer[iCommandIndex];
			if (pCommand->UserCallback)
			{
				pCommand->UserCallback(pCmdList, pCommand);
			}
			else
			{
				const D3D11_RECT oRect = { (LONG)pCommand->ClipRect.x, (LONG)pCommand->ClipRect.y, (LONG)pCommand->ClipRect.z, (LONG)pCommand->ClipRect.w };
				m_pDX11DeviceContext->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pCommand->TextureId);
				m_pDX11DeviceContext->RSSetScissorRects(1, &oRect);
				m_pDX11DeviceContext->DrawIndexed(pCommand->ElemCount, iIndexOffset, iVertexOffset);
			}
			iIndexOffset += pCommand->ElemCount;
		}
		iVertexOffset += pCmdList->VtxBuffer.size();
	}

	oDeviceContextBackupState.Restore(m_pDX11DeviceContext);
}