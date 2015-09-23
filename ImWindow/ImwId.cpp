
#include "ImwId.h"

#include <stdio.h>

using namespace ImWindow;

int ImwId::s_iNextId = 0;

ImwId::ImwId()
{
	m_iId = s_iNextId++;
	sprintf_s( m_pId, 11, "0x%08.8X", m_iId);
}

const ImU32 ImwId::GetId() const
{
	return m_iId;
}

const char* ImwId::GetStr() const
{
	return m_pId;
}
