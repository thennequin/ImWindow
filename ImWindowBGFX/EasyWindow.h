#ifndef __EASY_WINDOW_H__
#define __EASY_WINDOW_H__

class EasyWindow
{
public:
	template<typename R>
	class Callback
	{
		class Caller
		{
		public:
			virtual R					Call() = 0;
		};

		class FunctionCall : public Caller
		{
		public:
			FunctionCall(R(*pFunctionPtr)())
			{
				m_pFunctionPtr = pFunctionPtr;
			}
			virtual R					Call()
			{
				return m_pFunctionPtr();
			}
			R(*m_pFunctionPtr)();
		};

		template<typename C>
		class InstanceCall : public Caller
		{
		public:
			InstanceCall(C* pThis, R(C::*pMemberPtr)())
			{
				m_pThis = pThis;
				m_pMemberPtr = pMemberPtr;
			}
			virtual R					Call()
			{
				return (m_pThis->*m_pMemberPtr)();
			}
			R(C::*m_pMemberPtr)();
			C* m_pThis;
		};
	public:
		Callback()
		{
			m_pCaller = 0;
		}
		~Callback()
		{
			if (0 != m_pCaller) delete m_pCaller;
		}
		void							Set(R(*pFuncPtr)())
		{
			if (0 != m_pCaller) delete m_pCaller;
			m_pCaller = new FunctionCall(pFuncPtr);
		}

		template<typename C>
		void							Set(C* pThis, R(C::*pMemberPtr)())
		{
			if (0 != m_pCaller) delete m_pCaller;
			m_pCaller = new InstanceCall<C>(pThis, pMemberPtr);
		}

		R								operator()()
		{
			if (0 != m_pCaller) return m_pCaller->Call();
			return R();
		}
	protected:
		Caller*							m_pCaller;
	};

	template<typename T1, typename R>
	class CallbackOneArg
	{
		class Caller
		{
		public:
			virtual R					Call(T1 oT1) = 0;
		};

		class FunctionCall : public Caller
		{
		public:
			FunctionCall(R(*pFunctionPtr)(T1))
			{
				m_pFunctionPtr = pFunctionPtr;
			}
			virtual R					Call(T1 oT1)
			{
				return m_pFunctionPtr(oT1);
			}
			R(*m_pFunctionPtr)(T1);
		};

		template<typename C>
		class InstanceCall : public Caller
		{
		public:
			InstanceCall(C* pThis, R(C::*pMemberPtr)(T1))
			{
				m_pThis = pThis;
				m_pMemberPtr = pMemberPtr;
			}
			virtual R					Call(T1 oT1)
			{
				return (m_pThis->*m_pMemberPtr)(oT1);
			}
			R(C::*m_pMemberPtr)(T1);
			C* m_pThis;
		};
	public:
		CallbackOneArg()
		{
			m_pCaller = 0;
		}
		~CallbackOneArg()
		{
			if (0 != m_pCaller) delete m_pCaller;
		}
		void							Set(R(*pFuncPtr)(T1))
		{
			if (0 != m_pCaller) delete m_pCaller;
			m_pCaller = new FunctionCall(pFuncPtr);
		}

		template<typename C>
		void							Set(C* pThis, R(C::*pMemberPtr)(T1))
		{
			if (NULL != m_pCaller) delete m_pCaller;
			m_pCaller = new InstanceCall<C>(pThis, pMemberPtr);
		}

		R								operator()(T1 oT1)
		{
			if (0 != m_pCaller) return m_pCaller->Call(oT1);
			return R();
		}
	protected:
		Caller*							m_pCaller;
	};

	template<typename T1, typename T2, typename R>
	class CallbackTwoArg
	{
		class Caller
		{
		public:
			virtual R					Call(T1 oT1, T2 oT2) = 0;
		};

		class FunctionCall : public Caller
		{
		public:
			FunctionCall(R(*pFunctionPtr)(T1, T2))
			{
				m_pFunctionPtr = pFunctionPtr;
			}
			virtual R					Call(T1 oT1, T2 oT2)
			{
				return m_pFunctionPtr(oT1, oT2);
			}
			R(*m_pFunctionPtr)(T1, T2);
		};

		template<typename C>
		class InstanceCall : public Caller
		{
		public:
			InstanceCall(C* pThis, R(C::*pMemberPtr)(T1, T2))
			{
				m_pThis = pThis;
				m_pMemberPtr = pMemberPtr;
			}
			virtual R					Call(T1 oT1, T2 oT2)
			{
				return (m_pThis->*m_pMemberPtr)(oT1, oT2);
			}
			R(C::*m_pMemberPtr)(T1, T2);
			C* m_pThis;
		};
	public:
		CallbackTwoArg()
		{
			m_pCaller = 0;
		}
		~CallbackTwoArg()
		{
			if (0 != m_pCaller) delete m_pCaller;
		}
		void							Set(R(*pFuncPtr)(T1, T2))
		{
			if (0 != m_pCaller) delete m_pCaller;
			m_pCaller = new FunctionCall(pFuncPtr);
		}

		template<typename C>
		void							Set(C* pThis, R(C::*pMemberPtr)(T1, T2))
		{
			if (NULL != m_pCaller) delete m_pCaller;
			m_pCaller = new InstanceCall<C>(pThis, pMemberPtr);
		}

		R								operator()(T1 oT1, T2 oT2)
		{
			if (0 != m_pCaller) return m_pCaller->Call(oT1, oT2);
			return R();
		}

	protected:
		Caller*	m_pCaller;
	};

	enum EKey
	{
		KEY_NONE = 0,
		KEY_ESC,
		KEY_RETURN,
		KEY_TAB,
		KEY_SPACE,
		KEY_BACKSPACE,
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_INSERT,
		KEY_DELETE,
		KEY_HOME,
		KEY_END,
		KEY_PAGEUP,
		KEY_PAGEDOWN,
		KEY_PRINT,
		KEY_PLUS,
		KEY_MINUS,
		KEY_LEFTBRACKET,
		KEY_RIGHTBRACKET,
		KEY_SEMICOLON,
		KEY_QUOTE,
		KEY_COMMA,
		KEY_PERIOD,
		KEY_SLASH,
		KEY_BACKSLASH,
		KEY_TILDE,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_NUMPAD0,
		KEY_NUMPAD1,
		KEY_NUMPAD2,
		KEY_NUMPAD3,
		KEY_NUMPAD4,
		KEY_NUMPAD5,
		KEY_NUMPAD6,
		KEY_NUMPAD7,
		KEY_NUMPAD8,
		KEY_NUMPAD9,
		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
		KEY_A,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,

		// Modifiers
		KEY_LEFTALT,
		KEY_RIGHTALT,
		KEY_LEFTCTRL,
		KEY_RIGHTCTRL,
		KEY_LEFTSHIFT,
		KEY_RIGHTSHIFT,
		KEY_LEFTMETA,
		KEY_RIGHTMETA,
	};

	enum EWindowStyle
	{
		E_NORMAL,
		E_BORDERLESS,
		E_BORDERLESS_RESIZABLE,
		E_POPUP,
	};

	static EasyWindow*					Create(const char* pTitle, int iWidth, int iHeight, bool bClientSize, EasyWindow* pParent = 0, EWindowStyle eStyle = E_NORMAL);
	static const char*					KeyToString(EKey eKey);
	static EKey							StringToKey(const char* pString);

	virtual								~EasyWindow() {};
		
	virtual bool						Update() = 0;

	virtual void						Show(bool bShow = true) = 0;

	virtual void						SetSize(int iWidth, int iHeight, bool bClientSize) = 0;
	virtual void						SetPosition(int iPosX, int iPosY) = 0;
	virtual void						SetMaximized() = 0;
	virtual void						SetMinimized() = 0;
	virtual void						SetRestored() = 0;
	virtual void						SetTitle(const char* pTitle) = 0;
	virtual void						SetAlpha(unsigned char iAlpha) = 0;

	virtual int							GetWidth() = 0;
	virtual int							GetHeight() = 0;
	virtual int							GetClientWidth() = 0;
	virtual int							GetClientHeight() = 0;
	virtual int							GetPositionX() = 0;
	virtual int							GetPositionY() = 0;
	virtual int							GetClientPositionX() = 0;
	virtual int							GetClientPositionY() = 0;
	virtual bool						IsMaximized() = 0;
	virtual bool						IsMinimized() = 0;

	virtual bool						IsKeyCtrlDown() = 0;
	virtual bool						IsKeyAltDown() = 0;
	virtual bool						IsKeyShiftDown() = 0;

	virtual void*						GetHandle() = 0;

	/* int Width, int Height */
	CallbackTwoArg<int, int, void>		OnSize;
	/* int PosX, int PosY */
	CallbackTwoArg<int, int, void>		OnMove;
	Callback<void>						OnMaximize;
	Callback<void>						OnMinimize;
	Callback<void>						OnRestore;
	/* bool HasFocus */
	CallbackOneArg<bool, void>			OnFocus;
	/* return bool AbortClosing */
	Callback<bool>						OnClose;

	/* int Button, bool IsDown */
	CallbackTwoArg<int, bool, void>		OnMouseButton;
	/* int PosX, int PosY */
	CallbackTwoArg<int, int, void>		OnMouseMove;

	/* int Key, bool IsDown */
	CallbackTwoArg<EKey, bool, void>	OnKey;
	/* int Char */
	CallbackOneArg<int, void>			OnChar;
};

#endif //__EASY_WINDOW_H__