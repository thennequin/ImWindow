
#include "ImwStatusBar.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
//SFF_BEGIN
	ImwStatusBar::ImwStatusBar(int iHorizontalPriority, bool bAutoDeleted)
	{
		m_iHorizontalPriority = iHorizontalPriority;
		m_bAutoDeleted = bAutoDeleted;

		ImwWindowManager::GetInstance()->AddStatusBar(this);
	}

	ImwStatusBar::ImwStatusBar(const ImwStatusBar& oStatusBar)
	{
		m_iHorizontalPriority = oStatusBar.m_iHorizontalPriority;
	}

	ImwStatusBar::~ImwStatusBar()
	{
		ImwWindowManager::GetInstance()->RemoveStatusBar(this);
	}

	void ImwStatusBar::OnStatusBar()
	{
	}

	int ImwStatusBar::GetHorizontalPriority() const
	{
		return m_iHorizontalPriority;
	}

	bool ImwStatusBar::IsAutoDeleted()
	{
		return m_bAutoDeleted;
	}
//SFF_END
}