
#define IMGUI_DEFINE_MATH_OPERATORS
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
		m_pSpecialWindow = NULL;
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
		m_pSpecialWindow = NULL;
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
		ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();

		if (m_pSpecialWindow != NULL)
		{
			pWindowManager->RemoveWindow(m_pSpecialWindow);
			delete m_pSpecialWindow;
			m_pSpecialWindow = NULL;
		}

		while ( m_lWindows.begin() != m_lWindows.end() )
		{
			pWindowManager->RemoveWindow(*m_lWindows.begin());
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
				if (m_pSpecialWindow == NULL && m_lWindows.begin() == m_lWindows.end())
				{
					eOrientation = E_DOCK_ORIENTATION_CENTER;
				}

				switch (eOrientation)
				{
				case E_DOCK_ORIENTATION_CENTER:
					{
						if (pWindow->GetWindowMode() == E_WINDOW_MODE_NORMAL)
						{
							if (iPosition < 0 || iPosition >= (int)m_lWindows.size())
							{
								m_lWindows.push_back(pWindow);
								m_iActiveWindow = (int)m_lWindows.size() - 1;
							}
							else
							{
								ImwWindowVector::iterator itWindow = m_lWindows.begin();
								std::advance(itWindow, iPosition);
								m_lWindows.insert(itWindow, pWindow);
								m_iActiveWindow = iPosition;
							}
						}
						else
						{
							if (m_pSpecialWindow != NULL || m_lWindows.empty() == false)
							{
								CreateSplits();
								m_pSplits[0]->m_pSpecialWindow = m_pSpecialWindow;
								for (ImwWindowVector::iterator it = m_lWindows.begin(), itEnd = m_lWindows.end(); it != itEnd; ++it)
								{
									m_pSplits[0]->m_lWindows.push_back(*it);
								}
								m_pSplits[0]->m_iActiveWindow = m_iActiveWindow;
								m_pSplits[1]->m_pSpecialWindow = pWindow;
								m_pSpecialWindow = NULL;
								m_lWindows.clear();
								m_iActiveWindow = 0;
							}
							else
							{
								m_pSpecialWindow = pWindow;
							}
						}
					}
					break;
				case E_DOCK_ORIENTATION_TOP:
				case E_DOCK_ORIENTATION_LEFT:
					{
						m_bVerticalSplit = (eOrientation == E_DOCK_ORIENTATION_TOP);
						CreateSplits();
						// Dock new windows to Top/Left split
						m_pSplits[0]->Dock(pWindow);
						// Move windows to Bottom/Right Split
						m_pSplits[1]->m_pSpecialWindow = m_pSpecialWindow;
						m_pSplits[1]->m_iActiveWindow = m_iActiveWindow;
						for (ImwWindowVector::iterator it = m_lWindows.begin(), itEnd = m_lWindows.end(); it != itEnd; ++it)
						{
							m_pSplits[1]->m_lWindows.push_back(*it);
						}
						m_fSplitRatio = fRatio;
						m_pSpecialWindow = NULL;
						m_lWindows.clear();
						m_iActiveWindow = 0;
					}
					break;
				case E_DOCK_ORIENTATION_BOTTOM:
				case E_DOCK_ORIENTATION_RIGHT:
					{
						m_bVerticalSplit = (eOrientation == E_DOCK_ORIENTATION_BOTTOM);
						CreateSplits();
						// Move windows to Bottom/Right Split
						m_pSplits[0]->m_pSpecialWindow = m_pSpecialWindow;
						m_pSplits[0]->m_iActiveWindow = m_iActiveWindow;
						for (ImwWindowVector::iterator it = m_lWindows.begin(), itEnd = m_lWindows.end(); it != itEnd; ++it)
						{
							m_pSplits[0]->m_lWindows.push_back(*it);
						}
						// Dock new windows to Top/Left split
						m_pSplits[1]->Dock(pWindow);
						m_fSplitRatio = 1.f - fRatio;
						m_pSpecialWindow = NULL;
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
		if (m_pSpecialWindow == pWindow)
		{
			m_pSpecialWindow = NULL;
			return true;
		}

		ImwWindowVector::const_iterator itFind = std::find(m_lWindows.begin(), m_lWindows.end(), pWindow);
		if (itFind != m_lWindows.end())
		{
			m_lWindows.erase(itFind);
			if (m_iActiveWindow >= (int)m_lWindows.size())
			{
				m_iActiveWindow = (int)m_lWindows.size() - 1;
			}
			return true;
		}

		if (NULL != m_pSplits[0] && NULL != m_pSplits[1])
		{
			if (m_pSplits[0]->UnDock(pWindow))
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
						IM_ASSERT(m_lWindows.empty() && m_pSpecialWindow == NULL);
						m_pSpecialWindow = m_pSplits[1]->m_pSpecialWindow;
						m_iActiveWindow = m_pSplits[1]->m_iActiveWindow;
						for (ImwWindowVector::iterator it = m_pSplits[1]->m_lWindows.begin(), itEnd = m_pSplits[1]->m_lWindows.end(); it != itEnd; ++it)
						{
							m_lWindows.push_back(*it);
						}
						m_pSplits[1]->m_pSpecialWindow = NULL;
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
						IM_ASSERT(m_lWindows.empty() && m_pSpecialWindow == NULL);
						m_pSpecialWindow = m_pSplits[0]->m_pSpecialWindow;
						for (ImwWindowVector::iterator it = m_pSplits[0]->m_lWindows.begin(), itEnd = m_pSplits[0]->m_lWindows.end(); it != itEnd; ++it)
						{
							m_lWindows.push_back(*it);
						}
						m_iActiveWindow = m_pSplits[0]->m_iActiveWindow;
						m_pSplits[0]->m_pSpecialWindow = NULL;
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

	bool ImwContainer::DockToBest(ImwWindow* pWindow)
	{
		if (IsSplit())
		{
			if (m_fSplitRatio > 0.5f)
			{
				if (m_pSplits[0]->DockToBest(pWindow))
					return true;
			}

			if (m_pSplits[1]->DockToBest(pWindow))
				return true;

			// Force try to dock on the first split if the second fails
			if (m_pSplits[0]->DockToBest(pWindow))
				return true;
		}
		else if (m_pSpecialWindow == NULL || m_pSpecialWindow->m_eMode != E_WINDOW_MODE_ALONE)
		{
			Dock(pWindow);
			return true;
		}
		else if (m_pParent == NULL)
		{
			// Split to force dock
			Dock(pWindow, m_bVerticalSplit ? E_DOCK_ORIENTATION_BOTTOM : E_DOCK_ORIENTATION_RIGHT);
			return true;
		}
		return false;
	}

	bool ImwContainer::IsEmpty() const
	{
		//IM_ASSERT(IsSplit() != HasWindowTabbed());
		return !(IsSplit() || HasWindow());
	}

	bool ImwContainer::IsSplit() const
	{
		IM_ASSERT((NULL == m_pSplits[0]) == (NULL == m_pSplits[1]));
		return (NULL != m_pSplits[0] && NULL != m_pSplits[1]);
	}

	bool ImwContainer::HasWindow() const
	{
		return m_pSpecialWindow != NULL || m_lWindows.size() > 0;
	}

	ImwWindow* ImwContainer::GetWindowAtPos(const ImVec2& oPos) const
	{
		if (oPos.x >= m_oLastPosition.x && oPos.x <= (m_oLastPosition.x + m_oLastSize.x)
			&& oPos.y >= m_oLastPosition.y && oPos.y <= (m_oLastPosition.y + m_oLastSize.y))
		{
			if (IsSplit())
			{
				ImwWindow* pWindow = m_pSplits[0]->GetWindowAtPos(oPos);
				if (pWindow == NULL)
					pWindow = m_pSplits[1]->GetWindowAtPos(oPos);
				return pWindow;
			}
			else
			{
				return GetActiveWindow();
			}
		}

		return NULL;
	}

	const ImwContainer* ImwContainer::HasWindow(ImwWindow* pWindow) const
	{
		if (m_pSpecialWindow == pWindow)
		{
			return this;
		}
		else if (std::find(m_lWindows.begin(), m_lWindows.end(), pWindow) != m_lWindows.end())
		{
			return this;
		}
		else
		{
			if (NULL != m_pSplits[0])
			{
				const ImwContainer* pContainer = m_pSplits[0]->HasWindow(pWindow);
				if (NULL != pContainer)
				{
					return pContainer;
				}
			}
			if (NULL != m_pSplits[1])
			{
				const ImwContainer* pContainer = m_pSplits[1]->HasWindow(pWindow);
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
		ImwWindowVector::iterator itFind = std::find(m_lWindows.begin(), m_lWindows.end(), pWindow);
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

	bool ImwContainer::IsFocusedWindow(ImwWindow* pWindow) const
	{
		if (m_lWindows.empty() == false)
		{
			ImwWindowVector::const_iterator itActive = m_lWindows.begin();
			std::advance(itActive, m_iActiveWindow);
			return itActive == std::find(m_lWindows.begin(), m_lWindows.end(), pWindow);
		}
		else
		{
			return (NULL != m_pSplits[0] && m_pSplits[0]->IsFocusedWindow(pWindow)) || (NULL != m_pSplits[1] && m_pSplits[1]->IsFocusedWindow(pWindow));
		}
	}

	ImwWindow* ImwContainer::GetActiveWindow() const
	{
		if (!IsSplit() && !m_lWindows.empty())
		{
			ImwWindowVector::const_iterator itActiveWindow = m_lWindows.begin();
			std::advance(itActiveWindow, m_iActiveWindow);
			return *itActiveWindow;
		}
		return m_pSpecialWindow;
	}

	ImwPlatformWindow* ImwContainer::GetPlatformWindowParent() const
	{
		return m_pParentWindow;
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

		const int iSeparatorSize = 5;

		if (IsSplit())
		{
			if (m_bVerticalSplit)
			{
				float iFirstHeight = oSize.y * m_fSplitRatio - iSeparatorSize - pWindow->WindowPadding.x;

				ImVec4 oBackupColor = oStyle.Colors[ImGuiCol_ChildBg];

				oStyle.Colors[ImGuiCol_ChildBg].w = 0.f;
				ImGui::BeginChild("Top", ImVec2(0, iFirstHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				oStyle.Colors[ImGuiCol_ChildBg] = oBackupColor;
				m_pSplits[0]->Paint(/*iX, iY, iWidth, iFirstHeight*/);
				ImGui::EndChild();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - oStyle.ItemSpacing.y);

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

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - oStyle.ItemSpacing.y);

				oStyle.Colors[ImGuiCol_ChildBg].w = 0.f;
				ImGui::BeginChild("Bottom", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				oStyle.Colors[ImGuiCol_ChildBg] = oBackupColor;
				m_pSplits[1]->Paint(/*iX, iY + iFirstHeight, iWidth, iSecondHeight*/);
				ImGui::EndChild();
			}
			else
			{
				float iFirstWidth = oSize.x * m_fSplitRatio - iSeparatorSize - pWindow->WindowPadding.y;

				ImVec4 oBackupColor = oStyle.Colors[ImGuiCol_ChildBg];

				oStyle.Colors[ImGuiCol_ChildBg].w = 0.f;
				ImGui::BeginChild("Left", ImVec2(iFirstWidth, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				oStyle.Colors[ImGuiCol_ChildBg] = oBackupColor;
				m_pSplits[0]->Paint();
				ImGui::EndChild();

				ImGui::SameLine(0.f, 0.f);

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

				ImGui::SameLine(0.f, 0.f);

				oStyle.Colors[ImGuiCol_ChildBg].w = 0.f;
				ImGui::BeginChild("Right", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				oStyle.Colors[ImGuiCol_ChildBg] = oBackupColor;
				m_pSplits[1]->Paint();
				ImGui::EndChild();
			}
		}
		else if (HasWindow())
		{
			EWindowMode eWindowMode = GetWindowMode();

			ImwWindow* pDraggedWindow = pWindowManager->GetDraggedWindow();

			bool bDragThisIsBestContainer = false;
			bool bDragOverTabArea = false;
			if (pDraggedWindow != NULL)
			{
				bDragThisIsBestContainer = pWindowManager->GetDragBestContainer() == this;
				bDragOverTabArea = pWindowManager->GetDragOnTabArea();
			}

			ImwWindow* pActiveWindow = m_pSpecialWindow;
			if (eWindowMode == E_WINDOW_MODE_NORMAL
				|| m_lWindows.empty() == false
				|| (pDraggedWindow != NULL && bDragThisIsBestContainer && bDragOverTabArea && eWindowMode != E_WINDOW_MODE_ALONE))
			{
				ImVec2 oItemSpacing = oStyle.ItemSpacing;
				oStyle.ItemSpacing = ImVec2(oItemSpacing.x, 0.f);

				ImGui::InvisibleButton("##TabListButton", ImVec2(14.f, c_fTabHeight));
				ImGui::SameLine();

				if (ImGui::BeginPopupContextItem("TabListMenu", 0))
				{
					int iIndex = 0;
					for (ImwWindowVector::const_iterator itWindow = m_lWindows.begin(); itWindow != m_lWindows.end(); ++itWindow, ++iIndex)
					{
						if (ImGui::Selectable((*itWindow)->GetTitle(), m_iActiveWindow == iIndex))
						{
							m_iActiveWindow = iIndex;
						}
					}
					ImGui::EndPopup();
				}

				ImU32 iTabListButtonColor = ImGui::GetColorU32(ImGui::IsItemHovered() ? ImGuiCol_Text : ImGuiCol_TextDisabled);
				ImVec2 oButtonMin = ImGui::GetItemRectMin();
				ImVec2 oButtonMax = ImGui::GetItemRectMax();
				ImVec2 oButtonCenter = (oButtonMin + oButtonMax) / 2.f;

				pDrawList->AddRectFilled(
					ImVec2(oButtonCenter.x - 5.f, oButtonCenter.y - 7.f),
					ImVec2(oButtonCenter.x + 5.f, oButtonCenter.y - 4.f),
					iTabListButtonColor
				);

				pDrawList->AddTriangleFilled(
					ImVec2(oButtonCenter.x - 5.f, oButtonCenter.y - 2.f),
					ImVec2(oButtonCenter.x + 5.f, oButtonCenter.y - 2.f),
					ImVec2(oButtonCenter.x, oButtonCenter.y + 4.f),
					iTabListButtonColor
				);

				pDrawList->ChannelsSplit(2);

				//Tabs

				int iSize = (int)m_lWindows.size();
				const float fTabAreaWidth = GetTabAreaWidth();
				float fMaxTabSize = fTabAreaWidth / iSize;
				float fDraggedTabWidth = 0.f;
				int iDraggedTabPosition = 0;
				ImVec2 oDragTabSize;
				if (pDraggedWindow != NULL)
				{
					if (bDragThisIsBestContainer && bDragOverTabArea && eWindowMode != E_WINDOW_MODE_ALONE)
					{
						pActiveWindow = pDraggedWindow;

						iDraggedTabPosition = pWindowManager->GetDragTabPosition();
						fMaxTabSize = fTabAreaWidth / (iSize + 1);

						float fTabPosX = oCursorPos.x + pWindowManager->GetDragOffset().x;

						const char* pDragText = pDraggedWindow->GetTitle();
						ImVec2 oTabTextSize;
						GetTabSize(pDragText, fMaxTabSize, &oDragTabSize, &oTabTextSize);

						float fCursorDiff = pWindow->DC.CursorPos.x - pWindow->DC.CursorStartPos.x;
						float fMin = pWindow->DC.CursorPos.x;
						float fMax = fMin + m_oLastSize.x - oDragTabSize.x - fCursorDiff;

						if (fTabPosX < fMin)
							fTabPosX = fMin;
						if (fTabPosX > fMax)
							fTabPosX = fMax;

						ImVec2 oDraggedTabPos = ImVec2(fTabPosX, pWindow->DC.CursorPos.y);
						DrawTab(pDragText, true, oDraggedTabPos, oMin.x, oMax.x, oDragTabSize, &oTabTextSize);
						fDraggedTabWidth = oDragTabSize.x;
					}
					else
					{
						pDraggedWindow = NULL;
					}
				}

				bool bCanCreateMultipleWindow = pWindowManager->CanCreateMultipleWindow();

				int iIndex = 0;
				int iNewActive = -1;
				bool bFirstTab = true;
				ImVec2 oFirstTabPos;

				if (m_lWindows.empty() && pDraggedWindow != NULL)
				{
					ImGui::Dummy(oDragTabSize);
				}

				for (ImwWindowVector::iterator it = m_lWindows.begin(); it != m_lWindows.end(); ++it)
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
						if (ImGui::IsMouseDragging( ImGuiMouseButton_Left ))
						{
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
							int iDockIndex = 0;

							if (pWindowManager->GetMainPlatformWindow()->GetContainer()->IsEmpty())
							{
								ImGui::PushID(0);
								if (ImGui::Selectable("Main")) pWindowManager->Dock((*it));
								ImGui::PopID();
								++iDockIndex;
							}
							const ImwWindowVector& lWindows = pWindowManager->GetWindowList();
							for (ImwWindowVector::const_iterator itWindow = lWindows.begin(); itWindow != lWindows.end(); ++itWindow)
							{
								if ((*it) != (*itWindow) && (*itWindow)->GetWindowMode() == E_WINDOW_MODE_NORMAL)
								{
									ImGui::PushID(iDockIndex);
									if (ImGui::BeginMenu((*itWindow)->GetTitle()))
									{
										bool bHovered = false;
										ImwPlatformWindow* pPlatformWindow = pWindowManager->GetWindowParent((*itWindow));

										ImVec2 oLastWinPos = (*itWindow)->GetLastPosition();
										ImVec2 oLastWinSize = (*itWindow)->GetLastSize();

										if ((*itWindow)->GetWindowMode() != E_WINDOW_MODE_ALONE)
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
								++iDockIndex;
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

				oStyle.ItemSpacing = oItemSpacing;

				if (pDraggedWindow == NULL && m_iActiveWindow >= 0 && m_iActiveWindow < m_lWindows.size())
				{
					pActiveWindow = m_lWindows[m_iActiveWindow];
				}

				IM_ASSERT(pActiveWindow != NULL);
			}

			//Draw active
			if (pActiveWindow != NULL)
			{
				ImVec2 oWindowPaddingBackup;
				if (pActiveWindow->IsFillingSpace())
				{
					oWindowPaddingBackup = oStyle.WindowPadding;
					oStyle.WindowPadding = ImVec2(0.f, 0.f);
				}

				ImGui::BeginChild(pActiveWindow->GetId(), ImVec2(0.f, 0.f), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysUseWindowPadding);

				if (pActiveWindow->IsFillingSpace())
				{
					oStyle.WindowPadding = oWindowPaddingBackup;
				}

				ImVec2 oWinPos = ImGui::GetWindowPos();
				ImVec2 oWinSize = ImGui::GetWindowSize();

				for (ImwWindowVector::iterator it = m_lWindows.begin(); it != m_lWindows.end(); ++it)
				{
					(*it)->m_oLastPosition = oWinPos;
					(*it)->m_oLastSize = oWinSize;
				}

				{
#ifdef IMW_BEFORE_WINDOW_PAINT
					IMW_BEFORE_WINDOW_PAINT(pActiveWindow->GetTitle())
#endif //IMW_BEFORE_WINDOW_PAINT
					pActiveWindow->OnGui();
				}

				ImGui::EndChild();
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

		const char* pTitle = pWindow->GetTitle();
		ImVec2 oTabSize;
		ImVec2 oTabTextSize;
		GetTabSize(pTitle, fMaxSize, &oTabSize, &oTabTextSize);
		DrawTab(pTitle, bFocused, window->DC.CursorPos, fStartLinePos, fEndLinePos, oTabSize, &oTabTextSize);

		return ImGui::InvisibleButton(pWindow->GetIdStr(), oTabSize);
	}

	void ImwContainer::DrawTab(const char* pText, bool bFocused, ImVec2 oPos, float fStartLinePos, float fEndLinePos, const ImVec2& oTabSize, const ImVec2* pTextSize)
	{
		const ImwWindowManager::Config& oConfig = ImwWindowManager::GetInstance()->GetConfig();
		ImDrawList* pDrawList = ImGui::GetWindowDrawList();
		const ImGuiStyle& oStyle = ImGui::GetStyle();

		ImColor oNormalTab(0), oSelectedTab(0), oBorderColor(0);
		switch (oConfig.m_eTabColorMode)
		{
			case ImwWindowManager::E_TABCOLORMODE_TITLE:
				oNormalTab = oStyle.Colors[ImGuiCol_TitleBg];
				oNormalTab.Value.w = 1.f;
				oSelectedTab = oStyle.Colors[ImGuiCol_TitleBgActive];
				oSelectedTab.Value.w = 1.f;
				oBorderColor = oStyle.Colors[ImGuiCol_Border];
				oBorderColor.Value.w = 1.f;
				break;
			case ImwWindowManager::E_TABCOLORMODE_BACKGROUND:
				oNormalTab = ImColor(
					oStyle.Colors[ImGuiCol_WindowBg].x + (oStyle.Colors[ImGuiCol_ChildBg].x - oStyle.Colors[ImGuiCol_WindowBg].x) * 0.5f,
					oStyle.Colors[ImGuiCol_WindowBg].y + (oStyle.Colors[ImGuiCol_ChildBg].y - oStyle.Colors[ImGuiCol_WindowBg].y) * 0.5f,
					oStyle.Colors[ImGuiCol_WindowBg].z + (oStyle.Colors[ImGuiCol_ChildBg].z - oStyle.Colors[ImGuiCol_WindowBg].z) * 0.5f,
					oStyle.Colors[ImGuiCol_WindowBg].w + (oStyle.Colors[ImGuiCol_ChildBg].w - oStyle.Colors[ImGuiCol_WindowBg].w) * 0.5f
				);
				oSelectedTab = oStyle.Colors[ImGuiCol_ChildBg];
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
			const ImVec2 uv = ImGui::GetFontTexUvWhitePixel();
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
		pDrawList->PathBezierCubicCurveTo(
			ImVec2(oRectMin.x + oConfig.m_fTabSlopWidth * oConfig.m_fTabSlopP1Ratio - oConfig.m_fTabOverlap, oRectMin.y + (oRectMax.y - oRectMin.y) * oConfig.m_fTabSlopHRatio),
			ImVec2(oRectMin.x + oConfig.m_fTabSlopWidth * oConfig.m_fTabSlopP2Ratio - oConfig.m_fTabOverlap, oRectMin.y),
			ImVec2(oRectMin.x + oConfig.m_fTabSlopWidth - oConfig.m_fTabOverlap, oRectMin.y)
			);
		pDrawList->PathLineTo(ImVec2(oRectMax.x - oConfig.m_fTabSlopWidth + oConfig.m_fTabOverlap, oRectMin.y));
		pDrawList->PathBezierCubicCurveTo(
			ImVec2(oRectMax.x - oConfig.m_fTabSlopWidth * oConfig.m_fTabSlopP2Ratio + oConfig.m_fTabOverlap, oRectMin.y),
			ImVec2(oRectMax.x - oConfig.m_fTabSlopWidth * oConfig.m_fTabSlopP1Ratio + oConfig.m_fTabOverlap, oRectMin.y + (oRectMax.y - oRectMin.y) * oConfig.m_fTabSlopHRatio),
			ImVec2(oRectMax.x + oConfig.m_fTabOverlap, oRectMax.y)
			);

		if (bFocused)
		{
			pDrawList->AddConvexPolyFilled(pDrawList->_Path.Data + 1, pDrawList->_Path.Size - 1, bFocused ? oSelectedTab : oNormalTab);
			if (fEndLinePos > oRectMax.x)
			{
				pDrawList->PathLineTo(ImVec2(fEndLinePos, oRectMax.y));
			}

			if (oConfig.m_bShowTabBorder)
				pDrawList->AddPolyline(pDrawList->_Path.Data, pDrawList->_Path.Size, oBorderColor, false, 1.5f);
		}
		else
		{
			pDrawList->AddConvexPolyFilled(pDrawList->_Path.Data, pDrawList->_Path.Size, bFocused ? oSelectedTab : oNormalTab);
		}

		pDrawList->PathClear();

		ImVec2 oTextRectMin(oRectMin.x + 5, oRectMin.y);
		ImVec2 oTextRectMax(oRectMax.x - 5, oRectMax.y);
		ImGui::RenderTextClipped(oTextRectMin, oTextRectMax, pText, NULL, pTextSize, ImVec2(0.5f, 0.5f));
	}

	void ImwContainer::GetTabSize(const char* pText, float fMaxSize, ImVec2* pOutTabSize, ImVec2* pOutTextSize) const
	{
		const ImVec2 oTextSize = ImGui::CalcTextSize(pText);

		if (NULL != pOutTextSize)
		{
			*pOutTextSize = oTextSize;
		}

		//Clamp fMaxSize at a minimum to avoid glitch
		if (fMaxSize < 30.f)
		{
			fMaxSize = 30.f;
		}

		//Calculate tab size
		float fWidth = oTextSize.x + 15.f;
		if (fMaxSize != 1.f && fWidth > fMaxSize)
		{
			fWidth = fMaxSize;
		}

		*pOutTabSize = ImVec2(fWidth, c_fTabHeight);
	}

	float ImwContainer::GetTabAreaWidth() const
	{
		return m_oLastSize.x - 50.f;
	}

	const ImwContainer* ImwContainer::GetBestContainer(const ImVec2& oCursorPos) const
	{
		if (IsSplit())
		{
			const ImwContainer* pBestContainer = NULL;
			bool bInFirstSplit = m_bVerticalSplit
				? (oCursorPos.y <= (m_pSplits[0]->m_oLastPosition.y + m_pSplits[0]->m_oLastSize.y))
				: (oCursorPos.x <= (m_pSplits[0]->m_oLastPosition.x + m_pSplits[0]->m_oLastSize.x));

			if (bInFirstSplit)
			{
				pBestContainer = m_pSplits[0]->GetBestContainer(oCursorPos);
				if (pBestContainer != NULL)
					return pBestContainer;
			}

			pBestContainer = m_pSplits[1]->GetBestContainer(oCursorPos);
			if (pBestContainer != NULL)
				return pBestContainer;

			return m_pSplits[0]->GetBestContainer(oCursorPos);
		}
		else
		{
			if (m_pSpecialWindow == NULL)
				return this;
		}
		return NULL;
	}

	const ImwContainer* ImwContainer::GetBestDocking(const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, bool* pOutOnTabArea, int* pOutPosition, bool bLargeCheck) const
	{
		ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();

		if (m_pParent == NULL ||
			(oCursorPos.x >= m_oLastPosition.x && oCursorPos.x <= (m_oLastPosition.x + m_oLastSize.x) &&
			oCursorPos.y >= m_oLastPosition.y && oCursorPos.y <= (m_oLastPosition.y + m_oLastSize.y)))
		{
			if (IsSplit())
			{
				for (int iSplit = 0; iSplit < 2; ++iSplit)
				{
					const ImwContainer* pBestContainer = m_pSplits[iSplit]->GetBestDocking(oCursorPos, oOutOrientation, oOutAreaPos, oOutAreaSize, pOutOnTabArea, pOutPosition, bLargeCheck);
					if (NULL != pBestContainer)
					{
						return pBestContainer;
					}
				}

				if (bLargeCheck)
				{
					const ImwContainer* pBestContainer = GetBestContainer(oCursorPos);
					IM_ASSERT(pBestContainer != NULL);
					if (pBestContainer != NULL)
					{
						oOutOrientation = E_DOCK_ORIENTATION_CENTER;
						oOutAreaPos = pBestContainer->m_oLastPosition;
						oOutAreaSize = pBestContainer->m_oLastSize;
						*pOutOnTabArea = false;
						return pBestContainer;
					}
				}
			}
			else
			{
				EWindowMode eWindowMode = GetWindowMode();

				if (eWindowMode != E_WINDOW_MODE_ALONE && oCursorPos.y < m_oLastPosition.y + c_fTabHeight) // TODO change formula
				{
					oOutOrientation = E_DOCK_ORIENTATION_CENTER;
					oOutAreaPos = m_oLastPosition;
					oOutAreaSize = ImVec2(0,0);
					*pOutOnTabArea = true;
					//Search tab position
					float fMaxTabSize = GetTabAreaWidth() / (m_lWindows.size() + 1);
					float fCurrentTabPosX = m_oLastPosition.x;
					int iCurrentTab = 0;
					float fCursorX = oCursorPos.x + pWindowManager->GetDragOffset().x;
					for (ImwWindowVector::const_iterator itWindow = m_lWindows.begin(); itWindow != m_lWindows.end(); ++itWindow)
					{
						ImVec2 oTabSize;
						GetTabSize((*itWindow)->GetTitle(), fMaxTabSize, &oTabSize);
						if (fCursorX < (fCurrentTabPosX + oTabSize.x / 2.f))
						{
							break;
						}
						fCurrentTabPosX += oTabSize.x;
						++iCurrentTab;
					}
					*pOutPosition = iCurrentTab;
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
							if (m_pSpecialWindow == NULL || m_pSpecialWindow->GetWindowMode() != E_WINDOW_MODE_ALONE)
							{
								//Center
								ImRect oRectCenter(ImVec2(oCenter.x - c_fBoxHalfSize, oCenter.y - c_fBoxHalfSize), ImVec2(oCenter.x + c_fBoxHalfSize, oCenter.y + c_fBoxHalfSize));
								bIsInCenter = oRectCenter.Contains(oCursorPos);
								pWindowManager->DrawWindowArea(m_pParentWindow, oRectCenter.Min, oRectCenter.GetSize(), bIsInCenter ? oBoxHightlightColor : oBoxColor);
							}

							if (HasWindow())
							{
								if (m_oLastSize.y >= c_fMinSize)
								{
									//Top
									ImRect oRectTop(ImVec2(oCenter.x - c_fBoxHalfSize, oCenter.y - c_fBoxHalfSize * 4.f), ImVec2(oCenter.x + c_fBoxHalfSize, oCenter.y - c_fBoxHalfSize * 2.f));
									bIsInTop = oRectTop.Contains(oCursorPos);
									pWindowManager->DrawWindowArea(m_pParentWindow, oRectTop.Min, oRectTop.GetSize(), bIsInTop ? oBoxHightlightColor : oBoxColor);

									//Bottom
									ImRect oRectBottom(ImVec2(oCenter.x - c_fBoxHalfSize, oCenter.y + c_fBoxHalfSize * 2.f), ImVec2(oCenter.x + c_fBoxHalfSize, oCenter.y + c_fBoxHalfSize * 4.f));
									bIsInBottom = oRectBottom.Contains(oCursorPos);
									pWindowManager->DrawWindowArea(m_pParentWindow, oRectBottom.Min, oRectBottom.GetSize(), bIsInBottom ? oBoxHightlightColor : oBoxColor);
								}

								if (m_oLastSize.x >= c_fMinSize)
								{
									//Left
									ImRect oRectLeft(ImVec2(oCenter.x - c_fBoxHalfSize * 4.f, oCenter.y - c_fBoxHalfSize), ImVec2(oCenter.x - c_fBoxHalfSize * 2.f, oCenter.y + c_fBoxHalfSize));
									bIsInLeft = oRectLeft.Contains(oCursorPos);
									pWindowManager->DrawWindowArea(m_pParentWindow, oRectLeft.Min, oRectLeft.GetSize(), bIsInLeft ? oBoxHightlightColor : oBoxColor);

									//Right
									ImRect oRectRight(ImVec2(oCenter.x + c_fBoxHalfSize * 2.f, oCenter.y - c_fBoxHalfSize), ImVec2(oCenter.x + c_fBoxHalfSize * 4.f, oCenter.y + c_fBoxHalfSize));
									bIsInRight = oRectRight.Contains(oCursorPos);
									pWindowManager->DrawWindowArea(m_pParentWindow, oRectRight.Min, oRectRight.GetSize(), bIsInRight ? oBoxHightlightColor : oBoxColor);
								}
							}
						}

						if (bIsInCenter)
						{
							oOutOrientation = E_DOCK_ORIENTATION_CENTER;
							oOutAreaPos = m_oLastPosition;
							oOutAreaSize = m_oLastSize;
							*pOutOnTabArea = false;
							return this;
						}
						else if (bIsInTop)
						{
							oOutOrientation = E_DOCK_ORIENTATION_TOP;
							oOutAreaPos = m_oLastPosition;
							oOutAreaSize = ImVec2(m_oLastSize.x, m_oLastSize.y * c_fSplitRatio);
							*pOutOnTabArea = false;
							return this;
						}
						else if (bIsInLeft)
						{
							oOutOrientation = E_DOCK_ORIENTATION_LEFT;
							oOutAreaPos = m_oLastPosition;
							oOutAreaSize = ImVec2(m_oLastSize.x * c_fSplitRatio, m_oLastSize.y);
							*pOutOnTabArea = false;
							return this;
						}
						else if (bIsInRight)
						{
							oOutOrientation = E_DOCK_ORIENTATION_RIGHT;
							oOutAreaPos = ImVec2(m_oLastPosition.x + m_oLastSize.x * (1.f - c_fSplitRatio), m_oLastPosition.y);
							oOutAreaSize = ImVec2(m_oLastSize.x * c_fSplitRatio, m_oLastSize.y);
							*pOutOnTabArea = false;
							return this;
						}
						else if (bIsInBottom)
						{
							oOutOrientation = E_DOCK_ORIENTATION_BOTTOM;
							oOutAreaPos = ImVec2(m_oLastPosition.x, m_oLastPosition.y + m_oLastSize.y * (1.f - c_fSplitRatio));
							oOutAreaSize = ImVec2(m_oLastSize.x, m_oLastSize.y * c_fSplitRatio);
							*pOutOnTabArea = false;
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
		for (ImwWindowVector::const_iterator itWindow = m_lWindows.begin(); itWindow != m_lWindows.end(); ++itWindow)
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

	EWindowMode ImwContainer::GetWindowMode() const
	{
		IM_ASSERT(m_pSpecialWindow == NULL || m_pSpecialWindow->GetWindowMode() != E_WINDOW_MODE_NORMAL);
		IM_ASSERT(m_lWindows.empty() || m_lWindows.front()->GetWindowMode() == E_WINDOW_MODE_NORMAL);

		if (m_pSpecialWindow != NULL)
		{
			return m_pSpecialWindow->GetWindowMode();
		}

		return E_WINDOW_MODE_NORMAL;
	}

#ifdef IMW_USE_LAYOUT_SERIALIZATION
	bool ImwContainer::Save(JsonStthm::JsonValue& oJson)
	{
		oJson["Vertical"] = m_bVerticalSplit;
		oJson["SplitRatio"] = m_fSplitRatio;

		if (m_lWindows.size() != 0 || m_pSpecialWindow != NULL)
		{
			ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();

			if (m_pSpecialWindow != NULL)
			{
				const char* pClassName = pWindowManager->GetWindowClassName(m_pSpecialWindow);
				if (pClassName == NULL)
					return false;

				JsonStthm::JsonValue& oJsonSpecial = oJson["SpecialWindow"];
				oJsonSpecial["Class"] = pClassName;
				JsonStthm::JsonValue oParameters;
				m_pSpecialWindow->GetParameters( oParameters );
				if (oParameters.IsNull() == false)
					oJsonSpecial["Parameters"] = oParameters;
			}

			if (m_lWindows.size() != 0)
			{
				oJson["CurrentWindow"] = (int64_t)m_iActiveWindow;
				JsonStthm::JsonValue& oJsonWindows = oJson["Windows"];
				int iCurrentWindow = 0;
				for (ImwWindowVector::const_iterator itWindow = m_lWindows.begin(); itWindow != m_lWindows.end(); ++itWindow)
				{
					JsonStthm::JsonValue& oJsonWindow = oJsonWindows[iCurrentWindow++];
					const char* pClassName = pWindowManager->GetWindowClassName(*itWindow);
					if (pClassName == NULL)
						return false;
					oJsonWindow["Class"] = pClassName;
					JsonStthm::JsonValue oParameters;
					( *itWindow )->GetParameters( oParameters );
					if( oParameters.IsNull() == false )
						oJsonWindow["Parameters"] = oParameters;
				}
			}
			return true;
		}
		else
		{
			JsonStthm::JsonValue& oJsonSplits = oJson["Splits"];
			return m_pSplits[0]->Save(oJsonSplits[0]) && m_pSplits[1]->Save(oJsonSplits[1]);
		}
	}

	bool ImwContainer::Load(const JsonStthm::JsonValue& oJson, bool bJustCheck)
	{
		if (!oJson["Vertical"].IsBoolean() || !oJson["SplitRatio"].IsFloat())
			return false;

		if (!(oJson["Windows"].IsArray() || oJson["SpecialWindow"].IsObject() || (oJson["Splits"].IsArray() && oJson["Splits"].GetMemberCount() == 2)))
			return false;

		ImwWindowManager* pWindowManager = ImwWindowManager::GetInstance();

		if (!bJustCheck)
		{
			m_bVerticalSplit = oJson["Vertical"].ToBoolean();
			m_fSplitRatio = (float)oJson["SplitRatio"].ToFloat();

			//Clear
			ImwSafeDelete(m_pSpecialWindow);

			while (m_lWindows.begin() != m_lWindows.end())
			{
				pWindowManager->RemoveWindow(*m_lWindows.begin());
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
			const JsonStthm::JsonValue& oJsonSpecialWindow = oJson["SpecialWindow"];
			const JsonStthm::JsonValue& oJsonCurrentWindow = oJson["CurrentWindow"];
			const JsonStthm::JsonValue& oJsonWindows = oJson["Windows"];
			//Check
			if (!(oJsonSpecialWindow.IsNull() || (oJsonCurrentWindow.IsNull() || oJsonCurrentWindow.IsInteger())))
				return false;

			if (oJsonSpecialWindow.IsObject())
			{
				if (oJsonSpecialWindow["Class"].IsString() == false)
					return false;
				if (pWindowManager->CanCreateWindowByClassName(oJsonSpecialWindow["Class"].ToString()) == false)
					return false;
			}

			int iWindowCount = 0;

			if (oJsonWindows.IsArray())
			{
				iWindowCount = oJsonWindows.GetMemberCount();
				for (int iCurrent = 0; iCurrent < iWindowCount; ++iCurrent)
				{
					const JsonStthm::JsonValue& oJsonWindow = oJsonWindows[iCurrent];
					if (oJsonWindow.IsObject() == false || oJsonWindow["Class"].IsString() == false)
						return false;
					if (pWindowManager->CanCreateWindowByClassName(oJsonWindow["Class"].ToString()) == false)
						return false;
				}
			}

			if (!bJustCheck)
			{
				if (oJsonSpecialWindow.IsObject())
				{
					ImwWindow* pWindow = pWindowManager->CreateWindowByClassName(oJsonSpecialWindow["Class"].ToString());
					pWindow->SetParameters( oJsonSpecialWindow["Parameters"]);
					m_pSpecialWindow = pWindow;
				}

				if (oJsonWindows.IsArray())
				{
					for (int iCurrent = 0; iCurrent < iWindowCount; ++iCurrent)
					{
						const JsonStthm::JsonValue& oJsonWindow = oJsonWindows[iCurrent];
						ImwWindow* pWindow = pWindowManager->CreateWindowByClassName(oJsonWindow["Class"].ToString());
						pWindow->SetParameters(oJsonWindow["Parameters"]);
						m_lWindows.push_back(pWindow);
					}
				}

				if (oJsonCurrentWindow.IsInteger())
				{
					m_iActiveWindow = (int)oJsonCurrentWindow.ToInteger();
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
#endif //IMW_USE_LAYOUT_SERIALIZATION

//SFF_END
}
