#include "ImwWindow.h"

#include "ImwWindowManager.h"

namespace ImWindow
{
//SFF_BEGIN
	int ImwWindow::s_iNextId = 1;

#ifdef IMW_CUSTOM_IMPLEMENT_IMWWINDOW
	IMW_CUSTOM_IMPLEMENT_IMWWINDOW
#endif //IMW_CUSTOM_IMPLEMENT_IMWWINDOW

	ImwWindow::ImwWindow(EWindowMode eMode)
		: m_pTitle(NULL)
		, m_bClosable(true)
		, m_eMode(eMode)
		, m_bFillingSpace(false)
		, m_oLastPosition(0.f, 0.f)
		, m_oLastSize(0.f, 0.f)
	{
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

	void ImwWindow::OnDropFiles(int /*iCount*/, char** /*pFiles*/, const ImVec2& /*oPos*/)
	{
	}

	bool ImwWindow::IsFillingSpace() const
	{
		return m_bFillingSpace;
	}

	void ImwWindow::SetFillingSpace(bool bFilling)
	{
		m_bFillingSpace = bFilling;
	}

#ifdef IMW_USE_LAYOUT_SERIALIZATION
	void ImwWindow::GetParameters(JsonStthm::JsonValue& /*oOutParameters*/)
	{
	}

	void ImwWindow::SetParameters(const JsonStthm::JsonValue& /*oParameters*/)
	{
	}
#endif //IMW_USE_LAYOUT_SERIALIZATION

	ImU32 ImwWindow::GetId() const
	{
		return m_iId;
	}

	const char* ImwWindow::GetIdStr() const
	{
		return m_pId;
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

	void ImwWindow::SetClosable( bool bClosable )
	{
		m_bClosable = bClosable;
	}

	bool ImwWindow::IsClosable() const
	{
		return m_bClosable;
	}

	EWindowMode ImwWindow::GetWindowMode() const
	{
		return m_eMode;
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