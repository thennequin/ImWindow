
#ifndef __IM_WINDOW_MANAGER_H__
#define __IM_WINDOW_MANAGER_H__

#include "ImwConfig.h"
#include "ImwWindow.h"
#include "ImwPlatformWindow.h"

namespace ImWindow
{
	class ImwWindow;
//SFF_BEGIN
	class IMGUI_API ImwWindowManager
	{
		friend class ImwWindow;
		friend class ImwPlatformWindow;
		friend class ImwContainer;

		enum EPlatformWindowAction
		{
			E_PLATFORM_WINDOW_ACTION_DESTROY			= 1,
			E_PLATFORM_WINDOW_ACTION_SHOW			= 2,
			E_PLATFORM_WINDOW_ACTION_HIDE			= 4,
			E_PLATFORM_WINDOW_ACTION_SET_POSITION	= 8,
			E_PLATFORM_WINDOW_ACTION_SET_SIZE		= 16,
		};

		struct PlatformWindowAction
		{
			ImwPlatformWindow*		m_pPlatformWindow;
			unsigned int			m_iFlags;
			ImVec2					m_oPosition;
			ImVec2					m_oSize;
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
		struct Config
		{
			Config();
			float					m_fDragMarginRatio;
			float					m_fDragMarginSizeRatio;
			ImColor					m_oHightlightAreaColor;
		};
	public:
		ImwWindowManager();
		virtual								~ImwWindowManager();

		bool								Init();
		bool								Run();
		void								Exit();

		ImwPlatformWindow*					GetMainPlatformWindow();
		Config&								GetConfig();

		void								SetMainTitle(const char* pTitle);

		void								Dock(ImwWindow* pWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, ImwPlatformWindow* pToPlatformWindow = NULL);
		void								DockTo(ImwWindow* pWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, ImwContainer* pContainer = NULL);
		void								DockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER);
		void								Float(ImwWindow* pWindow, const ImVec2& oPosition = ImVec2(-1, -1), const ImVec2& oSize = ImVec2(-1, -1));

		const ImwWindowList&				GetWindowList() const;
		ImwPlatformWindow*					GetCurrentPlatformWindow();
		ImwPlatformWindow*					GetWindowParent(ImwWindow* pWindow);

	protected:
		virtual bool						CanCreateMultipleWindow();
		virtual ImwPlatformWindow*			CreatePlatformWindow(bool bMain, ImwPlatformWindow* pParent, bool bDragWindow) = 0;
		virtual void						InternalRun() = 0;
		virtual ImVec2						GetCursorPos() = 0;
		virtual bool						IsLeftClickDown() = 0;

		void								AddWindow(ImwWindow* pWindow);
		void								RemoveWindow(ImwWindow* pWindow);
		void								DestroyWindow(ImwWindow* pWindow);

		void								UnDock(ImwWindow* pWindow);
		void								InternalDock(ImwWindow* pWindow, EDockOrientation eOrientation, ImwPlatformWindow* pToPlatformWindow);
		void								InternalDockTo(ImwWindow* pWindow, EDockOrientation eOrientation, ImwContainer* pToContainer);
		void								InternalDockWith(ImwWindow* pWindow, ImwWindow* pWithWindow, EDockOrientation eOrientation);
		void								InternalFloat(ImwWindow* pWindow, ImVec2 oPosition, ImVec2 oSize);
		void								InternalUnDock(ImwWindow* pWindow);
		void								InternalDrag(ImwWindow* pWindow);

		void								OnClosePlatformWindow(ImwPlatformWindow* pWindow);

		void								DrawWindowArea( ImwPlatformWindow* pWindow, const ImVec2& oPos, const ImVec2& oSize, const ImColor& oColor );

		void								PreUpdate();
		void								Update();
		void								UpdatePlatformwWindowActions();
		void								UpdateDockActions();
		void								UpdateOrphans();

		void								Paint(ImwPlatformWindow* pWindow);

		void								StartDragWindow(ImwWindow* pWindow);
		void								StopDragWindow();
		void								UpdateDragWindow();
		ImwContainer*						GetBestDocking(ImwPlatformWindow* pPlatformWindow, const ImVec2 oCursorPos, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, bool bLargeCheck);
		
		Config								m_oConfig;
		ImwPlatformWindow*					m_pMainPlatformWindow;
		ImwPlatformWindowList				m_lPlatformWindows;
		ImwPlatformWindow*					m_pDragPlatformWindow;
		ImwWindowList						m_lWindows;
		ImwWindowList						m_lOrphanWindows;
		ImwWindowList						m_lToDestroyWindows;
		ImwPlatformWindowList				m_lToDestroyPlatformWindows;
		ImwList<PlatformWindowAction*>		m_lPlatformWindowActions;
		ImwList<DockAction*>				m_lDockActions;
		ImwList<DrawWindowAreaAction>		m_lDrawWindowAreas;

		ImwPlatformWindow*					m_pCurrentPlatformWindow;
		ImwWindow*							m_pDraggedWindow;

		ImVec2								m_oDragPreviewOffset;

		// Static
	public:
		static ImwWindowManager*			GetInstance();
	protected:
		static ImwWindowManager*			s_pInstance;
	};
//SFF_END
}


#endif // __IM_WINDOW_MANAGER_H__