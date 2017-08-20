
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "ImwWindowManagerDX11.h"
#include "imgui.h"

#include "NodeWindow.h"

//#define CONSOLE

#ifndef CONSOLE
#include <Windows.h>
#endif

#ifdef _WIN32
#define ImwNewline "\r\n"
#else
#define ImwNewline "\n"
#endif

using namespace ImWindow;

class MyStatusBar : public ImwStatusBar
{
public:
	MyStatusBar()
	{
		m_fTime = 0.f;
	}

	virtual void OnStatusBar()
	{
		m_fTime += ImGui::GetIO().DeltaTime;
		ImGui::Text("My status bar");
		ImGui::SameLine();
		while (m_fTime > 5.f)
			m_fTime -= 5.f;
		ImGui::ProgressBar(m_fTime / 5.f, ImVec2(150.f, 0.f));
	}

	float m_fTime;
};

class MyToolBar : public ImwToolBar
{
public:
	virtual void OnToolBar()
	{
		ImGui::Text("My tool bar");
	}
};

class MyMenu : public ImwMenu
{
public:
	MyMenu()
		: ImwMenu( -1 )
	{
	}

	virtual void OnMenu()
	{
		if (ImGui::BeginMenu("My menu"))
		{
			if (ImGui::MenuItem("Show content", NULL, ImWindow::ImwWindowManager::GetInstance()->GetMainPlatformWindow()->IsShowContent()))
			{
				ImWindow::ImwWindowManager::GetInstance()->GetMainPlatformWindow()->SetShowContent(!ImWindow::ImwWindowManager::GetInstance()->GetMainPlatformWindow()->IsShowContent());
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{
				ImWindow::ImwWindowManager::GetInstance()->Destroy();
			}

			ImGui::EndMenu();
		}
	}
};

class MyImwWindow3 : public ImwWindow
{
public:
	MyImwWindow3(const char* pTitle = "MyImwWindow3")
	{
		SetTitle(pTitle);
	}

	virtual void OnGui()
	{
		if (!IsClosable())
		{
			ImGui::TextWrapped("I'm not closeable.\n");
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
			ImGui::TextWrapped("So you can't close the platform window\nuntil closing me.");
			ImGui::PopStyleColor();
			ImGui::TextWrapped("For close me, click on button 'Close'");
			ImGui::Separator();
		}

		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (ImGui::Button("Close"))
		{
			Destroy();
		}
	}
};

class MyImwWindow2 : public ImwWindow
{
public:
	MyImwWindow2(const char* pTitle = "MyImwWindow2")
	{
		SetTitle(pTitle);
	}

	virtual void OnGui()
	{
		static float f = 0.0f;
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	}
};

class MyImwWindowFillSpace : public ImwWindow
{
public:
	MyImwWindowFillSpace()
	{
		SetTitle("Filling space");
		SetFillingSpace(true);
	}

	virtual void OnGui()
	{
		ImVec2 oSize = ImGui::GetContentRegionAvail();
		ImGui::Selectable("I'm a selectable\nwho fills the whole space", true, 0, oSize);
	}
};

class MyImwWindow : public ImwWindow, ImwMenu
{
public:
	MyImwWindow(const char* pTitle = "MyImwWindow")
		: ImwMenu( 0, false )
	{
		SetTitle(pTitle);
		SetAlone(true);
	}
	virtual void OnGui()
	{
		ImGui::Text("Hello, world! I'm an alone window");

		if (ImGui::Button("Create new MyImwWindow3"))
		{
			new MyImwWindow3();
		}
	}

	virtual void OnContextMenu()
	{
		if (ImGui::MenuItem("Focus"))
		{
			ImwWindowManager::GetInstance()->FocusWindow(this);
		}
	}

	virtual void OnMenu()
	{
		if (ImGui::BeginMenu("MyImwWindow"))
		{
			if (ImGui::MenuItem("Create new MyImwWindow3"))
			{
				new MyImwWindow3();
			}
			ImGui::EndMenu();
		}
	}
};

class StyleEditorWindow : public ImwWindow
{
public:
	StyleEditorWindow()
	{
		SetTitle("Style Editor");
	}
	virtual void OnGui()
	{
		ImGuiStyle* ref = NULL;
		
		ImGuiStyle& style = ImGui::GetStyle();

		ImwWindowManager::Config& oConfig = ImwWindowManager::GetInstance()->GetConfig();

		ImGui::Checkbox("Visible dragger", &oConfig.m_bVisibleDragger);
		ImGui::Text("Tab color mode");
		ImGui::RadioButton("1. Button", (int*)&oConfig.m_eTabColorMode, ImwWindowManager::E_TABCOLORMODE_TITLE);
		ImGui::RadioButton("2. Background", (int*)&oConfig.m_eTabColorMode, ImwWindowManager::E_TABCOLORMODE_BACKGROUND);
		ImGui::RadioButton("3. Custom", (int*)&oConfig.m_eTabColorMode, ImwWindowManager::E_TABCOLORMODE_CUSTOM);
		
		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("Drag margin ratio", &oConfig.m_fDragMarginRatio, 0.01f, 0.f, 1.f);
		ImGui::DragFloat("Drag margin size ratio", &oConfig.m_fDragMarginSizeRatio, 0.01f, 0.f, 1.f);
		ImGui::PopItemWidth();

		ImGui::Checkbox("Show tab border", &oConfig.m_bShowTabBorder);
		ImGui::Checkbox("Show tab shadows", &oConfig.m_bShowTabShadows);
		//ImGui::Color("Drag margin ratio", &oConfig.m_oHightlightAreaColor);

		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("Tab overlap", &oConfig.m_fTabOverlap);
		ImGui::DragFloat("Tab Slop Width", &oConfig.m_fTabSlopWidth);
		ImGui::DragFloat("Tab fTabSlopP1Ratio", &oConfig.m_fTabSlopP1Ratio, 0.01f);
		ImGui::DragFloat("Tab fTabSlopP2Ratio", &oConfig.m_fTabSlopP2Ratio, 0.01f);
		ImGui::DragFloat("Tab fTabSlopHRatio", &oConfig.m_fTabSlopHRatio, 0.01f);
		ImGui::DragFloat("Tab fTabShadowDropSize", &oConfig.m_fTabShadowDropSize, 0.01f);
		ImGui::DragFloat("Tab fTabShadowSlopRatio", &oConfig.m_fTabShadowSlopRatio, 0.01f);
		ImGui::DragFloat("Tab fTabShadowAlpha", &oConfig.m_fTabShadowAlpha, 0.01f);
		ImGui::PopItemWidth();

		ImGui::Separator();

		const ImGuiStyle def; // Default style
		if (ImGui::Button("Revert Style"))
			style =  def;
		if (ref)
		{
			ImGui::SameLine();
			if (ImGui::Button("Save Style"))
				*ref = style;
		}

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.55f);

		if (ImGui::TreeNode("Rendering"))
		{
			ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
			ImGui::Checkbox("Anti-aliased shapes", &style.AntiAliasedShapes);
			ImGui::PushItemWidth(100);
			ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, 2.0f);
			if (style.CurveTessellationTol < 0.0f) style.CurveTessellationTol = 0.10f;
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sizes"))
		{
			ImGui::SliderFloat("Alpha", &style.Alpha, 0.20f, 1.0f, "%.2f");                 // Not exposing zero here so user doesn't "lose" the UI. But application code could have a toggle to switch between zero and non-zero.
			ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
			ImGui::SliderFloat("ChildWindowRounding", &style.ChildWindowRounding, 0.0f, 16.0f, "%.0f");
			ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 16.0f, "%.0f");
			ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
			ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
			ImGui::SliderFloat("ScrollbarWidth", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
			ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
			ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
			ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 16.0f, "%.0f");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Colors"))
		{
			static int output_dest = 0;
			static bool output_only_modified = false;
			if (ImGui::Button("Output Colors"))
			{
				if (output_dest == 0)
					ImGui::LogToClipboard();
				else
					ImGui::LogToTTY();
				ImGui::LogText("ImGuiStyle& style = ImGui::GetStyle();" ImwNewline);
				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					const ImVec4& col = style.Colors[i];
					const char* name = ImGui::GetStyleColName(i);
					if (!output_only_modified || memcmp(&col, (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
						ImGui::LogText("style.Colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" ImwNewline, name, 22 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
				}
				ImGui::LogFinish();
			}
			ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY"); ImGui::PopItemWidth();
			ImGui::SameLine(); ImGui::Checkbox("Only Modified Fields", &output_only_modified);

			static ImGuiColorEditMode edit_mode = ImGuiColorEditMode_RGB;
			ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditMode_RGB);
			ImGui::SameLine();
			ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditMode_HSV);
			ImGui::SameLine();
			ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditMode_HEX);
			//ImGui::Text("Tip: Click on colored square to change edit mode.");

			static ImGuiTextFilter filter;
			filter.Draw("Filter colors", 200);

			ImGui::BeginChild("#colors", ImVec2(0, 300), true);
			ImGui::PushItemWidth(-160);
			ImGui::ColorEditMode(edit_mode);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const char* name = ImGui::GetStyleColName(i);
				if (!filter.PassFilter(name))
					continue;
				ImGui::PushID(i);
				ImGui::ColorEdit4(name, (float*)&style.Colors[i], true);
				if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
				{
					ImGui::SameLine(); if (ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : def.Colors[i];
					if (ref) { ImGui::SameLine(); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
				}
				ImGui::PopID();
			}
			ImGui::PopItemWidth();
			ImGui::EndChild();

			ImGui::TreePop();
		}

		ImGui::PopItemWidth();
	}
};

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ImwWindowManagerDX11 oMgr;

	oMgr.Init();

	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.095f, 0.095f, 0.095f, 1.f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.204f, 0.204f, 0.204f, 1.f);
	style.Colors[ImGuiCol_MenuBarBg] = style.Colors[ImGuiCol_WindowBg];

	//oMgr.GetMainPlatformWindow()->SetPos(2000,100);
	oMgr.SetMainTitle("ImWindow Test");

	ImwWindow* pWindow1 = new MyImwWindow();

	ImwWindow* pWindow2 = new MyImwWindowFillSpace();

	ImwWindow* pWindow3 = new MyImwWindow2("MyImwWindow2(1)");
	ImwWindow* pWindow4 = new MyImwWindow2("MyImwWindow2(2)");
	ImwWindow* pWindow5 = new MyImwWindow3();
	pWindow5->SetClosable(false);

	ImwWindow* pStyleEditor = new StyleEditorWindow();
	new MyMenu();
	new MyStatusBar();
	new MyToolBar();

	//ImwWindow* pNodeWindow = new NodeWindow();

	oMgr.Dock(pWindow1);
	oMgr.Dock(pWindow2, E_DOCK_ORIENTATION_LEFT);
	oMgr.DockWith(pWindow3, pWindow2, E_DOCK_ORIENTATION_TOP);
	oMgr.DockWith(pWindow4, pWindow3, E_DOCK_ORIENTATION_CENTER);
	oMgr.DockWith(pWindow5, pWindow1, E_DOCK_ORIENTATION_BOTTOM, 0.7f);

	//oMgr.Dock(pNodeWindow, E_DOCK_ORIENTATION_LEFT);
	oMgr.Dock(pStyleEditor, E_DOCK_ORIENTATION_RIGHT, 0.375f);

	//oMgr.Dock
	//MyImwWindow* pWindow2 = new MyImwWindow(pWindow1);
	//pWindow2->SetSize(300, 200);
	//pWindow2->Show();*/

	while (oMgr.Run(false) && oMgr.Run(true)) Sleep(16);

	ImGui::Shutdown();

	return 0;
}
