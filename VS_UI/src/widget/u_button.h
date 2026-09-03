/*-----------------------------------------------------------------------------

	u_button.h

	Button Widget.

	2000.6.7. KJTINC
	2000.8.4.

-----------------------------------------------------------------------------*/

#ifndef __U_BUTTON_H__
#define __U_BUTTON_H__

#include "Basics.h"

//----------------------------------------------------------------------------
// 위젯 버튼
//
// 설계:

//

//


//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ButtonBase 클래스
//

//----------------------------------------------------------------------------
class ButtonBase : public Rect
{
private:
	//---------------------------------------
	// 비공개 데이터
	//---------------------------------------
	bool		m_bl_focused; // 버튼 포커스 상태
	bool		m_bl_pressed; // 버튼 눌림 상태

public:
	ButtonBase();
	virtual ~ButtonBase();

	//---------------------------------------
	// 설정
	//---------------------------------------
	void	FocusOn();
	void	FocusOff();
	void	PressDown();
	void	PressUp();
	virtual void	InitState();

	//---------------------------------------
	// 반환
	//---------------------------------------
	bool	GetFocusState() const;
	bool	GetPressState() const;
};

//----------------------------------------------------------------------------
// Exec (추상 클래스)
//
// 버튼 실행 인터페이스.
//


//----------------------------------------------------------------------------
class Exec
{
public:
	virtual void Run(id_t id) = 0;
};

//----------------------------------------------------------------------------
// Button 클래스 (추상 클래스)
//
// 버튼 상태 변경 및 실행.
//



//
// [2000.9.25.]
//





//----------------------------------------------------------------------------
class Button : public ButtonBase
{
public:
	enum ClickOption
	{
		RUN_WHEN_PUSHUP,
		RUN_WHEN_PUSH,
	};

private:
	Exec *						m_pC_exec_handler;	// 실행 핸들러
	id_t							m_id;						// 자신의 실행 id
	ClickOption					m_click_option;

	void	Run();

public:
	//

	//
	// (1) Button * p = new Button(x, y, w, h, id, this);
	// (2) Button p;
	//		 p.Set(x, y, w, h);
	//		 p.SetID(id);
	//		 p.SetExecHandler(this);
	//		 p.SetClickOption(RUN_WHEN_PUSH);
	//
	Button(int _x, int _y, int _w, int _h, id_t id, Exec * pC_exec_handler, ClickOption co=Button::RUN_WHEN_PUSHUP);
	Button();
	virtual ~Button();




	void	SetID(id_t id);
	void	SetExecHandler(Exec * pC_exec_handler);
	void	SetClickOption(ClickOption co) { m_click_option = co; }

	//


	//
	virtual bool	MouseControl(UINT message, int _x, int _y);
	//virtual void	Show() = 0;

	id_t	GetID() const { return m_id; }
};

//-----------------------------------------------------------------------------
// EventButton (추상 클래스)
//



//-----------------------------------------------------------------------------
class EventButton : public Button
{
protected:
	bool			m_bl_prev_focus;

public:
	EventButton(int _x, int _y, int _w, int _h, id_t id, Exec * pC_exec_handler, Button::ClickOption co=Button::RUN_WHEN_PUSHUP);
	virtual ~EventButton();

	virtual void	EventFocusOn() {}
	virtual void	EventFocusOff() {}
	virtual void	EventPressDown() {}
	virtual void	EventPressUp() {}

	bool	MouseControl(UINT message, int _x, int _y);
	void	InitState();
};

#endif