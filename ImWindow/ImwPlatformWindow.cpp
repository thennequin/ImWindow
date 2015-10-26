
#include "ImwPlatformWindow.h"

#include "ImwWindowManager.h"

using namespace ImWindow;

ImwPlatformWindow::ImwPlatformWindow(bool bMain, bool bIsDragWindow, bool bCreateState)
{
	m_bMain = bMain;
	m_bIsDragWindow = bIsDragWindow;
	m_pContainer = new ImwContainer(this);
	m_pState = NULL;
	m_pPreviousState = NULL;

	if (bCreateState)
	{
		void* pTemp = ImGui::GetInternalState();
		m_pState = ImwMalloc(ImGui::GetInternalStateSize());
		ImGui::SetInternalState(m_pState, true);
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
	ImwSafeDelete(m_pState);
}

void ImwPlatformWindow::OnClose()
{
	ImwWindowManager::GetInstance()->OnClosePlatformWindow(this);
}

static bool s_bStatePush = false;

bool ImwPlatformWindow::IsStateSet()
{
	return s_bStatePush;
}

void ImwPlatformWindow::SetState()
{
	ImwAssert(s_bStatePush == false);
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
	ImwAssert(s_bStatePush == true);
	s_bStatePush = false;
	if (m_pState != NULL)
	{
		memcpy(&((ImGuiState*)m_pPreviousState)->Style, &((ImGuiState*)m_pState)->Style, sizeof(ImGuiStyle));
		ImGui::SetInternalState(m_pPreviousState);
	}
}

void ImwPlatformWindow::OnLoseFocus()
{
	ImGuiState& g = *((ImGuiState*)m_pState);
	g.SetNextWindowPosCond = g.SetNextWindowSizeCond = g.SetNextWindowContentSizeCond = g.SetNextWindowCollapsedCond = g.SetNextWindowFocus = 0;
}

void ImwPlatformWindow::Paint()
{
	ImwWindowManager::GetInstance()->Paint(this);
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

void ImwPlatformWindow::PaintContainer()
{
	m_pContainer->Paint();
}
