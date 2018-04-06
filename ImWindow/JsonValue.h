
#ifndef __IMW_JSON_VALUE_H__
#define __IMW_JSON_VALUE_H__

#include "ImwConfig.h"

namespace ImWindow
{
//SFF_BEGIN
	class IMGUI_API JsonValue
	{
	protected:

		template <typename T, int HeapSize = 1024>
		struct Buffer
		{
		public:
			Buffer(int iReserve = 8)
			{
				m_pData = m_pHeapData;
				m_iSize = m_iCapacity = 0;
				m_bUseHeap = true;
				Reserve(iReserve);
			}

			~Buffer()
			{
				if (!m_bUseHeap)
					ImwSafeFree(m_pData);
			}

			Buffer<T>& operator +=(const T& oValue)
			{
				Push(oValue);
				return *this;
			}

			void Push(const T& oValue)
			{
				Resize(m_iSize + 1);
				if (m_bUseHeap)
					m_pHeapData[m_iSize - 1] = oValue; 
				else
					m_pData[m_iSize - 1] = oValue;
			}

			int Size() const
			{
				return m_iSize;
			}

			void Reserve(int iCapacity, bool bForceAlloc = false)
			{
				if (iCapacity != m_iCapacity)
				{
					if (!m_bUseHeap || iCapacity >= HeapSize || bForceAlloc)
					{
						T* pTemp = (T*)ImwMalloc(iCapacity * sizeof(T));
						if (NULL != m_pData)
						{
							memcpy(pTemp, m_pData, (m_iCapacity > iCapacity ? m_iCapacity : iCapacity) * sizeof(T));
							if (!m_bUseHeap)
								ImwFree(m_pData);
						}
						m_pData = pTemp;
						m_bUseHeap = false;
					}
					
					m_iCapacity = iCapacity;
				}
			}

			void Resize(int iSize)
			{
				int iNewCapacity = m_iCapacity > 0 ? m_iCapacity : 8;
				while (iSize > iNewCapacity) 
					iNewCapacity *= 2;
				if (iNewCapacity != m_iCapacity)
					Reserve(iNewCapacity);
				m_iSize = iSize;
			}

			const T* Data() const { return m_bUseHeap ? m_pHeapData : m_pData; }

			T* Take()
			{
				ImwChar* pTemp;
				if (m_bUseHeap)
				{
					pTemp = (T*)ImwMalloc(m_iSize * sizeof(T));
					memcpy(pTemp, m_pHeapData, m_iSize * sizeof(T));
				}
				else
				{
					pTemp = m_pData;
					m_pData = NULL;
				}
				m_iCapacity = 0;
				m_iSize = 0;
				m_bUseHeap = true;
				return pTemp;
			}

			void Clear()
			{
				m_iSize = 0;
			}
		protected:
			T		m_pHeapData[HeapSize];
			T*		m_pData;
			int		m_iSize;
			int		m_iCapacity;
			bool	m_bUseHeap;
		};

		typedef Buffer<ImwChar> CharBuffer;

		struct JsonMember
		{
		public:
			JsonMember(const ImwChar* pName = NULL, JsonValue* pValue = NULL);
			JsonMember(const JsonMember& oSource);
			~JsonMember();
			void		SetName(const ImwChar* pName);
			ImwChar*	m_pName;
			JsonValue*	m_pValue;
		};

		struct ParseInfos;
	public:
		enum EType
		{
			E_TYPE_INVALID,		//null
			E_TYPE_OBJECT,		//JsonMembers
			E_TYPE_ARRAY,		//JsonArray
			E_TYPE_STRING,		//ImwString
			E_TYPE_BOOLEAN,		//bool
			E_TYPE_INTEGER,		//long
			E_TYPE_FLOAT		//double
		};

		struct JsonIterator
		{
			JsonValue* m_pChild;

			void operator++() {
				m_pChild = m_pChild->m_pNext;
			}
			bool operator!=(const JsonIterator &oIte) const {
				return m_pChild != oIte.m_pChild;
			}
			JsonValue* operator*() const {
				return m_pChild;
			}
			JsonValue* operator->() const {
				return m_pChild;
			}
		};

		static JsonValue	INVALID;
	public:
							JsonValue();
							JsonValue(const JsonValue& oSource);
							JsonValue(bool bValue);
							JsonValue(const ImwString& sValue);
							JsonValue(const ImwChar* pValue);
							JsonValue(long iValue);
							JsonValue(double fValue);
							~JsonValue();

		void				InitType(EType eType);

		int					ReadString(const ImwChar* pJson);
		int					ReadFile(const ImwChar* pFilename);

		void				WriteString(ImwString& sOutJson, bool bCompact = false);
		bool				WriteFile(const ImwChar* pFilename, bool bCompact = false);

		bool				IsNull() const { return m_eType == E_TYPE_INVALID; }
		bool				IsObject() const { return m_eType == E_TYPE_OBJECT; }
		bool				IsArray() const { return m_eType == E_TYPE_ARRAY; }
		bool				IsBoolean() const { return m_eType == E_TYPE_BOOLEAN; }
		bool				IsString() const { return m_eType == E_TYPE_STRING; }
		bool				IsInteger() const { return m_eType == E_TYPE_INTEGER; }
		bool				IsFloat() const { return m_eType == E_TYPE_FLOAT; }
		bool				IsNumeric() const { return m_eType == E_TYPE_INTEGER || m_eType == E_TYPE_FLOAT; }

		int					GetMemberCount() const;

		const ImwChar*		GetName() const { return m_pName; }

		const JsonValue&	operator [](const ImwChar* pName) const;
		JsonValue&			operator [](const ImwChar* pName);

		const JsonValue&	operator [](int iIndex) const;
		JsonValue&			operator [](int iIndex);
		
		JsonValue&			operator =(const JsonValue& oValue);
		JsonValue&			operator =(const ImwString& sValue);
		JsonValue&			operator =(const ImwChar* pValue);
		JsonValue&			operator =(bool bValue);
		JsonValue&			operator =(long iValue);
		JsonValue&			operator =(double fValue);

		JsonValue&			operator +=(const JsonValue& oValue);

							operator const char*() const;
							operator bool() const;
							operator long() const;
							operator double() const;
	protected:
		static JsonValue	CreateConst();
		void				Reset();
		void				SetString(const ImwChar* pString);
		
		void				Write(ImwString& sOutJson, int iIndent, bool bCompact);
		static void			WriteStringEscaped(ImwString& sOutJson, const ImwString& sInput);

		bool				m_bConst;
		EType				m_eType;
		ImwChar*			m_pName;
		JsonValue*			m_pNext;

		struct JsonChilds
		{
			JsonValue*		m_pFirst;
			JsonValue*		m_pLast;
		};
		union
		{
			JsonChilds		m_oChilds;
			ImwChar*		m_pString;
			bool			m_bBoolean;
			long			m_iInteger;
			double			m_fFloat;
		};

		const bool Parse(const ImwChar*& pString, CharBuffer& oTempBuffer);

		static inline bool	IsSpace(ImwChar cChar);
		static inline bool	IsDigit(ImwChar cChar);
		static inline bool	IsXDigit(ImwChar cChar);
		static inline int	CharToInt(ImwChar cChar);
		static inline void	SkipSpaces(const ImwChar*& pString);
		static inline bool	ReadSpecialChar(const ImwChar*& pString, CharBuffer& oTempBuffer);
		static inline bool	ReadStringValue(const ImwChar*& pString, CharBuffer& oTempBuffer);
		static inline bool	ReadStringValue(const ImwChar*& pString, JsonValue& oValue, CharBuffer& oTempBuffer);
		static inline bool	ReadNumericValue(const ImwChar*& pString, JsonValue& oValue);
		static inline bool	ReadObjectValue(const ImwChar*& pString, JsonValue& oValue, CharBuffer& oTempBuffer);
		static inline bool	ReadArrayValue(const ImwChar*& pString, JsonValue& oValue, CharBuffer& oTempBuffer);
	};
//SFF_END
}

#endif // __IMW_JSON_VALUE_H__

