
#ifndef __IM_WINDOW_MANAGER_H__
#define __IM_WINDOW_MANAGER_H__

#include "ImwConfig.h"
#include "ImwWindow.h"
#include "ImwMenu.h"
#include "ImwPlatformWindow.h"
#include "ImwStatusBar.h"
#include "ImwToolBar.h"

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

			ImVec2					m_oStatusBarFramePadding;
		};

		struct ClassNameFunctions
		{
			ClassNameFunctions();

			const char*				(*m_pGetClassName)(ImwWindow* pWindow);
			bool					(*m_pCanCreateWindowByClassName)(const char* pName);
			ImwWindow*				(*m_pCreateWindowByClassName)(const char* pName);
		};
	public:
		ImwWindowManager();
		virtual								~ImwWindowManager();

		bool								Init();
		bool								Run(bool bRender);
		void								Destroy();
		bool								IsExiting() const;

		ImGuiContext*						GetContext() const;
		ImwPlatformWindow*					GetMainPlatformWindow() const;
		const ImwPlatformWindowVector&		GetSecondariesPlatformWindows() const;
		ImwPlatformWindow*					GetFocusedPlatformWindow() const;
		Config&								GetConfig();

		void								SetMainTitle(const char* pTitle);
		const char*							GetMainTitle() const;

		void								Dock(ImwWindow* pWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, float fRatio = 0.5f, ImwPlatformWindow* pToPlatformWindow = NULL);
		void								DockTo(ImwWindow* pWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, float fRatio = 0.5f, ImwContainer* pContainer = NULL, int iPosition = -1);
		void								DockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, float fRatio = 0.5f);
		void								Float(ImwWindow* pWindow, const ImVec2& oPosition = ImVec2(-1, -1), const ImVec2& oSize = ImVec2(-1, -1));

		void								FocusWindow(ImwWindow* pWindow);

		const ImwWindowVector&				GetWindowList() const;
		ImwPlatformWindow*					GetCurrentPlatformWindow();
		ImwPlatformWindow*					GetWindowParent(ImwWindow* pWindow);

		void								MaximizeCurrentPlatformWindow();
		void								MinimizeCurrentPlatformWindow();
		void								RestoreCurrentPlatformWindow();

		bool								HasWantCaptureKeyboard() const { return m_bHasWantCaptureKeyboard; }
		bool								HasWantCaptureMouse() const { return m_bHasWantCaptureMouse; }

		bool								SaveLayoutToString(ImwString& sLayout, bool bCompact = false);
		bool								SaveLayoutToFile(const char* pFilePath, bool bCompact = false);

		bool								LoadLayoutFromString(const char* pLayout);
		bool								LoadLayoutFromFile(const char* pFilePath);

		void								SetClassNameFunctions( const ClassNameFunctions* pFunctions );
		virtual const char*					GetWindowClassName(ImwWindow* pWindow);
		virtual bool						CanCreateWindowByClassName(const char* pName);
		virtual ImwWindow*					CreateWindowByClassName(const char* pName);

		virtual bool						IsUsingCustomFrame() const;
		void								RegenFontTexture();
	protected:
		//To override for use multi window mode
		virtual bool						CanCreateMultipleWindow();
		// Init internal structures and fonts in InternalInit
		virtual bool						InternalInit();
		virtual void						InternalDestroy();
		virtual ImwPlatformWindow*			CreatePlatformWindow(EPlatformWindowType eType, ImwPlatformWindow* pParent);
		virtual ImVec2						GetCursorPos();
		virtual bool						IsLeftClickDown();
		virtual void						PreRender();
		virtual void						PostRender();

		virtual float						GetTitleBarHeight() const;
		virtual void						PaintTitleBar(ImwPlatformWindow* pPlatformWindow, bool bDrawTitle);

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

		bool								BeginTransparentChild(const char* pName, const ImVec2& oSize, bool bBorder, ImGuiWindowFlags iFlags);

		void								PreUpdate();
		void								Update();
		void								UpdatePlatformwWindowActions();
		void								UpdateDockActions();
		void								UpdateOrphans();
		void								Render();

		void								Paint(ImwPlatformWindow* pWindow);
		void								PostPaint(ImwPlatformWindow* pWindow);

		void								StartDragWindow(ImwWindow* pWindow, ImVec2 oOffset);
		void								StopDragWindow();
		void								UpdateDragWindow();
		float								GetStatusBarHeight() const;
		ImwWindow*							GetDraggedWindow() const;
		ImVec2								GetDragOffset() const;
		ImwContainer*						GetDragBestContainer() const;
		bool								GetDragOnTabArea() const;
		int									GetDragTabPosition() const;
		const ImwContainer*					GetBestDocking(ImwPlatformWindow* pPlatformWindow, const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, float& fOutRatio, bool* pOutOnTabArea, int* pOutPosition, bool bLargeCheck);

		Config								m_oConfig;
		bool								m_bSelfManagedTitleBar;
		ClassNameFunctions					m_oClassNameFunctions;
		char*								m_pMainTitle;
		ImGuiContext*						m_pImGuiContext;
		ImwPlatformWindow*					m_pMainPlatformWindow;
		ImwPlatformWindowVector				m_lPlatformWindows;
		ImwPlatformWindow*					m_pDragPlatformWindow;
		ImwWindowVector						m_lWindows;
		ImwWindowVector						m_lOrphanWindows;
		ImwWindowVector						m_lToDestroyWindows;
		ImwStatusBarVector					m_lStatusBars;
		ImwStatusBarVector					m_lToDestroyStatusBars;
		ImwToolBarVector					m_lToolBars;
		ImwToolBarVector					m_lToDestroyToolBars;
		ImwMenuVector						m_lMenus;
		ImwMenuVector						m_lToDestroyMenus;
		ImwPlatformWindowVector				m_lToDestroyPlatformWindows;
		ImVector<PlatformWindowAction*>		m_lPlatformWindowActions;
		ImVector<DockAction*>				m_lDockActions;
		ImVector<DrawWindowAreaAction>		m_lDrawWindowAreas;

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
