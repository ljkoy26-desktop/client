// u_button.cpp

#include "client_PCH.h"

#include "u_button.h"

#ifndef _LIB
EventButton *g_EventButton = NULL;
#endif

//-----------------------------------------------------------------------------
// EventButton
//
// 
//-----------------------------------------------------------------------------
EventButton::EventButton(int _x, int _y, int _w, int _h, id_t id, Exec * pC_exec_handler, Button::ClickOption co) :
										Button(_x, _y, _w, _h, id, pC_exec_handler, co)
{
	m_bl_prev_focus = false;
}

EventButton::~EventButton()
{
}

//-----------------------------------------------------------------------------
// InitState
//
// 
//-----------------------------------------------------------------------------
void EventButton::InitState()
{ 
	ButtonBase::InitState();

	m_bl_prev_focus = false; 
}

//-----------------------------------------------------------------------------
// EventButton::MouseControl
//

//-----------------------------------------------------------------------------
bool EventButton::MouseControl(UINT message, int _x, int _y)
{
	switch (message)
	{
		//
		// M_MOVING
		//
		// 포커스 켜짐/꺼짐
		//
		case M_MOVING:
			if (IsInRect(_x, _y))
			{
				if (!m_bl_prev_focus)
				{
					m_bl_prev_focus = true;
					EventFocusOn();
				}
			}
			else if (m_bl_prev_focus)
			{
				m_bl_prev_focus = false;
				EventFocusOff();
			}
			break;

		//
		// M_LEFTBUTTON_DOWN, M_LB_DOUBLECLICK
		//
		// 버튼 눌림.
		//
		case M_LEFTBUTTON_DOWN:
		case M_LB_DOUBLECLICK:
			if (IsInRect(_x, _y)) 
			{
				m_bl_prev_focus = true;
				EventPressDown();
			}
			break;

		//
		// M_LEFTBUTTON_UP
		//
		// 버튼 뗌.
		//
		case M_LEFTBUTTON_UP:
			if (GetFocusState()) // 포커스: 있음
			{
				if (GetPressState()) // 눌림: 있음
				{
					EventPressUp();
				}
			}
			break;

#ifndef _LIB
		case M_RIGHTBUTTON_UP:
			if (GetFocusState()) // 포커스: 있음
				g_EventButton = this;
			break;
#endif
	}

	return Button::MouseControl(message, _x, _y);
}

ButtonBase::ButtonBase()
{
	InitState();
}

ButtonBase::~ButtonBase()
{
#ifndef _LIB
	if(g_EventButton == this)g_EventButton = NULL;
#endif

}

void ButtonBase::InitState()
{
	m_bl_focused = false;
	m_bl_pressed = false;
}

void ButtonBase::FocusOn()
{
	m_bl_focused = true;
}

void ButtonBase::FocusOff()
{
	m_bl_focused = false;
}

void ButtonBase::PressDown()
{
	m_bl_pressed = true;
}

void ButtonBase::PressUp()
{
	m_bl_pressed = false;
}

bool ButtonBase::GetFocusState() const
{
	return m_bl_focused;
}

bool ButtonBase::GetPressState() const
{
	return m_bl_pressed;
}

//-----------------------------------------------------------------------------
// Button (Constructor)
//
// 
//-----------------------------------------------------------------------------
Button::Button(int _x, int _y, int _w, int _h, id_t id, Exec * pC_exec_handler, ClickOption co)
{
	Set(_x, _y, _w, _h);
	m_id = id;
	m_pC_exec_handler = pC_exec_handler;
	m_click_option = co;
}

Button::Button()
{

}

//-----------------------------------------------------------------------------
// ~Button (Destructor)
//
// 
//-----------------------------------------------------------------------------
Button::~Button()
{

}

//-----------------------------------------------------------------------------
// Run
//
// 
//-----------------------------------------------------------------------------
void Button::Run()
{
	if (m_pC_exec_handler) // 핸들러 설정됨?
		m_pC_exec_handler->Run(m_id);
}

void Button::SetExecHandler(Exec * pC_exec_handler)
{
	m_pC_exec_handler = pC_exec_handler;
}

void Button::SetID(id_t id)
{
	m_id = id;
}

//-----------------------------------------------------------------------------
// MouseControl
//
// 버튼 입력 인터페이스.
// 


//
// 예제:

//					if (button->MouseControl())
//						...
//
//

//
// 이력:
//				[2000.8.5. 3:15pm]
//



//
//				if (m_p_button_ok->MouseControl(message, x, y) == false)
//					if (m_p_button_cancel->MouseControl(message, x, y) == false)
//						...
//




//



//
//
//				[2000.8.5. 3:35pm]
//






//-----------------------------------------------------------------------------
bool Button::MouseControl(UINT message, int _x, int _y)
{
	switch (message)
	{
		//
		// M_MOVING
		//
		// 포커스 켜짐/꺼짐
		//
		case M_MOVING:
			if (IsInRect(_x, _y))
			{
				FocusOn();
			}
			else
			{
				FocusOff();
			}
			break;

		//
		// M_LEFTBUTTON_DOWN, M_LB_DOUBLECLICK
		//
		// 버튼 눌림.
		//
		case M_LEFTBUTTON_DOWN:
		case M_LB_DOUBLECLICK:
			if (IsInRect(_x, _y))
			{
				PressDown();

				if (m_click_option == RUN_WHEN_PUSH)
					Run();

				return false;
			}
			break;

		//
		// M_LEFTBUTTON_UP
		//
		// 버튼 뗌 -> 실행!
		//
		case M_LEFTBUTTON_UP:
			if (GetFocusState()) // 포커스: 있음
			{
				if (GetPressState()) // 눌림: 있음
				{
					PressUp();

					if (m_click_option == RUN_WHEN_PUSHUP)
						Run();
				}
				else // 눌림: 없음
				{
					// 코드 없음!
				}
			}
			else // 포커스: 없음
			{
				if (GetPressState()) // 눌림: 있음
				{
					PressUp();
				}
				else // 눌림: 없음
				{
					// 코드 없음!
				}
			}
	}

	return true;
}