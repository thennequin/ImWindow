
#include "ImwPlatformWindow.h"

#include "ImwWindowManager.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

using namespace ImWindow;

ImwPlatformWindow::ImwPlatformWindow(bool bMain)
{
	m_bMain = bMain;
	m_pContainer = new ImwContainer(NULL);
	m_pState = NULL;
	m_pPreviousState = NULL;

	void* pTemp = ImGui::GetInternalState();
	m_pState = ImwMalloc(ImGui::GetInternalStateSize());
	memcpy(m_pState, ImGui::GetInternalState(), ImGui::GetInternalStateSize());
	ImGui::SetInternalState(m_pState);
	//ImGui::SetInternalState(m_pState, true);
	ImGui::SetInternalState(pTemp);
}

ImwPlatformWindow::~ImwPlatformWindow()
{
	ImwSafeDelete(m_pContainer);
	ImwSafeDelete(m_pState);
}

void ImwPlatformWindow::OnClose()
{
	ImwWindowManager::GetInstance()->OnClosePlatformWindow(this);
	delete this;
}

void ImwPlatformWindow::SetState()
{
	m_pPreviousState = ImGui::GetInternalState();
	ImGui::SetInternalState(m_pState);
}

void ImwPlatformWindow::RestoreState()
{
	ImGui::SetInternalState(m_pPreviousState);
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
