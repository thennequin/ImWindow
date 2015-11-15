
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

	void ImwContainer::Dock(ImwWindow* pWindow, EDockOrientation eOrientation, float fRatio)
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
						m_lWindows.push_back(pWindow);
						m_iActiveWindow = (int)m_lWindows.size() - 1;
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
						//m_fSplitRatio = ImwWindowManager::GetInstance()->GetConfig().m_fDragMarginSizeRatio;
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
						//m_fSplitRatio = ImwWindowManager::GetInstance()->GetConfig().m_fDragMarginSizeRatio;
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
						//m_fSplitRatio = 1.f - ImwWindowManager::GetInstance()->GetConfig().m_fDragMarginSizeRatio;
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
						//m_fSplitRatio = 1.f - ImwWindowManager::GetInstance()->GetConfig().m_fDragMarginSizeRatio;
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

	ImwPlatformWindow* ImwContainer::GetPlatformWindowParent() const
	{
		return m_pParentWindow;
	}

	void ImwContainer::Paint(/* int iX, int iY, int iWidth, int iHeight */)
	{
		ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();
		ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
		const ImGuiStyle& oStyle = ImGui::GetStyle();
		ImDrawList* pDrawList = ImGui::GetWindowDrawList();

		const ImVec2 oPos = ImGui::GetWindowPos();
		const ImVec2 oSize = ImGui::GetWindowSize();
		const ImVec2 oMin = ImVec2(oPos.x + 1, oPos.y + 1);
		const ImVec2 oMax = ImVec2(oPos.x + oSize.x - 2, oPos.y + oSize.y - 2);

		m_oLastPosition = oPos;
		m_oLastSize = oSize;

		const int iSeparatorHalfSize = 2;
		const int iSeparatorSize = iSeparatorHalfSize * 2;

		if (IsSplit())
		{
			if (m_bVerticalSplit)
			{
				float iFirstHeight = oSize.y * m_fSplitRatio - iSeparatorHalfSize - pWindow->WindowPadding.x;
				//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
				//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
				/*ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0,0));
				ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0);*/
			

				ImGui::BeginChild("Split1", ImVec2(0, iFirstHeight), false, ImGuiWindowFlags_NoScrollbar);
				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,4));
				m_pSplits[0]->Paint(/*iX, iY, iWidth, iFirstHeight*/);
				//ImGui::PopStyleVar(1);
				ImGui::EndChild();


				ImRect oSeparatorRect( 0, iFirstHeight, oSize.x, iFirstHeight + iSeparatorSize);
				ImGui::Button("",oSeparatorRect.GetSize());
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

				ImGui::BeginChild("Split2", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,4));
				m_pSplits[1]->Paint(/*iX, iY + iFirstHeight, iWidth, iSecondHeight*/);
				//ImGui::PopStyleVar(1);
				ImGui::EndChild();

				//ImGui::PopStyleVar(1);
			}
			else
			{
				float iFirstWidth = oSize.x * m_fSplitRatio - iSeparatorHalfSize - pWindow->WindowPadding.y;
				ImGui::BeginChild("Split1", ImVec2(iFirstWidth, 0), false, ImGuiWindowFlags_NoScrollbar);
				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,4));
				m_pSplits[0]->Paint();
				//ImGui::PopStyleVar(1);
				ImGui::EndChild();

				ImGui::SameLine();

				ImRect oSeparatorRect( iFirstWidth, 0, iFirstWidth + iSeparatorSize, oSize.y);
				ImGui::Button("",oSeparatorRect.GetSize());
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

				ImGui::BeginChild("Split2", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,4));
				m_pSplits[1]->Paint();
				//ImGui::PopStyleVar(1);
				ImGui::EndChild();

				//ImGui::PopStyleVar(1);
			}
		}
		else if (HasWindowTabbed())
		{
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
			int iIndex = 0;
			int iNewActive = m_iActiveWindow;
			int iSize = (int)m_lWindows.size();
			float fMaxTabSize = (oSize.x - 50.f) / iSize;
			for (ImwWindowList::iterator it = m_lWindows.begin(); it != m_lWindows.end(); ++it)
			{
				ImGui::PushID(iIndex);

				bool bSelected = iIndex == m_iActiveWindow;
				if (Tab(*it, bSelected, oMin.x, oMax.x, fMaxTabSize))
				{
					iNewActive = iIndex;
				}

				if (iIndex < (iSize - 1))
				{
					ImGui::SameLine();
				}

				if (ImGui::IsItemActive())
				{
					if (ImGui::IsMouseDragging())
					{
						ImVec2 oOffset = ImVec2(oPos.x - ImGui::GetIO().MousePos.x, oPos.y - ImGui::GetIO().MousePos.y);
						pWindowManager->StartDragWindow(*it, oOffset);
					}
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
								
									ImGui::PushID(0);
									if (ImGui::Selectable("Tab")) pWindowManager->DockWith((*it), (*itWindow), E_DOCK_ORIENTATION_CENTER);
									if (ImGui::IsItemHovered() && NULL != pPlatformWindow)
									{
										bHovered = true;
										pWindowManager->DrawWindowArea(pPlatformWindow, oLastWinPos, oLastWinSize, ImColor(0.f, 0.5f, 1.f, 0.5f));
									}
									ImGui::PopID();

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
										if ( NULL != pPlatformWindow )
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
					if (ImGui::Selectable("Float")) pWindowManager->Float((*it));
					ImGui::EndPopup();
				}

				ImGui::PopID();

				++iIndex;
			}
			m_iActiveWindow = iNewActive;
			pDrawList->ChannelsMerge();


			ImwWindowList::iterator itActiveWindow = m_lWindows.begin();
			std::advance(itActiveWindow, m_iActiveWindow);
		
			//Draw active
			IM_ASSERT(itActiveWindow != m_lWindows.end());
			if (itActiveWindow != m_lWindows.end())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, oStyle.WindowPadding);
				//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(59, 59, 59, 255));
				ImGui::BeginChild((*itActiveWindow)->GetId(), ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
			

				ImVec2 oWinPos = ImGui::GetWindowPos();
				ImVec2 oWinSize = ImGui::GetWindowSize();

				for (ImwWindowList::iterator it = m_lWindows.begin(); it != m_lWindows.end(); ++it)
				{
					(*it)->m_oLastPosition = oWinPos;
					(*it)->m_oLastSize = oWinSize;
				}
				(*itActiveWindow)->OnGui();
			
				ImGui::EndChild();
				//ImGui::PopStyleColor(1);
				ImGui::PopStyleVar(1);
			}
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

		//ImVec2 oRectMin = ImGui::GetItemBoxMin();
		//ImVec2 oRectMax = ImGui::GetItemBoxMax();

		ImVec2 oTabSize;
		DrawTab(pWindow->GetTitle(), bFocused, window->DC.CursorPos, fStartLinePos, fEndLinePos, fMaxSize, &oTabSize);

		return ImGui::InvisibleButton(pWindow->GetIdStr(), oTabSize);
	}

	//bool ImwContainer::DrawTab(const ImwWindow* pWindow, bool bFocused, ImVec2 oPos, float fMaxSize, ImVec2* pSizeOut)
	void ImwContainer::DrawTab(const char* pText, bool bFocused, ImVec2 oPos, float fStartLinePos, float fEndLinePos, float fMaxSize, ImVec2* pSizeOut)
	{
		ImDrawList* pDrawList = ImGui::GetWindowDrawList();

		//Calculate text size
		const ImVec2 oTextSize = ImGui::CalcTextSize(pText);

		//Clamp fMaxSize to a minimum for avoid glitch
		if (fMaxSize < 30.f)
		{
			fMaxSize = 30.f;
		}

		//Calculate tab size
		ImVec2 oTabSize(oTextSize.x + 15, c_fTabHeight);
		if (fMaxSize != 1.f && oTabSize.x > fMaxSize)
		{
			oTabSize.x = fMaxSize;
		}

		if (pSizeOut != NULL)
		{
			*pSizeOut = oTabSize;
		}

		ImColor oNormalTab(50, 50, 50, 255); // normal
		ImColor oSelectedTab(37, 37, 37, 255); // selected
		ImColor oBorderColor(72, 72, 72, 255); // border

		ImVec2 oRectMin = oPos;
		ImVec2 oRectMax = ImVec2(oPos.x + oTabSize.x, oPos.y + oTabSize.y);

		const float fOverlap = 10.f;
		const float fSlopWidth = 30.f;
		const float sSlopP1Ratio = 0.6f;
		const float fSlopP2Ratio = 0.4f;
		const float fSlopHRatio = 0.f;
		const float fShadowDropSize = 15.f;
		const float fShadowSlopRatio = 0.6f;
		const float fShadowAlpha = 0.75f;

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
		const ImVec2 uv = GImGui->FontTexUvWhitePixel;
		pDrawList->PrimReserve(3, 3);
		pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx)); pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 1)); pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 2));
		pDrawList->PrimWriteVtx(ImVec2(oRectMin.x - fOverlap - fShadowDropSize, oRectMax.y), uv, ImColor(0.f, 0.f, 0.f, 0.f));
		pDrawList->PrimWriteVtx(ImVec2(oRectMin.x - fOverlap + fSlopWidth * fShadowSlopRatio, oRectMin.y), uv, ImColor(0.f, 0.f, 0.f, 0.f));
		pDrawList->PrimWriteVtx(ImVec2(oRectMin.x - fOverlap + fSlopWidth * fShadowSlopRatio, oRectMax.y), uv, ImColor(0.f, 0.f, 0.f, fShadowAlpha));
		if (bFocused)
		{
			pDrawList->PrimReserve(3, 3);
			pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx)); pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 1)); pDrawList->PrimWriteIdx((ImDrawIdx)(pDrawList->_VtxCurrentIdx + 2));
			pDrawList->PrimWriteVtx(ImVec2(oRectMax.x + fOverlap + fShadowDropSize, oRectMax.y), uv, ImColor(0.f, 0.f, 0.f, 0.f));
			pDrawList->PrimWriteVtx(ImVec2(oRectMax.x + fOverlap - fSlopWidth * fShadowSlopRatio, oRectMin.y), uv, ImColor(0.f, 0.f, 0.f, 0.f));
			pDrawList->PrimWriteVtx(ImVec2(oRectMax.x + fOverlap - fSlopWidth * fShadowSlopRatio, oRectMax.y), uv, ImColor(0.f, 0.f, 0.f, fShadowAlpha));
		}

		// Draw tab and border
		if (bFocused && fStartLinePos < oPos.x)
		{
			pDrawList->PathLineTo(ImVec2(fStartLinePos, oRectMax.y));
		}
		pDrawList->PathLineTo(ImVec2(oRectMin.x - fOverlap, oRectMax.y));
		pDrawList->PathBezierCurveTo(
			ImVec2(oRectMin.x + fSlopWidth * sSlopP1Ratio - fOverlap, oRectMin.y + (oRectMax.y - oRectMin.y) * fSlopHRatio),
			ImVec2(oRectMin.x + fSlopWidth * fSlopP2Ratio - fOverlap, oRectMin.y),
			ImVec2(oRectMin.x + fSlopWidth - fOverlap, oRectMin.y)
			);
		pDrawList->PathLineTo(ImVec2(oRectMax.x - fSlopWidth + fOverlap, oRectMin.y));
		pDrawList->PathBezierCurveTo(
			ImVec2(oRectMax.x - fSlopWidth * fSlopP2Ratio + fOverlap, oRectMin.y),
			ImVec2(oRectMax.x - fSlopWidth * sSlopP1Ratio + fOverlap, oRectMin.y + (oRectMax.y - oRectMin.y) * fSlopHRatio),
			ImVec2(oRectMax.x + fOverlap, oRectMax.y)
			);

		if (bFocused)
		{
			pDrawList->AddConvexPolyFilled(pDrawList->_Path.Data + 1, pDrawList->_Path.Size - 1, bFocused ? oSelectedTab : oNormalTab, true);
			if (fEndLinePos > oRectMax.x)
			{
				pDrawList->PathLineTo(ImVec2(fEndLinePos, oRectMax.y));
			}
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

	ImwContainer* ImwContainer::GetBestDocking(const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, bool bLargeCheck)
	{
		if (m_pParent == NULL || 
			(oCursorPos.x >= m_oLastPosition.x && oCursorPos.x <= (m_oLastPosition.x + m_oLastSize.x) &&
			oCursorPos.y >= m_oLastPosition.y && oCursorPos.y <= (m_oLastPosition.y + m_oLastSize.y)))
		{
			if (IsSplit())
			{
				ImwContainer* pBestContainer = NULL;
				pBestContainer = m_pSplits[0]->GetBestDocking(oCursorPos, oOutOrientation, oOutAreaPos, oOutAreaSize, bLargeCheck);
				if (NULL != pBestContainer)
				{
					return pBestContainer;
				}
				pBestContainer = m_pSplits[1]->GetBestDocking(oCursorPos, oOutOrientation, oOutAreaPos, oOutAreaSize, bLargeCheck);
				if (NULL != pBestContainer)
				{
					return pBestContainer;
				}
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
						//Center
						ImRect oRectCenter(ImVec2(oCenter.x - c_fBoxHalfSize, oCenter.y - c_fBoxHalfSize), ImVec2(oCenter.x + c_fBoxHalfSize, oCenter.y + c_fBoxHalfSize));
						bIsInCenter = oRectCenter.Contains(oCursorPos);
						ImwWindowManager::GetInstance()->DrawWindowArea(m_pParentWindow, oRectCenter.Min, oRectCenter.GetSize(), bIsInCenter ? oBoxHightlightColor : oBoxColor);

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
						return this;
					}
					else if (bIsInTop)
					{
						oOutOrientation = E_DOCK_ORIENTATION_TOP;
						oOutAreaPos = m_oLastPosition;
						oOutAreaSize = ImVec2(m_oLastSize.x, m_oLastSize.y * c_fSplitRatio);
						return this;
					}
					else if (bIsInLeft)
					{
						oOutOrientation = E_DOCK_ORIENTATION_LEFT;
						oOutAreaPos = m_oLastPosition;
						oOutAreaSize = ImVec2(m_oLastSize.x * c_fSplitRatio, m_oLastSize.y);
						return this;
					}
					else if (bIsInRight)
					{
						oOutOrientation = E_DOCK_ORIENTATION_RIGHT;
						oOutAreaPos = ImVec2(m_oLastPosition.x + m_oLastSize.x * (1.f - c_fSplitRatio), m_oLastPosition.y);
						oOutAreaSize = ImVec2(m_oLastSize.x * c_fSplitRatio, m_oLastSize.y);
						return this;
					}
					else if (bIsInBottom)
					{
						oOutOrientation = E_DOCK_ORIENTATION_BOTTOM;
						oOutAreaPos = ImVec2(m_oLastPosition.x, m_oLastPosition.y + m_oLastSize.y * (1.f - c_fSplitRatio));
						oOutAreaSize = ImVec2(m_oLastSize.x, m_oLastSize.y * c_fSplitRatio);
						return this;
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
//SFF_END
}