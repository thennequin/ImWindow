
#ifndef __IM_PLATFORM_WINDOW_H__
#define __IM_PLATFORM_WINDOW_H__

#include "ImwConfig.h"

#include "ImwContainer.h"
#include "JsonValue.h"

namespace ImWindow
{
//SFF_BEGIN
	enum EPlatformWindowType
	{
		E_PLATFORM_WINDOW_TYPE_MAIN,
		E_PLATFORM_WINDOW_TYPE_SECONDARY,
		E_PLATFORM_WINDOW_TYPE_DRAG_PREVIEW,
		E_PLATFORM_WINDOW_TYPE_DIALOG_BOX
	};

	class IMGUI_API ImwPlatformWindow
	{
		friend class ImwWindowManager;
	public:
											ImwPlatformWindow(EPlatformWindowType eType, bool bCreateContext);
		virtual								~ImwPlatformWindow();

		virtual bool						Init(ImwPlatformWindow* pParent);

		EPlatformWindowType					GetType() const;
		virtual ImVec2						GetPosition() const;
		virtual ImVec2						GetSize() const;
		virtual ImVec2						GetNormalPosition() const;
		virtual ImVec2						GetNormalSize() const;
		virtual bool						IsWindowMaximized() const;
		virtual bool						IsWindowMinimized() const;

		virtual void						Show(bool bShow);
		virtual void						SetSize(int iWidth, int iHeight);
		virtual void						SetPosition(int iX, int iY);
		virtual void						SetWindowMaximized(bool bMaximized);
		virtual void						SetWindowMinimized(bool bMinimized);
		virtual void						SetTitle(const char* pTtile);
		bool								IsShowContent() const;
		void								SetShowContent(bool bShow);

		void								Dock(ImwWindow* pWindow);
		bool								UnDock(ImwWindow* pWindow);

		ImwContainer*						GetContainer();
		ImwWindow*							GetWindowAtPos(const ImVec2& oPos) const;
		const ImwContainer*					HasWindow(ImwWindow* pWindow);
		bool								FocusWindow(ImwWindow* pWindow);

		bool								HasContext() const;
		ImGuiContext*						GetContext() { return (m_pContext != NULL) ? m_pContext : ImGui::GetCurrentContext(); }
		void								SetContext(bool bCopyStyle);
		void								RestoreContext(bool bCopyStyle);
		static bool							IsContextSet();
	protected:
		virtual void						PreUpdate();
		virtual void						PreRender();
		virtual void						OnOverlay();
		virtual void						RenderDrawLists(ImDrawData* pDrawData);

		void								PreDestroy();
		void								OnFocus(bool bFocused);
		void								Render();
		void								PaintContainer();
		void								RefreshTitle();
		void								OnClose();
		void								OnDropFiles(int iCount, char** pFiles, const ImVec2& oPos);

		bool								Save(JsonValue& oJson);
		bool								Load(const JsonValue& oJson, bool bJustCheck);

		EPlatformWindowType					m_eType;
		ImwContainer*						m_pContainer;
		ImGuiContext*						m_pContext;
		ImGuiContext*						m_pPreviousContext;
		bool								m_bNeedRender;
		bool								m_bShowContent;
	};

	typedef ImwList<ImwPlatformWindow*> ImwPlatformWindowList;
//SFF_END
}

#endif // __IM_PLATFORM_WINDOW_H__