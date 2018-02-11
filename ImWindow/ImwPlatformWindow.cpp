
#include "ImwPlatformWindow.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
//SFF_BEGIN
	ImwPlatformWindow::ImwPlatformWindow(EPlatformWindowType eType, bool bCreateContext)
	{
		m_eType = eType;
		m_pContainer = new ImwContainer(this);
		m_pContext = NULL;
		m_pPreviousContext = NULL;
		m_bNeedRender = false;
		m_bShowContent = true;

		if ( bCreateContext )
		{
			ImGuiContext* pGlobalContext = ImGui::GetCurrentContext();
			IM_ASSERT(pGlobalContext != NULL);

			m_pContext = ImGui::CreateContext( pGlobalContext->IO.MemAllocFn, pGlobalContext->IO.MemFreeFn );

			ImGuiIO& oGlobalIO = pGlobalContext->IO;
			ImGuiIO& oNewIO = m_pContext->IO;

			memcpy(&(oNewIO.KeyMap), &(oGlobalIO.KeyMap ), sizeof( pGlobalContext->IO.KeyMap ));
			oNewIO.RenderDrawListsFn = NULL;
			oNewIO.GetClipboardTextFn = oGlobalIO.GetClipboardTextFn;
			oNewIO.SetClipboardTextFn = oGlobalIO.SetClipboardTextFn;
			oNewIO.ImeSetInputScreenPosFn = oGlobalIO.ImeSetInputScreenPosFn;
			oNewIO.IniFilename = NULL;
		}
	}

	ImwPlatformWindow::~ImwPlatformWindow()
	{
		ImwSafeDelete(m_pContainer);

		if (m_pContext != NULL)
		{
			m_pContext->IO.Fonts = NULL;
			SetContext(false);
			ImGui::Shutdown();
			RestoreContext(false);
			ImGui::DestroyContext(m_pContext);
			m_pContext = NULL;
		}
	}

	bool ImwPlatformWindow::Init(ImwPlatformWindow* /*pParent*/)
	{
		return true;
	}

	EPlatformWindowType ImwPlatformWindow::GetType() const
	{
		return m_eType;
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

	ImVec2 ImwPlatformWindow::GetNormalPosition() const
	{
		return GetPosition();
	}

	ImVec2 ImwPlatformWindow::GetNormalSize() const
	{
		return GetSize();
	}

	bool ImwPlatformWindow::IsWindowMaximized() const
	{
		return false;
	}

	bool ImwPlatformWindow::IsWindowMinimized() const
	{
		return false;
	}

	void ImwPlatformWindow::Show(bool /*bShow*/)
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

	void  ImwPlatformWindow::SetWindowMinimized()
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

	void ImwPlatformWindow::PreUpdate()
	{
	}

	void ImwPlatformWindow::PreRender()
	{
	}

	void ImwPlatformWindow::OnOverlay()
	{
	}

	void ImwPlatformWindow::RenderDrawLists(ImDrawData* /*pDrawData */)
	{
	}

	void ImwPlatformWindow::Render()
	{
		if( m_bNeedRender )
		{
			m_bNeedRender = false;
			SetContext(false);
			ImGui::GetIO().DisplaySize = GetSize();
			PreRender();
			ImGui::Render();
			RenderDrawLists(ImGui::GetDrawData());
			RestoreContext(false);
		}
	}

	void ImwPlatformWindow::PaintContainer()
	{
		m_pContainer->Paint();
	}

	void ImwPlatformWindow::OnClose()
	{
		ImwWindowManager::GetInstance()->OnClosePlatformWindow(this);
	}

	void ImwPlatformWindow::OnDropFiles(int iCount, char** pFiles, const ImVec2& oPos)
	{
		ImwWindow* pWindow = GetWindowAtPos(oPos);
		IM_ASSERT(pWindow != NULL);
		if (pWindow != NULL)
		{
			pWindow->OnDropFiles(iCount, pFiles, oPos);
		}
	}

	bool ImwPlatformWindow::Save(JsonValue& oJson)
	{
		ImVec2 oSize = GetNormalSize();
		ImVec2 oPos = GetNormalPosition();

		oJson["Width"] = (long)oSize.x;
		oJson["Height"] = (long)oSize.y;
		oJson["Left"] = (long)oPos.x;
		oJson["Top"] = (long)oPos.y;
		oJson["Mode"] = (long)(IsWindowMaximized() ? 1 : (IsWindowMinimized() ? -1 : 0));

		return m_pContainer->Save(oJson["Container"]);
	}

	bool ImwPlatformWindow::Load(const JsonValue& oJson, bool bJustCheck)
	{
		if (!oJson["Width"].IsNumeric() || !oJson["Height"].IsNumeric() || !oJson["Left"].IsNumeric() || !oJson["Top"].IsNumeric() || (!oJson["Mode"].IsNumeric() && !oJson["Maximized"].IsBoolean()))
			return false;

		if (!bJustCheck)
		{
			SetSize((long)oJson["Width"], (long)oJson["Height"]);
			SetPosition((long)oJson["Left"], (long)oJson["Top"]);

			if (oJson["Mode"].IsNumeric())
			{
				long iMode = (long)oJson["Mode"];
				if (iMode < 0)
				{
					SetWindowMinimized();
				}
				else
				{
					SetWindowMaximized(iMode > 0);
				}
			}
			else if (oJson["Maximized"].IsBoolean())
			{
				SetWindowMaximized(oJson["Maximized"]);
			}
		}

		return m_pContainer->Load(oJson["Container"], bJustCheck);
	}

	static bool s_bContextPushed = false;

	bool ImwPlatformWindow::IsContextSet()
	{
		return s_bContextPushed;
	}

	bool ImwPlatformWindow::HasContext() const
	{
		return m_pContext != NULL;
	}

	void ImwPlatformWindow::SetContext(bool bCopyStyle)
	{
		IM_ASSERT(s_bContextPushed == false);
		s_bContextPushed = true;
		if (m_pContext != NULL)
		{
			IM_ASSERT( m_pPreviousContext == NULL );
			m_pPreviousContext = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(m_pContext);
			if (bCopyStyle)
			{
				//Copy style from Global context
				memcpy(&(m_pContext->Style), &(m_pPreviousContext->Style), sizeof(ImGuiStyle));
			}
		}
	}

	void ImwPlatformWindow::RestoreContext(bool bCopyStyle)
	{
		IM_ASSERT(s_bContextPushed == true);
		s_bContextPushed = false;
		if (m_pContext != NULL)
		{
			IM_ASSERT(m_pPreviousContext != NULL);
			if (bCopyStyle)
			{
				//Copy style to Global context
				memcpy(&(m_pPreviousContext->Style), &(m_pContext->Style), sizeof(ImGuiStyle));
			}
			ImGui::SetCurrentContext(m_pPreviousContext);
			m_pPreviousContext = NULL;
		}
	}

	void ImwPlatformWindow::OnLoseFocus()
	{
		if (NULL != m_pContext)
		{
			m_pContext->SetNextWindowPosCond = m_pContext->SetNextWindowSizeCond = m_pContext->SetNextWindowContentSizeCond = m_pContext->SetNextWindowCollapsedCond = m_pContext->SetNextWindowFocus = 0;
			m_pContext->ActiveId = 0;

			for (int i = 0; i < 512; ++i)
				m_pContext->IO.KeysDown[i] = false;

			for (int i = 0; i < 5; ++i)
				m_pContext->IO.MouseDown[i] = false;

			m_pContext->IO.KeyAlt = false;
			m_pContext->IO.KeyCtrl = false;
			m_pContext->IO.KeyShift = false;
		}
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

	ImwWindow* ImwPlatformWindow::GetWindowAtPos(const ImVec2& oPos) const
	{
		return m_pContainer->GetWindowAtPos(oPos);
	}

	const ImwContainer* ImwPlatformWindow::HasWindow(ImwWindow* pWindow)
	{
		return m_pContainer->HasWindow(pWindow);
	}

	bool ImwPlatformWindow::FocusWindow(ImwWindow* pWindow)
	{
		return m_pContainer->FocusWindow(pWindow);
	}

	void ImwPlatformWindow::RefreshTitle()
	{
		ImwWindow* pActiveWindow = m_pContainer->GetActiveWindow();
		if (pActiveWindow != NULL)
		{
			SetTitle(pActiveWindow->GetTitle());
		}
		else
		{
			SetTitle("");
		}
	}
//SFF_END
}