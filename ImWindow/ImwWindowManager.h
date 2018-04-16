
#ifndef __IM_WINDOW_MANAGER_H__
#define __IM_WINDOW_MANAGER_H__

#include "ImwConfig.h"
#include "ImwWindow.h"
#include "ImwMenu.h"
#include "ImwPlatformWindow.h"
#include "ImwStatusBar.h"
#include "ImwToolBar.h"
#include "JsonValue.h"

namespace ImWindow
{
//SFF_BEGIN
	class IMGUI_API ImwWindowManager
	{
		friend class ImwWindow;
		friend class ImwMenu;
		friend class ImwStatusBar;
		friend class ImwToolBar;
		friend class ImwPlatformWindow;
		friend class ImwContainer;

		enum EPlatformWindowAction
		{
			E_PLATFORM_WINDOW_ACTION_DESTROY,
			E_PLATFORM_WINDOW_ACTION_SHOW,
			E_PLATFORM_WINDOW_ACTION_HIDE,
			E_PLATFORM_WINDOW_ACTION_SET_POSITION,
			E_PLATFORM_WINDOW_ACTION_SET_SIZE,
			E_PLATFORM_WINDOW_ACTION_MAXIMIZE,
			E_PLATFORM_WINDOW_ACTION_MINIMIZE,
			E_PLATFORM_WINDOW_ACTION_RESTORE,
		};

		struct PlatformWindowAction
		{
			PlatformWindowAction(ImwPlatformWindow* pPlatformWindow, EPlatformWindowAction eAction, ImVec2 oValue = ImVec2(0.f, 0.f));
			ImwPlatformWindow*		m_pPlatformWindow;
			EPlatformWindowAction	m_eAction;
			ImVec2					m_oValue;
		};

		struct DockAction
		{
			ImwWindow*				m_pWindow;
			// Is Dock or Float
			bool					m_bFloat;
			//For Docking
			ImwWindow*				m_pWith;
			EDockOrientation		m_eOrientation;
			ImwPlatformWindow*		m_pToPlatformWindow;
			ImwContainer*			m_pToContainer;
			int						m_iPosition;
			float					m_fRatio;
			//For Floating
			ImVec2					m_oPosition;
			ImVec2					m_oSize;
		};

		struct DrawWindowAreaAction
		{
			DrawWindowAreaAction( ImwPlatformWindow* pWindow, const ImVec2& oRectPos, const ImVec2& oRectSize, const ImColor& oColor );
			ImwPlatformWindow*		m_pWindow;
			ImVec2					m_oRectPos;
			ImVec2					m_oRectSize;
			ImColor					m_oColor;
		};
	public:
		enum ETabColorMode
		{
			E_TABCOLORMODE_TITLE,
			E_TABCOLORMODE_BACKGROUND,
			E_TABCOLORMODE_CUSTOM
		};

		struct Config
		{
			Config();
			float					m_fDragMarginRatio;
			float					m_fDragMarginSizeRatio;
			ImColor					m_oHightlightAreaColor;
			ETabColorMode			m_eTabColorMode;
			bool					m_bVisibleDragger;
			bool					m_bShowTabBorder;
			bool					m_bShowTabShadows;

			ImColor					m_oTabColorNormal;
			ImColor					m_oTabColorActive;
			ImColor					m_oTabColorBorder;
			float					m_fTabOverlap;
			float					m_fTabSlopWidth;
			float					m_fTabSlopP1Ratio;
			float					m_fTabSlopP2Ratio;
			float					m_fTabSlopHRatio;
			float					m_fTabShadowDropSize;
			float					m_fTabShadowSlopRatio;
			float					m_fTabShadowAlpha;

			ImVec2					m_oStatusBarWindowPadding;
			ImVec2					m_oStatusBarFramePadding;
		};
	public:
		ImwWindowManager();
		virtual								~ImwWindowManager();

		bool								Init();
		bool								Run(bool bRender);
		void								Destroy();

		ImwPlatformWindow*					GetMainPlatformWindow() const;
		const ImwPlatformWindowList&		GetSecondariesPlatformWindows() const;
		ImwPlatformWindow*					GetFocusedPlatformWindow() const;
		Config&								GetConfig();

		void								SetMainTitle(const ImwChar* pTitle);
		const ImwChar*						GetMainTitle() const;

		void								Dock(ImwWindow* pWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, float fRatio = 0.5f, ImwPlatformWindow* pToPlatformWindow = NULL);
		void								DockTo(ImwWindow* pWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, float fRatio = 0.5f, ImwContainer* pContainer = NULL, int iPosition = -1);
		void								DockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, float fRatio = 0.5f);
		void								Float(ImwWindow* pWindow, const ImVec2& oPosition = ImVec2(-1, -1), const ImVec2& oSize = ImVec2(-1, -1));

		void								FocusWindow(ImwWindow* pWindow);

		const ImwWindowList&				GetWindowList() const;
		ImwPlatformWindow*					GetCurrentPlatformWindow();
		ImwPlatformWindow*					GetWindowParent(ImwWindow* pWindow);

		void								MaximizeCurrentPlatformWindow();
		void								MinimizeCurrentPlatformWindow();
		void								RestoreCurrentPlatformWindow();

		bool								HasWantCaptureKeyboard() const { return m_bHasWantCaptureKeyboard; }
		bool								HasWantCaptureMouse() const { return m_bHasWantCaptureMouse; }

		bool								SaveLayoutToString(ImwString& sLayout, bool bCompact = false);
		bool								SaveLayoutToFile(const ImwChar* pFilePath, bool bCompact = false);

		bool								LoadLayoutFromString(const ImwChar* pLayout);
		bool								LoadLayoutFromFile(const ImwChar* pFilePath);

		virtual const ImwChar*				GetWindowClassName(ImwWindow* pWindow);
		virtual bool						CanCreateWindowByClassName(const ImwChar* pName);
		virtual ImwWindow*					CreateWindowByClassName(const ImwChar* pName);
	protected:
		//To override for use multi window mode
		virtual bool						CanCreateMultipleWindow();
		virtual bool						InternalInit();
		virtual void						InternalDestroy();
		virtual ImwPlatformWindow*			CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);
		virtual ImVec2						GetCursorPos();
		virtual bool						IsLeftClickDown();
		virtual void						PreRender();
		virtual void						PostRender();

		void								AddWindow(ImwWindow* pWindow);
		void								RemoveWindow(ImwWindow* pWindow);
		void								DestroyWindow(ImwWindow* pWindow);

		void								AddStatusBar(ImwStatusBar* pStatusBar);
		void								RemoveStatusBar(ImwStatusBar* pStatusBar);
		void								DestroyStatusBar(ImwStatusBar* pStatusBar);

		void								AddMenu(ImwMenu* pMenu);
		void								RemoveMenu(ImwMenu* pMenu);
		void								DestroyMenu(ImwMenu* pWindow);

		void								AddToolBar(ImwToolBar* pToolBar);
		void								RemoveToolBar(ImwToolBar* pToolBar);
		void								DestroyToolBar(ImwToolBar* pToolBar);

		void								UnDock(ImwWindow* pWindow);
		void								InternalDock(ImwWindow* pWindow, EDockOrientation eOrientation, float fRatio, ImwPlatformWindow* pToPlatformWindow);
		void								InternalDockTo(ImwWindow* pWindow, EDockOrientation eOrientation, float fRatio, ImwContainer* pToContainer, int iPosition);
		void								InternalDockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation, float fRatio);
		void								InternalFloat(ImwWindow* pWindow, ImVec2 oPosition, ImVec2 oSize);
		void								InternalUnDock(ImwWindow* pWindow);

		void								OnClosePlatformWindow(ImwPlatformWindow* pWindow);

		void								DrawWindowArea( ImwPlatformWindow* pWindow, const ImVec2& oPos, const ImVec2& oSize, const ImColor& oColor );

		void								PreUpdate();
		void								Update();
		void								UpdatePlatformwWindowActions();
		void								UpdateDockActions();
		void								UpdateOrphans();
		void								Render();

		void								Paint(ImwPlatformWindow* pWindow);
		void								PostPaint(ImwPlatformWindow* pWindow);

		void								PushStyle(bool bRounding = true, bool bPadding = true);
		void								PopStyle();

		void								StartDragWindow(ImwWindow* pWindow, ImVec2 oOffset);
		void								StopDragWindow();
		void								UpdateDragWindow();
		ImwWindow*							GetDraggedWindow() const;
		ImVec2								GetDragOffset() const;
		ImwContainer*						GetDragBestContainer() const;
		bool								GetDragOnTabArea() const;
		int									GetDragTabPosition() const;
		const ImwContainer*					GetBestDocking(ImwPlatformWindow* pPlatformWindow, const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, float& fOutRatio, bool& bOutOnTabArea, int& iOutPosition, bool bLargeCheck);
		
		Config								m_oConfig;
		ImwChar*							m_pMainTitle;
		ImwPlatformWindow*					m_pMainPlatformWindow;
		ImwPlatformWindowList				m_lPlatformWindows;
		ImwPlatformWindow*					m_pDragPlatformWindow;
		ImwWindowList						m_lWindows;
		ImwWindowList						m_lOrphanWindows;
		ImwWindowList						m_lToDestroyWindows;
		ImwStatusBarList					m_lStatusBars;
		ImwStatusBarList					m_lToDestroyStatusBars;
		ImwToolBarList						m_lToolBars;
		ImwToolBarList						m_lToDestroyToolBars;
		ImwMenuList							m_lMenus;
		ImwMenuList							m_lToDestroyMenus;
		ImwPlatformWindowList				m_lToDestroyPlatformWindows;
		ImwList<PlatformWindowAction*>		m_lPlatformWindowActions;
		ImwList<DockAction*>				m_lDockActions;
		ImwList<DrawWindowAreaAction>		m_lDrawWindowAreas;

		ImwPlatformWindow*					m_pCurrentPlatformWindow;
		ImwPlatformWindow*					m_pFocusedPlatformWindow;
		ImwWindow*							m_pDraggedWindow;
		bool								m_bDragOnTab;
		ImwContainer*						m_pDragBestContainer;
		int									m_iDragBestContainerPosition;

		ImVec2								m_oDragPreviewOffset;

		float								m_fStyleBackupWindowRounding;
		ImVec2								m_oStyleBackupWindowPadding;
		ImVec2								m_oStyleBackupItemInnerSpacing;
		ImVec2								m_oStyleBackupItemSpacing;

		bool								m_bHasWantCaptureKeyboard;
		bool								m_bHasWantCaptureMouse;

		// Static
	public:
		static ImwWindowManager*			GetInstance();
	protected:
		static ImwWindowManager*			s_pInstance;
	};
//SFF_END
}


#endif // __IM_WINDOW_MANAGER_H__