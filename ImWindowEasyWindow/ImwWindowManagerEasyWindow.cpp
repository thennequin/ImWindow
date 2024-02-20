#include "ImwWindowManagerEasyWindow.h"

#include <Windows.h>
using namespace ImWindow;

ImwWindowManagerEasyWindow::ImwWindowManagerEasyWindow(bool bCustomFrame)
	: m_bCustomFrame(bCustomFrame)
{
	m_bSelfManagedTitleBar = false;
}

ImwWindowManagerEasyWindow::~ImwWindowManagerEasyWindow()
{
}

ImVec2 ImwWindowManagerEasyWindow::GetCursorPos()
{
	//TODO: make it multiplatform
	POINT oPoint;
	::GetCursorPos(&oPoint);
	return ImVec2((float)oPoint.x, (float)oPoint.y);
}

bool ImwWindowManagerEasyWindow::IsLeftClickDown()
{
	//TODO: make it multiplatform
	return GetAsyncKeyState(VK_LBUTTON) != 0;
}

bool ImwWindowManagerEasyWindow::IsUsingCustomFrame() const
{
	return m_bCustomFrame;
}