#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImwWindowManager.h"

#include <algorithm>

namespace ImWindow
{
//SFF_BEGIN

	ImwWindowManager::PlatformWindowAction::PlatformWindowAction(ImwPlatformWindow* pPlatformWindow, EPlatformWindowAction eAction, ImVec2 oValue)
	{
		m_pPlatformWindow = pPlatformWindow;
		m_eAction = eAction;
		m_oValue = oValue;
	}

	//////////////////////////////////////////////////////////////////////////

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
		, m_eTabColorMode( E_TABCOLORMODE_BACKGROUND )
		, m_bVisibleDragger( false )
		, m_bShowTabBorder( false )
		, m_bShowTabShadows( false )
		, m_oTabColorNormal( 50, 50, 50, 255 )
		, m_oTabColorActive( 37, 37, 37, 255 )
		, m_oTabColorBorder( 72, 72, 72, 255 )
		, m_fTabOverlap( 15.f )
		, m_fTabSlopWidth( 30.f )
		, m_fTabSlopP1Ratio( 0.6f )
		, m_fTabSlopP2Ratio( 0.4f )
		, m_fTabSlopHRatio( 0.f )
		, m_fTabShadowDropSize( 15.f )
		, m_fTabShadowSlopRatio( 0.6f )
		, m_fTabShadowAlpha( 0.75f )
		, m_oStatusBarWindowPadding( 4.f, 4.f )
		, m_oStatusBarFramePadding( 4.f, 2.f )
	{
	}

	//////////////////////////////////////////////////////////////////////////

	ImwWindowManager::ImwWindowManager()
	{
		s_pInstance = this;
		m_pMainTitle = NULL;
		m_pMainPlatformWindow = NULL;
		m_pDragPlatformWindow = NULL;
		m_pCurrentPlatformWindow = NULL;
		m_pDraggedWindow = NULL;
		m_pDragBestContainer = NULL;
		m_bDragOnTab = false;
		m_iDragBestContainerPosition = -1;
		m_oDragPreviewOffset = ImVec2(-20, -10);
		m_bHasWantCaptureKeyboard = false;
		m_bHasWantCaptureMouse = false;
	}

	ImwWindowManager::~ImwWindowManager()
	{
		Destroy();
		s_pInstance = 0;
		ImwSafeFree(m_pMainTitle);
	}

	bool ImwWindowManager::Init()
	{
		InternalInit();

		ImGuiIO& io = ImGui::GetIO();

		io.IniFilename = NULL;

		//io.Fonts->AddFontFromFileTTF( "res/DroidSans.ttf", 16 ) || io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF( "res/DroidSans-Bold.ttf", 16 ) || io.Fonts->AddFontDefault();

		m_pMainPlatformWindow = CreatePlatformWindow(E_PLATFORM_WINDOW_TYPE_MAIN, NULL);
		if (NULL != m_pMainPlatformWindow)
		{
			m_pMainPlatformWindow->Show(true);

			if (CanCreateMultipleWindow())
			{
				m_pDragPlatformWindow = CreatePlatformWindow(E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW, m_pMainPlatformWindow);
			}
		
			return true;
		}
		return false;
	}

	bool ImwWindowManager::Run(bool bRender)
	{
		if (bRender)
		{
			if (m_pMainPlatformWindow != NULL)
			{
				Render();
			}
			return true;
		}
		else
		{
			PreUpdate();
			if (m_pMainPlatformWindow != NULL)
			{
				Update();
			}
			return m_pMainPlatformWindow != NULL;
		}
	}

	void ImwWindowManager::Destroy()
	{
		if (m_pCurrentPlatformWindow != NULL)
		{
			if(m_pMainPlatformWindow != NULL)
				m_lPlatformWindowActions.push_back(new PlatformWindowAction(m_pMainPlatformWindow, E_PLATFORM_WINDOW_ACTION_DESTROY));
		}
		else
		{
			while (m_lMenus.begin() != m_lMenus.end())
			{
				ImwMenu* pMenu = m_lMenus.back();
				if (pMenu->IsAutoDeleted())
					delete pMenu;
				else
					m_lMenus.pop_back();
			}
		
			while (m_lStatusBars.begin() != m_lStatusBars.end())
			{
				ImwStatusBar* pStatusBar = m_lStatusBars.back();
				if (pStatusBar->IsAutoDeleted())
					delete pStatusBar;
				else
					m_lStatusBars.pop_back();
			}

			while (m_lToolBars.begin() != m_lToolBars.end())
			{
				ImwToolBar* pToolBar = m_lToolBars.back();
				if (pToolBar->IsAutoDeleted())
					delete pToolBar;
				else
					m_lToolBars.pop_back();
			}

			ImwSafeDelete(m_pDragPlatformWindow);
			while (m_lPlatformWindows.begin() != m_lPlatformWindows.end())
			{
				delete *m_lPlatformWindows.begin();
				m_lPlatformWindows.erase(m_lPlatformWindows.begin());
			}
			ImwSafeDelete(m_pMainPlatformWindow);
		}

		InternalDestroy();
	}

	ImwPlatformWindow* ImwWindowManager::GetMainPlatformWindow() const
	{
		return m_pMainPlatformWindow;
	}

	const ImwPlatformWindowList& ImwWindowManager::GetSecondariesPlatformWindows() const
	{
		return m_lPlatformWindows;
	}

	ImwWindowManager::Config& ImwWindowManager::GetConfig()
	{
		return m_oConfig;
	}

	void ImwWindowManager::SetMainTitle(const ImwChar* pTitle)
	{
		ImwSafeFree(m_pMainTitle);
		if (NULL != pTitle)
		{
			size_t iLen = strlen(pTitle) + 1;
			m_pMainTitle = (ImwChar*)ImwMalloc(sizeof(ImwChar) * iLen);
			strcpy(m_pMainTitle, pTitle);
		}

		ImwIsSafe(m_pMainPlatformWindow)->SetTitle(m_pMainTitle);

		for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
		{
			ImwIsSafe((*it))->RefreshTitle();
		}
	}

	const ImwChar* ImwWindowManager::GetMainTitle() const
	{
		return m_pMainTitle;
	}

	void ImwWindowManager::UnDock(ImwWindow* pWindow)
	{
		DockAction* pAction = new DockAction();
		pAction->m_bFloat = false;
		pAction->m_pWindow = pWindow;
		pAction->m_pWith = NULL;
		pAction->m_eOrientation = E_DOCK_ORIENTATION_CENTER;
		pAction->m_pToPlatformWindow = NULL;
		pAction->m_pToContainer = NULL;
		pAction->m_iPosition = -1;
		m_lDockActions.push_back(pAction);
	}

	void ImwWindowManager::Dock(ImwWindow* pWindow, EDockOrientation eOrientation, float fRatio, ImwPlatformWindow* pToPlatformWindow)
	{
		DockAction* pAction = new DockAction();
		pAction->m_bFloat = false;
		pAction->m_pWindow = pWindow;
		pAction->m_pWith = NULL;
		pAction->m_eOrientation = eOrientation;
		pAction->m_pToPlatformWindow = (pToPlatformWindow != NULL) ? pToPlatformWindow : m_pMainPlatformWindow;
		pAction->m_pToContainer = NULL;
		pAction->m_iPosition = -1;
		pAction->m_fRatio = fRatio;
		m_lDockActions.push_back(pAction);
	}

	void ImwWindowManager::DockTo(ImwWindow* pWindow, EDockOrientation eOrientation, float fRatio, ImwContainer* pContainer, int iPosition)
	{
		IM_ASSERT(NULL != pContainer);
		if (NULL != pContainer)
		{
			DockAction* pAction = new DockAction();
			pAction->m_bFloat = false;
			pAction->m_pWindow = pWindow;
			pAction->m_pWith = NULL;
			pAction->m_eOrientation = eOrientation;
			pAction->m_pToPlatformWindow = NULL;
			pAction->m_pToContainer = pContainer;
			pAction->m_iPosition = iPosition;
			pAction->m_fRatio = fRatio;
			m_lDockActions.push_back(pAction);
		}
	}

	void ImwWindowManager::DockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation, float fRatio)
	{
		DockAction* pAction = new DockAction();
		pAction->m_bFloat = false;
		pAction->m_pWindow = pWindow;
		pAction->m_pWith = pWithWindow;
		pAction->m_eOrientation = eOrientation;
		pAction->m_fRatio = fRatio;
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

	void ImwWindowManager::FocusWindow(ImwWindow* pWindow)
	{
		if (!m_pMainPlatformWindow->FocusWindow(pWindow))
		{
			for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
			{
				if ((*it)->FocusWindow(pWindow))
					break;
			}
		}
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
		const ImwContainer* pContainer = m_pMainPlatformWindow->HasWindow(pWindow);
		if (NULL != pContainer)
		{
			return m_pMainPlatformWindow;
		}

		for ( ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
		{
			pContainer = (*it)->HasWindow(pWindow);
			if (NULL != pContainer)
			{
				return *it;
			}
		}
		IM_ASSERT(false);
		return NULL;
	}

	void ImwWindowManager::MaximizeCurrentPlatformWindow()
	{
		if (m_pCurrentPlatformWindow != NULL)
			m_lPlatformWindowActions.push_back(new PlatformWindowAction(m_pCurrentPlatformWindow, E_PLATFORM_WINDOW_ACTION_MAXIMIZE));
	}

	void ImwWindowManager::MinimizeCurrentPlatformWindow()
	{
		if (m_pCurrentPlatformWindow != NULL)
			m_lPlatformWindowActions.push_back(new PlatformWindowAction(m_pCurrentPlatformWindow, E_PLATFORM_WINDOW_ACTION_MINIMIZE));
	}

	void ImwWindowManager::RestoreCurrentPlatformWindow()
	{
		if (m_pCurrentPlatformWindow != NULL)
			m_lPlatformWindowActions.push_back(new PlatformWindowAction(m_pCurrentPlatformWindow, E_PLATFORM_WINDOW_ACTION_RESTORE));
	}

	bool ImwWindowManager::SaveLayoutToString(ImwString& sLayout, bool bCompact)
	{
		JsonValue oJson;
		oJson.InitType(JsonValue::E_TYPE_OBJECT);
		
		if ( m_pMainPlatformWindow->Save(oJson["MainPlatformWindow"]) )
		{
			JsonValue& oJsonPlatformWindows = oJson["PlatformWindows"];
			int iCurrent = 0;
			for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
			{
				if ( !(*it)->Save(oJsonPlatformWindows[iCurrent++]) )
					return false;
			}

			sLayout.clear();
			oJson.WriteString(sLayout, bCompact);
			return true;
		}
		return false;
	}

	bool ImwWindowManager::SaveLayoutToFile(const ImwChar* pFilePath, bool bCompact)
	{
		FILE* pFile = fopen(pFilePath, "w");
		if (pFile != NULL)
		{
			ImwString sLayout;
			bool bSaved = SaveLayoutToString(sLayout, bCompact);
			if ( bSaved )
			{
				fwrite(sLayout.c_str(), 1, sLayout.size(), pFile);
			}
			fclose(pFile);
			return bSaved;
		}
		return false;
	}

	bool ImwWindowManager::LoadLayoutFromString(const ImwChar* pLayout)
	{
		JsonValue oJson;
		if (oJson.ReadString(pLayout) == 0 && oJson.IsObject())
		{
			JsonValue& oJsonMainPlatformWindow = oJson["MainPlatformWindow"];
			JsonValue& oJsonPlatformWindows = oJson["PlatformWindows"];

			//Check layout integrity
			if (!oJsonMainPlatformWindow.IsObject() || !m_pMainPlatformWindow->Load(oJsonMainPlatformWindow, true))
				return false;

			if (!oJsonPlatformWindows.IsArray() && !oJsonPlatformWindows.IsNull())
				return false;

			int iPlatformWindowCount = oJsonPlatformWindows.GetMemberCount();
			for (int iCurrent = 0; iCurrent < iPlatformWindowCount; ++iCurrent)
			{
				JsonValue& oJsonPlatformWindow = oJsonPlatformWindows[iCurrent];
				if (!oJsonPlatformWindow.IsObject() || !m_pMainPlatformWindow->Load(oJsonPlatformWindow, true))
					return false;
			}

			//Load layout
			if (!m_pMainPlatformWindow->Load(oJsonMainPlatformWindow, false))
				return false; //Something wrong

			while (m_lPlatformWindows.begin() != m_lPlatformWindows.end())
			{
				ImwPlatformWindow* pPlatformWindow = *m_lPlatformWindows.begin();
				m_lPlatformWindows.remove(pPlatformWindow);
				delete pPlatformWindow;
			}

			for (int iCurrent = 0; iCurrent < iPlatformWindowCount; ++iCurrent)
			{
				JsonValue& oJsonPlatformWindow = oJsonPlatformWindows[iCurrent];
				ImwPlatformWindow* pNewPlatformWindow = CreatePlatformWindow(E_PLATFORM_WINDOW_TYPE_SECONDARY, m_pMainPlatformWindow);
				m_lPlatformWindows.push_back(pNewPlatformWindow);
				pNewPlatformWindow->Show(true);
				if (!pNewPlatformWindow->Load(oJsonPlatformWindow, false))
					return false; //Something wrong
				
				pNewPlatformWindow->RefreshTitle();
			}

			m_lOrphanWindows.clear();

			return true;
		}
		return false;
	}

	bool ImwWindowManager::LoadLayoutFromFile(const ImwChar* pFilePath)
	{
		FILE* pFile = fopen(pFilePath, "r");
		if (NULL != pFile)
		{
			fseek(pFile, 0, SEEK_END);
			long iSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			ImwChar* pString = new ImwChar[iSize / sizeof(ImwChar)];
			fread(pString, 1, iSize, pFile);
			fclose(pFile);

			bool bReturn = LoadLayoutFromString(pString);

			delete[] pString;
			return bReturn;
		}
		return false;
	}

	const ImwChar* ImwWindowManager::GetWindowClassName(ImwWindow* /*pWindow*/)
	{
		return NULL;
	}

	bool ImwWindowManager::CanCreateWindowByClassName(const ImwChar* /*pName*/)
	{
		return false;
	}

	ImwWindow* ImwWindowManager::CreateWindowByClassName(const ImwChar* /*pName*/)
	{
		return NULL;
	}

	bool ImwWindowManager::IsUsingCustomFrame() const
	{
		return false;
	}

	bool ImwWindowManager::CanCreateMultipleWindow()
	{
		return false;
	}

	bool ImwWindowManager::InternalInit()
	{
		return true;
	}

	void ImwWindowManager::InternalDestroy()
	{
	}

	ImwPlatformWindow* ImwWindowManager::CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* /*pParent*/)
	{
		if (eType == E_PLATFORM_WINDOW_TYPE_MAIN)
		{
			return (ImWindow::ImwPlatformWindow*)new ImwPlatformWindow(eType, CanCreateMultipleWindow());
		}
		return NULL;
	}

	ImVec2 ImwWindowManager::GetCursorPos()
	{
		return ImGui::GetIO().MousePos;
	}

	bool ImwWindowManager::IsLeftClickDown()
	{
		return ImGui::GetIO().MouseDown[0];
	}

	float ImwWindowManager::GetTitleBarHeight() const
	{
		ImGuiContext* pContext = m_pMainPlatformWindow->GetContext();
		return pContext->Style.WindowPadding.y + pContext->Style.FramePadding.y * 2.f + pContext->FontSize;
	}

	void ImwWindowManager::PaintTitleBar(ImwPlatformWindow* pPlatformWindow)
	{
		const float c_fButtonWidth = 24.f;

		ImGui::TextUnformatted(GetMainTitle());

		ImDrawList* pDrawList = ImGui::GetWindowDrawList();

		ImU32 iColor = ImGui::GetColorU32(ImGuiCol_Text);

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvailWidth() - 3.f * c_fButtonWidth, 1.f));

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

		ImVec2 oMin, oMax, oCenter, oStartRect, oEndRect;
		
		//Minimize
		{
			ImGui::SameLine();
			if (ImGui::Button("##Minimize", ImVec2(c_fButtonWidth, 0.f)))
			{
				pPlatformWindow->SetWindowMinimized();
			}

			oMin = ImGui::GetItemRectMin();
			oMax = ImGui::GetItemRectMax();
			oCenter = (oMin + oMax) / 2.f;
			oStartRect = ImVec2(oCenter.x - 4.f, oCenter.y + 3.f);
			oEndRect = ImVec2(oCenter.x + 4.f, oCenter.y + 5.f);
			pDrawList->AddRectFilled(oStartRect, oEndRect, iColor);
		}

		//Maximize
		{
			ImGui::SameLine();
			if (ImGui::Button("##Maximize", ImVec2(c_fButtonWidth, 0.f)))
			{
				pPlatformWindow->SetWindowMaximized(!pPlatformWindow->IsWindowMaximized());
			}

			oMin = ImGui::GetItemRectMin();
			oMax = ImGui::GetItemRectMax();
			oCenter = (oMin + oMax) / 2.f;
			oStartRect = ImVec2(oCenter.x - 4.f, oCenter.y - 4.f);
			oEndRect = ImVec2(oCenter.x + 5.f, oCenter.y + 5.f);
			pDrawList->AddRect(oStartRect, oEndRect, iColor, 0.f, -1, 2.f);
			oEndRect = ImVec2(oCenter.x + 5.f, oCenter.y - 1.f);
			pDrawList->AddRectFilled(oStartRect, oEndRect, iColor);
		}

		//Close
		{
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(232.f / 255.f, 17.f / 255.f, 35.f / 255.f, 255.f / 255.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(139.f / 255.f, 50.f / 255.f, 91.f / 255.f, 255.f / 255.f));
			if (ImGui::Button("##Close", ImVec2(c_fButtonWidth, 0.f)))
			{
				OnClosePlatformWindow(pPlatformWindow);
			}
			ImGui::PopStyleColor(2);

			oMin = ImGui::GetItemRectMin();
			oMax = ImGui::GetItemRectMax();
			oCenter = (oMin + oMax) / 2.f;
			oStartRect = ImVec2(oCenter.x - 5.f, oCenter.y - 5.f);
			oEndRect = ImVec2(oCenter.x + 5.f, oCenter.y + 5.f);
			pDrawList->AddLine(oStartRect, oEndRect, iColor, 2.f);
			oStartRect = ImVec2(oCenter.x - 5.f, oCenter.y + 5.f);
			oEndRect = ImVec2(oCenter.x + 5.f, oCenter.y - 5.f);
			pDrawList->AddLine(oStartRect, oEndRect, iColor, 2.f);
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}

	void ImwWindowManager::PreUpdate()
	{
		ImwIsSafe(m_pMainPlatformWindow)->PreUpdate();

		for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
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

		while (m_lToDestroyStatusBars.begin() != m_lToDestroyStatusBars.end())
		{
			ImwStatusBar* pStatusBar = *m_lToDestroyStatusBars.begin();

			m_lToDestroyStatusBars.remove(pStatusBar);
			delete pStatusBar;
		}

		while (m_lToDestroyMenus.begin() != m_lToDestroyMenus.end())
		{
			ImwMenu* pMenu = *m_lToDestroyMenus.begin();

			m_lToDestroyMenus.remove(pMenu);
			delete pMenu;
		}

		while (m_lToDestroyToolBars.begin() != m_lToDestroyToolBars.end())
		{
			ImwToolBar* pToolBar = *m_lToDestroyToolBars.begin();

			m_lToDestroyToolBars.remove(pToolBar);
			delete pToolBar;
		}

		while (m_lToDestroyPlatformWindows.begin() != m_lToDestroyPlatformWindows.end())
		{
			ImwPlatformWindow* pPlatformWindow = *m_lToDestroyPlatformWindows.begin();
			m_lToDestroyPlatformWindows.remove(pPlatformWindow);
			m_lPlatformWindows.remove(pPlatformWindow);
			delete pPlatformWindow;
		}

		if (NULL != m_pMainPlatformWindow)
		{
			m_bHasWantCaptureKeyboard = false;
			m_bHasWantCaptureMouse = false;
			UpdateDragWindow();

			Paint(m_pMainPlatformWindow);
			if (CanCreateMultipleWindow())
			{
				Paint(m_pDragPlatformWindow);

				for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
				{
					Paint(*it);
				}
			}

			PostPaint(m_pMainPlatformWindow);

			//if (NULL != m_pDragPlatformWindow && m_pDragPlatformWindow->m_bNeedRender)
				//PostPaint(m_pDragPlatformWindow);

			for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
			{
				PostPaint(*it);
			}

			m_pCurrentPlatformWindow = NULL;
		}
	}

	void ImwWindowManager::UpdatePlatformwWindowActions()
	{
		while (m_lPlatformWindowActions.begin() != m_lPlatformWindowActions.end())
		{
			PlatformWindowAction* pAction = *m_lPlatformWindowActions.begin();
	
			if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_DESTROY)
			{
				bool bFound = false;
				if (m_pMainPlatformWindow == pAction->m_pPlatformWindow)
				{
					Destroy();
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
					IM_ASSERT(false); // ImwPlatformWindow not found, maybe already closed
				}
			}
			else if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_SHOW)
			{
				pAction->m_pPlatformWindow->Show(true);
			}
			else if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_HIDE)
			{
				pAction->m_pPlatformWindow->Show(false);
			}
			else if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_SET_POSITION)
			{
				pAction->m_pPlatformWindow->SetPosition((int)pAction->m_oValue.x, (int)pAction->m_oValue.y);
			}
			else if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_SET_SIZE)
			{
				pAction->m_pPlatformWindow->SetSize((int)pAction->m_oValue.x, (int)pAction->m_oValue.y);
			}
			else if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_MAXIMIZE)
			{
				pAction->m_pPlatformWindow->SetWindowMaximized(true);
			}
			else if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_MINIMIZE)
			{
				pAction->m_pPlatformWindow->SetWindowMinimized();
			}
			else if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_RESTORE)
			{
				pAction->m_pPlatformWindow->SetWindowMaximized(false);
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
					InternalDockWith(pAction->m_pWindow, pAction->m_pWith, pAction->m_eOrientation, pAction->m_fRatio);
				}
				else if (NULL != pAction->m_pToContainer)
				{
					InternalDockTo(pAction->m_pWindow, pAction->m_eOrientation, pAction->m_fRatio, pAction->m_pToContainer, pAction->m_iPosition);
				}
				else if (NULL != pAction->m_pToPlatformWindow)
				{
					InternalDock(pAction->m_pWindow, pAction->m_eOrientation, pAction->m_fRatio, pAction->m_pToPlatformWindow);
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
				InternalDock(*m_lOrphanWindows.begin(), E_DOCK_ORIENTATION_CENTER, 0.5f, m_pMainPlatformWindow);
			}
			else if (CanCreateMultipleWindow())
			{
				ImVec2 oSize = ImVec2(300.f, 300.f);
				ImVec2 oPos = m_pMainPlatformWindow->GetPosition();
				ImVec2 oMainSize = m_pMainPlatformWindow->GetSize();
				oPos += (oMainSize - oSize) / 2.f;
				InternalFloat(*m_lOrphanWindows.begin(), oPos, oSize);
			}
			else
			{
				m_pMainPlatformWindow->m_pContainer->DockToBest(*m_lOrphanWindows.begin());
			}
			m_lOrphanWindows.erase(m_lOrphanWindows.begin());
		}
	}

	void ImwWindowManager::Render()
	{
		if (NULL != m_pMainPlatformWindow && m_pMainPlatformWindow->m_bNeedRender)
		{
			m_pMainPlatformWindow->Render();
		}

		if (NULL != m_pDragPlatformWindow && m_pDragPlatformWindow->m_bNeedRender)
		{
			m_pDragPlatformWindow->Render();
		}

		for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
		{
			if ((*it)->m_bNeedRender)
				(*it)->Render();
		}
	}

	void ImwWindowManager::Paint(ImwPlatformWindow* pWindow)
	{
		IM_ASSERT(NULL != pWindow);
		if (NULL == pWindow)
			return;

		pWindow->m_bNeedRender = false;
		if (pWindow->m_eType == E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW && (NULL == m_pDraggedWindow || m_bDragOnTab || m_pDragBestContainer != NULL))
			return;

		pWindow->m_bNeedRender = true;
		m_pCurrentPlatformWindow = pWindow;
		pWindow->SetContext(true);

		ImGui::GetIO().DisplaySize = pWindow->GetSize();
		ImGuiContext* pContext = ImGui::GetCurrentContext();
		if (pContext->FrameCountEnded >= pContext->FrameCount || !pContext->Initialized)
			ImGui::NewFrame();

		ImGuiStyle& oStyle = ImGui::GetStyle();

		float fTop = IsUsingCustomFrame() ? GetTitleBarHeight() : 0.f;
		float fBottom = pWindow->IsMainWindow() ?  GetStatusBarHeight() : 0.f;

		const int c_iWindowFlags = ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoBringToFrontOnFocus;

		const int c_iWindowChildFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		const int c_iWindowChildFlagsWithPadding = c_iWindowChildFlags | ImGuiWindowFlags_AlwaysUseWindowPadding;

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(pWindow->GetSize(), ImGuiSetCond_Always);
		int iFlags = c_iWindowFlags;

		if (NULL != m_pDraggedWindow)
		{
			iFlags += ImGuiWindowFlags_NoInputs;
		}

		float fWindowRoundingBackup = oStyle.WindowRounding;
		ImVec2 oWindowPaddingBackup = oStyle.WindowPadding;
		oStyle.WindowRounding = 0.f;
		oStyle.WindowPadding = ImVec2(0.f, 0.f);
		bool bWindowDraw = ImGui::Begin("ImWindow", NULL, ImVec2(0.f, 0.f), 1.f, iFlags);
		oStyle.WindowRounding = fWindowRoundingBackup;
		oStyle.WindowPadding = oWindowPaddingBackup;
		if (bWindowDraw)
		{
			if (NULL != m_pDraggedWindow)
			{
				ImGuiID oId = ImGui::GetID("##DraggedWindow");
				ImGui::PushID(oId);
				ImGui::PopID();
				ImGui::SetActiveID(oId, ImGui::GetCurrentWindow());
			}

			if (pWindow->GetType() != E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW && fTop > 0.f)
			{
				if (BeginTransparentChild("##ImWindowTitle", ImVec2(0.f, fTop), false, c_iWindowChildFlagsWithPadding))
				{
					PaintTitleBar(pWindow);
				}
				ImGui::EndChild();
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - oStyle.ItemSpacing.y);

			if (BeginTransparentChild("##ImWindowMain", ImVec2(0.f, 0.), false, c_iWindowChildFlags | ((pWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN) ? ImGuiWindowFlags_MenuBar : 0)))
			{
				if (pWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN)
				{
					ImGui::BeginMenuBar();
					for (ImwMenuList::iterator it = m_lMenus.begin(), itEnd = m_lMenus.end(); it != itEnd; ++it)
					{
						(*it)->OnMenu();
					}
					ImGui::EndMenuBar();
				}
				
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - oStyle.WindowPadding.y + oStyle.ItemSpacing.y);

				if (BeginTransparentChild("##ImWindowContent", ImVec2(0.f, -fBottom), false, c_iWindowChildFlagsWithPadding))
				{
					if (pWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN)
					{
						if (!m_lToolBars.empty())
						{
							for (ImwToolBarList::iterator it = m_lToolBars.begin(), itEnd = m_lToolBars.end(); it != itEnd; ++it)
							{
								(*it)->OnToolBar();
							}
							ImGui::Separator();
						}
					}

					if (BeginTransparentChild("##ImWindowContentSub", ImVec2(0.f, 0.f), false, c_iWindowChildFlags))
					{
						ImGuiWindow* pCurrentWindow = ImGui::GetCurrentWindowRead();
						pWindow->m_oContentArea.Min = pCurrentWindow->Pos;
						pWindow->m_oContentArea.Max = pCurrentWindow->Pos + pCurrentWindow->Size;

						pWindow->PaintContainer();
					}
					ImGui::EndChild();
				}
				ImGui::EndChild();

				if (pWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN && m_lStatusBars.size() > 0)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - oStyle.WindowPadding.y - oStyle.ItemSpacing.y + 1.f);

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_oConfig.m_oStatusBarWindowPadding);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_oConfig.m_oStatusBarFramePadding);

					if (BeginTransparentChild("##ImWindowStatusBars", ImVec2(0.f, 0.f), false, c_iWindowChildFlagsWithPadding))
					{
						ImGui::AlignFirstTextHeightToWidgets();

						ImGui::Columns((int)m_lStatusBars.size());
						for (ImwStatusBarList::iterator it = m_lStatusBars.begin(); it != m_lStatusBars.end(); ++it)
						{
							(*it)->OnStatusBar();
							ImGui::NextColumn();
						}
						ImGui::Columns(1);
					}

					ImGui::EndChild();

					ImGui::PopStyleVar(2);
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();

		m_bHasWantCaptureKeyboard |= ImGui::GetIO().WantCaptureKeyboard;
		m_bHasWantCaptureMouse |= ImGui::GetIO().WantCaptureMouse;

		pWindow->RestoreContext(true);
	}

	void ImwWindowManager::PostPaint(ImwPlatformWindow* pWindow)
	{
		IM_ASSERT(NULL != pWindow);
		if (NULL == pWindow)
			return;

		m_pCurrentPlatformWindow = pWindow;
		pWindow->SetContext(true);

		ImDrawList* pDrawList = &(ImGui::GetCurrentContext()->OverlayDrawList);
		for (ImwList<DrawWindowAreaAction>::iterator it = m_lDrawWindowAreas.begin(); it != m_lDrawWindowAreas.end(); )
		{
			DrawWindowAreaAction& oAction = *it;
			//if (pWindow->HasWindow(oAction.m_pWindow))
			if (pWindow == oAction.m_pWindow)
			{
				ImVec2 oPosA = oAction.m_oRectPos;
				ImVec2 oPosB = oAction.m_oRectPos + oAction.m_oRectSize;
				
				//pDrawList->AddLine(ImGui::CalcItemRectClosestPoint(ImGui::GetIO().MousePos, true, -2.0f), ImGui::GetIO().MousePos, ImColor(ImGui::GetStyle().Colors[ImGuiCol_Button]), 4.0f);
				pDrawList->AddRectFilled(oPosA, oPosB, oAction.m_oColor);
				
				ImwList<DrawWindowAreaAction>::iterator toRemove = it;
				++it;
				m_lDrawWindowAreas.erase(toRemove);
			}
			else
			{
				++it;
			}
		}
		pWindow->OnOverlay();

		pWindow->RestoreContext(false);
	}

	void ImwWindowManager::StartDragWindow(ImwWindow* pWindow, ImVec2 oOffset)
	{
		if (NULL == m_pDraggedWindow)
		{
			m_pDraggedWindow = pWindow;
			m_oDragPreviewOffset = oOffset;

			if (NULL != m_pDragPlatformWindow)
			{
				ImVec2 oCursorPos = GetCursorPos();
				ImVec2 oPosition = ImVec2(oCursorPos.x + m_oDragPreviewOffset.x, oCursorPos.y + m_oDragPreviewOffset.y);
				ImVec2 oSize = ImVec2(pWindow->GetLastSize().x, pWindow->GetLastSize().y + ImwContainer::c_fTabHeight);

				m_lPlatformWindowActions.push_back(new PlatformWindowAction(m_pDragPlatformWindow, E_PLATFORM_WINDOW_ACTION_SHOW));
				m_lPlatformWindowActions.push_back(new PlatformWindowAction(m_pDragPlatformWindow, E_PLATFORM_WINDOW_ACTION_SET_POSITION, oPosition));
				m_lPlatformWindowActions.push_back(new PlatformWindowAction(m_pDragPlatformWindow, E_PLATFORM_WINDOW_ACTION_SET_SIZE, oSize));

				Dock(pWindow, E_DOCK_ORIENTATION_CENTER, 0.5f, m_pDragPlatformWindow);
				m_pDragPlatformWindow->m_pContext->IO.MouseDown[0] = true;
			}
			else
			{
				UnDock(pWindow);
			}
		}
	}

	void ImwWindowManager::StopDragWindow()
	{
		if ( NULL != m_pDragPlatformWindow )
		{
			m_pDragPlatformWindow->Show(false);
			m_pDragPlatformWindow->m_bNeedRender = false;
			m_lPlatformWindowActions.push_back(new PlatformWindowAction(m_pDragPlatformWindow, E_PLATFORM_WINDOW_ACTION_HIDE));
		}
		m_pDraggedWindow = NULL;
	}

	void ImwWindowManager::UpdateDragWindow()
	{
		if (NULL != m_pDraggedWindow)
		{
			ImVec2 oCursorPos = GetCursorPos();

			//Search best dock area
			EDockOrientation eBestDockOrientation;
			ImVec2 oHightlightPos;
			ImVec2 oHightlightSize;
			float fSizeRatio;

			m_pDragBestContainer = (ImwContainer*)GetBestDocking(m_pMainPlatformWindow, oCursorPos, eBestDockOrientation, oHightlightPos, oHightlightSize, fSizeRatio, m_bDragOnTab, m_iDragBestContainerPosition, !CanCreateMultipleWindow());
			if (NULL == m_pDragBestContainer)
			{
				for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end() && NULL == m_pDragBestContainer; ++it)
				{
					m_pDragBestContainer = (ImwContainer*)GetBestDocking(*it, oCursorPos, eBestDockOrientation, oHightlightPos, oHightlightSize, fSizeRatio, m_bDragOnTab, m_iDragBestContainerPosition, false);
				}
			}
			if (m_pDragBestContainer)
			{
				DrawWindowArea(m_pDragBestContainer->GetPlatformWindowParent(), oHightlightPos, oHightlightSize, m_oConfig.m_oHightlightAreaColor);
			}

			if (NULL != m_pDragPlatformWindow)
			{
				if (m_bDragOnTab || m_pDragBestContainer != NULL)
				{
					m_pDragPlatformWindow->Show(false);
				}
				else
				{
					m_pDragPlatformWindow->Show(true);

					m_pDragPlatformWindow->SetPosition((int)(oCursorPos.x + m_oDragPreviewOffset.x), (int)(oCursorPos.y + m_oDragPreviewOffset.y));
				}
			}

			if (!IsLeftClickDown())
			{
				InternalUnDock(m_pDraggedWindow);
				if (NULL != m_pDragBestContainer)
				{
					InternalDockTo(m_pDraggedWindow, eBestDockOrientation, fSizeRatio, m_pDragBestContainer, m_iDragBestContainerPosition);
				}
				else if (NULL != m_pDragPlatformWindow)
				{
					InternalFloat(m_pDraggedWindow, m_pDragPlatformWindow->GetPosition(), m_pDragPlatformWindow->GetSize());
				}
				else
				{
					InternalDock(m_pDraggedWindow, E_DOCK_ORIENTATION_CENTER, fSizeRatio, m_pMainPlatformWindow);
				}

				StopDragWindow();
			}
		}
	}

	float ImwWindowManager::GetStatusBarHeight() const
	{
		IM_ASSERT(m_pCurrentPlatformWindow != NULL);
		if (m_pCurrentPlatformWindow != NULL && m_pCurrentPlatformWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN && m_lStatusBars.size() > 0)
		{
			ImGuiStyle& oStyle = ImGui::GetStyle();
			return m_oConfig.m_oStatusBarWindowPadding.y + m_oConfig.m_oStatusBarFramePadding.y * 2.f + ImGui::GetTextLineHeight();
		}
		return 0.f;
	}

	ImwWindow* ImwWindowManager::GetDraggedWindow() const
	{
		return m_pDraggedWindow;
	}

	ImVec2 ImwWindowManager::GetDragOffset() const
	{
		return m_oDragPreviewOffset;
	}

	ImwContainer* ImwWindowManager::GetDragBestContainer() const
	{
		return m_pDragBestContainer;
	}

	bool ImwWindowManager::GetDragOnTabArea() const
	{
		return m_bDragOnTab;
	}

	int ImwWindowManager::GetDragTabPosition() const
	{
		return m_iDragBestContainerPosition;
	}


	const ImwContainer* ImwWindowManager::GetBestDocking(ImwPlatformWindow* pPlatformWindow, const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, float& fOutRatio, bool& bOutOnTabArea, int& iOutPosition, bool bLargeCheck)
	{
		ImVec2 oPlatformWindowPos = pPlatformWindow->GetPosition();

		const ImRect& oContentRect = pPlatformWindow->m_oContentArea;
		const ImVec2 oContentSize = oContentRect.GetSize();

		ImVec2 oCursorPosInWindow(oCursorPos.x - oPlatformWindowPos.x, oCursorPos.y - oPlatformWindowPos.y);

		if (bLargeCheck || oContentRect.Contains(oCursorPosInWindow))
		{
			// Set context because GetBestDocking call CalTextSize who need the Font
			pPlatformWindow->SetContext(false);
			const ImwContainer* pBestContainer = pPlatformWindow->GetContainer()->GetBestDocking(oCursorPosInWindow, oOutOrientation, oOutAreaPos, oOutAreaSize, bOutOnTabArea, iOutPosition, bLargeCheck);
			pPlatformWindow->RestoreContext(false);
			if (NULL != pBestContainer)
			{
				fOutRatio = 0.5f; //Default value
				return pBestContainer;
			}

			ImRect oLeftArea(oContentRect.Min.x, oContentRect.Min.y, oContentSize.x * m_oConfig.m_fDragMarginSizeRatio, oContentRect.Max.y);
			ImRect oRightArea(oContentRect.Max.x - oContentSize.x * m_oConfig.m_fDragMarginSizeRatio, oContentRect.Min.y, oContentRect.Max.x, oContentRect.Max.y);
			ImRect oTopArea(oContentRect.Min.x, oContentRect.Min.y, oContentRect.Max.x, oContentRect.Min.y + oContentSize.y * m_oConfig.m_fDragMarginSizeRatio);
			ImRect oBottomArea(oContentRect.Min.x, oContentRect.Max.y - oContentSize.y * m_oConfig.m_fDragMarginSizeRatio, oContentRect.Max.x, oContentRect.Max.y);

			//Left
 			if (oLeftArea.Contains(oCursorPosInWindow) || (bLargeCheck && oCursorPosInWindow.x <= oLeftArea.Min.x))
			{
				oOutOrientation = E_DOCK_ORIENTATION_LEFT;
				oOutAreaPos = oLeftArea.Min;
				oOutAreaSize = oLeftArea.GetSize();
				bOutOnTabArea = false;
			}
			//Right
			else if (oRightArea.Contains(oCursorPosInWindow) || (bLargeCheck && oCursorPosInWindow.x >= oRightArea.Max.x))
			{
				oOutOrientation = E_DOCK_ORIENTATION_RIGHT;
				oOutAreaPos = oRightArea.Min;
				oOutAreaSize = oRightArea.GetSize();
				bOutOnTabArea = false;
			}
			//Top
			else if (oTopArea.Contains(oCursorPosInWindow) || (bLargeCheck && oCursorPosInWindow.y <= oTopArea.Min.y))
			{
				oOutOrientation = E_DOCK_ORIENTATION_TOP;
				oOutAreaPos = oTopArea.Min;
				oOutAreaSize = oTopArea.GetSize();
				bOutOnTabArea = false;
			}
			//Bottom
			else if (oBottomArea.Contains(oCursorPosInWindow) || (bLargeCheck && oCursorPosInWindow.y >= oBottomArea.Max.y))
			{
				oOutOrientation = E_DOCK_ORIENTATION_BOTTOM;
				oOutAreaPos = oBottomArea.Min;
				oOutAreaSize = oBottomArea.GetSize();
				bOutOnTabArea = false;
			}
			else
			{
				oOutOrientation = E_DOCK_ORIENTATION_CENTER;
				oOutAreaPos = pPlatformWindow->m_oContentArea.Min;
				oOutAreaSize = oContentSize;
				bOutOnTabArea = false;
				//IM_ASSERT(false); //Best dock orientation not found
				return NULL;
			}
			fOutRatio = GetConfig().m_fDragMarginSizeRatio;
			return pPlatformWindow->GetContainer();
		}
		oOutOrientation = E_DOCK_ORIENTATION_CENTER;
		oOutAreaPos = pPlatformWindow->m_oContentArea.Min;
		oOutAreaSize = oContentSize;
		bOutOnTabArea = false;
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

void ImwWindowManager::AddStatusBar(ImwStatusBar* pStatusBar)
	{
		ImwStatusBarList::iterator it = m_lStatusBars.begin(), itEnd = m_lStatusBars.end();
		for (; it != itEnd; ++it)
		{
			if (pStatusBar->GetHorizontalPriority() <= (*it)->GetHorizontalPriority())
				break;
		}
		m_lStatusBars.insert(it, pStatusBar);
	}

	void ImwWindowManager::RemoveStatusBar(ImwStatusBar* pStatusBar)
	{
		m_lStatusBars.remove(pStatusBar);
	}

	void ImwWindowManager::DestroyStatusBar(ImwStatusBar* pStatusBar)
	{
		if (NULL != pStatusBar && std::find(m_lToDestroyStatusBars.begin(), m_lToDestroyStatusBars.end(), pStatusBar) == m_lToDestroyStatusBars.end())
		{
			m_lToDestroyStatusBars.push_back(pStatusBar);
		}
	}

	void ImwWindowManager::AddMenu(ImwMenu* pMenu)
	{
		ImwMenuList::iterator it = m_lMenus.begin(), itEnd = m_lMenus.end();
		for (; it != itEnd; ++it)
		{
			if (pMenu->GetHorizontalPriority() <= (*it)->GetHorizontalPriority())
				break;
		}
		m_lMenus.insert(it, pMenu);
	}

	void ImwWindowManager::RemoveMenu(ImwMenu* pMenu)
	{
		m_lMenus.remove(pMenu);
	}

	void ImwWindowManager::DestroyMenu(ImwMenu* pMenu)
	{
		if (NULL != pMenu && std::find(m_lToDestroyMenus.begin(), m_lToDestroyMenus.end(), pMenu) == m_lToDestroyMenus.end())
		{
			m_lToDestroyMenus.push_back(pMenu);
		}
	}

	void ImwWindowManager::AddToolBar(ImwToolBar* pToolBar)
	{
		ImwToolBarList::iterator it = m_lToolBars.begin(), itEnd = m_lToolBars.end();
		for (; it != itEnd; ++it)
		{
			if (pToolBar->GetHorizontalPriority() <= (*it)->GetHorizontalPriority())
				break;
		}
		m_lToolBars.insert(it, pToolBar);
	}

	void ImwWindowManager::RemoveToolBar(ImwToolBar* pToolBar)
	{
		m_lToolBars.remove(pToolBar);
	}

	void ImwWindowManager::DestroyToolBar(ImwToolBar* pToolBar)
	{
		if (NULL != pToolBar && std::find(m_lToDestroyToolBars.begin(), m_lToDestroyToolBars.end(), pToolBar) == m_lToDestroyToolBars.end())
		{
			m_lToDestroyToolBars.push_back(pToolBar);
		}
	}

	void ImwWindowManager::InternalDock(ImwWindow* pWindow, EDockOrientation eOrientation, float fRatio, ImwPlatformWindow* pToPlatformWindow)
	{
		pToPlatformWindow->m_pContainer->Dock(pWindow, eOrientation, fRatio);
		if (pToPlatformWindow->GetType() == E_PLATFORM_WINDOW_TYPE_SECONDARY)
		{
			pToPlatformWindow->RefreshTitle();
		}
	}

	void ImwWindowManager::InternalDockTo(ImwWindow* pWindow, EDockOrientation eOrientation, float fRatio, ImwContainer* pToContainer, int iPosition)
	{
		pToContainer->Dock(pWindow, eOrientation, fRatio, iPosition);
		if (pToContainer->GetPlatformWindowParent()->GetType() == E_PLATFORM_WINDOW_TYPE_SECONDARY)
		{
			pToContainer->GetPlatformWindowParent()->RefreshTitle();
		}
	}

	void ImwWindowManager::InternalDockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation, float fRatio)
	{
		ImwContainer* pContainer = (ImwContainer*)m_pMainPlatformWindow->HasWindow(pWithWindow);
		if (NULL != pContainer)
		{
			pContainer->Dock(pWindow, eOrientation, fRatio);
		}

		for ( ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
		{
			pContainer = (ImwContainer*)(*it)->HasWindow(pWithWindow);
			if (NULL != pContainer)
			{
				pContainer->Dock(pWindow, eOrientation, fRatio);
				(*it)->RefreshTitle();
				break;
			}
		}
	}

	void ImwWindowManager::InternalFloat(ImwWindow* pWindow, ImVec2 oPosition, ImVec2 oSize)
	{
		ImwPlatformWindow* pPlatformWindow = CreatePlatformWindow(E_PLATFORM_WINDOW_TYPE_SECONDARY, m_pMainPlatformWindow);
		if (NULL != pPlatformWindow)
		{
			m_lPlatformWindows.push_back(pPlatformWindow);

			const ImVec2 oVec2_1 = ImVec2(1, 1);
			const ImVec2 oVec2_N1 = ImVec2(-1, -1);

			if (oSize.x == oVec2_1.x && oSize.y == oVec2_1.y)
			{
				oSize = pWindow->GetLastSize();
			}
			if (oPosition.x == oVec2_N1.x && oPosition.y == oVec2_N1.y)
			{
				oPosition = GetCursorPos();
				oPosition.x -= 20;
				oPosition.x -= 10;
			}
			pPlatformWindow->Dock(pWindow);
			pPlatformWindow->RefreshTitle();
			pPlatformWindow->SetSize((int)oSize.x, (int)oSize.y);
			pPlatformWindow->SetPosition((int)oPosition.x, (int)oPosition.y);
			pPlatformWindow->Show(true);
		}
	}

	void ImwWindowManager::InternalUnDock(ImwWindow* pWindow)
	{
		if (m_pMainPlatformWindow->UnDock(pWindow))
		{
			return;
		}

		for ( ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
		{
			if ( (*it)->UnDock(pWindow) )
			{
				(*it)->RefreshTitle();
				//Destroy empty platform window if not main window
				if ( (*it)->GetType() != E_PLATFORM_WINDOW_TYPE_MAIN && (*it)->GetContainer()->IsEmpty() )
				{
					m_lToDestroyPlatformWindows.push_back(*it);
				}
				return;
			}
		}

		if ( NULL != m_pDragPlatformWindow )
		{
			m_pDragPlatformWindow->UnDock(pWindow);
		}
	}

	void ImwWindowManager::OnClosePlatformWindow(ImwPlatformWindow* pWindow)
	{
		if (NULL != pWindow && !pWindow->m_pContainer->HasUnclosableWindow())
		{
			if (pWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN)
			{
				for (ImwList<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
				{
					if ((*it)->m_pContainer->HasUnclosableWindow())
						return;
				}
			}
			m_lPlatformWindowActions.push_back(new PlatformWindowAction(pWindow, E_PLATFORM_WINDOW_ACTION_DESTROY));
		}
	}

	void ImwWindowManager::DrawWindowArea( ImwPlatformWindow* pWindow, const ImVec2& oPos, const ImVec2& oSize, const ImColor& oColor )
	{
		m_lDrawWindowAreas.push_back(DrawWindowAreaAction(pWindow, oPos, oSize, oColor));
	}

	bool ImwWindowManager::BeginTransparentChild(const char* pName, const ImVec2& oSize, bool bBorder, ImGuiWindowFlags iFlags)
	{
		ImGuiStyle& oStyle = ImGui::GetStyle();
		ImVec4 oBackupChildWindowBg = oStyle.Colors[ImGuiCol_ChildWindowBg];
		oStyle.Colors[ImGuiCol_ChildWindowBg].w = 0.f;
		bool bRet = ImGui::BeginChild(pName, oSize, bBorder, iFlags);
		oStyle.Colors[ImGuiCol_ChildWindowBg] = oBackupChildWindowBg;
		return bRet;
	}

	// Static
	ImwWindowManager* ImwWindowManager::GetInstance()
	{
		return s_pInstance;
	}
//SFF_END
}