
#ifndef __IMW_CONTAINER_H__
#define __IMW_CONTAINER_H__

#include "ImwConfig.h"
#include "ImwWindow.h"

namespace ImWindow
{
	class ImwPlatformWindow;
//SFF_BEGIN
	class IMGUI_API ImwContainer
	{
		friend class ImwPlatformWindow;
		friend class ImwWindowManager;
	public:

		void							Dock(ImwWindow* pWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER, float fRatio = 0.5f, int iPosition = -1);
		bool							UnDock(ImwWindow* pWindow);
		bool							DockToBest(ImwWindow* pWindow);

		bool							IsEmpty() const;
		bool							IsSplit() const;
		bool							HasWindow() const;
		ImwWindow*						GetWindowAtPos(const ImVec2& oPos) const;
		const ImwContainer*				HasWindow(ImwWindow* pWindow) const;
		bool							FocusWindow(ImwWindow* pWindow);
		ImwWindow*						GetActiveWindow() const;
		ImwPlatformWindow*				GetPlatformWindowParent() const;
		const ImwContainer*				GetBestContainer(const ImVec2& oCursorPos) const;
		const ImwContainer*				GetBestDocking(const ImVec2 oCursorPosInContainer, EDockOrientation& oOutOrientation, ImVec2& oOutAreaPos, ImVec2& oOutAreaSize, bool* pOutOnTabArea, int* pOutPosition, bool bLargeCheck) const;
		bool							HasUnclosableWindow() const;

		EWindowMode						GetWindowMode() const;
	protected:
										ImwContainer(ImwContainer* pParent);
										ImwContainer(ImwPlatformWindow* pParent);
										~ImwContainer();

		void							CreateSplits();

		void							Paint();

		bool							Tab(const ImwWindow* pWindow, bool bFocused, float fStartLinePos, float fEndLinePos, float fMaxSize = -1.f);
		void							DrawTab(const char* pText, bool bFocused, ImVec2 oPos, float fStartLinePos, float fEndLinePos, const ImVec2& oSize, const ImVec2* pTextSize = NULL);
		void							GetTabSize(const char* pText, float fMaxSize, ImVec2* pOutTabSize, ImVec2* pOutTextSize = NULL) const;
		float							GetTabAreaWidth() const;
#ifdef IMW_USE_LAYOUT_SERIALIZATION
		bool							Save(JsonStthm::JsonValue& oJson);
		bool							Load(const JsonStthm::JsonValue& oJson, bool bJustCheck);
#endif //IMW_USE_LAYOUT_SERIALIZATION

		ImwContainer*					m_pParent;
		ImwPlatformWindow*				m_pParentWindow;
		ImwWindow*						m_pSpecialWindow;
		ImwWindowVector					m_lWindows;
		ImwContainer*					m_pSplits[2];

		float							m_fSplitRatio;
		bool							m_bVerticalSplit;
		int								m_iActiveWindow;

		bool							m_bIsDrag;

		ImVec2							m_oLastPosition;
		ImVec2							m_oLastSize;

		static const float				c_fTabHeight;
	};
//SFF_END
}

#endif // __IMW_CONTAINER_H__