/*-----------------------------------------------------------------------------

	Timer2.h

	타이머 버전 2.

	2000.6.15. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __TIMER2__
#define __TIMER2__

#include "BasicS.h"

typedef long timer_id_t;

#define INVALID_TID						-1
#define INVALID_INDEX					-1

//----------------------------------------------------------------------------
// Timer 클래스 - 타이머 관리자.
//----------------------------------------------------------------------------
class C_TIMER2
{
private:
	//
	//
	struct S_TIMERUNIT
	{
		DWORD				bl_pause; // 일시정지 상태인가?
		DWORD				dw_prev_tickcount;
		DWORD				dw_millisec;

		//
		// tid
		//
		//
		timer_id_t		tid;
		void				(*fp_proc)(void);	// 실행할 메서드
	};

	//
	// 타이머 큐
	//
	struct S_TIMERQUEUE
	{
		int				size;
		S_TIMERUNIT	*	pS_timerunit;
	};

	S_TIMERQUEUE		m_timer_queue;

	//
	// m_id_generator
	//
	//
	timer_id_t			m_id_generator;

	void	Execute(S_TIMERUNIT *pS_timerunit);

public:
	C_TIMER2();
	~C_TIMER2();

	void	Execute();
	void	Refresh(timer_id_t tid);
	void	Pause(timer_id_t tid);
	void	Continue(timer_id_t tid);
	bool	Delete(timer_id_t &tid);
	void	ResetSpeed(timer_id_t tid, DWORD millisec);

	timer_id_t Add(DWORD dw_millisec, void (*fp_proc)(void));
};

extern C_TIMER2		gC_timer2;

#endif