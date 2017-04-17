
#include "ImwContainer.h"

#include "ImwWindowManager.h"

#include <algorithm>

namespace ImWindow
{
//SFF_BEGIN
	const float ImwContainer::c_fTabHeight = 25.f;
	
	ImwContainer::ImwContainer(ImwContainer* pParent)
	{
		IM_ASSERT(NULL != pParent);
		m_pSplits[0] = NULL;
		m_pSplits[1] = NULL;
		m_bVerticalSplit = false;
		m_iActiveWindow = 0;
		m_fSplitRatio = 0.5f;
		m_bIsDrag = false;
		m_pParent = pParent;
		m_pParentWindow = (NULL != pParent) ? pParent->m_pParentWindow : NULL;
	}

	ImwContainer::ImwContainer(ImwPlatformWindow* pParent)
	{
		IM_ASSERT(NULL != pParent);
		m_pSplits[0] = NULL;
		m_pSplits[1] = NULL;
		m_bVerticalSplit = false;
		m_iActiveWindow = 0;
		m_fSplitRatio = 0.5f;
		m_bIsDrag = false;
		m_pParent = NULL;
		m_pParentWindow = pParent;
	}

	ImwContainer::~ImwContainer()
	{
		while ( m_lWindows.begin() != m_lWindows.end() )
		{
			ImwWindowManager::GetInstance()->RemoveWindow(*m_lWindows.begin());
			delete *m_lWindows.begin();
			m_lWindows.erase(m_lWindows.begin());
		}

		ImwSafeDelete(m_pSplits[0]);
		ImwSafeDelete(m_pSplits[1]);
	}

	void ImwContainer::CreateSplits()
	{
		m_pSplits[0] = new ImwContainer(this);
		m_pSplits[1] = new ImwContainer(this);
	}

	void ImwContainer::Dock(ImwWindow* pWindow, EDockOrientation eOrientation, float fRatio, int iPosition)
	{
		IM_ASSERT(NULL != pWindow);
		IM_ASSERT(fRatio > 0.f && fRatio < 1.f);

		if ( NULL != pWindow )
		{
			IM_ASSERT(eOrientation != E_DOCK_ORIENTATION_CENTER || !IsSplit());

			if ( !IsSplit() )
			{
				if (m_lWindows.begin() ==  m_lWindows.end())
				{
					eOrientation = E_DOCK_ORIENTATION_CENTER;
				}

				switch (eOrientation)
				{
				case E_DOCK_ORIENTATION_CENTER:
					{
						if (iPosition < 0 || iPosition >= (int)m_lWindows.size())
						{
							m_lWindows.push_back(pWindow);
							m_iActiveWindow = (int)m_lWindows.size() - 1;
						}
						else
						{
							ImwWindowList::iterator itWindow = m_lWindows.begin();
							std::advance(itWindow, iPosition);
							m_lWindows.insert(itWindow, pWindow);
						}
						
					}
					break;
				case E_DOCK_ORIENTATION_TOP:
					{
						m_bVerticalSplit = true;
						CreateSplits();
						m_pSplits[0]->Dock(pWindow);
						m_pSplits[1]->m_lWindows.insert(m_pSplits[1]->m_lWindows.begin(), m_lWindows.begin(), m_lWindows.end());
						m_fSplitRatio = fRatio;
						m_lWindows.clear();
						m_iActiveWindow = 0;
					}
					break;
				case E_DOCK_ORIENTATION_LEFT:
					{
						m_bVerticalSplit = false;
						CreateSplits();
						m_pSplits[0]->Dock(pWindow);
						m_pSplits[1]->m_lWindows.insert(m_pSplits[1]->m_lWindows.begin(), m_lWindows.begin(), m_lWindows.end());
						m_fSplitRatio = fRatio;
						m_lWindows.clear();
						m_iActiveWindow = 0;
					}
					break;
				case E_DOCK_ORIENTATION_RIGHT:
					{
						m_bVerticalSplit = false;
						CreateSplits();
						m_pSplits[0]->m_lWindows.insert(m_pSplits[0]->m_lWindows.begin(), m_lWindows.begin(), m_lWindows.end());
						m_pSplits[1]->Dock(pWindow);
						m_fSplitRatio = 1.f - fRatio;
						m_lWindows.clear();
						m_iActiveWindow = 0;
					}
					break;
				case E_DOCK_ORIENTATION_BOTTOM:
					{
						m_bVerticalSplit = true;
						CreateSplits();
						m_pSplits[0]->m_lWindows.insert(m_pSplits[0]->m_lWindows.begin(), m_lWindows.begin(), m_lWindows.end());
						m_pSplits[1]->Dock(pWindow);
						m_fSplitRatio = 1.f - fRatio;
						m_lWindows.clear();
						m_iActiveWindow = 0;
					}
					break;
				}
			}
			else
			{
				switch (eOrientation)
				{
				case E_DOCK_ORIENTATION_CENTER:
					IM_ASSERT(false);
					break;
				case E_DOCK_ORIENTATION_TOP:
					{
						ImwContainer* pSplit0 = m_pSplits[0];
						ImwContainer* pSplit1 = m_pSplits[1];
						CreateSplits();
						m_pSplits[0]->m_lWindows.push_back(pWindow);
						m_pSplits[1]->m_bVerticalSplit = m_bVerticalSplit;
						m_pSplits[1]->m_fSplitRatio = m_fSplitRatio;
						m_pSplits[1]->m_pSplits[0] = pSplit0;
						m_pSplits[1]->m_pSplits[1] = pSplit1;
						m_pSplits[1]->m_pSplits[0]->m_pParent = m_pSplits[1];
						m_pSplits[1]->m_pSplits[1]->m_pParent = m_pSplits[1];
						m_fSplitRatio = fRatio;
						m_bVerticalSplit = true;
					}
					break;
				case E_DOCK_ORIENTATION_LEFT:
					{
						ImwContainer* pSplit0 = m_pSplits[0];
						ImwContainer* pSplit1 = m_pSplits[1];
						CreateSplits();
						m_pSplits[0]->m_lWindows.push_back(pWindow);
						m_pSplits[1]->m_bVerticalSplit = m_bVerticalSplit;
						m_pSplits[1]->m_fSplitRatio = m_fSplitRatio;
						m_pSplits[1]->m_pSplits[0] = pSplit0;
						m_pSplits[1]->m_pSplits[1] = pSplit1;
						m_pSplits[1]->m_pSplits[0]->m_pParent = m_pSplits[1];
						m_pSplits[1]->m_pSplits[1]->m_pParent = m_pSplits[1];
						m_fSplitRatio = fRatio;
						m_bVerticalSplit = false;
					}
					break;
				case E_DOCK_ORIENTATION_RIGHT:
					{
						ImwContainer* pSplit0 = m_pSplits[0];
						ImwContainer* pSplit1 = m_pSplits[1];
						CreateSplits();
						m_pSplits[1]->m_lWindows.push_back(pWindow);
						m_pSplits[0]->m_bVerticalSplit = m_bVerticalSplit;
						m_pSplits[0]->m_fSplitRatio = m_fSplitRatio;
						m_pSplits[0]->m_pSplits[0] = pSplit0;
						m_pSplits[0]->m_pSplits[1] = pSplit1;
						m_pSplits[0]->m_pSplits[0]->m_pParent = m_pSplits[0];
						m_pSplits[0]->m_pSplits[1]->m_pParent = m_pSplits[0];
						m_fSplitRatio = 1.f - fRatio;
						m_bVerticalSplit = false;
					}
					break;
				case E_DOCK_ORIENTATION_BOTTOM:
					{
						ImwContainer* pSplit0 = m_pSplits[0];
						ImwContainer* pSplit1 = m_pSplits[1];
						CreateSplits();
						m_pSplits[1]->m_lWindows.push_back(pWindow);
						m_pSplits[0]->m_bVerticalSplit = m_bVerticalSplit;
						m_pSplits[0]->m_fSplitRatio = m_fSplitRatio;
						m_pSplits[0]->m_pSplits[0] = pSplit0;
						m_pSplits[0]->m_pSplits[1] = pSplit1;
						m_pSplits[0]->m_pSplits[0]->m_pParent = m_pSplits[0];
						m_pSplits[0]->m_pSplits[1]->m_pParent = m_pSplits[0];
						m_fSplitRatio = 1.f - fRatio;
						m_bVerticalSplit = true;
					}
					break;
				}
			}
		}
	}

	bool ImwContainer::UnDock(ImwWindow* pWindow)
	{
		if (std::find(m_lWindows.begin(), m_lWindows.end(), pWindow) != m_lWindows.end())
		{
			m_lWindows.remove(pWindow);
			if (m_iActiveWindow >= (int)m_lWindows.size())
			{
				m_iActiveWindow = (int)m_lWindows.size() - 1;
			}
			return true;
		}
		if (NULL != m_pSplits[0] && NULL != m_pSplits[1])
		{
			if ( m_pSplits[0]->UnDock(pWindow) )
			{
				if (m_pSplits[0]->IsEmpty())
				{
					if (m_pSplits[1]->IsSplit())
					{
						ImwContainer* pSplit = m_pSplits[1];
						m_bVerticalSplit = pSplit->m_bVerticalSplit;
						ImwSafeDelete(m_pSplits[0]);
						m_pSplits[0] = pSplit->m_pSplits[0];
						m_pSplits[1] = pSplit->m_pSplits[1];
						m_fSplitRatio = pSplit->m_fSplitRatio;
						pSplit->m_pSplits[0] = NULL;
						pSplit->m_pSplits[1] = NULL;
						m_pSplits[0]->m_pParent = this;
						m_pSplits[1]->m_pParent = this;
						ImwSafeDelete(pSplit);
					}
					else
					{
						m_lWindows.insert(m_lWindows.end(), m_pSplits[1]->m_lWindows.begin(), m_pSplits[1]->m_lWindows.end());
						m_pSplits[1]->m_lWindows.clear();
						m_pSplits[1]->m_iActiveWindow = 0;
						ImwSafeDelete(m_pSplits[0]);
						ImwSafeDelete(m_pSplits[1]);
					}
				}
				return true;
			}

			if (m_pSplits[1]->UnDock(pWindow))
			{
				if (m_pSplits[1]->IsEmpty())
				{
					if (m_pSplits[0]->IsSplit())
					{
						ImwContainer* pSplit = m_pSplits[0];
						m_bVerticalSplit = pSplit->m_bVerticalSplit;
						ImwSafeDelete(m_pSplits[1]);
						m_pSplits[0] = pSplit->m_pSplits[0];
						m_pSplits[1] = pSplit->m_pSplits[1];
						m_fSplitRatio = pSplit->m_fSplitRatio;
						pSplit->m_pSplits[0] = NULL;
						pSplit->m_pSplits[1] = NULL;
						m_pSplits[0]->m_pParent = this;
						m_pSplits[1]->m_pParent = this;
						ImwSafeDelete(pSplit);
					}
					else
					{
						m_lWindows.insert(m_lWindows.end(), m_pSplits[0]->m_lWindows.begin(), m_pSplits[0]->m_lWindows.end());
						m_pSplits[0]->m_lWindows.clear();
						m_pSplits[0]->m_iActiveWindow = 0;
						ImwSafeDelete(m_pSplits[0]);
						ImwSafeDelete(m_pSplits[1]);
					}
				}
				return true;
			}
		}

		return false;
	}

	void ImwContainer::DockToBest(ImwWindow* pWindow)
	{
		if (IsSplit())
		{
			if (m_fSplitRatio < 0.5f)
			{
				m_pSplits[0]->DockToBest(pWindow);
			}
			else
			{
				m_pSplits[1]->DockToBest(pWindow);
			}
		}
		else
		{
			Dock(pWindow);
		}
	}

	bool ImwContainer::IsEmpty() const
	{
		//IM_ASSERT(IsSplit() != HasWindowTabbed());
		return !(IsSplit() || HasWindowTabbed());
	}

	bool ImwContainer::IsSplit() const
	{
		IM_ASSERT((NULL == m_pSplits[0]) == (NULL == m_pSplits[1]));
		return (NULL != m_pSplits[0] && NULL != m_pSplits[1]);
	}

	bool ImwContainer::HasWindowTabbed() const
	{
		return m_lWindows.size() > 0;
	}

	ImwContainer* ImwContainer::HasWindow( const ImwWindow* pWindow)
	{
		if (std::find(m_lWindows.begin(), m_lWindows.end(), pWindow) != m_lWindows.end())
		{
			return this;
		}
		else
		{
			if (NULL != m_pSplits[0])
			{
				ImwContainer* pContainer = m_pSplits[0]->HasWindow(pWindow);
				if (NULL != pContainer)
				{
					return pContainer;
				}
			}
			if (NULL != m_pSplits[1])
			{
				ImwContainer* pContainer = m_pSplits[1]->HasWindow(pWindow);
				if (NULL != pContainer)
				{
					return pContainer;
				}
			}
		}
		return NULL;
	}

	bool ImwContainer::FocusWindow(ImwWindow* pWindow)
	{
		ImwWindowList::iterator itFind = std::find(m_lWindows.begin(), m_lWindows.end(), pWindow);
		if ( itFind != m_lWindows.end())
		{
			m_iActiveWindow = (int)std::distance(m_lWindows.begin(), itFind);
			return true;
		}
		else
		{
			return (NULL != m_pSplits[0] && m_pSplits[0]->FocusWindow(pWindow)) || (NULL != m_pSplits[1] && m_pSplits[1]->FocusWindow(pWindow));
		}
	}



	ImwPlatformWindow* ImwContainer::GetPlatformWindowParent() const
	{
		return m_pParentWindow;
	}

	bool ImwContainer::BeginChildAlpha(const char* pStrId, const ImVec2& oSizeArg, float fAlpha, ImGuiWindowFlags eExtraFlags)
	{
		ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
		ImGuiWindowFlags eFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;

		const ImVec2 oContentAvailable = ImGui::GetContentRegionAvail();
		ImVec2 oSize = ImRound(oSizeArg);
		if (oSize.x <= 0.0f)
		{
			if (oSize.x == 0.0f)
				eFlags |= ImGuiWindowFlags_ChildWindowAutoFitX;
			oSize.x = ImMax(oContentAvailable.x, 4.0f) - fabsf(oSize.x); // Arbitrary minimum zero-ish child size of 4.0f (0.0f causing too much issues)
		}
		if (oSize.y <= 0.0f)
		{
			if (oSize.y == 0.0f)
				eFlags |= ImGuiWindowFlags_ChildWindowAutoFitY;
			oSize.y = ImMax(oContentAvailable.y, 4.0f) - fabsf(oSize.y);
		}
		
		eFlags |= eExtraFlags;

		char pTitle[256];
		ImFormatString(pTitle, IM_ARRAYSIZE(pTitle), "%s.%s", pWindow->Name, pStrId);

		bool ret = ImGui::Begin(pTitle, NULL, oSize, fAlpha, eFlags);

		if (!(pWindow->Flags & ImGuiWindowFlags_ShowBorders))
			ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

		return ret;
	}

	void ImwContainer::Paint(/* int iX, int iY, int iWidth, int iHeight */)
	{
		ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();
		ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
		ImGuiStyle& oStyle = ImGui::GetStyle();
		ImDrawList* pDrawList = ImGui::GetWindowDrawList();

		const ImVec2 oPos = ImGui::GetWindowPos();
		const ImVec2 oSize = ImGui::GetWindowSize();
		const ImVec2 oMin = ImVec2(oPos.x, oPos.y);
		const ImVec2 oMax = ImVec2(oPos.x +  oSize.x, oPos.y + oSize.y);
		const ImVec2 oCursorPos = ImGui::GetIO().MousePos;

		m_oLastPosition = oPos;
		m_oLastSize = oSize;

		const int iSeparatorHalfSize = 2;
		const int iSeparatorSize = iSeparatorHalfSize * 2;

		if (IsSplit())
		{
			if (m_bVerticalSplit)
			{
				float iFirstHeight = oSize.y * m_fSplitRatio - iSeparatorHalfSize - pWindow->WindowPadding.x;
			
				BeginChildAlpha("Split1", ImVec2(0, iFirstHeight), 0.f, ImGuiWindowFlags_NoScrollbar);
				m_pSplits[0]->Paint(/*iX, iY, iWidth, iFirstHeight*/);
				ImGui::EndChild();

				ImRect oSeparatorRect( 0, iFirstHeight, oSize.x, iFirstHeight + iSeparatorSize);
				if (pWindowManager->GetConfig().m_bVisibleDragger)
					ImGui::Button("##Dragger", oSeparatorRect.GetSize());
				else
					ImGui::InvisibleButton("##Dragger", oSeparatorRect.GetSize());

				if (ImGui::IsItemHovered() || m_bIsDrag)
				{
					 ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
				}
				if (ImGui::IsItemActive())
				{
					if (!m_bIsDrag)
					{
						m_bIsDrag = true;
					}
					m_fSplitRatio += ImGui::GetIO().MouseDelta.y / oSize.y;
					m_fSplitRatio = ImClamp( m_fSplitRatio, 0.05f, 0.95f );

				}
				else
				{
					m_bIsDrag = false;
				}

				BeginChildAlpha("Split2", ImVec2(0, 0), 0.f, ImGuiWindowFlags_NoScrollbar);
				m_pSplits[1]->Paint(/*iX, iY + iFirstHeight, iWidth, iSecondHeight*/);
				ImGui::EndChild();
			}
			else
			{
				float iFirstWidth = oSize.x * m_fSplitRatio - iSeparatorHalfSize - pWindow->WindowPadding.y;
				BeginChildAlpha("Split1", ImVec2(iFirstWidth, 0), 0.f, ImGuiWindowFlags_NoScrollbar);
				m_pSplits[0]->Paint();
				ImGui::EndChild();

				ImGui::SameLine();

				ImRect oSeparatorRect( iFirstWidth, 0, iFirstWidth + iSeparatorSize, oSize.y);
				if (pWindowManager->GetConfig().m_bVisibleDragger)
					ImGui::Button("##Dragger", oSeparatorRect.GetSize());
				else
					ImGui::InvisibleButton("##Dragger", oSeparatorRect.GetSize());

				if (ImGui::IsItemHovered() || m_bIsDrag)
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				}
				if (ImGui::IsItemActive())
				{
					if (!m_bIsDrag)
					{
						m_bIsDrag = true;
					}
				
					m_fSplitRatio += ImGui::GetIO().MouseDelta.x / oSize.x;
					m_fSplitRatio = ImClamp( m_fSplitRatio, 0.05f, 0.95f );
				}
				else
				{
					m_bIsDrag = false;
				}

				ImGui::SameLine();

				BeginChildAlpha("Split2", ImVec2(0, 0), 0.f, ImGuiWindowFlags_NoScrollbar);
				m_pSplits[1]->Paint();
				ImGui::EndChild();
			}
		}
		else if (HasWindowTabbed())
		{
			pWindowManager->PopStyle();

			bool bAlone = m_lWindows.front()->IsAlone();
			ImwWindow* pActiveWindow = NULL;
			if (!bAlone)
			{
				ImVec2 oItemSpacing = ImGui::GetStyle().ItemSpacing;
				ImGui::GetStyle().ItemSpacing = ImVec2(oItemSpacing.x, 0.f);

				ImGui::InvisibleButton("TabListButton", ImVec2(16, 16));
				ImGui::SameLine();

				if (ImGui::BeginPopupContextItem("TabListMenu", 0))
				{
					int iIndex = 0;
					for (ImwWindowList::const_iterator itWindow = m_lWindows.begin(); itWindow != m_lWindows.end(); ++itWindow, ++iIndex)
					{
						if (ImGui::Selectable((*itWindow)->GetTitle()))
						{
							m_iActiveWindow = iIndex;
						}
					}
					ImGui::EndPopup();
				}

				ImColor oLinesColor = ImColor(160, 160, 160, 255);
				if (ImGui::IsItemHovered())
				{
					oLinesColor = ImColor(255, 255, 255, 255);
				}
				ImVec2 oButtonMin = ImGui::GetItemRectMin();
				ImVec2 oButtonMax = ImGui::GetItemRectMax();
				ImVec2 oButtonSize = ImVec2(oButtonMax.x - oButtonMin.x, oButtonMax.y - oButtonMin.y);
				pDrawList->AddLine(
					ImVec2(oButtonMin.x + 1, oButtonMin.y + oButtonSize.y / 2),
					ImVec2(oButtonMax.x - 1, oButtonMin.y + oButtonSize.y / 2),
					oLinesColor);

				pDrawList->AddLine(
					ImVec2(oButtonMin.x + 1, oButtonMin.y + oButtonSize.y / 2 - 4),
					ImVec2(oButtonMax.x - 1, oButtonMin.y + oButtonSize.y / 2 - 4),
					oLinesColor);

				pDrawList->AddLine(
					ImVec2(oButtonMin.x + 1, oButtonMin.y + oButtonSize.y / 2 + 4),
					ImVec2(oButtonMax.x - 1, oButtonMin.y + oButtonSize.y / 2 + 4),
					oLinesColor);

				pDrawList->ChannelsSplit(2);

				//Tabs

				int iSize = (int)m_lWindows.size();
				float fMaxTabSize = GetTabAreaWidth() / iSize;
				ImwWindow* pDraggedWindow = ImwWindowManager::GetInstance()->GetDraggedWindow();
				float fDraggedTabWidth = 0.f;
				int iDraggedTabPosition = 0;
				if (pDraggedWindow != NULL)
				{
					if (ImwWindowManager::GetInstance()->GetDragBestContainer() == this &&
						ImwWindowManager::GetInstance()->GetDragOnTabArea())
					{
						iDraggedTabPosition = ImwWindowManager::GetInstance()->GetDragTabPosition();
						fMaxTabSize = GetTabAreaWidth() / (iSize + 1);
						//Tab(pDraggedWindow, true, oMin.x, oMax.x, fMaxTabSize);
						ImGuiWindow* window = ImGui::GetCurrentWindow();
						ImVec2 oTabSize;
						float fMin = window->DC.CursorPos.x;
						float fMax = fMin + m_oLastSize.x - fMaxTabSize;

						float fTabPosX = oCursorPos.x + ImwWindowManager::GetInstance()->GetDragOffset().x;

						if (fTabPosX < fMin) fTabPosX = fMin;
						if (fTabPosX > fMax) fTabPosX = fMax;

						ImVec2 oDraggedTabPos = ImVec2(fTabPosX, window->DC.CursorPos.y);
						DrawTab(pDraggedWindow->GetTitle(), true, oDraggedTabPos, oMin.x, oMax.x, fMaxTabSize, &oTabSize);
						fDraggedTabWidth = oTabSize.x;
					}
					else
					{
						pDraggedWindow = NULL;
					}
				}

				bool bCanCreateMultipleWindow = ImwWindowManager::GetInstance()->CanCreateMultipleWindow();

				int iIndex = 0;
				int iNewActive = -1;
				bool bFirstTab = true;
				ImVec2 oFirstTabPos;
				for (ImwWindowList::iterator it = m_lWindows.begin(); it != m_lWindows.end(); ++it)
				{
					if (pDraggedWindow != NULL && iDraggedTabPosition == iIndex)
					{
						ImGui::Dummy(ImVec2(fDraggedTabWidth, 1.f));
						ImGui::SameLine();
					}
					ImGui::PushID(iIndex);

					bool bSelected = iIndex == m_iActiveWindow && pDraggedWindow == NULL;
					if (Tab(*it, bSelected, oMin.x, oMax.x, fMaxTabSize))
					{
						iNewActive = iIndex;
					}

					if (bFirstTab)
					{
						bFirstTab = false;
						oFirstTabPos = ImGui::GetItemRectMin();
					}

					if (iIndex < (iSize - 1))
					{
						ImGui::SameLine();
					}

					if (ImGui::IsItemActive())
					{
						if (ImGui::IsMouseDragging())
						{
							//ImGui::GetIO().MouseClickedPos[0];
							float fOffsetX = (oCursorPos.x - ImGui::GetItemRectMin().x) + (oFirstTabPos.x - oPos.x);
							float fOffsetY = (oCursorPos.y - ImGui::GetItemRectMin().y);
							ImVec2 oOffset = ImVec2(-fOffsetX, -fOffsetY);
							pWindowManager->StartDragWindow(*it, oOffset);
						}
					}
					else if (ImGui::IsItemHovered() && (*it)->IsClosable() && ImGui::GetIO().MouseClicked[2])
					{
						(*it)->Destroy();
					}

					if (ImGui::BeginPopupContextItem("TabMenu"))
					{
						if ((*it)->IsClosable() && ImGui::Selectable("Close"))
						{
							(*it)->Destroy();
						}

						if (ImGui::BeginMenu("Dock to"))
						{
							int iIndex = 0;

							if (pWindowManager->GetMainPlatformWindow()->GetContainer()->IsEmpty())
							{
								ImGui::PushID(0);
								if (ImGui::Selectable("Main")) pWindowManager->Dock((*it));
								ImGui::PopID();
								++iIndex;
							}
							const ImwWindowList& lWindows = pWindowManager->GetWindowList();
							for (ImwWindowList::const_iterator itWindow = lWindows.begin(); itWindow != lWindows.end(); ++itWindow)
							{
								if ((*it) != (*itWindow))
								{
									ImGui::PushID(iIndex);
									if (ImGui::BeginMenu((*itWindow)->GetTitle()))
									{
										bool bHovered = false;
										ImwPlatformWindow* pPlatformWindow = pWindowManager->GetWindowParent((*itWindow));

										ImVec2 oLastWinPos = (*itWindow)->GetLastPosition();
										ImVec2 oLastWinSize = (*itWindow)->GetLastSize();

										if (!(*itWindow)->IsAlone())
										{
											ImGui::PushID(0);
											if (ImGui::Selectable("Tab")) pWindowManager->DockWith((*it), (*itWindow), E_DOCK_ORIENTATION_CENTER);
											if (ImGui::IsItemHovered() && NULL != pPlatformWindow)
											{
												bHovered = true;
												pWindowManager->DrawWindowArea(pPlatformWindow, oLastWinPos, oLastWinSize, ImColor(0.f, 0.5f, 1.f, 0.5f));
											}
											ImGui::PopID();
										}

										ImGui::PushID(1);
										if (ImGui::Selectable("Top")) pWindowManager->DockWith((*it), (*itWindow), E_DOCK_ORIENTATION_TOP);
										if (ImGui::IsItemHovered() && NULL != pPlatformWindow)
										{
											bHovered = true;
											pWindowManager->DrawWindowArea(pPlatformWindow, oLastWinPos, ImVec2(oLastWinSize.x, oLastWinSize.y / 2.f), ImColor(0.f, 0.5f, 1.f, 0.5f));
										}
										ImGui::PopID();

										ImGui::PushID(2);
										if (ImGui::Selectable("Left")) pWindowManager->DockWith((*it), (*itWindow), E_DOCK_ORIENTATION_LEFT);
										if (ImGui::IsItemHovered() && NULL != pPlatformWindow)
										{
											bHovered = true;
											pWindowManager->DrawWindowArea(pPlatformWindow, oLastWinPos, ImVec2(oLastWinSize.x / 2.f, oLastWinSize.y), ImColor(0.f, 0.5f, 1.f, 0.5f));
										}
										ImGui::PopID();

										ImGui::PushID(3);
										if (ImGui::Selectable("Right")) pWindowManager->DockWith((*it), (*itWindow), E_DOCK_ORIENTATION_RIGHT);
										if (ImGui::IsItemHovered() && NULL != pPlatformWindow)
										{
											bHovered = true;
											pWindowManager->DrawWindowArea(pPlatformWindow, ImVec2(oLastWinPos.x + oLastWinSize.x / 2.f, oLastWinPos.y), ImVec2(oLastWinSize.x / 2.f, oLastWinSize.y), ImColor(0.f, 0.5f, 1.f, 0.5f));
										}
										ImGui::PopID();

										ImGui::PushID(4);
										if (ImGui::Selectable("Bottom")) pWindowManager->DockWith((*it), (*itWindow), E_DOCK_ORIENTATION_BOTTOM);
										if (ImGui::IsItemHovered() && NULL != pPlatformWindow)
										{
											bHovered = true;
											pWindowManager->DrawWindowArea(pPlatformWindow, ImVec2(oLastWinPos.x, oLastWinPos.y + oLastWinSize.y / 2.f), ImVec2(oLastWinSize.x, oLastWinSize.y / 2.f), ImColor(0.f, 0.5f, 1.f, 0.5f));
										}
										ImGui::PopID();

										if (!bHovered)
										{
											if (NULL != pPlatformWindow)
											{
												pWindowManager->DrawWindowArea(pPlatformWindow, oLastWinPos, oLastWinSize, ImColor(0.f, 0.5f, 1.f, 0.5f));
											}
										}

										ImGui::EndMenu();
									}
									ImGui::PopID();
								}
								++iIndex;
							}

							ImGui::EndMenu();
						}

						if (bCanCreateMultipleWindow && ImGui::Selectable("Float"))
						{
							pWindowManager->Float((*it), ImVec2(-1.f, -1.f), (*it)->m_oLastSize);
						}

						(*it)->OnContextMenu();

						ImGui::EndPopup();
					}

					ImGui::PopID();

					++iIndex;
				}

				if (iNewActive >= 0)
					m_iActiveWindow = iNewActive;

				pDrawList->ChannelsMerge();

				ImGui::GetStyle().ItemSpacing = oItemSpacing;

				pActiveWindow = pDraggedWindow;
			}

			if (pActiveWindow == NULL)
			{
				ImwWindowList::iterator itActiveWindow = m_lWindows.begin();
				std::advance(itActiveWindow, m_iActiveWindow);

				IM_ASSERT(itActiveWindow != m_lWindows.end());
				pActiveWindow = *itActiveWindow;
			}

			//Draw active
			if (pActiveWindow != NULL)
			{
				ImVec4 oBorderColor = oStyle.Colors[ImGuiCol_Border];
				ImVec4 oBorderShadowColor = oStyle.Colors[ImGuiCol_BorderShadow];
				oStyle.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
				oStyle.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
				ImGui::BeginChild(pActiveWindow->GetId(), ImVec2(0.f, 0.f), true, ImGuiWindowFlags_HorizontalScrollbar);

				oStyle.Colors[ImGuiCol_Border] = oBorderColor;
				oStyle.Colors[ImGuiCol_BorderShadow] = oBorderShadowColor;

				if (!bAlone)
					ImGui::Dummy(ImVec2(0.f, 2.f)); //2 pixels space;

				ImVec2 oWinPos = ImGui::GetWindowPos();
				ImVec2 oWinSize = ImGui::GetWindowSize();

				for (ImwWindowList::iterator it = m_lWindows.begin(); it != m_lWindows.end(); ++it)
				{
					(*it)->m_oLastPosition = oWinPos;
					(*it)->m_oLastSize = oWinSize;
				}
				
				pActiveWindow->OnGui();

				ImGui::EndChild();
			}

			pWindowManager->PushStyle();
		}
		else
		{
			// This case can happened only where it's main container
			IM_ASSERT(m_pParent == NULL);
		}
	}

	bool ImwContainer::Tab(const ImwWindow* pWindow, bool bFocused, float fStartLinePos, float fEndLinePos, float fMaxSize)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImVec2 oTabSize;
		DrawTab(pWindow->GetTitle(), bFocused, window->DC.CursorPos, fStartLinePos, fEndLinePos, fMaxSize, &oTabSize);

		return ImGui::InvisibleButton(pWindow->GetIdStr(), oTabSize);
	}

	void ImwContainer::DrawTab(const ImwChar* pText, bool bFocused, ImVec2 oPos, float fStartLinePos, float fEndLinePos, float fMaxSize, ImVec2* pSizeOut)
	{
		const ImwWindowManager::Config& oConfig = ImwWindowManager::GetInstance()->GetConfig();
		ImDrawList* pDrawList = ImGui::GetWindowDrawList();
		const ImGuiStyle& oStyle = ImGui::GetStyle();

		//Calculate text size
		ImVec2 oTextSize;
		float fTabWidth = GetTabWidth(pText, fMaxSize, &oTextSize);

		//Calculate tab size
		ImVec2 oTabSize(fTabWidth, c_fTabHeight);
		if (pSizeOut != NULL)
		{
			*pSizeOut = oTabSize;
		}

		ImColor oNormalTab(0), oSelectedTab(0), oBorderColor(0);
		switch (oConfig.m_eTabColorMode)
		{
			case ImwWindowManager::E_TABCOLORMODE_TITLE:
				oNormalTab = ImGui::GetStyle().Colors[ImGuiCol_TitleBg];
				oNormalTab.Value.w = 1.f;
				oSelectedTab = ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive];
				oSelectedTab.Value.w = 1.f;
				oBorderColor = ImGui::GetStyle().Colors[ImGuiCol_Border];
				oBorderColor.Value.w = 1.f;
				break;
			case ImwWindowManager::E_TABCOLORMODE_BACKGROUND:
				oNormalTab = ImColor(
					oStyle.Colors[ImGuiCol_WindowBg].x + (oStyle.Colors[ImGuiCol_ChildWindowBg].x - oStyle.Colors[ImGuiCol_WindowBg].x) * 0.5f,
					oStyle.Colors[ImGuiCol_WindowBg].y + (oStyle.Colors[ImGuiCol_ChildWindowBg].y - oStyle.Colors[ImGuiCol_WindowBg].y) * 0.5f,
					oStyle.Colors[ImGuiCol_WindowBg].z + (oStyle.Colors[ImGuiCol_ChildWindowBg].z - oStyle.Colors[ImGuiCol_WindowBg].z) * 0.5f,
					oStyle.Colors[ImGuiCol_WindowBg].w + (oStyle.Colors[ImGuiCol_ChildWindowBg].w - oStyle.Colors[ImGuiCol_WindowBg].w) * 0.5f
				);;
				oSelectedTab = oStyle.Colors[ImGuiCol_ChildWindowBg];
				oBorderColor = oStyle.Colors[ImGuiCol_Border];
				break;
			case ImwWindowManager::E_TABCOLORMODE_CUSTOM:
				oNormalTab = oConfig.m_oTabColorNormal;
				oSelectedTab = oConfig.m_oTabColorActive;
				oBorderColor = oConfig.m_oTabColorBorder;
				break;
		}
		
		ImVec2 oRectMin = oPos;
		ImVec2 oRectMax = ImVec2(oPos.x + oTabSize.x, oPos.y + oTabSize.y);

		pDrawList->PathClear();
		if (bFocused)
		{
			pDrawList->ChannelsSetCurrent(1);
		}
		else
		{
			pDrawList->ChannelsSetCurrent(0);
		}

		//Drop shadows
		if (oConfig.m_bShowTabShadows)
		{
			const ImVec2 uv = GImGui->FontTexUvWhitePixel;
			pDrawList->PrimReserve(3, 3);
			pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx)); pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 1)); pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 2));
			pDrawList->PrimWriteVtx(ImVec2(oRectMin.x - oConfig.m_fTabOverlap - oConfig.m_fTabShadowDropSize, oRectMax.y), uv, ImColor(0.f, 0.f, 0.f, 0.f));
			pDrawList->PrimWriteVtx(ImVec2(oRectMin.x - oConfig.m_fTabOverlap + oConfig.m_fTabSlopWidth * oConfig.m_fTabShadowSlopRatio, oRectMin.y), uv, ImColor(0.f, 0.f, 0.f, 0.f));
			pDrawList->PrimWriteVtx(ImVec2(oRectMin.x - oConfig.m_fTabOverlap + oConfig.m_fTabSlopWidth * oConfig.m_fTabShadowSlopRatio, oRectMax.y), uv, ImColor(0.f, 0.f, 0.f, oConfig.m_fTabShadowAlpha));
			if (bFocused)
			{
				pDrawList->PrimReserve(3, 3);
				pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx)); pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 1)); pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 2));
				pDrawList->PrimWriteVtx(ImVec2(oRectMax.x + oConfig.m_fTabOverlap + oConfig.m_fTabShadowDropSize, oRectMax.y), uv, ImColor(0.f, 0.f, 0.f, 0.f));
				pDrawList->PrimWriteVtx(ImVec2(oRectMax.x + oConfig.m_fTabOverlap - oConfig.m_fTabSlopWidth * oConfig.m_fTabShadowSlopRatio, oRectMin.y), uv, ImColor(0.f, 0.f, 0.f, 0.f));
				pDrawList->PrimWriteVtx(ImVec2(oRectMax.x + oConfig.m_fTabOverlap - oConfig.m_fTabSlopWidth * oConfig.m_fTabShadowSlopRatio, oRectMax.y), uv, ImColor(0.f, 0.f, 0.f, oConfig.m_fTabShadowAlpha));
			}
		}

		// Draw tab and border
		if (bFocused && fStartLinePos < oPos.x)
		{
			pDrawList->PathLineTo(ImVec2(fStartLinePos, oRectMax.y));
		}
		pDrawList->PathLineTo(ImVec2(oRectMin.x - oConfig.m_fTabOverlap, oRectMax.y));
		pDrawList->PathBezierCurveTo(
			ImVec2(oRectMin.x + oConfig.m_fTabSlopWidth * oConfig.m_fTabSlopP1Ratio - oConfig.m_fTabOverlap, oRectMin.y + (oRectMax.y - oRectMin.y) * oConfig.m_fTabSlopHRatio),
			ImVec2(oRectMin.x + oConfig.m_fTabSlopWidth * oConfig.m_fTabSlopP2Ratio - oConfig.m_fTabOverlap, oRectMin.y),
			ImVec2(oRectMin.x + oConfig.m_fTabSlopWidth - oConfig.m_fTabOverlap, oRectMin.y)
			);
		pDrawList->PathLineTo(ImVec2(oRectMax.x - oConfig.m_fTabSlopWidth + oConfig.m_fTabOverlap, oRectMin.y));
		pDrawList->PathBezierCurveTo(
			ImVec2(oRectMax.x - oConfig.m_fTabSlopWidth * oConfig.m_fTabSlopP2Ratio + oConfig.m_fTabOverlap, oRectMin.y),
			ImVec2(oRectMax.x - oConfig.m_fTabSlopWidth * oConfig.m_fTabSlopP1Ratio + oConfig.m_fTabOverlap, oRectMin.y + (oRectMax.y - oRectMin.y) * oConfig.m_fTabSlopHRatio),
			ImVec2(oRectMax.x + oConfig.m_fTabOverlap, oRectMax.y)
			);

		if (bFocused)
		{
			pDrawList->AddConvexPolyFilled(pDrawList->_Path.Data + 1, pDrawList->_Path.Size - 1, bFocused ? oSelectedTab : oNormalTab, true);
			if (fEndLinePos > oRectMax.x)
			{
				pDrawList->PathLineTo(ImVec2(fEndLinePos, oRectMax.y));
			}

			if (oConfig.m_bShowTabBorder)
				pDrawList->AddPolyline(pDrawList->_Path.Data, pDrawList->_Path.Size, oBorderColor, false, 1.5f, true);
		}
		else
		{
			pDrawList->AddConvexPolyFilled(pDrawList->_Path.Data, pDrawList->_Path.Size, bFocused ? oSelectedTab : oNormalTab, true);
		}

		pDrawList->PathClear();

		ImVec2 oTextRectMin(oRectMin.x + 5, oRectMin.y);
		ImVec2 oTextRectMax(oRectMax.x - 5, oRectMax.y);
		ImGui::RenderTextClipped(oTextRectMin, oTextRectMax, pText, NULL, &oTextSize, ImGuiAlign_Center | ImGuiAlign_VCenter);
	}

	float ImwContainer::GetTabWidth(const ImwChar* pText, float fMaxSize, ImVec2* pOutTextSize)
	{
		const ImVec2 oTextSize = ImGui::CalcTextSize(pText);

		if (NULL != pOutTextSize)
		{
			*pOutTextSize = oTextSize;
		}

		//Clamp fMaxSize to a minimum for avoid glitch
		if (fMaxSize < 30.f)
		{
			fMaxSize = 30.f;
		}

		//Calculate tab size
		float fWidth = oTextSize.x + 15;
		if (fMaxSize != 1.f && fWidth > fMaxSize)
		{
			fWidth = fMaxSize;
		}
		
		return fWidth;
	}

	float ImwContainer::GetTabAreaWidth() const
	{
		return m_oLastSize.x - 50.f;
	}

	ImwContainer* ImwContainer::GetBestDocking(const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, bool& bOutOnTabArea, int& iOutPosition, bool bLargeCheck)
	{
		if (m_pParent == NULL || 
			(oCursorPos.x >= m_oLastPosition.x && oCursorPos.x <= (m_oLastPosition.x + m_oLastSize.x) &&
			oCursorPos.y >= m_oLastPosition.y && oCursorPos.y <= (m_oLastPosition.y + m_oLastSize.y)))
		{
			if (IsSplit())
			{
				ImwContainer* pBestContainer = NULL;
				pBestContainer = m_pSplits[0]->GetBestDocking(oCursorPos, oOutOrientation, oOutAreaPos, oOutAreaSize, bOutOnTabArea, iOutPosition, bLargeCheck);
				if (NULL != pBestContainer)
				{
					return pBestContainer;
				}
				pBestContainer = m_pSplits[1]->GetBestDocking(oCursorPos, oOutOrientation, oOutAreaPos, oOutAreaSize, bOutOnTabArea, iOutPosition, bLargeCheck);
				if (NULL != pBestContainer)
				{
					return pBestContainer;
				}
			}
			else
			{
				if (oCursorPos.y < m_oLastPosition.y + c_fTabHeight)
				{
					oOutOrientation = E_DOCK_ORIENTATION_CENTER;
					oOutAreaPos = m_oLastPosition;
					oOutAreaSize = ImVec2(0,0);
					bOutOnTabArea = true;
					//Search tab position
					float fMaxTabSize = GetTabAreaWidth() / (m_lWindows.size() + 1);
					float fCurrentTabPosX = m_oLastPosition.x;
					int iCurrentTab = 0;
					float fCursorX = oCursorPos.x + ImwWindowManager::GetInstance()->GetDragOffset().x;
					for (ImwWindowList::const_iterator itWindow = m_lWindows.begin(); itWindow != m_lWindows.end(); ++itWindow)
					{
						float fTabWidth = GetTabWidth((*itWindow)->m_pTitle, fMaxTabSize);
						if (fCursorX < (fCurrentTabPosX + fTabWidth / 2.f))
						{
							break;
						}
						fCurrentTabPosX += fTabWidth;
						++iCurrentTab;
					}
					iOutPosition = iCurrentTab;
					return this;
				}
				else
				{
					const float c_fBoxHalfSize = 20.f;
					const float c_fBoxSize = c_fBoxHalfSize * 2.f;
					const float c_fMinSize = c_fBoxSize * 4.f;
					const float c_fSplitRatio = 0.5f;
					//const float c_fSplitRatio = oConfig.m_fDragMarginSizeRatio;
					const ImColor oBoxColor(200, 200, 255, 255);
					const ImColor oBoxHightlightColor(100, 100, 255, 255);

					if (m_oLastSize.x < c_fMinSize && m_oLastSize.y < c_fMinSize)
					{
						oOutOrientation = E_DOCK_ORIENTATION_CENTER;
						oOutAreaPos = m_oLastPosition;
						oOutAreaSize = m_oLastSize;
						return this;
					}
					else
					{
						const ImVec2 oCenter = ImVec2(m_oLastPosition.x + m_oLastSize.x / 2.f, m_oLastPosition.y + m_oLastSize.y / 2.f);

						bool bIsInCenter = false;
						bool bIsInTop = false;
						bool bIsInLeft = false;
						bool bIsInRight = false;
						bool bIsInBottom = false;

						if (bLargeCheck)
						{
							ImRect oRectCenter(ImVec2(oCenter.x - c_fBoxHalfSize * 2.f, oCenter.y - c_fBoxHalfSize * 2.f), ImVec2(oCenter.x + c_fBoxHalfSize * 2.f, oCenter.y + c_fBoxHalfSize * 2.f));

							ImRect oRectTop = ImRect(ImVec2(m_oLastPosition.x, m_oLastPosition.y), ImVec2(m_oLastPosition.x + m_oLastSize.x, oCenter.y - c_fBoxHalfSize * 2.f));
							ImRect oRectBottom = ImRect(ImVec2(m_oLastPosition.x, oCenter.y + c_fBoxHalfSize * 2.f), ImVec2(m_oLastPosition.x + m_oLastSize.x, m_oLastPosition.y + m_oLastSize.y));

							ImRect oRectLeft = ImRect(ImVec2(m_oLastPosition.x, m_oLastPosition.y), ImVec2(oCenter.x - c_fBoxHalfSize * 2.f, m_oLastPosition.y + m_oLastSize.y));
							ImRect oRectRight = ImRect(ImVec2(oCenter.x + c_fBoxHalfSize * 2.f, m_oLastPosition.y), ImVec2(m_oLastPosition.x + m_oLastSize.x, m_oLastPosition.y + m_oLastSize.y));

							bIsInCenter = oRectCenter.Contains(oCursorPos);

							if (m_oLastSize.y >= c_fMinSize)
							{
								bIsInTop = oRectTop.Contains(oCursorPos);
								bIsInBottom = oRectBottom.Contains(oCursorPos);
							}

							if (m_oLastSize.x >= c_fMinSize)
							{
								bIsInLeft = oRectLeft.Contains(oCursorPos);
								bIsInRight = oRectRight.Contains(oCursorPos);
							}
						}
						else
						{
							if (!m_lWindows.front()->IsAlone())
							{
								//Center
								ImRect oRectCenter(ImVec2(oCenter.x - c_fBoxHalfSize, oCenter.y - c_fBoxHalfSize), ImVec2(oCenter.x + c_fBoxHalfSize, oCenter.y + c_fBoxHalfSize));
								bIsInCenter = oRectCenter.Contains(oCursorPos);
								ImwWindowManager::GetInstance()->DrawWindowArea(m_pParentWindow, oRectCenter.Min, oRectCenter.GetSize(), bIsInCenter ? oBoxHightlightColor : oBoxColor);
							}

							if (m_oLastSize.y >= c_fMinSize)
							{
								//Top
								ImRect oRectTop(ImVec2(oCenter.x - c_fBoxHalfSize, oCenter.y - c_fBoxHalfSize * 4.f), ImVec2(oCenter.x + c_fBoxHalfSize, oCenter.y - c_fBoxHalfSize * 2.f));
								bIsInTop = oRectTop.Contains(oCursorPos);
								ImwWindowManager::GetInstance()->DrawWindowArea(m_pParentWindow, oRectTop.Min, oRectTop.GetSize(), bIsInTop ? oBoxHightlightColor : oBoxColor);

								//Bottom
								ImRect oRectBottom(ImVec2(oCenter.x - c_fBoxHalfSize, oCenter.y + c_fBoxHalfSize * 2.f), ImVec2(oCenter.x + c_fBoxHalfSize, oCenter.y + c_fBoxHalfSize * 4.f));
								bIsInBottom = oRectBottom.Contains(oCursorPos);
								ImwWindowManager::GetInstance()->DrawWindowArea(m_pParentWindow, oRectBottom.Min, oRectBottom.GetSize(), bIsInBottom ? oBoxHightlightColor : oBoxColor);
							}

							if (m_oLastSize.x >= c_fMinSize)
							{
								//Left
								ImRect oRectLeft(ImVec2(oCenter.x - c_fBoxHalfSize * 4.f, oCenter.y - c_fBoxHalfSize), ImVec2(oCenter.x - c_fBoxHalfSize * 2.f, oCenter.y + c_fBoxHalfSize));
								bIsInLeft = oRectLeft.Contains(oCursorPos);
								ImwWindowManager::GetInstance()->DrawWindowArea(m_pParentWindow, oRectLeft.Min, oRectLeft.GetSize(), bIsInLeft ? oBoxHightlightColor : oBoxColor);

								//Right
								ImRect oRectRight(ImVec2(oCenter.x + c_fBoxHalfSize * 2.f, oCenter.y - c_fBoxHalfSize), ImVec2(oCenter.x + c_fBoxHalfSize * 4.f, oCenter.y + c_fBoxHalfSize));
								bIsInRight = oRectRight.Contains(oCursorPos);
								ImwWindowManager::GetInstance()->DrawWindowArea(m_pParentWindow, oRectRight.Min, oRectRight.GetSize(), bIsInRight ? oBoxHightlightColor : oBoxColor);
							}
						}

						if (bIsInCenter)
						{
							oOutOrientation = E_DOCK_ORIENTATION_CENTER;
							oOutAreaPos = m_oLastPosition;
							oOutAreaSize = m_oLastSize;
							bOutOnTabArea = false;
							return this;
						}
						else if (bIsInTop)
						{
							oOutOrientation = E_DOCK_ORIENTATION_TOP;
							oOutAreaPos = m_oLastPosition;
							oOutAreaSize = ImVec2(m_oLastSize.x, m_oLastSize.y * c_fSplitRatio);
							bOutOnTabArea = false;
							return this;
						}
						else if (bIsInLeft)
						{
							oOutOrientation = E_DOCK_ORIENTATION_LEFT;
							oOutAreaPos = m_oLastPosition;
							oOutAreaSize = ImVec2(m_oLastSize.x * c_fSplitRatio, m_oLastSize.y);
							bOutOnTabArea = false;
							return this;
						}
						else if (bIsInRight)
						{
							oOutOrientation = E_DOCK_ORIENTATION_RIGHT;
							oOutAreaPos = ImVec2(m_oLastPosition.x + m_oLastSize.x * (1.f - c_fSplitRatio), m_oLastPosition.y);
							oOutAreaSize = ImVec2(m_oLastSize.x * c_fSplitRatio, m_oLastSize.y);
							bOutOnTabArea = false;
							return this;
						}
						else if (bIsInBottom)
						{
							oOutOrientation = E_DOCK_ORIENTATION_BOTTOM;
							oOutAreaPos = ImVec2(m_oLastPosition.x, m_oLastPosition.y + m_oLastSize.y * (1.f - c_fSplitRatio));
							oOutAreaSize = ImVec2(m_oLastSize.x, m_oLastSize.y * c_fSplitRatio);
							bOutOnTabArea = false;
							return this;
						}
					}
				}
			}
		}

		return NULL;
	}

	bool ImwContainer::HasUnclosableWindow() const
	{
		for (ImwWindowList::const_iterator itWindow = m_lWindows.begin(); itWindow != m_lWindows.end(); ++itWindow)
		{
			if ( !(*itWindow)->IsClosable() )
			{
				return true;
			}
		}
		if (IsSplit())
		{
			return m_pSplits[0]->HasUnclosableWindow() || m_pSplits[1]->HasUnclosableWindow();
		}
		return false;
	}

	bool ImwContainer::Save(JsonValue& oJson)
	{
		oJson["Vertical"] = m_bVerticalSplit;
		oJson["SplitRatio"] = m_fSplitRatio;

		if (m_lWindows.size() > 0)
		{
			ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();
			oJson["CurrentWindow"] = (long)m_iActiveWindow;
			JsonValue& oJsonWindows = oJson["Windows"];
			int iCurrentWindow = 0;
			for (ImwWindowList::const_iterator itWindow = m_lWindows.begin(); itWindow != m_lWindows.end(); ++itWindow)
			{
				JsonValue& oJsonWindow = oJsonWindows[iCurrentWindow++];
				const ImwChar* pClassName = pWindowManager->GetWindowClassName(*itWindow);
				if (pClassName == NULL)
					return false;
				oJsonWindow["Class"] = pClassName;
				(*itWindow)->GetParameters(oJsonWindow["Parameters"]);
			}
			return true;
		}
		else
		{
			JsonValue& oJsonSplits = oJson["Splits"];
			return m_pSplits[0]->Save(oJsonSplits[0]) && m_pSplits[1]->Save(oJsonSplits[1]);
		}
	}

	bool ImwContainer::Load(const JsonValue& oJson, bool bJustCheck)
	{
		if (!oJson["Vertical"].IsBoolean() || !oJson["SplitRatio"].IsFloat())
			return false;

		if (!(oJson["Windows"].IsArray() || (oJson["Splits"].IsArray() && oJson["Splits"].GetMemberCount() == 2)))
			return false;

		if (!bJustCheck)
		{
			m_bVerticalSplit = oJson["Vertical"];
			m_fSplitRatio = (float)(double)oJson["SplitRatio"];

			//Clear
			while (m_lWindows.begin() != m_lWindows.end())
			{
				ImwWindowManager::GetInstance()->RemoveWindow(*m_lWindows.begin());
				delete *m_lWindows.begin();
				m_lWindows.erase(m_lWindows.begin());
			}

			ImwSafeDelete(m_pSplits[0]);
			ImwSafeDelete(m_pSplits[1]);
		}
		
		if (oJson["Splits"].IsArray())
		{
			if (!bJustCheck)
			{
				CreateSplits();
				if (!m_pSplits[0]->Load(oJson["Splits"][0], bJustCheck) || !m_pSplits[1]->Load(oJson["Splits"][1], bJustCheck))
					return false;
			}
			else
			{
				if (!Load(oJson["Splits"][0], bJustCheck) || !Load(oJson["Splits"][1], bJustCheck))
					return false;
			}
		}
		else
		{
			ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();

			const JsonValue& oJsonCurrentWindow = oJson["CurrentWindow"];
			const JsonValue& oJsonWindows = oJson["Windows"];
			int iWindowCount = oJsonWindows.GetMemberCount();
			//Check
			if (!(oJsonCurrentWindow.IsNull() || oJsonCurrentWindow.IsInteger()))
				return false;

			for (int iCurrent = 0; iCurrent < iWindowCount; ++iCurrent)
			{
				const JsonValue& oJsonWindow = oJsonWindows[iCurrent];
				if (!oJsonWindow.IsObject() || !oJsonWindow["Class"].IsString())
					return false;
				if (!pWindowManager->CanCreateWindowByClassName(oJsonWindow["Class"]))
					return false;
			}

			if (!bJustCheck)
			{
				for (int iCurrent = 0; iCurrent < iWindowCount; ++iCurrent)
				{
					const JsonValue& oJsonWindow = oJsonWindows[iCurrent];
					ImwWindow* pWindow = pWindowManager->CreateWindowByClassName(oJsonWindow["Class"]);
					pWindow->SetParameters(oJsonWindow["Parameters"]);
					m_lWindows.push_back(pWindow);
				}

				if (oJsonCurrentWindow.IsInteger())
				{
					m_iActiveWindow = (int)(long)oJsonCurrentWindow;
					if (m_iActiveWindow < 0 || m_iActiveWindow >= iWindowCount)
						m_iActiveWindow = 0;
				}
				else
				{
					m_iActiveWindow = 0;
				}
			}
		}

		return true;
	}

//SFF_END
}