#include "ImwWindowManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
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

//////////////////////////////////////////////////////////////////////////

ImwWindowManager::Config::Config()
	: m_fDragMarginRatio( 0.1f )
	, m_fDragMarginSizeRatio( 0.25f )
	, m_oHightlightAreaColor( 0.f, 0.5f, 1.f, 0.5f )
{
}

//////////////////////////////////////////////////////////////////////////

ImwWindowManager::ImwWindowManager()
{
	s_pInstance = this;
	m_pMainPlatformWindow = NULL;
	m_pDragPlatformWindow = NULL;
	m_pCurrentPlatformWindow = NULL;
	m_pDraggedWindow = NULL;
	m_oDragPreviewOffset = ImVec2(-20, -10);
}

ImwWindowManager::~ImwWindowManager()
{
	ImwSafeDelete(m_pMainPlatformWindow);
	ImwSafeDelete(m_pDragPlatformWindow);
	s_pInstance = 0;
	ImGui::Shutdown();
}

bool ImwWindowManager::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	io.IniFilename = NULL;

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

ImwWindowManager::Config& ImwWindowManager::GetConfig()
{
	return m_oConfig;
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
	pAction->m_pToContainer = NULL;
	m_lDockActions.push_back(pAction);
}

void ImwWindowManager::DockTo(ImwWindow* pWindow, EDockOrientation eOrientation, ImwContainer* pContainer)
{
	ImwAssert(NULL != pContainer);
	if (NULL != pContainer)
	{
		DockAction* pAction = new DockAction();
		pAction->m_bFloat = false;
		pAction->m_pWindow = pWindow;
		pAction->m_pWith = NULL;
		pAction->m_eOrientation = eOrientation;
		pAction->m_pToPlatformWindow = NULL;
		pAction->m_pToContainer = pContainer;
		m_lDockActions.push_back(pAction);
	}
}

void ImwWindowManager::DockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation)
{
	DockAction* pAction = new DockAction();
	pAction->m_bFloat = false;
	pAction->m_pWindow = pWindow;
	pAction->m_pWith = pWithWindow;
	pAction->m_eOrientation = eOrientation;
	m_lDockActions.push_back(pAction);
}

void ImwWindowManager::Float(ImwWindow* pWindow, const ImVec2& oPosition, const ImVec2& oSize)
{
	DockAction* pAction = new DockAction();
	pAction->m_bFloat = true;
	pAction->m_pWindow = pWindow;
	pAction->m_oPosition = oPosition;
	pAction->m_oSize = oSize;
	m_lDockActions.push_back(pAction);
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

void ImwWindowManager::Log(const char* pFormat, ...)
{
	char pBuffer[32768];
	va_list argptr;
	va_start(argptr, pFormat);
	vsprintf_s(pBuffer, sizeof(char) * 32767, pFormat, argptr);
	va_end(argptr);
	LogFormatted(pBuffer);
}

void ImwWindowManager::PreUpdate()
{
	ImwIsSafe(m_pMainPlatformWindow)->PreUpdate();

	for (std::list<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
	{
		(*it)->PreUpdate();
	}
}

void ImwWindowManager::Update()
{
	UpdatePlatformwWindowActions();
	UpdateDockActions();
	UpdateOrphans();

	while (m_lToDestroyWindows.begin() != m_lToDestroyWindows.end())
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

	UpdateDragWindow();
	
	m_pCurrentPlatformWindow = m_pMainPlatformWindow;
	ImwIsSafe(m_pMainPlatformWindow)->Paint();

	for ( std::list<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
	{
		m_pCurrentPlatformWindow = (*it);
		(*it)->Paint();
	}

	m_pCurrentPlatformWindow = NULL;

	
}

void ImwWindowManager::UpdatePlatformwWindowActions()
{
	while (m_lPlatformWindowActions.begin() != m_lPlatformWindowActions.end())
	{
		PlatformWindowAction* pAction = *m_lPlatformWindowActions.begin();
		
		ImwAssert((pAction->m_iFlags & E_PLATFORM_WINDOW_ACTION_SHOW & E_PLATFORM_WINDOW_ACTION_HIDE) == 0); // Can't show and hide		

		if (pAction->m_iFlags & E_PLATFORM_WINDOW_ACTION_DESTOY)
		{
			//pAction->m_pPlatformWindow->Show();
			//todo destroy
			bool bFound = false;
			if (m_pMainPlatformWindow == pAction->m_pPlatformWindow)
			{
				while (m_lPlatformWindows.begin() != m_lPlatformWindows.end())
				{
					delete *m_lPlatformWindows.begin();
					m_lPlatformWindows.erase(m_lPlatformWindows.begin());
				}
				delete m_pMainPlatformWindow;
				m_pMainPlatformWindow = NULL;
				bFound = true;
			}
			else
			{
				for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
				{
					if (*it == pAction->m_pPlatformWindow)
					{
						delete *it;
						m_lPlatformWindows.erase(it);
						bFound = true;
						break;
					}
				}
			}

			if (!bFound)
			{
				ImwAssert(false, "ImwPlatformWindow not found, maybe already closed");
			}
		}
		if (pAction->m_iFlags & E_PLATFORM_WINDOW_ACTION_SHOW)
		{
			pAction->m_pPlatformWindow->Show();
		}
		if (pAction->m_iFlags & E_PLATFORM_WINDOW_ACTION_HIDE)
		{
			pAction->m_pPlatformWindow->Hide();
		}
		if (pAction->m_iFlags & E_PLATFORM_WINDOW_ACTION_SET_POSITION)
		{
			pAction->m_pPlatformWindow->SetPosition(pAction->m_oPosition.x, pAction->m_oPosition.y);
		}
		if (pAction->m_iFlags & E_PLATFORM_WINDOW_ACTION_SET_SIZE)
		{
			pAction->m_pPlatformWindow->SetSize(pAction->m_oSize.x, pAction->m_oSize.y);
		}

		delete *m_lPlatformWindowActions.begin();
		m_lPlatformWindowActions.erase(m_lPlatformWindowActions.begin());
	}
}

void ImwWindowManager::UpdateDockActions()
{
	while (m_lDockActions.begin() != m_lDockActions.end())
	{
		DockAction* pAction = *m_lDockActions.begin();

		InternalUnDock(pAction->m_pWindow);

		if (pAction->m_bFloat)
		{
			InternalFloat(pAction->m_pWindow, pAction->m_oPosition, pAction->m_oSize);
		}
		else
		{
			if (NULL != pAction->m_pWith)
			{
				InternalDockWith(pAction->m_pWindow, pAction->m_pWith, pAction->m_eOrientation);
			}
			else if (NULL != pAction->m_pToContainer)
			{
				InternalDockTo(pAction->m_pWindow, pAction->m_eOrientation, pAction->m_pToContainer);
			}
			else
			{
				InternalDock(pAction->m_pWindow, pAction->m_eOrientation, pAction->m_pToPlatformWindow);
			}
		}

		m_lOrphanWindows.remove(pAction->m_pWindow);

		delete pAction;
		m_lDockActions.erase(m_lDockActions.begin());
	}
}

void ImwWindowManager::UpdateOrphans()
{
	while (m_lOrphanWindows.begin() != m_lOrphanWindows.end())
	{
		if (m_pMainPlatformWindow->m_pContainer->IsEmpty())
		{
			InternalDock(*m_lOrphanWindows.begin(), E_DOCK_ORIENTATION_CENTER, m_pMainPlatformWindow);
		}
		else
		{
			ImVec2 oSize = ImVec2(300, 300);
			ImVec2 oPos = m_pMainPlatformWindow->GetPosition();
			ImVec2 oMainSize = m_pMainPlatformWindow->GetSize();
			oPos.x += (oMainSize.x - oSize.x) / 2;
			oPos.y += (oMainSize.y - oSize.y) / 2;
			InternalFloat(*m_lOrphanWindows.begin(), oPos, oSize);
		}
		m_lOrphanWindows.erase(m_lOrphanWindows.begin());
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
	ImGui::SetNextWindowSize(ImVec2(pWindow->GetSize().x, pWindow->GetSize().y - fY), ImGuiSetCond_Always);
	int iFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	if (NULL != m_pDraggedWindow)
	{
		iFlags += ImGuiWindowFlags_NoInputs;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(40,40,40,0));
	//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(59, 59, 59, 255));
	ImGui::Begin( "Main", NULL, iFlags );
	pWindow->PaintContainer();
	ImGui::BeginTooltip();
	ImGui::Text("Tooltip");
	ImGui::EndTooltip();
	ImGui::End();

	//ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(1);

	ImGui::PushStyleColor(ImGuiCol_TooltipBg, ImColor(0,0,0,0));
	ImGui::BeginTooltip();
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
			m_lDrawWindowAreas.erase(toRemove);
		}
		else
		{
			++it;
		}
	}

	ImGui::EndTooltip();
	ImGui::PopStyleColor();

	
	ImGui::Render();
}

void ImwWindowManager::StartDragWindow(ImwWindow* pWindow)
{
	if (NULL == m_pDraggedWindow)
	{
		m_pDraggedWindow = pWindow;

		PlatformWindowAction* pAction = new PlatformWindowAction();
		pAction->m_pPlatformWindow = m_pDragPlatformWindow;
		pAction->m_iFlags = E_PLATFORM_WINDOW_ACTION_SHOW | E_PLATFORM_WINDOW_ACTION_SET_POSITION | E_PLATFORM_WINDOW_ACTION_SET_SIZE;
		ImVec2 oCursorPos = GetCursorPos();
		pAction->m_oPosition = ImVec2(oCursorPos.x + m_oDragPreviewOffset.x, oCursorPos.y + m_oDragPreviewOffset.y);
		pAction->m_oSize = ImVec2(pWindow->GetLastSize().x, pWindow->GetLastSize().y);
		m_lPlatformWindowActions.push_back(pAction);

		Dock(pWindow, E_DOCK_ORIENTATION_CENTER, m_pDragPlatformWindow);
		((ImGuiState*)m_pDragPlatformWindow->m_pState)->IO.MouseDown[0] = true;
	}
}

void ImwWindowManager::StopDragWindow()
{
	PlatformWindowAction* pAction = new PlatformWindowAction();
	pAction->m_pPlatformWindow = m_pDragPlatformWindow;
	pAction->m_iFlags = E_PLATFORM_WINDOW_ACTION_HIDE;
	m_pDragPlatformWindow->Hide();
	m_lPlatformWindowActions.push_back(pAction);
	m_pDraggedWindow = NULL;
}

void ImwWindowManager::UpdateDragWindow()
{
	if (NULL != m_pDraggedWindow)
	{
		m_pCurrentPlatformWindow = m_pDragPlatformWindow;
		m_pDragPlatformWindow->Paint();
		m_pCurrentPlatformWindow = NULL;

		ImVec2 oCursorPos = GetCursorPos();
		m_pDragPlatformWindow->SetPosition(oCursorPos.x + m_oDragPreviewOffset.x, oCursorPos.y + m_oDragPreviewOffset.y);

		//Search best dock area
		EDockOrientation eBestDockOrientation;
		ImVec2 oHightlightPos;
		ImVec2 oHightlightSize;
		ImwContainer* pBestContainer = GetBestDocking(m_pMainPlatformWindow, oCursorPos, eBestDockOrientation, oHightlightPos, oHightlightSize);
		if (NULL == pBestContainer)
		{
			for (std::list<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end() && NULL == pBestContainer; ++it)
			{
				pBestContainer = GetBestDocking(*it, oCursorPos, eBestDockOrientation, oHightlightPos, oHightlightSize);
			}
		}
		if (pBestContainer)
		{
			DrawWindowArea(pBestContainer->GetPlatformWindowParent(), oHightlightPos, oHightlightSize, m_oConfig.m_oHightlightAreaColor);
		}

		//if (!((ImGuiState*)m_pDragPlatformWindow->m_pState)->IO.MouseDown[0])
		if (!IsLeftClickDown())
		{
			if (NULL != pBestContainer)
			{
				DockTo(m_pDraggedWindow, eBestDockOrientation, pBestContainer);
			}
			else
			{
				Float(m_pDraggedWindow, m_pDragPlatformWindow->GetPosition(), m_pDragPlatformWindow->GetSize());
			}

			StopDragWindow();
		}
	}
}

ImwContainer* ImwWindowManager::GetBestDocking(ImwPlatformWindow* pPlatformWindow, const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize)
{
	ImVec2 oPos = pPlatformWindow->GetPosition();
	ImVec2 oSize = pPlatformWindow->GetSize();
	if (oCursorPos.x >= oPos.x && oCursorPos.x <= (oPos.x + oSize.x) &&
		oCursorPos.y >= oPos.y && oCursorPos.y <= (oPos.y + oSize.y))
	{
		ImVec2 oRectPos(oCursorPos.x - oPos.x, oCursorPos.y - oPos.y);

		ImwContainer* pBestContainer = pPlatformWindow->GetContainer()->GetBestDocking(oRectPos, oOutOrientation, oOutAreaPos, oOutAreaSize);
		if (NULL != pBestContainer)
		{
			return pBestContainer;
		}

		//Left
		if (oRectPos.x <= oSize.x * m_oConfig.m_fDragMarginRatio)
		{
			oOutOrientation = E_DOCK_ORIENTATION_LEFT;
			oOutAreaPos = IM_VEC2_0;
			oOutAreaSize = ImVec2(oSize.x * m_oConfig.m_fDragMarginSizeRatio, oSize.y);
		}
		//Right
		else if (oRectPos.x >=  oSize.x * (1.f - m_oConfig.m_fDragMarginRatio))
		{
			oOutOrientation = E_DOCK_ORIENTATION_RIGHT;
			oOutAreaPos = ImVec2(oSize.x * (1.f - m_oConfig.m_fDragMarginSizeRatio), 0.f);
			oOutAreaSize = ImVec2(oSize.x * m_oConfig.m_fDragMarginSizeRatio, oSize.y);
		}
		//Top
		else if (oRectPos.y <= oSize.y * m_oConfig.m_fDragMarginRatio)
		{
			oOutOrientation = E_DOCK_ORIENTATION_TOP;
			oOutAreaPos = IM_VEC2_0;
			oOutAreaSize = ImVec2(oSize.x, oSize.y * m_oConfig.m_fDragMarginSizeRatio);
		}
		//Bottom
		else if (oRectPos.y >=  oSize.y * (1.f - m_oConfig.m_fDragMarginRatio))
		{
			oOutOrientation = E_DOCK_ORIENTATION_BOTTOM;
			oOutAreaPos = ImVec2(0.f, oSize.y * (1.f - m_oConfig.m_fDragMarginSizeRatio));
			oOutAreaSize = ImVec2(oSize.x, oSize.y * m_oConfig.m_fDragMarginSizeRatio);
		}
		else
		{
			oOutOrientation = E_DOCK_ORIENTATION_CENTER;
			oOutAreaPos = IM_VEC2_0;
			oOutAreaSize = ImVec2(oSize.x, oSize.y);
			//ImwAssert(false); //Best dock orientation not found
			return NULL;
		}
		return pPlatformWindow->GetContainer();
	}
	return NULL;
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

void ImwWindowManager::InternalDockTo(ImwWindow* pWindow, EDockOrientation eOrientation, ImwContainer* pToContainer)
{
	pToContainer->Dock(pWindow, eOrientation);
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

void ImwWindowManager::InternalFloat(ImwWindow* pWindow, ImVec2 oPosition, ImVec2 oSize)
{
	ImwPlatformWindow* pPlatformWindow = CreatePlatformWindow(false, m_pMainPlatformWindow, false);
	if (NULL != pPlatformWindow)
	{
		m_lPlatformWindows.push_back(pPlatformWindow);

		if (oSize.x == IM_VEC2_N1.x && oSize.y == IM_VEC2_N1.y)
		{
			oSize = pWindow->GetLastSize();
		}
		if (oPosition.x == IM_VEC2_N1.x && oPosition.y == IM_VEC2_N1.y)
		{
			oPosition = GetCursorPos();
			oPosition.x -= 20;
			oPosition.x -= 10;
		}
		pPlatformWindow->Dock(pWindow);
		pPlatformWindow->SetSize(oSize.x, oSize.y);
		pPlatformWindow->SetPosition(oPosition.x, oPosition.y);
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
	PlatformWindowAction* pAction = new PlatformWindowAction();
	pAction->m_iFlags = E_PLATFORM_WINDOW_ACTION_DESTOY;
	pAction->m_pPlatformWindow = pWindow;
	m_lPlatformWindowActions.push_back(pAction);
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
