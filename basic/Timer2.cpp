/*-----------------------------------------------------------------------------

	Timer2.cpp

	타이머 버전 2 구현.

	2000.6.15. KJTINC

-----------------------------------------------------------------------------*/

#include "Timer2.h"
#include <cstring>
#include <cstdlib>

//----------------------------------------------------------------------------
// 전역 인스턴스
//----------------------------------------------------------------------------
C_TIMER2		gC_timer2;

//----------------------------------------------------------------------------
// 생성자
//----------------------------------------------------------------------------
C_TIMER2::C_TIMER2()
{
	m_timer_queue.size = 0;
	m_timer_queue.pS_timerunit = NULL;
	m_id_generator = 0;
}

//----------------------------------------------------------------------------
// 소멸자
//----------------------------------------------------------------------------
C_TIMER2::~C_TIMER2()
{
	if (m_timer_queue.pS_timerunit != NULL)
	{
		free(m_timer_queue.pS_timerunit);
		m_timer_queue.pS_timerunit = NULL;
	}
	m_timer_queue.size = 0;
}

//----------------------------------------------------------------------------
// Add - 새 타이머를 추가한다
//----------------------------------------------------------------------------
timer_id_t
C_TIMER2::Add(DWORD dw_millisec, void (*fp_proc)(void))
{
	// 큐를 확장해야 하는지 확인한다
	if (m_id_generator >= m_timer_queue.size)
	{
		// 한 번에 8개씩 타이머를 확장한다
		int new_size = m_timer_queue.size + 8;
		S_TIMERUNIT* new_queue = (S_TIMERUNIT*)realloc(m_timer_queue.pS_timerunit,
														new_size * sizeof(S_TIMERUNIT));
		if (new_queue == NULL)
		{
			return INVALID_TID;
		}

		// 새 항목들을 초기화한다
		for (int i = m_timer_queue.size; i < new_size; i++)
		{
			new_queue[i].tid = INVALID_TID;
			new_queue[i].fp_proc = NULL;
		}

		m_timer_queue.pS_timerunit = new_queue;
		m_timer_queue.size = new_size;
	}

	// id 생성기를 사용해 슬롯을 찾는다
	timer_id_t tid = m_id_generator++;
	S_TIMERUNIT* pUnit = &m_timer_queue.pS_timerunit[tid];

	// 타이머 유닛을 초기화한다
	pUnit->tid = tid;
	pUnit->fp_proc = fp_proc;
	pUnit->dw_millisec = dw_millisec;
	pUnit->dw_prev_tickcount = GetTickCount();
	pUnit->bl_pause = 1; // 일시정지 상태로 시작

	return tid;
}

//----------------------------------------------------------------------------
// Delete - 타이머를 삭제한다
//----------------------------------------------------------------------------
bool
C_TIMER2::Delete(timer_id_t &tid)
{
	if (tid < 0 || tid >= m_id_generator)
	{
		return false;
	}

	S_TIMERUNIT* pUnit = &m_timer_queue.pS_timerunit[tid];
	if (pUnit->tid == INVALID_TID)
	{
		return false;
	}

	// 삭제된 것으로 표시한다
	pUnit->tid = INVALID_TID;
	pUnit->fp_proc = NULL;
	tid = INVALID_TID;

	return true;
}

//----------------------------------------------------------------------------
// Execute - 활성화된 모든 타이머를 실행한다
//----------------------------------------------------------------------------
void
C_TIMER2::Execute()
{
	DWORD dw_current_tickcount = GetTickCount();

	for (int i = 0; i < m_id_generator; i++)
	{
		S_TIMERUNIT* pUnit = &m_timer_queue.pS_timerunit[i];

		// 유효하지 않거나 일시정지된 타이머는 건너뛴다
		if (pUnit->tid == INVALID_TID || pUnit->bl_pause)
		{
			continue;
		}

		// 타이머 시간이 경과했는지 확인한다
		if (dw_current_tickcount - pUnit->dw_prev_tickcount >= pUnit->dw_millisec)
		{
			// 타이머 콜백을 실행한다
			if (pUnit->fp_proc != NULL)
			{
				Execute(pUnit);
			}

			// 이전 틱 카운트를 갱신한다
			pUnit->dw_prev_tickcount = dw_current_tickcount;
		}
	}
}

//----------------------------------------------------------------------------
// Execute - 단일 타이머 유닛을 실행한다 (private)
//----------------------------------------------------------------------------
void
C_TIMER2::Execute(S_TIMERUNIT *pS_timerunit)
{
	if (pS_timerunit != NULL && pS_timerunit->fp_proc != NULL)
	{
		pS_timerunit->fp_proc();
	}
}

//----------------------------------------------------------------------------
// Refresh - 타이머의 틱 카운트를 초기화한다
//----------------------------------------------------------------------------
void
C_TIMER2::Refresh(timer_id_t tid)
{
	if (tid < 0 || tid >= m_id_generator)
	{
		return;
	}

	S_TIMERUNIT* pUnit = &m_timer_queue.pS_timerunit[tid];
	if (pUnit->tid != INVALID_TID)
	{
		pUnit->dw_prev_tickcount = GetTickCount();
	}
}

//----------------------------------------------------------------------------
// Pause - 타이머를 일시정지한다
//----------------------------------------------------------------------------
void
C_TIMER2::Pause(timer_id_t tid)
{
	if (tid < 0 || tid >= m_id_generator)
	{
		return;
	}

	S_TIMERUNIT* pUnit = &m_timer_queue.pS_timerunit[tid];
	if (pUnit->tid != INVALID_TID)
	{
		pUnit->bl_pause = 1;
	}
}

//----------------------------------------------------------------------------
// Continue - 일시정지된 타이머를 재개한다
//----------------------------------------------------------------------------
void
C_TIMER2::Continue(timer_id_t tid)
{
	if (tid < 0 || tid >= m_id_generator)
	{
		return;
	}

	S_TIMERUNIT* pUnit = &m_timer_queue.pS_timerunit[tid];
	if (pUnit->tid != INVALID_TID)
	{
		pUnit->bl_pause = 0;
		pUnit->dw_prev_tickcount = GetTickCount();
	}
}

//----------------------------------------------------------------------------
// ResetSpeed - 타이머의 주기를 변경한다
//----------------------------------------------------------------------------
void
C_TIMER2::ResetSpeed(timer_id_t tid, DWORD millisec)
{
	if (tid < 0 || tid >= m_id_generator)
	{
		return;
	}

	S_TIMERUNIT* pUnit = &m_timer_queue.pS_timerunit[tid];
	if (pUnit->tid != INVALID_TID)
	{
		pUnit->dw_millisec = millisec;
		pUnit->dw_prev_tickcount = GetTickCount();
	}
}
