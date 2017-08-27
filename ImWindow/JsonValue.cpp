
#include "JsonValue.h"

namespace ImWindow
{
//SFF_BEGIN

	JsonValue::JsonMember::JsonMember(const ImwChar* pName, JsonValue* pValue)
	{
		m_pName = NULL;
		SetName(pName);
		m_pValue = pValue;
	}

	JsonValue::JsonMember::JsonMember(const JsonMember& oSource)
	{
		m_pName = NULL;
		SetName(oSource.m_pName);
		m_pValue = new JsonValue(*oSource.m_pValue);
	}

	JsonValue::JsonMember::~JsonMember()
	{
		ImwSafeFree(m_pName);
		delete m_pValue;
	}

	void JsonValue::JsonMember::SetName(const ImwChar* pName)
	{
		ImwSafeFree(m_pName);
		if (pName != NULL)
		{
			size_t iSize = (strlen(pName) + 1) * sizeof(ImwChar);
			m_pName = (ImwChar*)ImwMalloc(iSize);
			memcpy(m_pName, pName, iSize);
		}
	}

	JsonValue JsonValue::INVALID = JsonValue::CreateConst();

	JsonValue::JsonValue()
		: m_bConst( false )
		, m_eType( E_TYPE_INVALID )
		, m_pName( NULL )
		, m_pNext( NULL )
	{
		
	}

	JsonValue::JsonValue(const JsonValue& oSource)
	{
		JsonValue();
		m_bConst = oSource.m_bConst;
		*this = oSource;
	}

	JsonValue::JsonValue(bool bValue)
	{
		JsonValue();
		*this = bValue;
	}

	JsonValue::JsonValue(const ImwString& sValue)
	{
		JsonValue();
		*this = sValue;
	}

	JsonValue::JsonValue(const ImwChar* pValue)
	{
		JsonValue();
		*this = pValue;
	}

	JsonValue::JsonValue(long iValue)
	{
		JsonValue();
		*this = iValue;
	}

	JsonValue::JsonValue(double fValue)
	{
		JsonValue();
		*this = fValue;
	}

	JsonValue::~JsonValue()
	{
		ImwSafeFree(m_pName);
		Reset();
	}

	JsonValue JsonValue::CreateConst()
	{
		JsonValue oValue;
		oValue.m_bConst = true;
		return oValue;
	}

	void JsonValue::InitType(EType eType)
	{
		if (m_eType != eType)
		{
			Reset();

			m_eType = eType;
			switch (m_eType)
			{
			case E_TYPE_OBJECT:
			case E_TYPE_ARRAY:
				m_oChilds.m_pFirst = NULL;
				m_oChilds.m_pLast = NULL;
				break;
			case E_TYPE_STRING:
				m_pString = NULL;
				break;
			default:
				break;
			}
		}
	}

	void JsonValue::Reset()
	{
		switch (m_eType)
		{
		case E_TYPE_OBJECT:
		case E_TYPE_ARRAY:
		{
			JsonValue* pChild = m_oChilds.m_pFirst;
			while (pChild != NULL)
			{
				JsonValue* pTemp = pChild->m_pNext;
				delete pChild;
				pChild = pTemp;
			}
			m_oChilds.m_pFirst = NULL;
			m_oChilds.m_pLast = NULL;
		}
		break;
		case E_TYPE_STRING:
			ImwSafeFree(m_pString);
			break;
		default:
			break;
		}
		m_eType = E_TYPE_INVALID;
	}

	void JsonValue::SetString(const ImwChar* pString)
	{
		ImwSafeFree(m_pString);
		if (NULL != pString)
		{
			size_t iLen  = sizeof(ImwChar) * (1 + strlen(pString));
			m_pString = (ImwChar*)ImwMalloc(iLen);
			memcpy(m_pString, pString, iLen);
		}
	}

	void JsonValue::Write(ImwString& sOutJson, int iIndent, bool bCompact)
	{
		if (m_eType == E_TYPE_OBJECT)
		{
			ImwString sIndent(iIndent, '\t');
			ImwString sIndent2(iIndent + 1, '\t');
			sOutJson += "{";
			//JsonMembers& oMembers = *m_pObject;
			JsonValue* pChild = m_oChilds.m_pFirst;
			bool bFirst = true;
			//for (JsonMembers::iterator it = oMembers.begin(), itEnd = oMembers.end(); it != itEnd; ++it)
			while (pChild != NULL)
			{
				if (!bFirst)
				{
					sOutJson += ",";
				}
				else
				{
					bFirst = false;
				}

				if (!bCompact)
				{
					sOutJson += "\n";
					sOutJson += sIndent2;
				}
				
				sOutJson += "\"";
				WriteStringEscaped(sOutJson, pChild->m_pName);
				sOutJson += "\": ";
				pChild->Write(sOutJson, iIndent + 1, bCompact);
				pChild = pChild->m_pNext;
			}
			if (!bCompact)
			{
				sOutJson += "\n";
				sOutJson += sIndent;
			}
			sOutJson += "}";
		}
		else if (m_eType == E_TYPE_ARRAY)
		{
			ImwString sIndent(iIndent, '\t');
			ImwString sIndent2(iIndent + 1, '\t');
			sOutJson += "[";
			//JsonArray& oArray = *m_pArray;
			JsonValue* pChild = m_oChilds.m_pFirst;
			bool bFirst = true;
			//for (JsonArray::iterator it = oArray.begin(), itEnd = oArray.end(); it != itEnd; ++it)
			while (pChild != NULL)
			{
				if (!bFirst)
				{
					sOutJson += ",";
				}
				else
				{
					bFirst = false;
				}

				if (!bCompact)
				{
					sOutJson += "\n";
					sOutJson += sIndent2;
				}

				pChild->Write(sOutJson, iIndent + 1, bCompact);
				pChild = pChild->m_pNext;
			}
			if (!bCompact)
			{
				sOutJson += "\n";
				sOutJson += sIndent;
			}
			sOutJson += "]";
		}
		else if (m_eType == E_TYPE_STRING)
		{
			sOutJson += "\"";
			WriteStringEscaped(sOutJson, m_pString);
			sOutJson += "\"";
		}
		else if (m_eType == E_TYPE_BOOLEAN)
		{
			if (m_bBoolean)
			{
				sOutJson += "true";
			}
			else
			{
				sOutJson += "false";
			}
		}
		else if (m_eType == E_TYPE_INTEGER)
		{
			ImwChar sBuffer[256];
			sprintf(sBuffer, "%d", m_iInteger);
			sOutJson += sBuffer;
		}
		else if (m_eType == E_TYPE_FLOAT)
		{
			ImwChar sBuffer[256];
			sprintf(sBuffer, "%.17Lg", m_fFloat);
			sOutJson += sBuffer;
		}
		else
		{
			sOutJson += "null";
		}
	}

	void JsonValue::WriteStringEscaped(ImwString& sOutJson, const ImwString& sInput)
	{
		const ImwChar* pString = sInput.c_str();
		while (*pString != '\0')
		{
			ImwChar cChar = *pString;
			if (cChar == '\n')
				sOutJson += "\\n";
			else if (cChar == '\r')
				sOutJson += "\\r";
			else if (cChar == '\t')
				sOutJson += "\\t";
			else if (cChar == '\b')
				sOutJson += "\\b";
			else if (cChar == '\f')
				sOutJson += "\\f";
			else if (cChar == '"')
				sOutJson += "\\\"";
			else if (cChar == '\\')
				sOutJson += "\\\\";
			else if ((unsigned ImwChar)cChar < 0x80)
				sOutJson += cChar;
			else
			{
				sOutJson += "\\u";
				unsigned int iChar = (unsigned char)cChar;

				if ((iChar & 0xF0) == 0xF0) // 4 bytes
				{
					iChar = ((((unsigned ImwChar)*(pString)) & 0x07) << 18) + ((((unsigned ImwChar)*(pString + 1)) & 0x3F) << 12) + ((((unsigned ImwChar)*(pString + 2)) & 0x3F) << 6) + ((((unsigned ImwChar)*(pString + 3)) & 0x3F));
					pString += 3;
				}
				else if ((iChar & 0xF0) == 0xE0) // 3 bytes
				{
					iChar = ((((unsigned ImwChar)*(pString)) & 0x0F) << 12) + ((((unsigned ImwChar)*(pString + 1)) & 0x3F) << 6) + ((((unsigned ImwChar)*(pString + 2)) & 0x3F));
					pString += 2;
				}
				else if ((iChar & 0xF0) == 0xC0) // 2 byte
				{
					iChar = ((((unsigned ImwChar)*(pString)) & 0x1F) << 6) + ((((unsigned ImwChar)*(pString + 1)) & 0x3F));
					pString += 1;
				}
				

				ImwChar sHexa[5];
				const char* const  pHexa = "0123456789ABCDEF";
				sHexa[0] = pHexa[(iChar >> 12) & 0x0f];
				sHexa[1] = pHexa[(iChar >> 8) & 0x0f];
				sHexa[2] = pHexa[(iChar >> 4) & 0x0f];
				sHexa[3] = pHexa[(iChar) & 0x0f];
				sHexa[4] = '\0';
				
				sOutJson += sHexa;
			}

			++pString;
		}
	}

	void JsonValue::WriteString(ImwString& sOutJson, bool bCompact)
	{
		Write(sOutJson, 0, bCompact);
	}

	bool JsonValue::WriteFile(const ImwChar* pFilename, bool bCompact)
	{
		FILE* pFile = fopen(pFilename, "w");
		if (NULL != pFile)
		{
			ImwString sJson;
			WriteString(sJson, bCompact);
			bool bRet = fwrite(sJson.c_str(), sizeof(ImwChar), sJson.length(), pFile) == (sizeof(ImwChar) * sJson.length());
			fclose(pFile);
			return bRet;
		}
		return false;
	}

	int JsonValue::GetMemberCount() const
	{
		int iCount = 0;
		if (m_eType == E_TYPE_OBJECT || m_eType == E_TYPE_ARRAY)
		{
			JsonValue* pChild = m_oChilds.m_pFirst;
			while (pChild != NULL)
			{
				++iCount;
				pChild = pChild->m_pNext;
			}
		}

		return iCount;
	}

	const JsonValue& JsonValue::operator[](const ImwChar* pName) const
	{
		if (m_eType == E_TYPE_OBJECT)
		{
			JsonValue* pChild = m_oChilds.m_pFirst;
			while (pChild != NULL)
			{
				if (strcmp(pChild->m_pName, pName) == 0)
					return *pChild;
				if (pChild->m_pNext == NULL)
					break;
				pChild = pChild->m_pNext;
			}
		}
		return JsonValue::INVALID;
	}

	JsonValue& JsonValue::operator[](const ImwChar* pName)
	{
		if (m_eType == E_TYPE_INVALID)
			InitType(E_TYPE_OBJECT);
		if (m_eType == E_TYPE_OBJECT)
		{
			JsonValue* pChild = m_oChilds.m_pFirst;
			while (pChild != NULL)
			{
				if (strcmp(pChild->m_pName, pName) == 0)
					return *pChild;
				if (pChild->m_pNext == NULL)
					break;
				pChild = pChild->m_pNext;
			}
			if (!m_bConst)
			{
				JsonValue* pNewMember = new JsonValue();
				pNewMember->m_pName = ImStrdup(pName);
				
				if (NULL != m_oChilds.m_pLast)
					m_oChilds.m_pLast->m_pNext = pNewMember;
				else
					m_oChilds.m_pFirst = pNewMember;

				m_oChilds.m_pLast = pNewMember;
				return *pNewMember;
			}
		}
		return JsonValue::INVALID;
	}

	const JsonValue& JsonValue::operator [](int iIndex) const
	{
		if (m_eType == E_TYPE_OBJECT || m_eType == E_TYPE_ARRAY)
		{
			JsonValue* pChild = m_oChilds.m_pFirst;
			int iCurrent = 0;
			while (pChild != NULL)
			{
				if (iCurrent++ == iIndex)
					return *pChild;
				pChild = pChild->m_pNext;
			}
		}
		return JsonValue::INVALID;
	}

	JsonValue& JsonValue::operator[](int iIndex)
	{
		if (m_eType == E_TYPE_INVALID)
			InitType(E_TYPE_ARRAY);
		if (m_eType == E_TYPE_OBJECT || m_eType == E_TYPE_ARRAY)
		{
			JsonValue* pChild = m_oChilds.m_pFirst;
			int iCurrent = 0;
			while (pChild != NULL)
			{
				if (iCurrent++ == iIndex)
					return *pChild;
				if (pChild->m_pNext == NULL)
					break;
				pChild = pChild->m_pNext;
			}
			if (m_eType == E_TYPE_ARRAY)
			{
				do
				{
					JsonValue* pNewChild = new JsonValue();

					if (NULL != m_oChilds.m_pLast)
						m_oChilds.m_pLast->m_pNext = pNewChild;
					else
						m_oChilds.m_pFirst = pNewChild;

					m_oChilds.m_pLast = pNewChild;
				}
				while (iCurrent++ != iIndex);
				return *m_oChilds.m_pLast;
			}
		}
		return JsonValue::INVALID;
	}

	JsonValue& JsonValue::operator =(const JsonValue& oValue)
	{
		if (oValue.m_pName != NULL)
		{
			m_pName = ImStrdup(oValue.m_pName);
		}

		if (oValue.m_eType == E_TYPE_OBJECT || oValue.m_eType == E_TYPE_ARRAY)
		{
			InitType(oValue.m_eType);
			
			JsonValue* pSourceChild = oValue.m_oChilds.m_pFirst;
			while (pSourceChild != NULL)
			{
				JsonValue* pNewChild = new JsonValue(*pSourceChild);
				if (NULL != m_oChilds.m_pLast)
					m_oChilds.m_pLast->m_pNext = pNewChild;
				else
					m_oChilds.m_pFirst = pNewChild;

				m_oChilds.m_pLast = pNewChild;

				pSourceChild = pSourceChild->m_pNext;
			}
		}
		else if (oValue.m_eType == E_TYPE_BOOLEAN)
		{
			*this = (bool)oValue;
		}
		else if (oValue.m_eType == E_TYPE_STRING)
		{
			*this = (ImwString)oValue;
		}
		else if (oValue.m_eType == E_TYPE_INTEGER)
		{
			*this = (long)oValue;
		}
		else if (oValue.m_eType == E_TYPE_FLOAT)
		{
			*this = (double)oValue;
		}
		return *this;
	}

	JsonValue& JsonValue::operator =(const ImwString& sValue)
	{
		if (!m_bConst)
		{
			InitType(E_TYPE_STRING);
			SetString(sValue.c_str());
		}
		return *this;
	}

	JsonValue& JsonValue::operator =(const ImwChar* pValue)
	{
		if (!m_bConst)
		{
			if (NULL != pValue)
			{
				InitType(E_TYPE_STRING);
				SetString(pValue);
			}
			else
			{
				InitType(E_TYPE_INVALID);
			}
		}
		return *this;
	}

	JsonValue& JsonValue::operator =(bool bValue)
	{
		if (!m_bConst)
		{
			InitType(E_TYPE_BOOLEAN);
			m_bBoolean = bValue;
		}
		return *this;
	}

	JsonValue& JsonValue::operator =(long iValue)
	{
		if (!m_bConst)
		{
			InitType(E_TYPE_INTEGER);
			m_iInteger = iValue;
		}
		return *this;
	}

	JsonValue& JsonValue::operator =(double fValue)
	{
		if (!m_bConst)
		{
			InitType(E_TYPE_FLOAT);
			m_fFloat = fValue;
		}
		return *this;
	}

	JsonValue& JsonValue::operator +=(const JsonValue& oValue)
	{
		if (m_eType == E_TYPE_ARRAY)
		{
			JsonValue* pNewValue = new JsonValue(oValue);
			
			if (NULL != m_oChilds.m_pLast)
				m_oChilds.m_pLast->m_pNext = pNewValue;
			else
				m_oChilds.m_pFirst = pNewValue;
			
			m_oChilds.m_pLast = pNewValue;
		}
		else if (m_eType == E_TYPE_STRING)
		{
			if (oValue.IsString())
			{
				ImwString sValue = m_pString;
				sValue += (ImwString)oValue;
				SetString(sValue.c_str());
			}
		}
		return *this;
	}

	JsonValue::operator const char*() const
	{
		if (m_eType == E_TYPE_STRING)
			return m_pString;
		return NULL;
	}

	JsonValue::operator bool() const
	{
		if (m_eType == E_TYPE_BOOLEAN)
			return m_bBoolean;
		return false;
	}

	JsonValue::operator long() const
	{
		if (m_eType == E_TYPE_INTEGER)
			return m_iInteger;
		else if (m_eType == E_TYPE_FLOAT)
			return (long)m_fFloat;
		return 0;
	}

	JsonValue::operator double() const
	{
		if (m_eType == E_TYPE_FLOAT)
			return m_fFloat;
		else if (m_eType == E_TYPE_INTEGER)
			return (double)m_iInteger;
		return 0.0;
	}

	//Reading

	bool JsonValue::IsSpace(ImwChar cChar) {
		return cChar == ' ' || (cChar >= '\t' && cChar <= '\r');
	}

	bool JsonValue::IsDigit(ImwChar cChar)
	{
		return (cChar >= '0' && cChar <= '9');
	}

	bool JsonValue::IsXDigit(ImwChar cChar)
	{
		return (cChar >= '0' && cChar <= '9') || ((cChar & ~' ') >= 'A' && (cChar & ~' ') <= 'F') || ((cChar & ~' ') >= 'a' && (cChar & ~' ') <= 'f');
	}

	int	JsonValue::CharToInt(ImwChar cChar)
	{
		if (cChar <= '9')
			return cChar - '0';
		else
			return (cChar & ~' ') - 'A' + 10;
	}

	void JsonValue::SkipSpaces(const ImwChar*& pString)
	{
		while (IsSpace(*pString)) ++pString;
	}

	bool JsonValue::ReadSpecialChar(const ImwChar*& pString, CharBuffer& oTempBuffer)
	{
		if (*pString == 'n') oTempBuffer += '\n';
		else if (*pString == 'r') oTempBuffer += '\r';
		else if (*pString == 't') oTempBuffer += '\t';
		else if (*pString == 'b') oTempBuffer += '\b';
		else if (*pString == 'f') oTempBuffer += '\f';
		else if (*pString == '"') oTempBuffer += '"';
		else if (*pString == '\\') oTempBuffer += '\\';
		else if (*pString == 'u')
		{
			int iChar = 0;
			for (int i = 0; i < 4; ++i)
			{
				if (IsXDigit(*++pString))
					iChar = iChar * 16 + CharToInt((unsigned char)*pString);
				else
					return false;
			}
			if (iChar < 0x0080)
			{
				oTempBuffer += (ImwChar)iChar;
			}
			else if (iChar >= 0x80 && iChar < 0x800)
			{
				oTempBuffer += (ImwChar)(0xC0 | (iChar >> 6));
				oTempBuffer += (ImwChar)(0x80 | (iChar & 0x3F));
			}
			else if (iChar >= 0x800 && iChar < 0x7FFF)
			{
				oTempBuffer += (ImwChar)(0xE0 | (iChar >> 12));
				oTempBuffer += (ImwChar)(0x80 | ((iChar >> 6) & 0x3F));
				oTempBuffer += (ImwChar)(0x80 | (iChar & 0x3F));
			}
			else if (iChar >= 0x8000 && iChar < 0x7FFFF)
			{
				oTempBuffer += (ImwChar)(0xF0 | (iChar >> 18));
				oTempBuffer += (ImwChar)(0xE0 | ((iChar >> 12) & 0x3F));
				oTempBuffer += (ImwChar)(0x80 | ((iChar >> 6) & 0x3F));
				oTempBuffer += (ImwChar)(0x80 | (iChar & 0x3F));
			}
			else
			{
				return false;
			}
			return true;
		}
		else
			return false;

		return true;
	}

	bool JsonValue::ReadStringValue(const ImwChar*& pString, CharBuffer& oTempBuffer)
	{
		oTempBuffer.Clear();
		while (*pString != 0)
		{
			if (*pString == '\\')
			{
				++pString;
				if (!ReadSpecialChar(pString, oTempBuffer))
					return false;
				++pString;
			}
			else if (*pString == '"')
			{
				oTempBuffer += '\0';
				++pString;
				return true;
			}
			else if ((unsigned int)*pString < ' ' || *pString == '\x7F')
			{
				return false;
			}
			else
			{
				oTempBuffer += *pString;
				++pString;
			}
		}
		return false;
	}

	bool JsonValue::ReadStringValue(const ImwChar*& pString, JsonValue& oValue, CharBuffer& oTempBuffer)
	{
		if (ReadStringValue(pString, oTempBuffer))
		{
			oValue.InitType(E_TYPE_STRING);
			oValue.m_pString = oTempBuffer.Take();
			//oValue = oTempBuffer.Data();//Use Take();
			return true;
		}
		return false;
	}

	bool JsonValue::ReadNumericValue(const ImwChar*& pString, JsonValue& oValue)
	{
		bool bNeg = false;
		long lValue = 0;
		if (*pString == '-')
		{
			++pString;
			bNeg = true;
		}

		while (IsDigit(*pString))
			lValue = lValue * 10 + (*pString++ - '0');

		if (*pString == '.')
		{
			++pString;
			double fValue = (double)lValue;
			double fFraction = 1;
			while (IsDigit(*pString))
			{
				fFraction *= 0.1;
				fValue += (*pString++ - '0') * fFraction;
			}

			if (*pString == 'e' || *pString == 'E')
			{
				++pString;

				double fBase = 10;
				if (*pString == '+')
				{
					++pString;
				}
				else if (*pString == '-')
				{
					++pString;
					fBase = 0.1;
				}

				unsigned int iExponent = 0;
				while (*pString != 0 && IsDigit(*pString))
					iExponent = (iExponent * 10) + (*pString++ - '0');

				double fPow = 1;
				for (; iExponent; iExponent >>= 1, fBase *= fBase)
					if (iExponent & 1)
						fPow *= fBase;

				fValue *= fPow;
			}
			oValue = bNeg ? -fValue : fValue;
		}
		else
		{
			//TODO manage E/e for long?
			oValue = bNeg ? -lValue : lValue;
		}
		return true;
	}

	bool JsonValue::ReadObjectValue(const ImwChar*& pString, JsonValue& oValue, CharBuffer& oTempBuffer)
	{
		oValue.InitType(JsonValue::E_TYPE_OBJECT);
		while (*pString != 0)
		{
			SkipSpaces(pString);

			// Read member name
			if (*pString != '"' || !ReadStringValue(++pString, oTempBuffer))
				return false;

			JsonValue* pNewMember = new JsonValue();
			pNewMember->m_pName = oTempBuffer.Take();

			SkipSpaces(pString);

			if (*pString != ':')
				return false;
			
			++pString;

			SkipSpaces(pString);

			if (!pNewMember->Parse(pString, oTempBuffer))
			{
				delete pNewMember;
				return false;
			}

			if (oValue.m_oChilds.m_pFirst == NULL)
			{
				oValue.m_oChilds.m_pFirst = pNewMember;
			}
			else
			{
				oValue.m_oChilds.m_pLast->m_pNext = pNewMember;
			}
			oValue.m_oChilds.m_pLast = pNewMember;

			SkipSpaces(pString);

			if (*pString == '}')
			{
				++pString;
				return true;
			}
			else if (*pString != ',')
			{
				return false;
			}
			++pString;
		}
		return false;
	}

	bool JsonValue::ReadArrayValue(const ImwChar*& pString, JsonValue& oValue, CharBuffer& oTempBuffer)
	{
		oValue.InitType(JsonValue::E_TYPE_ARRAY);
		while (*pString != 0)
		{
			SkipSpaces(pString);

			JsonValue* pNewValue = new JsonValue();
			
			if (!pNewValue->Parse(pString, oTempBuffer))
			{
				delete pNewValue;
				return false;
			}

			if (oValue.m_oChilds.m_pFirst == NULL)
			{
				oValue.m_oChilds.m_pFirst = pNewValue;
			}
			else
			{
				oValue.m_oChilds.m_pLast->m_pNext = pNewValue;
			}
			oValue.m_oChilds.m_pLast = pNewValue;

			SkipSpaces(pString);

			if (*pString == ']')
			{
				++pString;
				return true;
			}
			else if (*pString != ',')
			{
				return false;
			}
			++pString;
		}
		return false;
	}

	int JsonValue::ReadString(const ImwChar* pJson)
	{
		if (pJson != NULL)
		{
			CharBuffer oTempBuffer;
			Reset();
			const ImwChar* pEnd = pJson;
			if (!Parse(pEnd, oTempBuffer))
			{
				int iLine = 1;
				int iReturn = 1;
				while (pJson != pEnd)
				{
					if (*pJson == '\n')
						++iLine;
					else if (*pJson == '\r')
						++iReturn;
					++pJson;
				}
				if (iReturn > iLine)
					iLine = iReturn;
				return iLine;
			}
			return 0;
		}
		return -1;
	}

	const bool JsonValue::Parse(const ImwChar*& pString, CharBuffer& oTempBuffer)
	{
		bool bOk = pString != NULL && *pString != 0;
		while (*pString != 0 && bOk)
		{
			while (IsSpace(*pString)) ++pString;
			if (*pString == '"')
			{
				++pString;
				if (!ReadStringValue(pString, *this, oTempBuffer))
					bOk = false;
				break;
			}
			else if (IsDigit(*pString) || *pString == '-')
			{
				if (!ReadNumericValue(pString, *this))
					bOk = false;
				break;
			}
			else if (pString[0] == 't' && pString[1] == 'r' && pString[2] == 'u' && pString[3] == 'e')
			{
				pString += 4;
				*this = true;
				break;
			}
			else if (pString[0] == 'f' && pString[1] == 'a' && pString[2] == 'l' && pString[3] == 's' && pString[4] == 'e')
			{
				pString += 5;
				*this = false;
				break;
			}
			else if (pString[0] == 'n' && pString[1] == 'u' && pString[2] == 'l' && pString[3] == 'l')
			{
				pString += 4;
				InitType(E_TYPE_INVALID);
				break;
			}
			else if (*pString == '{')
			{
				++pString;
				if (!ReadObjectValue(pString, *this, oTempBuffer))
				{
					bOk = false;
				}
				break;
			}
			else if (*pString == '[')
			{
				++pString;
				if (!ReadArrayValue(pString, *this, oTempBuffer))
					bOk = false;
				break;
			}
			else
			{
				//Error
				bOk = false;
				break;
			}
		}
		return bOk;
	}

	int JsonValue::ReadFile(const ImwChar* pFilename)
	{
		FILE* pFile = fopen(pFilename, "r");
		if (NULL != pFile)
		{
			Reset();

			fseek(pFile, 0, SEEK_END);
			long iSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			ImwChar* pString = new ImwChar[iSize / sizeof(ImwChar)];
			fread(pString, 1, iSize, pFile);
			fclose(pFile);

			int iLine = ReadString(pString);

			delete[] pString;
			return iLine;
		}
		return -1;
	}

//SFF_END
}