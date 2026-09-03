/*-----------------------------------------------------------------------------

	Ci_macOS.cpp

	macOS stub implementations for Korean/Chinese Input Method classes.

	This file provides basic stub implementations for CI, CI_KOREAN, and CI_CHINESE
	classes on macOS since the original Windows IME implementation cannot be used.

-----------------------------------------------------------------------------*/

#include "Client_PCH.h"
#include "CI.h"
#include "Timer2.h"
#include "Vs_ui.h"  // C_VS_UI 클래스를 위해
#include "Fl2.h"    // g_GetStringByMoney 선언을 위해
#include <string>   // std::string을 위해

CI *gC_ci = NULL;

static timer_id_t	g_tid_cursor_blink = INVALID_TID;
static bool	gbl_draw_cursor = false;

//----------------------------------------------------------------------------
// 타이머 콜백
//----------------------------------------------------------------------------
void Timer_BlinkCursor()
{
	gbl_draw_cursor = !gbl_draw_cursor;
}

//----------------------------------------------------------------------------
// CI 기반 클래스 구현
//----------------------------------------------------------------------------
void CI::RunCursorBlinker()
{
	g_tid_cursor_blink = gC_timer2.Add(700, Timer_BlinkCursor);
	gC_timer2.Continue(g_tid_cursor_blink);
}

CI::CI()
{
	Init();
}

CI::~CI()
{
}

void CI::Init()
{
	m_bl_insert_mode = true;
	m_bl_end_of_ime = false;
	m_bl_ime_running = false;
	m_composing_char[0] = '\0';
	m_composing_char[1] = '\0';
	m_composing_string[0] = '\0';
}

void CI::ClearCurrentIMEComposition()
{
	m_composing_char[0] = '\0';
	m_composing_char[1] = '\0';
	m_composing_string[0] = '\0';
}

bool CI::GetEndOfIME()
{
	return m_bl_end_of_ime;
}

bool CI::GetCursorBlink() const
{
	return gbl_draw_cursor;
}

void CI::ForceShowCursor() const
{
	gbl_draw_cursor = true;
}

void CI::FinishImeRunning()
{
	m_bl_ime_running = false;
}

bool CI::IsEngInput() const
{
	/* 스텁: macOS에서는 영문 입력으로 간주 */
	return true;
}

void CI::SetEngInput(bool bHangul)
{
	/* 스텁: macOS에서는 아무 동작 안 함 */
	(void)bHangul;
}

//----------------------------------------------------------------------------
// CI_KOREAN 구현
//----------------------------------------------------------------------------
void CI_KOREAN::IME_MessageProcessor(UINT message, WPARAM wParam, LPARAM lParam)
{
	// macOS/SDL2에서는 Windows IME 시스템을 완전히 우회한다.
	// 키보드 이벤트는 이미 일반 이벤트 흐름을 통해 전달된다:
	// DXKeyboardEvent → gC_vs_ui.KeyboardControl → WindowManager → Window
	//
	// 여기서 메시지를 다시 전달하면 무한 루프가 발생하므로 해서는 안 된다:
	// KeyboardControl → IME_MessageProcessor → KeyboardControl → ...
	//
	// 이 함수는 호환성을 위해 스텁으로 남겨둔다.
	(void)message;
	(void)wParam;
	(void)lParam;
}

void CI_KOREAN::IME_NextComposition()
{
	/* 스텁: macOS에서는 한글 IME 미구현 */
}

void CI_KOREAN::IME_Composition()
{
	/* 스텁: macOS에서는 한글 IME 미구현 */
}

//----------------------------------------------------------------------------
// CI_CHINESE 구현
//----------------------------------------------------------------------------
void CI_CHINESE::IME_MessageProcessor(UINT message, WPARAM wParam, LPARAM lParam)
{
	// macOS/SDL2에서는 Windows IME 시스템을 완전히 우회한다.
	// 키보드 이벤트는 이미 일반 이벤트 흐름을 통해 전달된다:
	// DXKeyboardEvent → gC_vs_ui.KeyboardControl → WindowManager → Window
	//
	// 여기서 메시지를 다시 전달하면 무한 루프가 발생하므로 해서는 안 된다:
	// KeyboardControl → IME_MessageProcessor → KeyboardControl → ...
	//
	// 이 함수는 호환성을 위해 스텁으로 남겨둔다.
	(void)message;
	(void)wParam;
	(void)lParam;
}

void CI_CHINESE::IME_NextComposition()
{
	/* 스텁: macOS에서는 중국어 IME 미구현 */
}

void CI_CHINESE::IME_Composition()
{
	/* 스텁: macOS에서는 중국어 IME 미구현 */
}

// g_GetStringByMoney는 SDL 빌드에서 RenderingFunctions.cpp가 제공한다.
