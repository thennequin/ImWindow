#include "ImwWindowManager.h"

#include <imgui/imgui.h>
#include <algorithm>

using namespace ImWindow;

ImwWindowManager::DrawWindowAreaAction::DrawWindowAreaAction( ImwPlatformWindow* pWindow, const ImVec2& oRectPos, const ImVec2& oRectSize, const ImColor& oColor )
	: m_oColor( oColor )
{
	m_pWindow = pWindow;
	m_oRectPos = oRectPos;
	m_oRectSize = oRectSize;
}

ImwWindowManager* ImwWindowManager::s_pInstance = 0;

ImwWindowManager::ImwWindowManager()
{
	s_pInstance = this;
	m_pMainPlatformWindow = NULL;
	m_pDragPlatformWindow = NULL;
	m_pCurrentPlatformWindow = NULL;
	m_pDraggedWindow = NULL;
}

ImwWindowManager::~ImwWindowManager()
{
	s_pInstance = 0;
}

bool ImwWindowManager::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	//io.Fonts->AddFontFromFileTTF( "res/DroidSans.ttf", 16 ) || io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF( "res/DroidSans-Bold.ttf", 16 ) || io.Fonts->AddFontDefault();

	style.FrameRounding = 2.f;
	style.WindowRounding = 0.f;
	style.ScrollbarRounding = 0.f;

	m_pMainPlatformWindow = CreatePlatformWindow(true, NULL, false);
	if (NULL != m_pMainPlatformWindow)
	{
		m_pMainPlatformWindow->Show();

		m_pDragPlatformWindow = CreatePlatformWindow(false, m_pMainPlatformWindow, true);
		return true;
	}
	return false;
}

bool ImwWindowManager::Run()
{
	InternalRun();
	return m_pMainPlatformWindow != NULL;
}

void ImwWindowManager::Exit()
{
	//TODO : Manual exit
}

ImwPlatformWindow* ImwWindowManager::GetMainPlatformWindow()
{
	return m_pMainPlatformWindow;
}

void ImwWindowManager::SetMainTitle(const char* pTitle)
{
	ImwIsSafe(m_pMainPlatformWindow)->SetTitle(pTitle);
}


void ImwWindowManager::Dock(ImwWindow* pWindow, EDockOrientation eOrientation, ImwPlatformWindow* pToPlatformWindow)
{
	DockAction* pAction = new DockAction();
	pAction->m_bFloat = false;
	pAction->m_pWindow = pWindow;
	pAction->m_pWith = NULL;
	pAction->m_eOrientation = eOrientation;
	pAction->m_pToPlatformWindow = (pToPlatformWindow != NULL) ? pToPlatformWindow : m_pMainPlatformWindow;
	m_lDockAction.push_back(pAction);
}

void ImwWindowManager::DockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation)
{
	DockAction* pAction = new DockAction();
	pAction->m_bFloat = false;
	pAction->m_pWindow = pWindow;
	pAction->m_pWith = pWithWindow;
	pAction->m_eOrientation = eOrientation;
	m_lDockAction.push_back(pAction);
}

void ImwWindowManager::Float(ImwWindow* pWindow)
{
	DockAction* pAction = new DockAction();
	pAction->m_bFloat = true;
	pAction->m_pWindow = pWindow;
	m_lDockAction.push_back(pAction);
}

const ImwWindowList& ImwWindowManager::GetWindowList() const
{
	return m_lWindows;
}

ImwPlatformWindow* ImwWindowManager::GetCurrentPlatformWindow()
{
	return m_pCurrentPlatformWindow;
}

ImwPlatformWindow* ImwWindowManager::GetWindowParent(ImwWindow* pWindow)
{
	ImwContainer* pContainer = m_pMainPlatformWindow->HasWindow(pWindow);
	if (NULL != pContainer)
	{
		return m_pMainPlatformWindow;
	}

	for ( std::list<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
	{
		pContainer = (*it)->HasWindow(pWindow);
		if (NULL != pContainer)
		{
			return *it;
		}
	}
	ImwAssert(false);
	return NULL;
}

void ImwWindowManager::Update()
{
	while ( m_lDockAction.begin() != m_lDockAction.end() )
	{
		DockAction* pAction = *m_lDockAction.begin();

		InternalUnDock(pAction->m_pWindow);

		if ( pAction->m_bFloat )
		{
			InternalFloat(pAction->m_pWindow);
		}
		else
		{
			if ( NULL != pAction->m_pWith )
			{
				InternalDockWith( pAction->m_pWindow, pAction->m_pWith, pAction->m_eOrientation );
			}
			else
			{
				InternalDock( pAction->m_pWindow, pAction->m_eOrientation, pAction->m_pToPlatformWindow );
			}
		}

		m_lOrphanWindows.remove(pAction->m_pWindow);

		delete pAction;
		m_lDockAction.erase(m_lDockAction.begin());
	}

	while ( m_lOrphanWindows.begin() != m_lOrphanWindows.end() )
	{
		if ( m_pMainPlatformWindow->m_pContainer->IsEmpty() )
		{
			InternalDock(*m_lOrphanWindows.begin(), E_DOCK_ORIENTATION_CENTER, m_pMainPlatformWindow);
		}
		else
		{
			InternalFloat(*m_lOrphanWindows.begin());
		}
		m_lOrphanWindows.remove(*m_lOrphanWindows.begin());
	}

	m_pCurrentPlatformWindow = m_pMainPlatformWindow;
	ImwIsSafe(m_pMainPlatformWindow)->Paint();

	for ( std::list<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
	{
		m_pCurrentPlatformWindow = (*it);
		(*it)->Paint();
	}

	if (NULL != m_pDraggedWindow)
	{
		m_pCurrentPlatformWindow = m_pDragPlatformWindow;
		m_pDragPlatformWindow->Paint();

		ImVec2 oCursorPos = GetCursorPos();
		m_pDragPlatformWindow->SetPosition(oCursorPos.x - 20, oCursorPos.y - 10);
	}

	m_pCurrentPlatformWindow = NULL;

	while ( m_lToDestroyWindows.begin() != m_lToDestroyWindows.end() )
	{
		ImwWindow* pWindow = *m_lToDestroyWindows.begin();
		
		m_lToDestroyWindows.remove(pWindow);
		m_lOrphanWindows.remove(pWindow);
		m_lWindows.remove(pWindow);

		InternalUnDock(pWindow);

		delete pWindow;
	}

	while (m_lToDestroyPlatformWindows.begin() != m_lToDestroyPlatformWindows.end())
	{
		ImwPlatformWindow* pPlatformWindow = *m_lToDestroyPlatformWindows.begin();
		m_lToDestroyPlatformWindows.remove(pPlatformWindow);
		m_lPlatformWindows.remove(pPlatformWindow);
		delete pPlatformWindow;
	}
}

void ImwWindowManager::Paint(ImwPlatformWindow* pWindow)
{
	float fY = 0.f;
	if (pWindow->IsMain())
	{
		ImGui::BeginMainMenuBar();
		for ( ImwWindowList::iterator it = m_lWindows.begin(); it != m_lWindows.end(); ++it )
		{
			(*it)->OnMenu();
		}
		fY = ImGui::GetWindowHeight();
		ImGui::EndMainMenuBar();
	}

	ImGui::SetNextWindowPos(ImVec2(0, fY), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2((float)pWindow->GetWidth(), (float)pWindow->GetHeight() - fY), ImGuiSetCond_Always);
	int iFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(40,40,40,0));
	//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(59, 59, 59, 255));
	ImGui::Begin( "Main", NULL, iFlags );
	pWindow->PaintContainer();
	
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	for (ImwList<DrawWindowAreaAction>::iterator it = m_lDrawWindowAreas.begin(); it != m_lDrawWindowAreas.end(); )
	{
		DrawWindowAreaAction& oAction = *it;
		//if (pWindow->HasWindow(oAction.m_pWindow))
		if (pWindow == oAction.m_pWindow)
		{
			ImVec2 oPosA = oAction.m_oRectPos;
			ImVec2 oPosB = oAction.m_oRectSize;
			oPosB.x += oPosA.x;
			oPosB.y += oPosA.y;
			
			pDrawList->PushClipRectFullScreen();
			//pDrawList->AddLine(ImGui::CalcItemRectClosestPoint(ImGui::GetIO().MousePos, true, -2.0f), ImGui::GetIO().MousePos, ImColor(ImGui::GetStyle().Colors[ImGuiCol_Button]), 4.0f);
			pDrawList->AddRectFilled(oPosA, oPosB, oAction.m_oColor);
			pDrawList->PopClipRect();
			ImwList<DrawWindowAreaAction>::iterator toRemove = it;
			++it;
			m_lDrawWindowAreas.erase( toRemove );
		}
		else
		{ 
			++it;
		}
	}
	
	ImGui::End();

	//ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(1);
	
	ImGui::Render();

	if (pWindow == m_pDragPlatformWindow)
	{
		if (!ImGui::GetIO().MouseDown[0])
		{
			StopDragWindow();
		}
	}
}

void ImwWindowManager::StartDragWindow(ImwWindow* pWindow)
{
	if (NULL == m_pDraggedWindow)
	{
		m_pDraggedWindow = pWindow;
		m_pDragPlatformWindow->Show();
		m_pDragPlatformWindow->SetSize(pWindow->GetLastSize().x, pWindow->GetLastSize().y);
		ImVec2 oCursorPos = GetCursorPos();
		m_pDragPlatformWindow->SetPosition(oCursorPos.x - 20, oCursorPos.y - 10);
		Dock(pWindow, E_DOCK_ORIENTATION_CENTER, m_pDragPlatformWindow);
	}
}

void ImwWindowManager::StopDragWindow()
{
	m_pDragPlatformWindow->Hide();
	//TODO : Find place to dock
}

void ImwWindowManager::AddWindow(ImwWindow* pWindow)
{
	m_lWindows.push_back(pWindow);

	m_lOrphanWindows.push_back(pWindow);
}

void ImwWindowManager::RemoveWindow(ImwWindow* pWindow)
{
	m_lWindows.remove(pWindow);
	m_lOrphanWindows.remove(pWindow);
}

void ImwWindowManager::DestroyWindow(ImwWindow* pWindow)
{
	if (NULL != pWindow && std::find(m_lToDestroyWindows.begin(), m_lToDestroyWindows.end(), pWindow) == m_lToDestroyWindows.end())
	{
		m_lToDestroyWindows.push_back(pWindow);
	}
}

void ImwWindowManager::InternalDock(ImwWindow* pWindow, EDockOrientation eOrientation, ImwPlatformWindow* pToPlatformWindow)
{
	pToPlatformWindow->m_pContainer->Dock(pWindow, eOrientation);
}

void ImwWindowManager::InternalDockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation)
{
	ImwContainer* pContainer = m_pMainPlatformWindow->HasWindow(pWithWindow);
	if (NULL != pContainer)
	{
		pContainer->Dock(pWindow, eOrientation);
	}

	for ( std::list<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
	{
		pContainer = (*it)->HasWindow(pWithWindow);
		if (NULL != pContainer)
		{
			pContainer->Dock(pWindow, eOrientation);
			break;
		}
	}
}

void ImwWindowManager::InternalFloat(ImwWindow* pWindow)
{
	ImwPlatformWindow* pPlatformWindow = CreatePlatformWindow(false, m_pMainPlatformWindow, false);
	if (NULL != pPlatformWindow)
	{
		m_lPlatformWindows.push_back(pPlatformWindow);

		pPlatformWindow->Dock(pWindow);
		pPlatformWindow->SetSize(300,300);
		pPlatformWindow->Show();
	}
}

void ImwWindowManager::InternalUnDock(ImwWindow* pWindow)
{
	if (m_pMainPlatformWindow->UnDock(pWindow))
	{
		return;
	}

	for ( std::list<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
	{
		if ( (*it)->UnDock(pWindow) )
		{
			//Destroy empty platform window if not main window
			if ( !(*it)->IsMain() && (*it)->GetContainer()->IsEmpty() )
			{
				m_lToDestroyPlatformWindows.push_back(*it);
			}
			return;
		}
	}

	m_pDragPlatformWindow->UnDock(pWindow);
}

void ImwWindowManager::OnClosePlatformWindow(ImwPlatformWindow* pWindow)
{
	if (m_pMainPlatformWindow == pWindow)
	{
		m_pMainPlatformWindow = NULL;
		return;
	}
	else
	{
		for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
		{
			if (*it == pWindow)
			{
				m_lPlatformWindows.erase(it);
				return;
			}
		}
	}

	ImwAssert(false, "ImwPlatformWindow not found, maybe already closed");
}

void ImwWindowManager::DrawWindowArea( ImwPlatformWindow* pWindow, const ImVec2& oPos, const ImVec2& oSize, const ImColor& oColor )
{
	m_lDrawWindowAreas.push_back(DrawWindowAreaAction(pWindow, oPos, oSize, oColor));
}

// Static
ImwWindowManager* ImwWindowManager::GetInstance()
{
	return s_pInstance;
}
