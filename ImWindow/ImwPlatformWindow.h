
#ifndef __IM_PLATFORM_WINDOW_H__
#define __IM_PLATFORM_WINDOW_H__

#include "ImwConfig.h"

#include "ImwId.h"
#include "ImwContainer.h"

namespace ImWindow
{
	class ImwPlatformWindow
	{
		friend class ImwWindowManager;
	public:
		ImwPlatformWindow(bool bMainWindow);
		virtual								~ImwPlatformWindow();

		virtual bool						Init(ImwPlatformWindow* pParent) = 0;

		virtual int							GetWidth() const = 0;
		virtual int							GetHeight() const = 0;

		virtual void						Show() = 0;
		virtual void						Hide() = 0;
		virtual void						SetSize(int iWidth, int iHeight) = 0;
		virtual void						SetPos(int iX, int iY) = 0;
		virtual void						SetTitle(const char* pTtile) = 0;

		bool								IsMain();

		void								Dock(ImwWindow* pWindow);
		bool								UnDock(ImwWindow* pWindow);

		ImwContainer*						GetContainer();
		ImwContainer*						HasWindow(ImwWindow* pWindow);
	protected:
		void								SetState();
		void								RestoreState();
		virtual void						Paint();
		virtual void						StartDrag() = 0;
		virtual void						StopDrag() = 0;
		virtual bool						IsDraging() = 0;

		void								PaintContainer();
		void								OnClose();
		void								OnResize(int iNewWidth, int iNewHeight);

		ImwId								m_oId;
		bool								m_bMain;
		ImwContainer*						m_pContainer;
		void*								m_pState;
		void*								m_pPreviousState;
	};

	typedef ImwList<ImwPlatformWindow*> ImwPlatformWindowList;
}

#endif // __IM_PLATFORM_WINDOW_H__