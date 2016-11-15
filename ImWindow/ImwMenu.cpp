
#include "ImwMenu.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
//SFF_BEGIN
	ImwMenu::ImwMenu(int iHorizontalPriority, bool bAutoDeleted)
	{
		m_iHorizontalPriority = iHorizontalPriority;
		m_bAutoDeleted = bAutoDeleted;

		ImwWindowManager::GetInstance()->AddMenu(this);
	}

	ImwMenu::ImwMenu(const ImwMenu& oStatusBar)
	{
		m_iHorizontalPriority = oStatusBar.m_iHorizontalPriority;
	}

	ImwMenu::~ImwMenu()
	{
		ImwWindowManager::GetInstance()->RemoveMenu(this);
	}

	int ImwMenu::GetHorizontalPriority() const
	{
		return m_iHorizontalPriority;
	}

	bool ImwMenu::IsAutoDeleted()
	{
		return m_bAutoDeleted;
	}
//SFF_END
}