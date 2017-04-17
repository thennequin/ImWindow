#include "ImwWindow.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
//SFF_BEGIN
	int ImwWindow::s_iNextId = 0;

#ifdef IMW_CUSTOM_IMPLEMENT_IMWWINDOW
	IMW_CUSTOM_IMPLEMENT_IMWWINDOW
#endif //IMW_CUSTOM_IMPLEMENT_IMWWINDOW

	ImwWindow::ImwWindow()
	{
		m_pTitle = NULL;
		m_bClosable  = true;
		m_bAlone = false;
		m_iId = s_iNextId++;
	
		//Write Id to string
		int iIndex = 0;
		int iNumber = m_iId;
		do
		{
			m_pId[iIndex++] = iNumber % 10 + '0';
		}
		while ((iNumber /= 10) > 0 && iIndex <= 10);
		m_pId[iIndex] = '\0';

		ImwWindowManager::GetInstance()->AddWindow(this);
	}

	ImwWindow::~ImwWindow()
	{
		ImwWindowManager::GetInstance()->RemoveWindow(this);
		ImwSafeFree(m_pTitle);
	}

	void ImwWindow::OnContextMenu()
	{
	}

	void ImwWindow::GetParameters(JsonValue& /*oOutParameters*/)
	{
	}

	void ImwWindow::SetParameters(const JsonValue& /*oParameters*/)
	{
	}

	ImU32 ImwWindow::GetId() const
	{
		return m_iId;
	}

	const ImwChar* ImwWindow::GetIdStr() const
	{
		return m_pId;
	}

	void ImwWindow::Destroy()
	{
		ImwWindowManager::GetInstance()->DestroyWindow(this);
	}

	void ImwWindow::SetTitle(const ImwChar* pTitle)
	{
		ImwSafeFree(m_pTitle);
		if (NULL != pTitle)
		{
			size_t iLen = strlen(pTitle) + 1;
			m_pTitle = (ImwChar*)ImwMalloc(sizeof(ImwChar) * iLen);
			strcpy(m_pTitle, pTitle);
		}
	}

	const ImwChar* ImwWindow::GetTitle() const
	{
		return m_pTitle;
	}

	void ImwWindow::SetClosable( bool bClosable )
	{
		m_bClosable = bClosable;
	}

	bool ImwWindow::IsClosable() const
	{
		return m_bClosable;
	}

	void ImwWindow::SetAlone(bool bAlone)
	{
		m_bAlone = bAlone;
	}

	bool ImwWindow::IsAlone() const
	{
		return m_bAlone;
	}

	const ImVec2& ImwWindow::GetLastPosition() const
	{
		return m_oLastPosition;
	}

	const ImVec2& ImwWindow::GetLastSize() const
	{
		return m_oLastSize;
	}
//SFF_END
}