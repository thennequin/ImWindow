
#ifndef __IMW_CONTAINER_H__
#define __IMW_CONTAINER_H__

#include "ImwConfig.h"
#include "ImwWindow.h"

namespace ImWindow
{
	class ImwContainer
	{
		friend class ImwPlatformWindow;
	public:

		void							Dock(ImwWindow* pWindow, EDockOrientation eOrientation = E_DOCK_ORIENTATION_CENTER);
		bool							UnDock(ImwWindow* pWindow);

		bool							IsEmpty();
		bool							IsSplit();
		bool							HasWindowTabbed();
		ImwContainer*					HasWindow(const ImwWindow* pWindow);

	protected:
										ImwContainer(ImwContainer* pParent);
										~ImwContainer();

		void							CreateSplits();

		void							Paint();

		ImwContainer*					m_pParent;
		ImwWindowList					m_lWindows;
		ImwContainer*					m_pSplits[2];

		float							m_fSplitRatio;
		bool							m_bVerticalSplit;
		int								m_iActiveWindow;

		bool							m_bIsDrag;
		float							m_fDragSplitStart;
	};
}

#endif // __IMW_CONTAINER_H__