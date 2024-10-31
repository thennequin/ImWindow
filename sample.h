#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "imgui.h"

#ifdef _WIN32
#define ImwNewline "\r\n"
#else
#define ImwNewline "\n"
#endif

//#define CONSOLE
#ifndef CONSOLE
#include <Windows.h>
#endif

using namespace ImWindow;

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

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

		ImGui::Indent();
		ImGui::ColorEdit4("Active##Tab", (float*)&oConfig.m_oTabColorActive.Value.x, true);
		ImGui::ColorEdit4("Normal##Tab", (float*)&oConfig.m_oTabColorNormal.Value.x, true);
		ImGui::ColorEdit4("Border##Tab", (float*)&oConfig.m_oTabColorBorder.Value.x, true);
		ImGui::Unindent();

		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("Drag margin ratio", &oConfig.m_fDragMarginRatio, 0.01f, 0.f, 1.f);
		ImGui::DragFloat("Drag margin size ratio", &oConfig.m_fDragMarginSizeRatio, 0.01f, 0.f, 1.f);
		ImGui::PopItemWidth();

		ImGui::Checkbox("Show tab border", &oConfig.m_bShowTabBorder);
		ImGui::Checkbox("Show tab shadows", &oConfig.m_bShowTabShadows);
		//ImGui::Color("Drag margin ratio", &oConfig.m_oHightlightAreaColor);

		ImGui::DragFloat2("StatusBarFramePadding", (float*)&oConfig.m_oStatusBarFramePadding);

		if (ImGui::SmallButton("Default##Tab"))
		{
			oConfig.m_bShowTabShadows = false;
			oConfig.m_fTabOverlap = 15.f;
			oConfig.m_fTabSlopWidth = 30.f;
			oConfig.m_fTabSlopP1Ratio = 0.6f;
			oConfig.m_fTabSlopP2Ratio = 0.4f;
			oConfig.m_fTabSlopHRatio = 0.f;
			oConfig.m_fTabShadowDropSize = 15.f;
			oConfig.m_fTabShadowSlopRatio = 0.6f;
			oConfig.m_fTabShadowAlpha = 0.75f;
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Chrome##Tab"))
		{
			oConfig.m_bShowTabShadows = true;
			oConfig.m_fTabOverlap = 10.f;
			oConfig.m_fTabSlopWidth = 15.f;
			oConfig.m_fTabSlopP1Ratio = 1.f;
			oConfig.m_fTabSlopP2Ratio = 1.f;
			oConfig.m_fTabSlopHRatio = 0.f;
			oConfig.m_fTabShadowDropSize = 7.5f;
			oConfig.m_fTabShadowSlopRatio = 1.f;
			oConfig.m_fTabShadowAlpha = 1.f;
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Square##Tab"))
		{
			oConfig.m_bShowTabShadows = false;
			oConfig.m_fTabOverlap = 3.f;
			oConfig.m_fTabSlopWidth = 0.f;
			oConfig.m_fTabSlopP1Ratio = 0.f;
			oConfig.m_fTabSlopP2Ratio = 0.f;
			oConfig.m_fTabSlopHRatio = 0.f;
			oConfig.m_fTabShadowDropSize = 7.5f;
			oConfig.m_fTabShadowSlopRatio = 1.f;
			oConfig.m_fTabShadowAlpha = 1.f;
		}

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

		if (ImGui::SmallButton("Default##Color"))
		{
			oConfig.m_eTabColorMode = ImwWindowManager::E_TABCOLORMODE_BACKGROUND;
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
			//style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.67f, 0.40f, 0.40f, 0.60f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.40f, 0.40f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("Dark##Color"))
		{
			oConfig.m_eTabColorMode = ImwWindowManager::E_TABCOLORMODE_CUSTOM;
			oConfig.m_oTabColorActive.Value = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
			oConfig.m_oTabColorNormal.Value = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);

			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
			//style.Colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.99f);
			style.Colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.19f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("Dark2##Color"))
		{
			oConfig.m_eTabColorMode = ImwWindowManager::E_TABCOLORMODE_CUSTOM;
			oConfig.m_oTabColorActive.Value = ImVec4(0.14f, 0.14f, 0.18f, 1.00f);
			oConfig.m_oTabColorNormal.Value = ImVec4(0.20f, 0.20f, 0.23f, 1.00f);

			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.10f, 1.00f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.18f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
			//style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.99f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
		}

		if (ImGui::SmallButton("Blue##Color"))
		{
			oConfig.m_eTabColorMode = ImwWindowManager::E_TABCOLORMODE_BACKGROUND;
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text] = ImVec4(0.93f, 0.94f, 0.95f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.93f, 0.94f, 0.95f, 0.58f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.24f, 0.31f, 0.95f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.18f, 0.24f, 0.92f);
			//style.Colors[ImGuiCol_PopupBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.17f, 0.24f, 0.31f, 0.00f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.17f, 0.24f, 0.31f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.27f, 0.37f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.22f, 0.31f, 0.41f, 0.75f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.22f, 0.31f, 0.41f, 0.47f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.16f, 0.50f, 0.73f, 0.21f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.16f, 0.50f, 0.73f, 0.80f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.16f, 0.50f, 0.73f, 0.50f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.16f, 0.50f, 0.73f, 0.50f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.86f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.16f, 0.50f, 0.73f, 0.76f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.86f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.16f, 0.50f, 0.73f, 0.32f);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.16f, 0.50f, 0.73f, 0.15f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.93f, 0.94f, 0.95f, 0.63f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.93f, 0.94f, 0.95f, 0.63f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.16f, 0.50f, 0.73f, 0.43f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.22f, 0.31f, 0.41f, 0.73f);
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("White##Color"))
		{
			oConfig.m_eTabColorMode = ImwWindowManager::E_TABCOLORMODE_BACKGROUND;
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text] = ImVec4(0.31f, 0.25f, 0.24f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
			//style.Colors[ImGuiCol_PopupBg] = ImVec4(0.89f, 0.98f, 1.00f, 0.99f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.62f, 0.70f, 0.72f, 0.56f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.33f, 0.14f, 0.47f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.97f, 0.31f, 0.13f, 0.81f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.42f, 0.75f, 1.00f, 0.53f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.65f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.74f, 0.74f, 0.94f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.40f, 0.62f, 0.80f, 0.15f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.64f, 0.80f, 0.30f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.67f, 0.80f, 0.59f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.48f, 0.53f, 0.67f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.48f, 0.47f, 0.47f, 0.71f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.47f, 0.99f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.79f, 0.18f, 0.78f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.82f, 1.00f, 0.81f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.72f, 1.00f, 1.00f, 0.86f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.65f, 0.78f, 0.84f, 0.80f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.88f, 0.94f, 0.80f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.68f, 0.74f, 0.80f);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.60f, 0.60f, 0.80f, 0.30f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.99f, 0.54f, 0.43f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
		}

		const ImGuiStyle def; // Default style
		if (ImGui::Button("Revert Style"))
			style = def;
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
			ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
			ImGui::PushItemWidth(100);
			ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, ImGuiSliderFlags_Logarithmic );
			if (style.CurveTessellationTol < 0.0f) style.CurveTessellationTol = 0.10f;
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sizes"))
		{
			ImGui::SliderFloat("Alpha", &style.Alpha, 0.20f, 1.0f, "%.2f");                 // Not exposing zero here so user doesn't "lose" the UI. But application code could have a toggle to switch between zero and non-zero.
			ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
			ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 16.0f, "%.0f");
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
					const char* name = ImGui::GetStyleColorName(i);
					if (!output_only_modified || memcmp(&col, (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
						ImGui::LogText("style.Colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" ImwNewline, name, 22 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
				}
				ImGui::LogFinish();
			}
			ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY"); ImGui::PopItemWidth();
			ImGui::SameLine(); ImGui::Checkbox("Only Modified Fields", &output_only_modified);

			static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_InputRGB|ImGuiColorEditFlags_DisplayRGB;
			ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_InputRGB|ImGuiColorEditFlags_DisplayRGB);
			ImGui::SameLine();
			ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_InputHSV|ImGuiColorEditFlags_DisplayHSV);
			ImGui::SameLine();
			ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);
			//ImGui::Text("Tip: Click on colored square to change edit mode.");

			static ImGuiTextFilter filter;
			filter.Draw("Filter colors", 200);

			ImGui::PushID("#colors");
			ImGui::PushItemWidth(-160);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const char* name = ImGui::GetStyleColorName(i);
				if (!filter.PassFilter(name))
					continue;
				ImGui::PushID(i);
				ImGui::ColorEdit4(name, (float*)&style.Colors[i], edit_mode);
				if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0)
				{
					ImGui::SameLine(); if (ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : def.Colors[i];
					if (ref) { ImGui::SameLine(); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
				}
				ImGui::PopID();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();

			ImGui::TreePop();
		}

		ImGui::PopItemWidth();
	}
};


class NodeWindow : public ImWindow::ImwWindow
{
public:
	NodeWindow(void)
	{
		SetTitle("Node editor");
	}

	virtual void OnGui()
	{
		// Dummy
		struct Node
		{
			int     ID;
			char    Name[32];
			ImVec2  Pos, Size;
			float   Value;
			ImVec4  Color;
			int     InputsCount, OutputsCount;

			Node(int id, const char* name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count) { ID = id; strncpy(Name, name, 31); Name[31] = 0; Pos = pos; Value = value; Color = color; InputsCount = inputs_count; OutputsCount = outputs_count; }

			ImVec2 GetInputSlotPos(int slot_no) const { return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
			ImVec2 GetOutputSlotPos(int slot_no) const { return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
		};
		struct NodeLink
		{
			int     InputIdx, InputSlot, OutputIdx, OutputSlot;

			NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
		};

		static ImVector<Node> nodes;
		static ImVector<NodeLink> links;
		static bool inited = false;
		static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
		static bool show_grid = true;
		static int node_selected = -1;
		if (!inited)
		{
			nodes.push_back(Node(0, "MainTex", ImVec2(40, 50), 0.5f, ImColor(255, 100, 100), 1, 1));
			nodes.push_back(Node(1, "BumpMap", ImVec2(40, 150), 0.42f, ImColor(200, 100, 200), 1, 1));
			nodes.push_back(Node(2, "Combine", ImVec2(270, 80), 1.0f, ImColor(0, 200, 100), 2, 2));
			links.push_back(NodeLink(0, 0, 2, 0));
			links.push_back(NodeLink(1, 0, 2, 1));
			inited = true;
		}

		// Draw a list of nodes on the left side
		bool open_context_menu = false;
		int node_hovered_in_list = -1;
		int node_hovered_in_scene = -1;
		ImGui::BeginChild("node_list", ImVec2(100, 0));
		ImGui::Text("Nodes");
		ImGui::Separator();
		for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
		{
			Node* node = &nodes[node_idx];
			ImGui::PushID(node->ID);
			if (ImGui::Selectable(node->Name, node->ID == node_selected))
				node_selected = node->ID;
			if (ImGui::IsItemHovered())
			{
				node_hovered_in_list = node->ID;
				open_context_menu |= ImGui::IsMouseClicked(1);
			}
			ImGui::PopID();
		}
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginGroup();

		const float NODE_SLOT_RADIUS = 4.0f;
		const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

		// Create our child canvas
		ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
		ImGui::SameLine(ImGui::GetWindowWidth() - 100);
		ImGui::Checkbox("Show grid", &show_grid);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(60 / 255.f, 60 / 255.f, 70 / 255.f, 200 / 255.f));
		ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
		ImGui::PushItemWidth(120.0f);

		ImVec2 offset = ImGui::GetCursorScreenPos() - scrolling;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->ChannelsSplit(2);

		// Display grid
		if (show_grid)
		{
			ImU32 GRID_COLOR = ImColor(120, 120, 120, 40);
			float GRID_SZ = 32.0f;
			ImVec2 win_pos = ImGui::GetCursorScreenPos();
			ImVec2 canvas_sz = ImGui::GetWindowSize();
			for (float x = fmodf(offset.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
				draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
			for (float y = fmodf(offset.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
				draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
		}

		// Display links
		draw_list->ChannelsSetCurrent(0); // Background
		for (int link_idx = 0; link_idx < links.Size; link_idx++)
		{
			NodeLink* link = &links[link_idx];
			Node* node_inp = &nodes[link->InputIdx];
			Node* node_out = &nodes[link->OutputIdx];
			ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot);
			ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot);
			draw_list->AddBezierCubic(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, ImColor(200, 200, 100), 3.0f);
		}

		// Display nodes
		for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
		{
			Node* node = &nodes[node_idx];
			ImGui::PushID(node->ID);
			ImVec2 node_rect_min = offset + node->Pos;

			// Display node contents first
			draw_list->ChannelsSetCurrent(1); // Foreground
			bool old_any_active = ImGui::IsAnyItemActive();
			ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
			ImGui::BeginGroup(); // Lock horizontal position
			ImGui::Text("%s", node->Name);
			ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
			ImGui::ColorEdit3("##color", &node->Color.x);
			ImGui::EndGroup();

			// Save the size of what we have emitted and whether any of the widgets are being used
			bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
			node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
			ImVec2 node_rect_max = node_rect_min + node->Size;

			// Display node box
			draw_list->ChannelsSetCurrent(0); // Background
			ImGui::SetCursorScreenPos(node_rect_min);
			ImGui::InvisibleButton("node", node->Size);
			if (ImGui::IsItemHovered())
			{
				node_hovered_in_scene = node->ID;
				open_context_menu |= ImGui::IsMouseClicked(1);
			}
			bool node_moving_active = ImGui::IsItemActive();
			if (node_widgets_active || node_moving_active)
				node_selected = node->ID;
			if (node_moving_active && ImGui::IsMouseDragging(0))
				node->Pos = node->Pos + ImGui::GetIO().MouseDelta;

			ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? ImColor(75, 75, 75) : ImColor(60, 60, 60);
			draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 3.0f);
			draw_list->AddRect(node_rect_min, node_rect_max, ImColor(100, 100, 100), 3.0f);
			for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++)
				draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, ImColor(150, 150, 150, 150));
			for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++)
				draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, ImColor(150, 150, 150, 150));

			ImGui::PopID();
		}
		draw_list->ChannelsMerge();

		// Open context menu
		if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
		{
			node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
			open_context_menu = true;
		}
		if (open_context_menu)
		{
			ImGui::OpenPopup("context_menu");
			if (node_hovered_in_list != -1)
				node_selected = node_hovered_in_list;
			if (node_hovered_in_scene != -1)
				node_selected = node_hovered_in_scene;
		}

		// Draw context menu
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::BeginPopup("context_menu"))
		{
			Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
			ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
			if (node)
			{
				ImGui::Text("Node '%s'", node->Name);
				ImGui::Separator();
				if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
				if (ImGui::MenuItem("Delete", NULL, false, false)) {}
				if (ImGui::MenuItem("Copy", NULL, false, false)) {}
			}
			else
			{
				if (ImGui::MenuItem("Add")) { nodes.push_back(Node(nodes.Size, "New node", scene_pos, 0.5f, ImColor(100, 100, 200), 2, 2)); }
				if (ImGui::MenuItem("Paste", NULL, false, false)) {}
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		// Scrolling
		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
			scrolling = scrolling - ImGui::GetIO().MouseDelta;

		ImGui::PopItemWidth();
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
		ImGui::EndGroup();
	}
};

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
		ImGui::SameLine();
		if (ImGui::SmallButton("Open Node window"))
		{
			new NodeWindow();
		}
	}
};

class MyMenu : public ImwMenu
{
public:
	MyMenu()
		: ImwMenu(-1)
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

		ImwWindowManager* pManager = ImwWindowManager::GetInstance();
		if (pManager->GetFocusedPlatformWindow() == pManager->GetCurrentPlatformWindow())
			ImGui::Text("Platform Window is focused");
		else
			ImGui::TextDisabled("Platform Window is not focused");
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
		: ImwWindow(ImWindow::E_WINDOW_MODE_ALONE)
		, ImwMenu(0, false)
	{
		SetTitle(pTitle);
		m_pText[0] = 0;
	}
	virtual void OnGui()
	{
		ImGui::Text("Hello, world! I'm an alone window");

		if (ImGui::Button("Create new MyImwWindow3"))
		{
			new MyImwWindow3();
		}

		ImGui::InputText("Input", m_pText, 512);

		//ImGui::ShowMetricsWindow();
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

	char m_pText[512];
};

class PlaceholderWindow : public ImWindow::ImwWindow
{
public:
	PlaceholderWindow()
		: ImwWindow(ImWindow::E_WINDOW_MODE_PLACEHOLDER)
	{
	}

	virtual void OnGui()
	{
		ImGui::Text("I'm a placeholder");
		ImGui::Text("Drag & drop window on me");
	}
};

void PreInitSample()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

void InitSample()
{
	ImWindow::ImwWindowManager& oMgr = *ImWindow::ImwWindowManager::GetInstance();
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.095f, 0.095f, 0.095f, 1.f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.204f, 0.204f, 0.204f, 1.f);
	style.Colors[ImGuiCol_MenuBarBg] = style.Colors[ImGuiCol_WindowBg];

	//oMgr.GetMainPlatformWindow()->SetPos(2000,100);
	oMgr.SetMainTitle("ImWindow sample");

	ImwWindow* pWindowPlaceholder = new PlaceholderWindow();

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

	oMgr.Dock(pWindow1);
	oMgr.Dock(pWindow2, E_DOCK_ORIENTATION_LEFT);
	oMgr.DockWith(pWindowPlaceholder, pWindow2, E_DOCK_ORIENTATION_BOTTOM);
	oMgr.DockWith(pWindow3, pWindow2, E_DOCK_ORIENTATION_TOP);
	oMgr.DockWith(pWindow4, pWindow3, E_DOCK_ORIENTATION_CENTER);
	oMgr.DockWith(pWindow5, pWindow1, E_DOCK_ORIENTATION_BOTTOM, 0.7f);

	//oMgr.Dock(pNodeWindow, E_DOCK_ORIENTATION_LEFT);
	oMgr.Dock(pStyleEditor, E_DOCK_ORIENTATION_RIGHT, 0.375f);

	//oMgr.Dock
	//MyImwWindow* pWindow2 = new MyImwWindow(pWindow1);
	//pWindow2->SetSize(300, 200);
	//pWindow2->Show();*/

}