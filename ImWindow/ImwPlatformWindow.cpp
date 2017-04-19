
#include "ImwPlatformWindow.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
//SFF_BEGIN
	ImwPlatformWindow::ImwPlatformWindow(bool bMain, bool bIsDragWindow, bool bCreateState)
	{
		m_bMain = bMain;
		m_bIsDragWindow = bIsDragWindow;
		m_pContainer = new ImwContainer(this);
		m_pState = NULL;
		m_pPreviousState = NULL;
		m_bNeedRender = false;
		m_bShowContent = true;

		if (bCreateState)
		{
			void* pTemp = ImGui::GetInternalState();

			ImGuiIO& oCurrentIO = ImGui::GetIO();
			m_pState = ImwMalloc(ImGui::GetInternalStateSize());
			ImGui::SetInternalState(m_pState, true);
			ImGuiIO& oNewIO = ImGui::GetIO();
			memcpy(&((ImGuiState*)m_pState)->IO.KeyMap, &((ImGuiState*)pTemp)->IO.KeyMap, sizeof(int) * ImGuiKey_COUNT);
			oNewIO.RenderDrawListsFn = oCurrentIO.RenderDrawListsFn;
			oNewIO.GetClipboardTextFn = oCurrentIO.GetClipboardTextFn;
			oNewIO.SetClipboardTextFn = oCurrentIO.SetClipboardTextFn;
			oNewIO.MemAllocFn = oCurrentIO.MemAllocFn;
			oNewIO.MemFreeFn = oCurrentIO.MemFreeFn;
			oNewIO.ImeSetInputScreenPosFn = oCurrentIO.ImeSetInputScreenPosFn;
			ImGui::GetIO().IniFilename = NULL;

			ImGui::SetInternalState(pTemp);
		}
	}

	ImwPlatformWindow::~ImwPlatformWindow()
	{
		ImwSafeDelete(m_pContainer);

		SetState();
		if (!IsMain())
		{
			ImGui::GetIO().Fonts = NULL;
		}
		ImGui::Shutdown();
		RestoreState();
		ImwSafeFree(m_pState);
	}

	bool ImwPlatformWindow::Init(ImwPlatformWindow* /*pParent*/)
	{
		return true;
	}

	const ImVec2 c_oVec2_0 = ImVec2(0,0);
	ImVec2 ImwPlatformWindow::GetPosition() const
	{
		return c_oVec2_0;
	}

	ImVec2 ImwPlatformWindow::GetSize() const
	{
		return ImGui::GetIO().DisplaySize;
	}

	bool ImwPlatformWindow::IsWindowMaximized() const
	{
		return false;
	}

	void ImwPlatformWindow::Show()
	{
	}

	void ImwPlatformWindow::Hide()
	{
	}

	void ImwPlatformWindow::SetSize(int /*iWidth*/, int /*iHeight*/)
	{
	}

	void ImwPlatformWindow::SetPosition(int /*iX*/, int /*iY*/)
	{
	}

	void ImwPlatformWindow::SetWindowMaximized(bool /*bMaximized*/)
	{
	}

	void ImwPlatformWindow::SetTitle(const char* /*pTtile*/)
	{
	}

	bool ImwPlatformWindow::IsShowContent() const
	{
		return m_bShowContent;
	}

	void ImwPlatformWindow::SetShowContent(bool bShow)
	{
		m_bShowContent = bShow;
	}

	void ImwPlatformWindow::OnClose()
	{
		ImwWindowManager::GetInstance()->OnClosePlatformWindow(this);
	}

	bool ImwPlatformWindow::Save(JsonValue& oJson)
	{
		oJson["Width"] = (long)GetSize().x;
		oJson["Height"] = (long)GetSize().y;
		oJson["Left"] = (long)GetPosition().x;
		oJson["Top"] = (long)GetPosition().y;
		oJson["Maximized"] = IsWindowMaximized();

		return m_pContainer->Save(oJson["Container"]);
	}

	bool ImwPlatformWindow::Load(const JsonValue& oJson, bool bJustCheck)
	{
		if (!oJson["Width"].IsNumeric() || !oJson["Height"].IsNumeric() || !oJson["Left"].IsNumeric() || !oJson["Top"].IsNumeric() || !oJson["Maximized"].IsBoolean())
			return false;

		if (!bJustCheck)
		{
			SetSize((long)oJson["Width"], (long)oJson["Height"]);
			SetPosition((long)oJson["Left"], (long)oJson["Top"]);
			SetWindowMaximized(oJson["Maximized"]);
		}

		return m_pContainer->Load(oJson["Container"], bJustCheck);
	}

	static bool s_bStatePush = false;

	bool ImwPlatformWindow::IsStateSet()
	{
		return s_bStatePush;
	}

	bool ImwPlatformWindow::HasState() const
	{
		return m_pState != NULL;
	}

	void ImwPlatformWindow::SetState()
	{
		IM_ASSERT(s_bStatePush == false);
		s_bStatePush = true;
		if (m_pState != NULL)
		{
			m_pPreviousState = ImGui::GetInternalState();
			ImGui::SetInternalState(m_pState);
			memcpy(&((ImGuiState*)m_pState)->Style, &((ImGuiState*)m_pPreviousState)->Style, sizeof(ImGuiStyle));
		}
	}

	void ImwPlatformWindow::RestoreState()
	{
		IM_ASSERT(s_bStatePush == true);
		s_bStatePush = false;
		if (m_pState != NULL)
		{
			memcpy(&((ImGuiState*)m_pPreviousState)->Style, &((ImGuiState*)m_pState)->Style, sizeof(ImGuiStyle));
			ImGui::SetInternalState(m_pPreviousState);
		}
	}

	void ImwPlatformWindow::OnLoseFocus()
	{
		if (NULL != m_pState)
		{
			ImGuiState& g = *((ImGuiState*)m_pState);
			g.SetNextWindowPosCond = g.SetNextWindowSizeCond = g.SetNextWindowContentSizeCond = g.SetNextWindowCollapsedCond = g.SetNextWindowFocus = 0;
			g.ActiveId = 0;

			for (int i = 0; i < 512; ++i)
				g.IO.KeysDown[i] = false;

			for (int i = 0; i < 5; ++i)
				g.IO.MouseDown[i] = false;

			g.IO.KeyAlt = false;
			g.IO.KeyCtrl = false;
			g.IO.KeyShift = false;
		}
	}

	void ImwPlatformWindow::PreUpdate()
	{
	}

	void ImwPlatformWindow::Destroy()
	{
	}

	void ImwPlatformWindow::StartDrag()
	{
	}

	void ImwPlatformWindow::StopDrag()
	{
	}

	bool ImwPlatformWindow::IsDraging()
	{
		return false;
	}

	void ImwPlatformWindow::Render()
	{
		if (m_bNeedRender)
		{
			m_bNeedRender = false;
			SetState();
			ImGui::Render();
			RestoreState();
		}
	}

	bool ImwPlatformWindow::IsMain()
	{
		return m_bMain;
	}

	void ImwPlatformWindow::Dock(ImwWindow* pWindow)
	{
		m_pContainer->Dock(pWindow);
	}

	bool ImwPlatformWindow::UnDock(ImwWindow* pWindow)
	{
		return m_pContainer->UnDock(pWindow);
	}

	ImwContainer* ImwPlatformWindow::GetContainer()
	{
		return m_pContainer;
	}

	ImwContainer* ImwPlatformWindow::HasWindow(ImwWindow* pWindow)
	{
		return m_pContainer->HasWindow(pWindow);
	}

	bool ImwPlatformWindow::FocusWindow(ImwWindow* pWindow)
	{
		return m_pContainer->FocusWindow(pWindow);
	}

	void ImwPlatformWindow::PaintContainer()
	{
		m_pContainer->Paint();
	}
//SFF_END
}