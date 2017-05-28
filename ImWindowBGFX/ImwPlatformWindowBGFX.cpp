#include "ImwPlatformWindowBGFX.h"

#include "ImwWindowManager.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/fpumath.h>
#include <bgfx/embedded_shader.h>

#include "imgui/vs_ocornut_imgui.bin.h"
#include "imgui/fs_ocornut_imgui.bin.h"

static const bgfx::EmbeddedShader s_embeddedShaders[] =
{
	BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
	BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),

	BGFX_EMBEDDED_SHADER_END()
};

using namespace ImWindow;

ImwPlatformWindowBGFX::ImwPlatformWindowBGFX(bool bMain, bool bIsDragWindow, bool bCreateState, bgfx::RendererType::Enum eRenderer)
	: ImwPlatformWindow(bMain, bIsDragWindow, bCreateState)
{
	m_eRenderer = eRenderer;
}

ImwPlatformWindowBGFX::~ImwPlatformWindowBGFX()
{
	if (m_bMain)
	{
		bgfx::destroyProgram(m_hProgram);
		bgfx::destroyTexture(m_hTexture);
		bgfx::destroyUniform(m_hUniformTexture);
	}
	
	bgfx::destroyFrameBuffer(m_hFrameBufferHandle);
	bgfx::frame();
	bgfx::frame();
	ImwSafeDelete(m_pWindow);
}

inline bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexDecl& _decl, uint32_t _numIndices)
{
	return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _decl)
		&& _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices)
		;
}

bool ImwPlatformWindowBGFX::Init(ImwPlatformWindow* pMain)
{
	ImwPlatformWindowBGFX* pMainBGFX = ((ImwPlatformWindowBGFX*)pMain);

	m_pWindow = EasyWindow::Create("ImwPlatformWindowBGFX", 800, 600, false, pMain != NULL ? pMainBGFX->m_pWindow : NULL, m_bIsDragWindow ? EasyWindow::E_POPUP : EasyWindow::E_NORMAL);
	m_pWindow->OnClose.Set(this, &ImwPlatformWindowBGFX::OnClose);
	m_pWindow->OnFocus.Set(this, &ImwPlatformWindowBGFX::OnFocus);
	m_pWindow->OnSize.Set(this, &ImwPlatformWindowBGFX::OnSize);
	m_pWindow->OnMouseButton.Set(this, &ImwPlatformWindowBGFX::OnMouseButton);
	m_pWindow->OnMouseMove.Set(this, &ImwPlatformWindowBGFX::OnMouseMove);
	m_pWindow->OnKey.Set(this, &ImwPlatformWindowBGFX::OnKey);
	m_pWindow->OnChar.Set(this, &ImwPlatformWindowBGFX::OnChar);

	if (m_bMain)
	{
		bgfx::PlatformData pd;
		memset(&pd, 0, sizeof(pd));
		pd.nwh = m_pWindow->GetHandle();
		bgfx::setPlatformData(pd);

		bgfx::init(m_eRenderer);
		bgfx::reset(m_pWindow->GetClientWidth(), m_pWindow->GetClientHeight());
	}

	if (m_bIsDragWindow)
		m_pWindow->SetAlpha(128);

	m_hFrameBufferHandle = bgfx::createFrameBuffer(m_pWindow->GetHandle(), uint16_t(m_pWindow->GetClientWidth()), uint16_t(m_pWindow->GetClientHeight()));

	bgfx::setViewFrameBuffer(255, m_hFrameBufferHandle);

	SetState();
	ImGuiIO& io = ImGui::GetIO();
	
	if (pMainBGFX != NULL)
	{
		m_hTexture = pMainBGFX->m_hTexture;
		m_hUniformTexture = pMainBGFX->m_hUniformTexture;
		m_hProgram = pMainBGFX->m_hProgram;
		m_oVertexDecl = pMainBGFX->m_oVertexDecl;
	}
	else
	{
		uint8_t* data;
		int32_t width;
		int32_t height;
		io.Fonts->AddFontDefault();
		io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

		m_hTexture = bgfx::createTexture2D(
			(uint16_t)width
			, (uint16_t)height
			, false
			, 1
			, bgfx::TextureFormat::BGRA8
			, 0
			, bgfx::copy(data, width*height * 4)
		);

		m_hUniformTexture = bgfx::createUniform("s_tex", bgfx::UniformType::Int1);

		m_hProgram = bgfx::createProgram(
			bgfx::createEmbeddedShader(s_embeddedShaders, m_eRenderer, "vs_ocornut_imgui")
			, bgfx::createEmbeddedShader(s_embeddedShaders, m_eRenderer, "fs_ocornut_imgui")
			, true
		);

		m_oVertexDecl
			.begin()
			.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();
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

	//m_hCursorArrow = LoadCursor( NULL, IDC_ARROW );
	//m_hCursorResizeNS = LoadCursor( NULL, IDC_SIZENS );
	//m_hCursorResizeWE = LoadCursor( NULL, IDC_SIZEWE );

	if( pMain == NULL )
	{
		//ImGui_ImplDX11_NewFrame();
	}

	return true;
}

ImVec2 ImwPlatformWindowBGFX::GetPosition() const
{
	return ImVec2(float(m_pWindow->GetClientPositionX()), float(m_pWindow->GetClientPositionY()));
}

ImVec2 ImwPlatformWindowBGFX::GetSize() const
{
	return ImVec2(float(m_pWindow->GetClientWidth()), float(m_pWindow->GetClientHeight()));
}

bool ImwPlatformWindowBGFX::IsWindowMaximized() const
{
	return m_pWindow->IsMaximized();
}

bool ImwPlatformWindowBGFX::IsWindowMinimized() const
{
	return m_pWindow->IsMinimized();
}

void ImwPlatformWindowBGFX::Show()
{
	m_pWindow->Show(true);
}

void ImwPlatformWindowBGFX::Hide()
{
	m_pWindow->Show(false);
}

void ImwPlatformWindowBGFX::SetSize(int iWidth, int iHeight)
{
	m_pWindow->SetSize(iWidth, iHeight, false);
}

void ImwPlatformWindowBGFX::SetPosition(int iX, int iY)
{
	m_pWindow->SetPosition(iX, iY);
}

void ImwPlatformWindowBGFX::SetWindowMaximized(bool bMaximized)
{
	if (bMaximized)
		m_pWindow->SetMaximized();
	else
		m_pWindow->SetRestored();
}

void ImwPlatformWindowBGFX::SetWindowMinimized()
{
	m_pWindow->SetMinimized();
}

void ImwPlatformWindowBGFX::SetTitle(const ImwChar* pTitle)
{
	m_pWindow->SetTitle(pTitle);
}

void ImwPlatformWindowBGFX::PreUpdate()
{
	m_pWindow->Update();
	ImGuiIO& oIO = ((ImGuiState*)m_pState)->IO;
	oIO.KeyCtrl = m_pWindow->IsKeyCtrlDown();
	oIO.KeyShift = m_pWindow->IsKeyShiftDown();
	oIO.KeyAlt = m_pWindow->IsKeyAltDown();
	oIO.KeySuper = false;
}

void ImwPlatformWindowBGFX::Render()
{
	if (!m_bNeedRender)
		return;

	bgfx::reset(uint16_t(m_pWindow->GetClientWidth()), uint16_t(m_pWindow->GetClientHeight()));
	bgfx::setViewFrameBuffer(255, m_hFrameBufferHandle);
	bgfx::setViewRect(255, 0, 0, uint16_t(m_pWindow->GetClientWidth()), uint16_t(m_pWindow->GetClientHeight()));

	SetState();
	ImVec2 oSize = ImVec2(float(m_pWindow->GetClientWidth()), float(m_pWindow->GetClientHeight()));
	ImGui::GetIO().DisplaySize = oSize;

	ImGui::Render();
	RenderDrawList(ImGui::GetDrawData());

	bgfx::frame();

	RestoreState();
}

bool ImwPlatformWindowBGFX::OnClose()
{
	ImwPlatformWindow::OnClose();
	return true;
}

void ImwPlatformWindowBGFX::OnFocus(bool bHasFocus)
{
	if (!bHasFocus)
		OnLoseFocus();
}

void ImwPlatformWindowBGFX::OnSize(int iWidth, int iHeight)
{
	bgfx::destroyFrameBuffer(m_hFrameBufferHandle);
	m_hFrameBufferHandle = bgfx::createFrameBuffer(m_pWindow->GetHandle(), uint16_t(m_pWindow->GetClientWidth()), uint16_t(m_pWindow->GetClientHeight()));
	
	if (m_bMain)
	{
		bgfx::setViewFrameBuffer(255, m_hFrameBufferHandle);
		bgfx::reset(iWidth, iHeight);
	}
}

void ImwPlatformWindowBGFX::OnMouseButton(int iButton, bool bDown)
{
	((ImGuiState*)m_pState)->IO.MouseDown[iButton] = bDown;
}

void ImwPlatformWindowBGFX::OnMouseMove(int iX, int iY)
{
	((ImGuiState*)m_pState)->IO.MousePos = ImVec2((float)iX, (float)iY);
}

void ImwPlatformWindowBGFX::OnKey(EasyWindow::EKey eKey, bool bDown)
{
	((ImGuiState*)m_pState)->IO.KeysDown[eKey] = bDown;
}

void ImwPlatformWindowBGFX::OnChar(int iChar)
{
	((ImGuiState*)m_pState)->IO.AddInputCharacter((ImwChar)iChar);
}

#define IMGUI_FLAGS_NONE        UINT8_C(0x00)
#define IMGUI_FLAGS_ALPHA_BLEND UINT8_C(0x01)

void ImwPlatformWindowBGFX::RenderDrawList(ImDrawData* pDrawData)
{
	const ImGuiIO& io = ImGui::GetIO();
	const float width = io.DisplaySize.x;
	const float height = io.DisplaySize.y;

	{
		float ortho[16];
		bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, -1.0f, 1.0f);
		bgfx::setViewTransform(255, NULL, ortho);
	}

	// Render command lists
	for (int32_t ii = 0, num = pDrawData->CmdListsCount; ii < num; ++ii)
	{
		bgfx::TransientVertexBuffer tvb;
		bgfx::TransientIndexBuffer tib;

		const ImDrawList* drawList = pDrawData->CmdLists[ii];
		uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
		uint32_t numIndices = (uint32_t)drawList->IdxBuffer.size();

		if (!checkAvailTransientBuffers(numVertices, m_oVertexDecl, numIndices))
		{
			// not enough space in transient buffer just quit drawing the rest...
			break;
		}

		bgfx::allocTransientVertexBuffer(&tvb, numVertices, m_oVertexDecl);
		bgfx::allocTransientIndexBuffer(&tib, numIndices);

		ImDrawVert* verts = (ImDrawVert*)tvb.data;
		bx::memCopy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

		ImDrawIdx* indices = (ImDrawIdx*)tib.data;
		bx::memCopy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

		uint32_t offset = 0;
		for (const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
		{
			if (cmd->UserCallback)
			{
				cmd->UserCallback(drawList, cmd);
			}
			else if (0 != cmd->ElemCount)
			{
				uint64_t state = 0
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_ALPHA_WRITE
					| BGFX_STATE_MSAA
					;

				bgfx::TextureHandle th = m_hTexture;
				bgfx::ProgramHandle program = m_hProgram;

				if (NULL != cmd->TextureId)
				{
					union { ImTextureID ptr; struct { bgfx::TextureHandle handle; uint8_t flags; uint8_t mip; } s; } texture = { cmd->TextureId };
					state |= 0 != (IMGUI_FLAGS_ALPHA_BLEND & texture.s.flags)
						? BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
						: BGFX_STATE_NONE
						;
					th = texture.s.handle;
				}
				else
				{
					state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
				}

				const uint16_t xx = uint16_t(bx::fmax(cmd->ClipRect.x, 0.0f));
				const uint16_t yy = uint16_t(bx::fmax(cmd->ClipRect.y, 0.0f));
				bgfx::setScissor(xx, yy
					, uint16_t(bx::fmin(cmd->ClipRect.z, 65535.0f) - xx)
					, uint16_t(bx::fmin(cmd->ClipRect.w, 65535.0f) - yy)
				);

				bgfx::setState(state);
				bgfx::setTexture(0, m_hUniformTexture, th);
				bgfx::setVertexBuffer(0, &tvb, 0, numVertices);
				bgfx::setIndexBuffer(&tib, offset, cmd->ElemCount);
				bgfx::submit(255, program);
			}

			offset += cmd->ElemCount;
		}
	}
}