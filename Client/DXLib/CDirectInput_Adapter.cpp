/*-----------------------------------------------------------------------------

	CDirectInput_Adapter.cpp

	DXLibBackend를 사용하는 DirectInput 어댑터.
	이 파일은 CDirectInput 클래스에 대한 SDL2 백엔드 지원을 제공한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#include "CDirectInput.h"
#include "DXLibBackend.h"

#define MSB		0x80

/* 전역 인스턴스 */
CSDLInput*	g_pSDLInput = NULL;

/* 원본 키 이름 테이블 유지 - 헤더에 정의되어 있음 */

/*=============================================================================
 * SDL 백엔드 구현
 *=============================================================================*/

#ifdef DXLIB_BACKEND_SDL

/* 생성자 */
CSDLInput::CSDLInput()
{
	m_pDI				= NULL;
	m_pMouse			= NULL;
	m_pKeyboard		= NULL;
	m_mouse_x		= 0;
	m_mouse_y		= 0;
	m_mouse_z		= 0;	
	m_limit_x		= 0;
	m_limit_y		= 0;
	m_mouse_info[0]=0;
	m_mouse_info[1]=0;
	m_mouse_info[2]=0;

	m_fp_mouse_event_receiver = NULL;
	m_fp_keyboard_event_receiver = NULL;
	
	Clear();
}

/* 소멸자 */
CSDLInput::~CSDLInput()
{
	FreeDirectInput();
}

/* 입력 상태를 초기화한다 */
void CSDLInput::Clear()
{
	for (int i=0; i<256; i++)
	{
		m_key[i] = FALSE;
	}
	
	m_lb_down = FALSE;
	m_rb_down = FALSE;
	m_cb_down = FALSE;
	m_lb_up = FALSE;
	m_rb_up = FALSE;
	m_cb_up = FALSE;
}

/* SDL 백엔드를 사용해 초기화한다 */
BOOL CSDLInput::Init(HWND hWnd, HINSTANCE hInst, E_EXCLUSIVE ex)
{
	// SDL 백엔드를 초기화한다
	if (dxlib_input_init(hWnd) != 0) {
		return FALSE;
	}

	// 초기화됨으로 표시한다 (null이 아닌 값을 지표로 사용)
	m_pDI = (IDirectInput*)0x01;
	m_pMouse = (IDirectInputDevice*)0x01;
	m_pKeyboard = (IDirectInputDevice*)0x01;

	return TRUE;
}

/* SDL 백엔드를 해제한다 */
void CSDLInput::FreeDirectInput()
{
	dxlib_input_release();

	m_pDI = NULL;
	m_pMouse = NULL;
	m_pKeyboard = NULL;
}

/* SDL 백엔드를 사용해 입력을 갱신한다 */
void CSDLInput::UpdateInput()
{
	// 백엔드를 갱신한다
	dxlib_input_update();

	// UP 이벤트를 초기화한다 (한 프레임 동안만 true여야 함)
	m_lb_up = FALSE;
	m_rb_up = FALSE;
	m_cb_up = FALSE;

	// 참고: 여기서 DOWN 상태를 초기화하지 않는다 - 버튼이 떼어질 때까지 유지되어야 한다

	// 키보드 상태를 갱신한다
	for (int i = 0; i < 256; i++) {
		BOOL down = dxlib_input_key_down(i) ? TRUE : FALSE;

		// 상태 변화를 확인하고 이벤트를 발생시킨다
		if (down && !m_key[i]) {
			m_key[i] = TRUE;
			if (m_fp_keyboard_event_receiver) {
				m_fp_keyboard_event_receiver(KEYDOWN, i);
			}
		} else if (!down && m_key[i]) {
			m_key[i] = FALSE;
			if (m_fp_keyboard_event_receiver) {
				m_fp_keyboard_event_receiver(KEYUP, i);
			}
		}
	}

	// 마우스 위치를 갱신한다
	int old_x = m_mouse_x;
	int old_y = m_mouse_y;
	dxlib_input_get_mouse_pos(&m_mouse_x, &m_mouse_y);

	// 마우스 이동을 확인한다
	if (old_x != m_mouse_x || old_y != m_mouse_y) {
		if (m_fp_mouse_event_receiver) {
			m_fp_mouse_event_receiver(MOVE, m_mouse_x, m_mouse_y, m_mouse_z);
		}
	}

	// 마우스 휠을 갱신한다
	int old_z = m_mouse_z;
	m_mouse_z = dxlib_input_get_mouse_wheel();

	// 휠 이동을 확인한다
	if (old_z != m_mouse_z) {
		if (m_fp_mouse_event_receiver) {
			if (m_mouse_z > old_z) {
				m_fp_mouse_event_receiver(WHEELUP, m_mouse_x, m_mouse_y, m_mouse_z);
			} else {
				m_fp_mouse_event_receiver(WHEELDOWN, m_mouse_x, m_mouse_y, m_mouse_z);
			}
		}
	}

	// 마우스 버튼을 갱신한다
	int left, right, center;
	dxlib_input_get_mouse_buttons(&left, &right, &center);

	// 버튼 상태 변화를 확인한다
	if (left && !m_lb_down) {
		m_lb_down = TRUE;
		if (m_fp_mouse_event_receiver) {
			m_fp_mouse_event_receiver(LEFTDOWN, m_mouse_x, m_mouse_y, m_mouse_z);
		}
	}
	if (!left && m_lb_down) {
		m_lb_up = TRUE;
		m_lb_down = FALSE;
		if (m_fp_mouse_event_receiver) {
			m_fp_mouse_event_receiver(LEFTUP, m_mouse_x, m_mouse_y, m_mouse_z);
		}
	}

	if (right && !m_rb_down) {
		m_rb_down = TRUE;
		if (m_fp_mouse_event_receiver) {
			m_fp_mouse_event_receiver(RIGHTDOWN, m_mouse_x, m_mouse_y, m_mouse_z);
		}
	}
	if (!right && m_rb_down) {
		m_rb_up = TRUE;
		m_rb_down = FALSE;
		if (m_fp_mouse_event_receiver) {
			m_fp_mouse_event_receiver(RIGHTUP, m_mouse_x, m_mouse_y, m_mouse_z);
		}
	}

	if (center && !m_cb_down) {
		m_cb_down = TRUE;
		if (m_fp_mouse_event_receiver) {
			m_fp_mouse_event_receiver(CENTERDOWN, m_mouse_x, m_mouse_y, m_mouse_z);
		}
	}
	if (!center && m_cb_down) {
		m_cb_up = TRUE;
		m_cb_down = FALSE;
		if (m_fp_mouse_event_receiver) {
			m_fp_mouse_event_receiver(CENTERUP, m_mouse_x, m_mouse_y, m_mouse_z);
		}
	}
}

/* Acquire 설정 (SDL 백엔드 - 아무 동작도 하지 않음) */
HRESULT CSDLInput::SetAcquire(bool active_app)
{
	if (!m_pMouse || !m_pKeyboard)
		return S_FALSE;
	return S_OK;
}

/* 마우스 위치 설정 (SDL 백엔드) */
void CSDLInput::SetMousePosition(int x, int y)
{
	m_mouse_x = x;
	m_mouse_y = y;
	dxlib_input_set_mouse_pos(x, y);
}

/* 마우스 속도 설정 (SDL 백엔드 - 스텁) */
void CSDLInput::SetMouseSpeed()
{
	// SDL 백엔드는 시스템 마우스 설정을 사용한다
}

/* 마우스 가속 값 가져오기 (SDL에는 해당 없음) */
int CSDLInput::GetMouseAcceleration(int value)
{
	return value;
}

/* 마우스 이동 제한 설정 */
void CSDLInput::SetMouseMoveLimit(int x, int y)
{
	m_mouse_x = 0;
	m_mouse_y = 0;
	m_mouse_z = 0;

	m_limit_x = x;
	m_limit_y = y;
}

/* 이벤트 수신자 설정 */
void CSDLInput::SetMouseEventReceiver(void (*fp_receiver)(E_MOUSE_EVENT, int, int, int))
{
	m_fp_mouse_event_receiver = fp_receiver;
}

void CSDLInput::SetKeyboardEventReceiver(void (*fp_receiver)(E_KEYBOARD_EVENT, DWORD))
{
	m_fp_keyboard_event_receiver = fp_receiver;
}

/* 사용하지 않는 메서드에 대한 스텁 구현 */
void CSDLInput::OnMouseInput() { /* UpdateInput에서 처리됨 */ }
void CSDLInput::OnKeyboardInput() { /* UpdateInput에서 처리됨 */ }
HRESULT CSDLInput::InitDI(HWND hWnd, HINSTANCE hInst, E_EXCLUSIVE ex) { 
	return Init(hWnd, hInst, ex) ? S_OK : S_FALSE; 
}

#endif /* DXLIB_BACKEND_SDL */
