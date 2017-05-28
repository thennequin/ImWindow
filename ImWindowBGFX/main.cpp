
#include "bgfx\bgfx.h"

#include "ImwWindowManagerBGFX.h"

class MyMenu : public ImWindow::ImwMenu
{
public:
	virtual void OnMenu()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
				ImWindow::ImwWindowManager::GetInstance()->Destroy();
			ImGui::EndMenu();
		}
	}
};

class MyWindow : public ImWindow::ImwWindow
{
public:
	MyWindow()
	{
		SetTitle("My window");
	}
	virtual void OnGui()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
};

class MyWindow2 : public ImWindow::ImwWindow
{
public:
	MyWindow2()
	{
		SetTitle("My window 2");
		memset(m_pBuffer, 0, sizeof(m_pBuffer));
	}

	virtual void OnGui()
	{
		ImGui::Text("super\ntest");
		
		ImGui::InputText("Input text", m_pBuffer, 256);
	}

	char m_pBuffer[256];
};

int main()
{
	ImWindow::ImwWindowManagerBGFX oManager(bgfx::RendererType::Direct3D11);

	new MyMenu();
	new MyWindow();
	new MyWindow2();

	if (oManager.Init())
	{
		oManager.SetMainTitle("Sample");

		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.095f, 0.095f, 0.095f, 1.f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.204f, 0.204f, 0.204f, 1.f);
		style.Colors[ImGuiCol_MenuBarBg] = style.Colors[ImGuiCol_WindowBg];

		while (oManager.Run(false) && oManager.Run(true));
	}
}