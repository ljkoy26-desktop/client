/*-----------------------------------------------------------------------------

	CDirectInput.h

	다이렉트 입력 클래스 라이브러리.

	2000.3.2. KJTINC

-----------------------------------------------------------------------------*/

/* 타입 정의를 위해 Platform.h를 가장 먼저 include한다 (헤더 가드 바깥) */
#include "../basic/Platform.h"

#ifndef __CSDLINPUT_H__
#define __CSDLINPUT_H__

/* 플랫폼 독립적인 키 코드를 include한다 (SDL 기반) */
#include "../../basic/InputCodes.h"

/* DirectInput 타입에 대한 전방 선언 (불투명 포인터) */
/* 실제 구현은 모든 플랫폼에서 SDL2를 사용한다 */
struct IDirectInput;
struct IDirectInputDevice;

#define BUFFER_SIZE           32

/*-----------------------------------------------------------------------------
  Class Direct Input
-----------------------------------------------------------------------------*/
class CSDLInput
{
private:
	IDirectInput *          m_pDI;
	IDirectInputDevice *    m_pMouse;
	IDirectInputDevice *		m_pKeyboard;
	BOOL                    m_bSwapMouseButtons;
	int							m_mouse_info[3]; // 마우스 정보를 얻기 위한 정수 배열.

	BOOL					m_key[256];				// 키가 눌려있는가?
	static const char*		s_KeyName[256];

public:
	int							m_mouse_x;
	int							m_mouse_y;
	int							m_mouse_z;	// 휠 동작
	BOOL						m_lb_down;	// 왼쪽 버튼 - 눌림
	BOOL						m_rb_down;	// 오른쪽 버튼 - 눌림
	BOOL						m_cb_down;	// 가운데 버튼 - 눌림
	BOOL						m_lb_up;	// 왼쪽 버튼 - 뗌
	BOOL						m_rb_up;	// 오른쪽 버튼 - 뗌
	BOOL						m_cb_up;	// 가운데 버튼 - 뗌

	enum E_EXCLUSIVE // debugging 할 때를 위해서...
	{
		EXCLUSIVE,
		NONEXCLUSIVE,
	};

	enum E_MOUSE_EVENT // MOUSE_EVENT는 wincon.h에 정의 되어 있음! ~.~
	{
		MOVE = 0,
		LEFTDOWN,
		LEFTUP,
		RIGHTDOWN,
		RIGHTUP,
		CENTERDOWN,
		CENTERUP,
		WHEELDOWN,
		WHEELUP,
	};

	enum E_KEYBOARD_EVENT
	{
		KEYUP,
		KEYDOWN,
	};

private:
	enum
	{
		FIRST_THRESHOLD,
		SECOND_THRESHOLD,
		ACCEL_LEVEL,
	};

	int							m_limit_x; // mouse 이동 제한
	int							m_limit_y; // mouse 이동 제한

	HRESULT	InitDI(HWND hWnd, HINSTANCE hInst, E_EXCLUSIVE ex);
	void		FreeDirectInput();
	void		OnMouseInput();
	void		OnKeyboardInput();
	void		SetMouseSpeed();
	int			GetMouseAcceleration(int value);

	void		(*m_fp_mouse_event_receiver)(E_MOUSE_EVENT event, int x, int y, int z);
	void		(*m_fp_keyboard_event_receiver)(E_KEYBOARD_EVENT event, DWORD scan_code);

public:
	CSDLInput();
	~CSDLInput();

	void		Clear();
	BOOL		Init(HWND hWnd, HINSTANCE hInst, E_EXCLUSIVE ex=EXCLUSIVE);
	HRESULT		SetAcquire(bool active_app);
	void		SetMouseEventReceiver(void (*fp_receiver)(E_MOUSE_EVENT, int, int, int));
	void		SetKeyboardEventReceiver(void (*fp_receiver)(E_KEYBOARD_EVENT, DWORD));
	void		SetMouseMoveLimit(int x, int y);
	void		SetMousePosition(int x, int y);
	void		UpdateInput();

	// dik key가 눌려졌는가?
	BOOL		KeyDown(DWORD dik)		{ return m_key[dik]; }

	//int 		GetMouseInfo1() const { return m_mouse_info[0]; }
	//int 		GetMouseInfo2() const { return m_mouse_info[1]; }
	//int 		GetMouseInfo3() const { return m_mouse_info[2]; }
	static const char*	GetKeyName(DWORD dik)	{ return s_KeyName[dik]; }
};


extern	CSDLInput*	g_pSDLInput;

#endif
