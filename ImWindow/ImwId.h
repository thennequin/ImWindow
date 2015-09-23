
#ifndef __IMW_ID_H__
#define __IMW_ID_H__

#include <imgui/imgui.h>

namespace ImWindow
{
	struct ImwId
	{
		ImwId();
		const ImU32				GetId() const;
		const char*				GetStr() const;
	private:
		ImU32					m_iId;
		char					m_pId[11];
		static int				s_iNextId;
	};
}

#endif // __IMW_ID_H__