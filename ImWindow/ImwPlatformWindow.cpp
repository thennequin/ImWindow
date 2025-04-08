#define IMGUI_DEFINE_MATH_OPERATORS
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
		m_eHoveredArea = E_PLATFORMWINDOWHOVEREDAREA_NONE;

		if (bCreateContext)
		{
			ImGuiContext* pGlobalContext = ImGui::GetCurrentContext();
			IM_ASSERT(pGlobalContext != NULL);

			m_pContext = ImGui::CreateContext(pGlobalContext->IO.Fonts);
			ImGuiIO& oGlobalIO = pGlobalContext->IO;
			ImGuiIO& oNewIO = m_pContext->IO;

			oNewIO.ClipboardUserData = oGlobalIO.ClipboardUserData;
			oNewIO.GetClipboardTextFn = oGlobalIO.GetClipboardTextFn;
			oNewIO.SetClipboardTextFn = oGlobalIO.SetClipboardTextFn;
			oNewIO.SetPlatformImeDataFn = oGlobalIO.SetPlatformImeDataFn;
			oNewIO.IniFilename = NULL;
		}
	}

	ImwPlatformWindow::~ImwPlatformWindow()
	{
		PreDestroy();
	}

	bool ImwPlatformWindow::Init(ImwPlatformWindow* /*pParent*/)
	{
		return true;
	}

	void ImwPlatformWindow::RegenFontTexture( ImwPlatformWindow* /*pMain*/ )
	{

	}

	EPlatformWindowType ImwPlatformWindow::GetType() const
	{
		return m_eType;
	}

	bool ImwPlatformWindow::IsMainWindow() const
	{
		return m_eType == E_PLATFORM_WINDOW_TYPE_MAIN;
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

	void  ImwPlatformWindow::SetWindowMinimized(bool /*bMinimized*/)
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

	void ImwPlatformWindow::PreDestroy()
	{
		ImwSafeDelete(m_pContainer);

		if (m_pContext != NULL)
		{
			SetContext(false);
			ImGui::DestroyContext(m_pContext);
			RestoreContext(false);
			m_pContext = NULL;
		}

		if (ImwWindowManager::GetInstance() != NULL && ImwWindowManager::GetInstance()->m_pFocusedPlatformWindow == this)
			ImwWindowManager::GetInstance()->m_pFocusedPlatformWindow = NULL;
	}

	void ImwPlatformWindow::OnFocus(bool bFocused)
	{
		if (bFocused)
		{
			ImwWindowManager::GetInstance()->m_pFocusedPlatformWindow = this;
		}
		else
		{
			if (ImwWindowManager::GetInstance()->m_pFocusedPlatformWindow == this)
				ImwWindowManager::GetInstance()->m_pFocusedPlatformWindow = NULL;

			if (NULL != m_pContext)
			{
				m_pContext->NextWindowData.PosCond = m_pContext->NextWindowData.SizeCond = m_pContext->NextWindowData.CollapsedCond = 0;
				m_pContext->NextWindowData.Flags = m_pContext->NextWindowData.Flags & ~( ImGuiNextWindowDataFlags_HasFocus | ImGuiNextWindowDataFlags_HasContentSize );
				m_pContext->ActiveId = 0;

				m_pContext->IO.ClearInputKeys();
			}
		}

		if (NULL != m_pContext)
		{
			m_pContext->IO.AddFocusEvent(bFocused);
		}
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
		if (pWindow != NULL)
		{
			pWindow->OnDropFiles(iCount, pFiles, oPos);
		}
	}

	void ImwPlatformWindow::Moving(bool bFirst)
	{
		ImVec2 oCursorPos = ImwWindowManager::GetInstance()->GetCursorPos();
		if (bFirst)
		{
			m_oMovingStartPos = oCursorPos;
			m_bMoving = false;
			m_oMovingOffset = oCursorPos - GetPosition();
		}
		else
		{
			ImVec2 oCursorDiff = oCursorPos - m_oMovingStartPos;
			const int c_iPixelThreshold = 2;
			if (m_bMoving == false && (oCursorDiff.x * oCursorDiff.x + oCursorDiff.y * oCursorDiff.y) > (c_iPixelThreshold * c_iPixelThreshold))
				m_bMoving = true;

			if (m_bMoving)
			{
				if (IsWindowMaximized())
				{
					SetWindowMaximized(false);
					ImVec2 oSize = GetSize();
					if (m_oMovingOffset.x > oSize.x / 2.f)
					{
						m_oMovingOffset.x = oSize.x / 2.f;
					}
				}

				ImVec2 oNewPos = oCursorPos - m_oMovingOffset;
				SetPosition((int)oNewPos.x, (int)oNewPos.y);
			}
		}
	}

#ifdef IMW_USE_LAYOUT_SERIALIZATION
	bool ImwPlatformWindow::Save(JsonStthm::JsonValue& oJson)
	{
		ImVec2 oSize = GetNormalSize();
		ImVec2 oPos = GetNormalPosition();

		oJson["Width"] = (int64_t)oSize.x;
		oJson["Height"] = (int64_t)oSize.y;
		oJson["Left"] = (int64_t)oPos.x;
		oJson["Top"] = (int64_t)oPos.y;
		oJson["Maximized"] = IsWindowMaximized();
		oJson["Minimized"] = IsWindowMinimized();

		return m_pContainer->Save(oJson["Container"]);
	}

	bool ImwPlatformWindow::Load(const JsonStthm::JsonValue& oJson, bool bJustCheck)
	{
		if (!oJson["Width"].IsNumeric() || !oJson["Height"].IsNumeric() || !oJson["Left"].IsNumeric() || !oJson["Top"].IsNumeric() || (!oJson["Mode"].IsNumeric() && !oJson["Maximized"].IsBoolean()))
			return false;

		if (!bJustCheck)
		{
			SetSize((int)oJson["Width"].ToInteger(), (int)oJson["Height"].ToInteger());
			SetPosition((int)oJson["Left"].ToInteger(), (int)oJson["Top"].ToInteger());

			if (oJson["Mode"].IsNumeric())
			{
				int iMode = (int)oJson["Mode"].ToInteger();
				if (iMode < 0)
				{
					SetWindowMinimized(true);
				}
				else
				{
					SetWindowMaximized(iMode > 0);
				}
			}

			if (oJson["Maximized"].IsBoolean())
			{
				SetWindowMaximized(oJson["Maximized"].ToBoolean());
			}

			if( oJson[ "Minimized" ].IsBoolean() )
			{
				SetWindowMinimized(oJson["Minimized"].ToBoolean());
			}
		}

		return m_pContainer->Load(oJson["Container"], bJustCheck);
	}
#endif //IMW_USE_LAYOUT_SERIALIZATION

	static bool s_bContextPushed = false;

	bool ImwPlatformWindow::IsContextSet()
	{
		return s_bContextPushed;
	}

	bool ImwPlatformWindow::HasContext() const
	{
		return m_pContext != NULL;
	}

	ImGuiContext* ImwPlatformWindow::GetContext()
	{
		if (m_pContext != NULL)
			return m_pContext;
		return ImGui::GetCurrentContext();
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

	bool ImwPlatformWindow::IsFocusedWindow(ImwWindow* pWindow) const
	{
		return m_pContainer->IsFocusedWindow(pWindow);
	}

	void ImwPlatformWindow::RefreshTitle()
	{
		const char* pMainTitle = ImwWindowManager::GetInstance()->GetMainTitle();

		ImwWindow* pActiveWindow = m_pContainer->GetActiveWindow();
		const char* pActiveWindowTitle = NULL;

		if (pActiveWindow != NULL)
		{
			pActiveWindowTitle = pActiveWindow->GetTitle();
		}

		const size_t c_iMaxTitleLen = 512;
		char pTitle[c_iMaxTitleLen + 1];
		size_t iCurrentIndex = 0;

		if (pMainTitle != NULL)
		{
			size_t iLen = strlen(pMainTitle);
			if (iLen > (c_iMaxTitleLen - iCurrentIndex))
				iLen = c_iMaxTitleLen - iCurrentIndex;
			if (iLen > 0)
			{
				memcpy(pTitle + iCurrentIndex, pMainTitle, iLen);
				iCurrentIndex += iLen;
			}
		}

		if (pActiveWindowTitle != NULL)
		{
			if (iCurrentIndex != 0)
			{
				const char* const c_pSeparator = " - ";
				size_t iLen = strlen(c_pSeparator);
				if (iLen > (c_iMaxTitleLen - iCurrentIndex))
					iLen = c_iMaxTitleLen - iCurrentIndex;
				if (iLen > 0)
				{
					memcpy(pTitle + iCurrentIndex, c_pSeparator, iLen);
					iCurrentIndex += iLen;
				}
			}

			size_t iLen = strlen(pActiveWindowTitle);
			if (iLen > (c_iMaxTitleLen - iCurrentIndex))
				iLen = c_iMaxTitleLen - iCurrentIndex;
			if (iLen > 0)
			{
				memcpy(pTitle + iCurrentIndex, pActiveWindowTitle, iLen);
				iCurrentIndex += iLen;
			}
		}

		pTitle[iCurrentIndex] = 0;
		SetTitle(pTitle);
	}
//SFF_END
}
