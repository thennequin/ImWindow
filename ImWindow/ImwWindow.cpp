#include "ImwWindow.h"

#include "ImwWindowManager.h"

using namespace ImWindow;

ImwWindow::ImwWindow()
{
	m_pTitle = NULL;
	ImwWindowManager::GetInstance()->AddWindow(this);
}

ImwWindow::~ImwWindow()
{
	ImwWindowManager::GetInstance()->RemoveWindow(this);
	ImwSafeFree(m_pTitle);
}

void ImwWindow::Destroy()
{
	ImwWindowManager::GetInstance()->DestroyWindow(this);
}

void ImwWindow::SetTitle(const char* pTitle)
{
	ImwSafeFree(m_pTitle);
	if (NULL != pTitle)
	{
		size_t iLen = strlen(pTitle) + 1;
		m_pTitle = (char*)ImwMalloc(sizeof(char) * iLen);
		strcpy(m_pTitle, pTitle);
	}
}

const char* ImwWindow::GetTitle() const
{
	return m_pTitle;
}

const ImVec2& ImwWindow::GetLastPosition() const
{
	return m_oLastPosition;
}

const ImVec2& ImwWindow::GetLastSize() const
{
	return m_oLastSize;
}