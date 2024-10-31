
#ifndef __IM_PLATFORM_WINDOW_H__
#define __IM_PLATFORM_WINDOW_H__

#include "ImwConfig.h"

#include "ImwContainer.h"

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

	enum EPlatformWindowHoveredArea
	{
		E_PLATFORMWINDOWHOVEREDAREA_NONE,
		E_PLATFORMWINDOWHOVEREDAREA_MENU,
		E_PLATFORMWINDOWHOVEREDAREA_CAPTION,
		E_PLATFORMWINDOWHOVEREDAREA_MINIMIZE,
		E_PLATFORMWINDOWHOVEREDAREA_MAXIMIZE,
		E_PLATFORMWINDOWHOVEREDAREA_CLOSE
	};

	class IMGUI_API ImwPlatformWindow
	{
		friend class ImwWindowManager;
	public:
											ImwPlatformWindow(EPlatformWindowType eType, bool bCreateContext);
		virtual								~ImwPlatformWindow();

		virtual bool						Init(ImwPlatformWindow* pParent);
		virtual void						RegenFontTexture(ImwPlatformWindow* pMain);

		EPlatformWindowType					GetType() const;
		bool								IsMainWindow() const;
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
		ImGuiContext*						GetContext();
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

		void								Moving(bool bFirst);

#ifdef IMW_USE_LAYOUT_SERIALIZATION
		bool								Save(JsonStthm::JsonValue& oJson);
		bool								Load(const JsonStthm::JsonValue& oJson, bool bJustCheck);
#endif //IMW_USE_LAYOUT_SERIALIZATION

		EPlatformWindowType					m_eType;
		ImwContainer*						m_pContainer;
		ImGuiContext*						m_pContext;
		ImGuiContext*						m_pPreviousContext;
		bool								m_bNeedRender;
		bool								m_bShowContent;
		ImRect								m_oContentArea;
		ImVec2								m_oMovingOffset;
		ImVec2								m_oMovingStartPos;
		bool								m_bMoving;
		EPlatformWindowHoveredArea			m_eHoveredArea;
	};

	typedef ImVector<ImwPlatformWindow*> ImwPlatformWindowVector;
//SFF_END
}

#endif // __IM_PLATFORM_WINDOW_H__
