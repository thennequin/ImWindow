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
		, m_oStatusBarFramePadding( 4.f, 2.f )
	{
	}

	//////////////////////////////////////////////////////////////////////////

	ImwWindowManager::ClassNameFunctions::ClassNameFunctions()
		: m_pGetClassName(NULL)
		, m_pCanCreateWindowByClassName(NULL)
		, m_pCreateWindowByClassName(NULL)
	{
	}

	//////////////////////////////////////////////////////////////////////////

	ImwWindowManager::ImwWindowManager()
	{
		s_pInstance = this;
		m_bSelfManagedTitleBar = true;
		m_pMainTitle = NULL;
		m_pImGuiContext = NULL;
		m_pMainPlatformWindow = NULL;
		m_pDragPlatformWindow = NULL;
		m_pCurrentPlatformWindow = NULL;
		m_pFocusedPlatformWindow = NULL;
		m_pDraggedWindow = NULL;
		m_pDragBestContainer = NULL;
		m_bDragOnTab = false;
		m_iDragBestContainerPosition = -1;
		m_oDragPreviewOffset = ImVec2(-20, -10);
		m_bHasWantCaptureKeyboard = false;
		m_bHasWantCaptureMouse = false;
		m_bHasWantTextInput = false;
	}

	ImwWindowManager::~ImwWindowManager()
	{
		Destroy();
		s_pInstance = 0;
		ImwSafeFree(m_pMainTitle);
	}

	bool ImwWindowManager::Init()
	{
		m_pImGuiContext = ImGui::CreateContext();
		if (m_pImGuiContext == NULL)
			return false;

		ImGui::SetCurrentContext(m_pImGuiContext);

		InternalInit();

		ImGuiIO& io = ImGui::GetIO();

		io.IniFilename = NULL;

		// Add default font
		if (io.Fonts->Fonts.size() == 0)
		{
			io.Fonts->AddFontDefault();
			io.Fonts->Build();
		}

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
				(*m_lPlatformWindows.begin())->PreDestroy();
				delete *m_lPlatformWindows.begin();
				m_lPlatformWindows.erase(m_lPlatformWindows.begin());
			}

			if (m_pDragPlatformWindow != NULL)
			{
				m_pDragPlatformWindow->PreDestroy();
				ImwSafeDelete(m_pDragPlatformWindow);
			}

			if (m_pMainPlatformWindow != NULL)
			{
				m_pMainPlatformWindow->PreDestroy();
				ImwSafeDelete(m_pMainPlatformWindow);
			}

			InternalDestroy();
			if (m_pImGuiContext != NULL)
			{
				ImGui::DestroyContext(m_pImGuiContext);
				m_pImGuiContext = NULL;
			}
		}
	}

	ImGuiContext* ImwWindowManager::GetContext() const
	{
		return m_pImGuiContext;
	}

	bool ImwWindowManager::IsExiting() const
	{
		for (ImVector<PlatformWindowAction*>::const_iterator it = m_lPlatformWindowActions.begin(), itEnd = m_lPlatformWindowActions.end(); it != itEnd; ++it)
		{
			if ((*it)->m_eAction == E_PLATFORM_WINDOW_ACTION_DESTROY && (*it)->m_pPlatformWindow == m_pMainPlatformWindow)
			{
				return true;
			}
		}
		return false;
	}

	ImwPlatformWindow* ImwWindowManager::GetMainPlatformWindow() const
	{
		return m_pMainPlatformWindow;
	}

	const ImwPlatformWindowVector& ImwWindowManager::GetSecondariesPlatformWindows() const
	{
		return m_lPlatformWindows;
	}

	ImwPlatformWindow* ImwWindowManager::GetFocusedPlatformWindow() const
	{
		return m_pFocusedPlatformWindow;
	}

	ImwWindowManager::Config& ImwWindowManager::GetConfig()
	{
		return m_oConfig;
	}

	void ImwWindowManager::SetMainTitle(const char* pTitle)
	{
		ImwSafeFree(m_pMainTitle);
		if (NULL != pTitle)
		{
			size_t iLen = strlen(pTitle) + 1;
			m_pMainTitle = (char*)ImwMalloc(sizeof(char) * iLen);
			strcpy(m_pMainTitle, pTitle);
		}

		ImwIsSafe(m_pMainPlatformWindow)->SetTitle(m_pMainTitle);

		for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
		{
			ImwIsSafe((*it))->RefreshTitle();
		}
	}

	const char* ImwWindowManager::GetMainTitle() const
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
			for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
			{
				if ((*it)->FocusWindow(pWindow))
					break;
			}
		}
	}

	const ImwWindowVector& ImwWindowManager::GetWindowList() const
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

		for ( ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
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

#ifdef IMW_USE_LAYOUT_SERIALIZATION
	bool ImwWindowManager::SaveLayoutToString(ImwString& sLayout, bool bCompact)
	{
		JsonStthm::JsonValue oJson;
		oJson.InitType(JsonStthm::JsonValue::E_TYPE_OBJECT);

		if ( m_pMainPlatformWindow->Save(oJson["MainPlatformWindow"]) )
		{
			JsonStthm::JsonValue& oJsonPlatformWindows = oJson["PlatformWindows"];
			int iCurrent = 0;
			for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
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

	bool ImwWindowManager::SaveLayoutToFile(const char* pFilePath, bool bCompact)
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

	bool ImwWindowManager::LoadLayoutFromString(const char* pLayout)
	{
		JsonStthm::JsonValue oJson;
		if (oJson.ReadString(pLayout) == 0 && oJson.IsObject())
		{
			JsonStthm::JsonValue& oJsonMainPlatformWindow = oJson["MainPlatformWindow"];
			JsonStthm::JsonValue& oJsonPlatformWindows = oJson["PlatformWindows"];

			//Check layout integrity
			if (!oJsonMainPlatformWindow.IsObject() || !m_pMainPlatformWindow->Load(oJsonMainPlatformWindow, true))
				return false;

			if (!oJsonPlatformWindows.IsArray() && !oJsonPlatformWindows.IsNull())
				return false;

			int iPlatformWindowCount = oJsonPlatformWindows.GetMemberCount();
			for (int iCurrent = 0; iCurrent < iPlatformWindowCount; ++iCurrent)
			{
				JsonStthm::JsonValue& oJsonPlatformWindow = oJsonPlatformWindows[iCurrent];
				if (!oJsonPlatformWindow.IsObject() || !m_pMainPlatformWindow->Load(oJsonPlatformWindow, true))
					return false;
			}

			//Load layout
			if (!m_pMainPlatformWindow->Load(oJsonMainPlatformWindow, false))
				return false; //Something wrong

			for (ImwPlatformWindowVector::iterator it = m_lPlatformWindows.begin(), itEnd = m_lPlatformWindows.end(); it != itEnd; ++it)
			{
				ImwPlatformWindow* pPlatformWindow = *it;
				pPlatformWindow->PreDestroy();
				delete pPlatformWindow;
			}
			m_lPlatformWindows.clear();

			for (int iCurrent = 0; iCurrent < iPlatformWindowCount; ++iCurrent)
			{
				JsonStthm::JsonValue& oJsonPlatformWindow = oJsonPlatformWindows[iCurrent];
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

	bool ImwWindowManager::LoadLayoutFromFile(const char* pFilePath)
	{
		FILE* pFile = fopen(pFilePath, "r");
		if (NULL != pFile)
		{
			fseek(pFile, 0, SEEK_END);
			long iSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			bool bReturn = false;
			if (iSize > 0)
			{
				char* pString = new char[1 + iSize / sizeof(char)];
				fread(pString, 1, iSize, pFile);
				pString[iSize] = 0;

				bReturn = LoadLayoutFromString(pString);

				delete[] pString;
			}

			fclose( pFile );

			return bReturn;
		}
		return false;
	}
#endif //IMW_USE_LAYOUT_SERIALIZATION

	void ImwWindowManager::SetClassNameFunctions(const ClassNameFunctions* pFunctions)
	{
		m_oClassNameFunctions.m_pGetClassName = (pFunctions != NULL) ? pFunctions->m_pGetClassName: NULL;
		m_oClassNameFunctions.m_pCanCreateWindowByClassName = ( pFunctions != NULL ) ? pFunctions->m_pCanCreateWindowByClassName : NULL;
		m_oClassNameFunctions.m_pCreateWindowByClassName = ( pFunctions != NULL ) ? pFunctions->m_pCreateWindowByClassName : NULL;
	}

	const char* ImwWindowManager::GetWindowClassName(ImwWindow* pWindow)
	{
		if (m_oClassNameFunctions.m_pGetClassName != NULL)
		{
			return m_oClassNameFunctions.m_pGetClassName(pWindow);
		}
		return NULL;
	}

	bool ImwWindowManager::CanCreateWindowByClassName(const char* pName)
	{
		if (m_oClassNameFunctions.m_pCanCreateWindowByClassName != NULL)
		{
			return m_oClassNameFunctions.m_pCanCreateWindowByClassName(pName);
		}
		return false;
	}

	ImwWindow* ImwWindowManager::CreateWindowByClassName(const char* pName)
	{
		if (m_oClassNameFunctions.m_pCreateWindowByClassName != NULL)
		{
			return m_oClassNameFunctions.m_pCreateWindowByClassName(pName);
		}
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

	void ImwWindowManager::PaintTitleBar(ImwPlatformWindow* pPlatformWindow, bool bDrawTitle)
	{
		const float c_fButtonWidth = 24.f;

		ImDrawList* pDrawList = ImGui::GetWindowDrawList();

		if (bDrawTitle)
		{
			ImGui::TextUnformatted(GetMainTitle());
			ImGui::SameLine();
		}

		ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - 3.f * c_fButtonWidth, 1.f));

		ImU32 iColor = ImGui::GetColorU32(ImGuiCol_Text);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

		ImVec2 oMin, oMax, oCenter, oStartRect, oEndRect;

		//Minimize
		{
			ImGui::SameLine();
			if (ImGui::Button("##Minimize", ImVec2(c_fButtonWidth, 0.f)) && m_bSelfManagedTitleBar)
			{
				pPlatformWindow->SetWindowMinimized(true);
			}
			if (ImGui::IsItemHovered())
			{
				pPlatformWindow->m_eHoveredArea = E_PLATFORMWINDOWHOVEREDAREA_MINIMIZE;
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
			if (ImGui::Button("##Maximize", ImVec2(c_fButtonWidth, 0.f)) && m_bSelfManagedTitleBar)
			{
				pPlatformWindow->SetWindowMaximized(!pPlatformWindow->IsWindowMaximized());
			}
			if (ImGui::IsItemHovered())
			{
				pPlatformWindow->m_eHoveredArea = E_PLATFORMWINDOWHOVEREDAREA_MAXIMIZE;
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
			if (ImGui::Button("##Close", ImVec2(c_fButtonWidth, 0.f)) && m_bSelfManagedTitleBar)
			{
				pPlatformWindow->OnClose();
			}
			if (ImGui::IsItemHovered())
			{
				pPlatformWindow->m_eHoveredArea = E_PLATFORMWINDOWHOVEREDAREA_CLOSE;
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

		ImGuiWindow* pCurrentImGuiWindow = ImGui::GetCurrentWindow();
		ImRect oDraggableArea(pCurrentImGuiWindow->DC.CursorStartPos, pCurrentImGuiWindow->DC.CursorMaxPos - ImVec2(3.f * c_fButtonWidth, 0.f));

		if (ImGui::IsMouseHoveringRect(oDraggableArea.Min, oDraggableArea.Max) && pPlatformWindow->m_eHoveredArea == E_PLATFORMWINDOWHOVEREDAREA_NONE)
		{
			pPlatformWindow->m_eHoveredArea = E_PLATFORMWINDOWHOVEREDAREA_CAPTION;
		}

		if (m_bSelfManagedTitleBar)
		{
			bool bHover, bHeld;
			ImGuiID oDraggableId = ImGui::GetID("##DraggableArea");
			ImGui::ButtonBehavior(oDraggableArea, oDraggableId, &bHover, &bHeld, 0);

			ImGuiID iDoubleClickedID = ImGui::GetID("IsDoubleClicked");
			ImGuiID iFirstClickedID = ImGui::GetID("FirstClicked");
			bool bDoubleClicked = ImGui::GetStateStorage()->GetBool(iDoubleClickedID, false);
			bool bFirstClicked = ImGui::GetStateStorage()->GetBool(iFirstClickedID, false);

			if (ImGui::IsMouseHoveringRect(oDraggableArea.Min, oDraggableArea.Max) && ImGui::GetIO().MouseDoubleClicked[0] && ImGui::IsMouseDragging(0) == false)
			{
				pPlatformWindow->SetWindowMaximized(!pPlatformWindow->IsWindowMaximized());
				ImGui::GetStateStorage()->SetBool(iDoubleClickedID, true);
				bDoubleClicked = true;
			}
			else if (bHeld)
			{
				if (bDoubleClicked == false)
				{
					pPlatformWindow->Moving(bFirstClicked == false);

					if (bFirstClicked == false)
					{
						ImGui::GetStateStorage()->SetBool(iFirstClickedID, true);
						bFirstClicked = true;
					}
				}
			}
			else
			{
				if (bFirstClicked)
				{
					ImGui::GetStateStorage()->SetBool(iFirstClickedID, false);
					bFirstClicked = false;
				}
				if (bDoubleClicked)
				{
					ImGui::GetStateStorage()->SetBool(iDoubleClickedID, false);
					bDoubleClicked = false;
				}
			}
		}
	}

	void ImwWindowManager::PreRender()
	{

	}

	void ImwWindowManager::PostRender()
	{

	}

	void ImwWindowManager::PreUpdate()
	{
		ImwIsSafe(m_pMainPlatformWindow)->PreUpdate();

		for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
		{
			(*it)->PreUpdate();
		}
	}

	void ImwWindowManager::Update()
	{
		UpdatePlatformwWindowActions();
		UpdateDockActions();
		UpdateOrphans();

		for (ImwWindowVector::iterator it = m_lToDestroyWindows.begin(), itEnd = m_lToDestroyWindows.end(); it != itEnd; ++it)
		{
			ImwWindow* pWindow = *it;

			ImwWindowVector::iterator itFind = std::find(m_lOrphanWindows.begin(), m_lOrphanWindows.end(), pWindow);
			if (itFind != m_lOrphanWindows.end())
				m_lOrphanWindows.erase(itFind);

			itFind = std::find(m_lWindows.begin(), m_lWindows.end(), pWindow);
			if (itFind != m_lWindows.end())
				m_lWindows.erase(itFind);

			InternalUnDock(pWindow);

			delete pWindow;
		}
		m_lToDestroyWindows.clear();

		for (ImwStatusBarVector::iterator it = m_lToDestroyStatusBars.begin(), itEnd = m_lToDestroyStatusBars.end(); it != itEnd; ++it)
		{
			delete *it;
		}
		m_lToDestroyStatusBars.clear();

		for (ImwMenuVector::iterator it = m_lToDestroyMenus.begin(), itEnd = m_lToDestroyMenus.end(); it != itEnd; ++it)
		{
			delete *it;
		}
		m_lToDestroyMenus.clear();

		for (ImwToolBarVector::iterator it = m_lToDestroyToolBars.begin(), itEnd = m_lToDestroyToolBars.end(); it != itEnd; ++it)
		{
			delete *it;
		}
		m_lToDestroyToolBars.clear();

		for (ImwPlatformWindowVector::iterator it = m_lToDestroyPlatformWindows.begin(), itEnd = m_lToDestroyPlatformWindows.end(); it != itEnd; ++it)
		{
			ImwPlatformWindow* pPlatformWindow = *it;

			ImwPlatformWindowVector::iterator itFind = std::find(m_lPlatformWindows.begin(), m_lPlatformWindows.end(), pPlatformWindow);
			if (itFind != m_lPlatformWindows.end())
				m_lPlatformWindows.erase(itFind);

			pPlatformWindow->PreDestroy();
			delete pPlatformWindow;
		}
		m_lToDestroyPlatformWindows.clear();

		if (NULL != m_pMainPlatformWindow)
		{
			m_bHasWantCaptureKeyboard = false;
			m_bHasWantCaptureMouse = false;
			m_bHasWantTextInput = false;
			UpdateDragWindow();

			Paint(m_pMainPlatformWindow);
			if (CanCreateMultipleWindow())
			{
				Paint(m_pDragPlatformWindow);

				for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
				{
					Paint(*it);
				}
			}

			PostPaint(m_pMainPlatformWindow);

			//if (NULL != m_pDragPlatformWindow && m_pDragPlatformWindow->m_bNeedRender)
				//PostPaint(m_pDragPlatformWindow);

			for ( ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
			{
				PostPaint(*it);
			}

			m_lDrawWindowAreas.clear();

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
					for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
					{
						if (*it == pAction->m_pPlatformWindow)
						{
							(*it)->PreDestroy();
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
				pAction->m_pPlatformWindow->SetWindowMinimized(true);
			}
			else if (pAction->m_eAction == E_PLATFORM_WINDOW_ACTION_RESTORE)
			{
				pAction->m_pPlatformWindow->SetWindowMaximized(false);
				pAction->m_pPlatformWindow->SetWindowMinimized(false);
			}

			delete *m_lPlatformWindowActions.begin();
			m_lPlatformWindowActions.erase(m_lPlatformWindowActions.begin());
		}
	}

	void ImwWindowManager::UpdateDockActions()
	{
		for (ImVector<DockAction*>::iterator it = m_lDockActions.begin(), itEnd = m_lDockActions.end(); it != itEnd; ++it)
		{
			DockAction* pAction = *it;

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
				else
				{
					GetMainPlatformWindow()->GetContainer()->DockToBest(pAction->m_pWindow);
				}
			}

			ImwWindowVector::iterator itFind = std::find(m_lOrphanWindows.begin(), m_lOrphanWindows.end(), pAction->m_pWindow);
			if (itFind != m_lOrphanWindows.end())
				m_lOrphanWindows.erase(itFind);

			delete pAction;
		}
		m_lDockActions.clear();
	}

	void ImwWindowManager::UpdateOrphans()
	{
		for (ImwWindowVector::iterator it = m_lOrphanWindows.begin(), itEnd = m_lOrphanWindows.end(); it != itEnd; ++it)
		{
			if (m_pMainPlatformWindow->m_pContainer->IsEmpty())
			{
				InternalDock(*it, E_DOCK_ORIENTATION_CENTER, 0.5f, m_pMainPlatformWindow);
			}
			else if (CanCreateMultipleWindow())
			{
				ImVec2 oSize = ImVec2(300.f, 300.f);
				ImVec2 oPos = m_pMainPlatformWindow->GetPosition();
				ImVec2 oMainSize = m_pMainPlatformWindow->GetSize();
				oPos.x += (oMainSize.x - oSize.x) / 2;
				oPos.y += (oMainSize.y - oSize.y) / 2;
				InternalFloat(*it, oPos, oSize);
			}
			else
			{
				m_pMainPlatformWindow->m_pContainer->DockToBest(*it);
			}
		}
		m_lOrphanWindows.clear();
	}

	void ImwWindowManager::Render()
	{
		PreRender();

		if (NULL != m_pMainPlatformWindow && m_pMainPlatformWindow->m_bNeedRender)
		{
			m_pCurrentPlatformWindow = m_pMainPlatformWindow;
			m_pMainPlatformWindow->Render();
		}

		if (NULL != m_pDragPlatformWindow && m_pDragPlatformWindow->m_bNeedRender)
		{
			m_pCurrentPlatformWindow = m_pDragPlatformWindow;
			m_pDragPlatformWindow->Render();
		}

		for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
		{
			if ((*it)->m_bNeedRender)
			{
				m_pCurrentPlatformWindow = *it;
				(*it)->Render();
			}
		}

		m_pCurrentPlatformWindow = NULL;

		PostRender();
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

		ImGuiContext* pContext = ImGui::GetCurrentContext();
		ImGuiIO& oIO = pContext->IO;
		oIO.DisplaySize = pWindow->GetSize();
		ImGui::NewFrame();

		ImGuiStyle& oStyle = ImGui::GetStyle();

		float fTop = IsUsingCustomFrame() ? GetTitleBarHeight() : 0.f;
		float fBottom = pWindow->IsMainWindow() ?  (GetStatusBarHeight() + oStyle.ItemSpacing.y) : 0.f;

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

		bool bDisplayMenus = pWindow->IsShowContent() || oIO.MousePos.y <= 50.f || pContext->OpenPopupStack.size() > 0;
		bool bDisplayWindow = bDisplayMenus || IsUsingCustomFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		if (pWindow->IsShowContent())
		{
			ImGui::SetNextWindowSize(pWindow->GetSize(), ImGuiCond_Always);
		}
		else
		{
			ImVec2 oSize = pWindow->GetSize();
			oSize.y = 0;
			if (IsUsingCustomFrame())
			{
				oSize.y += GetTitleBarHeight();
			}
			if (bDisplayMenus)
			{
				oSize.y += pContext->FontSize + pContext->Style.FramePadding.y * 2.0f;
				oSize.y += ImGui::GetCurrentWindowRead()->MenuBarHeight();

			}
			ImGui::SetNextWindowSize(oSize, ImGuiCond_Always);
		}
		int iFlags = c_iWindowFlags;

		if (NULL != m_pDraggedWindow)
		{
			iFlags += ImGuiWindowFlags_NoInputs;
		}

		if (bDisplayWindow)
		{
			float fWindowRoundingBackup = oStyle.WindowRounding;
			ImVec2 oWindowPaddingBackup = oStyle.WindowPadding;
			ImVec2 oWindowMinSizeBackup = oStyle.WindowMinSize;
			oStyle.WindowRounding = 0.f;
			oStyle.WindowPadding = ImVec2(0.f, 0.f);
			oStyle.WindowMinSize = ImVec2(0.f, 0.f);
			bool bWindowDraw = ImGui::Begin("ImWindow", NULL, iFlags);
			ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Tab, ImGui::GetCurrentWindow()->ID); // Disable ImGui CTRL+Tab
			ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Tab, ImGui::GetCurrentWindow()->ID);
			oStyle.WindowRounding = fWindowRoundingBackup;
			oStyle.WindowPadding = oWindowPaddingBackup;
			oStyle.WindowMinSize = oWindowMinSizeBackup;
			if (bWindowDraw)
			{
				if (NULL != m_pDraggedWindow)
				{
					ImGuiID oId = ImGui::GetID("##DraggedWindow");
					ImGui::PushID(oId);
					ImGui::PopID();
					ImGui::SetActiveID(oId, ImGui::GetCurrentWindow());
				}

				pWindow->m_eHoveredArea = E_PLATFORMWINDOWHOVEREDAREA_NONE;
				if (pWindow->GetType() != E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW && fTop > 0.f)
				{
					if (BeginTransparentChild("##ImWindowTitle", ImVec2(0.f, fTop), false, c_iWindowChildFlagsWithPadding))
					{
						PaintTitleBar(pWindow, true);
					}
					ImGui::EndChild();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - oStyle.ItemSpacing.y);
				}

				int iMainWindowFlags = (pWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN && bDisplayMenus) ? (c_iWindowChildFlagsWithPadding | ImGuiWindowFlags_MenuBar) : c_iWindowChildFlagsWithPadding;

				if (BeginTransparentChild("##ImWindowMain", ImVec2(0.f, 0.f), false, iMainWindowFlags))
				{
					if (pWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN)
					{
						if (bDisplayMenus) // Autohide menu bar
						{
							if (ImGui::BeginMenuBar())
							{
								for (ImwMenuVector::iterator it = m_lMenus.begin(), itEnd = m_lMenus.end(); it != itEnd; ++it)
								{
									(*it)->OnMenu();
								}
								ImGui::EndMenuBar();
							}
						}
					}

					if (pWindow->IsShowContent())
					{
						if (BeginTransparentChild("##ImWindowContent", ImVec2(0.f, -fBottom), false, c_iWindowChildFlags))
						{
							if (pWindow->GetType() == E_PLATFORM_WINDOW_TYPE_MAIN)
							{
								if (!m_lToolBars.empty())
								{
									for (ImwToolBarVector::iterator it = m_lToolBars.begin(), itEnd = m_lToolBars.end(); it != itEnd; ++it)
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
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_oConfig.m_oStatusBarFramePadding);

							if (BeginTransparentChild("##ImWindowStatusBars", ImVec2(0.f, 0.f), false, c_iWindowChildFlags))
							{
								ImGui::AlignTextToFramePadding();

								ImGui::Columns((int)m_lStatusBars.size());
								for (ImwStatusBarVector::iterator it = m_lStatusBars.begin(); it != m_lStatusBars.end(); ++it)
								{
									(*it)->OnStatusBar();
									ImGui::NextColumn();
								}
								ImGui::Columns(1);
							}

							ImGui::EndChild();

							ImGui::PopStyleVar(1);
						}
					}
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}

		m_bHasWantCaptureKeyboard |= ImGui::GetIO().WantCaptureKeyboard;
		m_bHasWantCaptureMouse |= ImGui::GetIO().WantCaptureMouse;
		m_bHasWantTextInput |= ImGui::GetIO().WantTextInput;

		pWindow->RestoreContext(true);
	}

	void ImwWindowManager::PostPaint(ImwPlatformWindow* pWindow)
	{
		IM_ASSERT(NULL != pWindow);
		if (NULL == pWindow)
			return;

		m_pCurrentPlatformWindow = pWindow;
		pWindow->SetContext(true);

		ImDrawList* pDrawList = ImGui::GetForegroundDrawList();
		for (ImVector<DrawWindowAreaAction>::iterator it = m_lDrawWindowAreas.begin(); it != m_lDrawWindowAreas.end(); ++it)
		{
			DrawWindowAreaAction& oAction = *it;
			//if (pWindow->HasWindow(oAction.m_pWindow))
			if (pWindow == oAction.m_pWindow)
			{
				ImVec2 oPosA = oAction.m_oRectPos;
				ImVec2 oPosB = oAction.m_oRectPos + oAction.m_oRectSize;

				//pDrawList->AddLine(ImGui::CalcItemRectClosestPoint(ImGui::GetIO().MousePos, true, -2.0f), ImGui::GetIO().MousePos, ImColor(ImGui::GetStyle().Colors[ImGuiCol_Button]), 4.0f);
				pDrawList->AddRectFilled(oPosA, oPosB, oAction.m_oColor);
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

			m_pDragBestContainer = (ImwContainer*)GetBestDocking(m_pMainPlatformWindow, oCursorPos, eBestDockOrientation, oHightlightPos, oHightlightSize, fSizeRatio, &m_bDragOnTab, &m_iDragBestContainerPosition, !CanCreateMultipleWindow());
			if (NULL == m_pDragBestContainer)
			{
				for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end() && NULL == m_pDragBestContainer; ++it)
				{
					m_pDragBestContainer = (ImwContainer*)GetBestDocking(*it, oCursorPos, eBestDockOrientation, oHightlightPos, oHightlightSize, fSizeRatio, &m_bDragOnTab, &m_iDragBestContainerPosition, false);
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
			return ImGui::GetTextLineHeight() + m_oConfig.m_oStatusBarFramePadding.y * 2.f;
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


	const ImwContainer* ImwWindowManager::GetBestDocking(ImwPlatformWindow* pPlatformWindow, const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, float& fOutRatio, bool* pOutOnTabArea, int* pOutPosition, bool bLargeCheck)
	{
		ImVec2 oPlatformWindowPos = pPlatformWindow->GetPosition();

		const ImRect& oContentRect = pPlatformWindow->m_oContentArea;
		const ImVec2 oContentSize = oContentRect.GetSize();

		ImVec2 oCursorPosInWindow(oCursorPos.x - oPlatformWindowPos.x, oCursorPos.y - oPlatformWindowPos.y);

		if (bLargeCheck || oContentRect.Contains(oCursorPosInWindow))
		{
			// Set context because GetBestDocking call CalTextSize who need the Font
			pPlatformWindow->SetContext(false);
			const ImwContainer* pBestContainer = pPlatformWindow->GetContainer()->GetBestDocking(oCursorPosInWindow, oOutOrientation, oOutAreaPos, oOutAreaSize, pOutOnTabArea, pOutPosition, bLargeCheck);
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
				*pOutOnTabArea = false;
			}
			//Right
			else if (oRightArea.Contains(oCursorPosInWindow) || (bLargeCheck && oCursorPosInWindow.x >= oRightArea.Max.x))
			{
				oOutOrientation = E_DOCK_ORIENTATION_RIGHT;
				oOutAreaPos = oRightArea.Min;
				oOutAreaSize = oRightArea.GetSize();
				*pOutOnTabArea = false;
			}
			//Top
			else if (oTopArea.Contains(oCursorPosInWindow) || (bLargeCheck && oCursorPosInWindow.y <= oTopArea.Min.y))
			{
				oOutOrientation = E_DOCK_ORIENTATION_TOP;
				oOutAreaPos = oTopArea.Min;
				oOutAreaSize = oTopArea.GetSize();
				*pOutOnTabArea = false;
			}
			//Bottom
			else if (oBottomArea.Contains(oCursorPosInWindow) || (bLargeCheck && oCursorPosInWindow.y >= oBottomArea.Max.y))
			{
				oOutOrientation = E_DOCK_ORIENTATION_BOTTOM;
				oOutAreaPos = oBottomArea.Min;
				oOutAreaSize = oBottomArea.GetSize();
				*pOutOnTabArea = false;
			}
			else
			{
				oOutOrientation = E_DOCK_ORIENTATION_CENTER;
				oOutAreaPos = pPlatformWindow->m_oContentArea.Min;
				oOutAreaSize = oContentSize;
				*pOutOnTabArea = false;
				//IM_ASSERT(false); //Best dock orientation not found
				return NULL;
			}
			fOutRatio = GetConfig().m_fDragMarginSizeRatio;
			return pPlatformWindow->GetContainer();
		}
		oOutOrientation = E_DOCK_ORIENTATION_CENTER;
		oOutAreaPos = pPlatformWindow->m_oContentArea.Min;
		oOutAreaSize = oContentSize;
		*pOutOnTabArea = false;
		return NULL;
	}

	void ImwWindowManager::AddWindow(ImwWindow* pWindow)
	{
		m_lWindows.push_back(pWindow);

		m_lOrphanWindows.push_back(pWindow);
	}

	void ImwWindowManager::RemoveWindow(ImwWindow* pWindow)
	{
		IM_ASSERT(m_pCurrentPlatformWindow == NULL); // Don't call delete on ImwWindow instance, prefer calling Destroy

		ImwWindowVector::iterator itFind = std::find(m_lWindows.begin(), m_lWindows.end(), pWindow);
		if (itFind != m_lWindows.end())
			m_lWindows.erase(itFind);

		itFind = std::find(m_lOrphanWindows.begin(), m_lOrphanWindows.end(), pWindow);
		if (itFind != m_lOrphanWindows.end())
			m_lOrphanWindows.erase(itFind);
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
		ImwStatusBarVector::iterator it = m_lStatusBars.begin(), itEnd = m_lStatusBars.end();
		for (; it != itEnd; ++it)
		{
			if (pStatusBar->GetHorizontalPriority() <= (*it)->GetHorizontalPriority())
				break;
		}
		m_lStatusBars.insert(it, pStatusBar);
	}

	void ImwWindowManager::RemoveStatusBar(ImwStatusBar* pStatusBar)
	{
		ImwStatusBarVector::iterator itFind = std::find(m_lStatusBars.begin(), m_lStatusBars.end(), pStatusBar);
		if (itFind != m_lStatusBars.end())
			m_lStatusBars.erase(itFind);
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
		ImwMenuVector::iterator it = m_lMenus.begin(), itEnd = m_lMenus.end();
		for (; it != itEnd; ++it)
		{
			if (pMenu->GetHorizontalPriority() <= (*it)->GetHorizontalPriority())
				break;
		}
		m_lMenus.insert(it, pMenu);
	}

	void ImwWindowManager::RemoveMenu(ImwMenu* pMenu)
	{
		ImwMenuVector::iterator itFind = std::find(m_lMenus.begin(), m_lMenus.end(), pMenu);
		if (itFind != m_lMenus.end())
			m_lMenus.erase(itFind);
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
		ImwToolBarVector::iterator it = m_lToolBars.begin(), itEnd = m_lToolBars.end();
		for (; it != itEnd; ++it)
		{
			if (pToolBar->GetHorizontalPriority() <= (*it)->GetHorizontalPriority())
				break;
		}
		m_lToolBars.insert(it, pToolBar);
	}

	void ImwWindowManager::RemoveToolBar(ImwToolBar* pToolBar)
	{
		ImwToolBarVector::iterator itFind = std::find(m_lToolBars.begin(), m_lToolBars.end(), pToolBar);
		if (itFind != m_lToolBars.end())
			m_lToolBars.erase(itFind);
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

		for ( ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
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

		for ( ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it )
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
				for (ImVector<ImwPlatformWindow*>::iterator it = m_lPlatformWindows.begin(); it != m_lPlatformWindows.end(); ++it)
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
		ImVec4 oBackupChildWindowBg = oStyle.Colors[ImGuiCol_ChildBg];
		oStyle.Colors[ImGuiCol_ChildBg].w = 0.f;
		bool bRet = ImGui::BeginChild(pName, oSize, bBorder, iFlags);
		oStyle.Colors[ImGuiCol_ChildBg] = oBackupChildWindowBg;
		return bRet;
	}

	// Static
	ImwWindowManager* ImwWindowManager::GetInstance()
	{
		return s_pInstance;
	}

	void ImwWindowManager::RegenFontTexture()
	{
		m_pMainPlatformWindow->RegenFontTexture(NULL);
		m_pDragPlatformWindow->RegenFontTexture(m_pMainPlatformWindow);
		for (ImwPlatformWindow* pWindow : m_lPlatformWindows)
		{
			pWindow->RegenFontTexture(m_pMainPlatformWindow);
		}
	}

//SFF_END
}
