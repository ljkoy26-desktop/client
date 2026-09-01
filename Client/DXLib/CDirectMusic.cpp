//----------------------------------------------------------------------
// CSDLMusic.cpp
//
// SDL2 구현 (크로스플랫폼)
// Windows DirectMusic 구현은 제거됨 - 모든 플랫폼에서 SDL2_mixer를 사용한다
// 참고: 이 클래스는 더 이상 사용되지 않는다(deprecated). SDL_mixer의 Mix_PlayMusic()을 대신 사용하라.
//----------------------------------------------------------------------

#include "CDirectSound.h"
#include "CDirectMusic.h"

//-----------------------------------------------------------------------------
// 전역 인스턴스
//-----------------------------------------------------------------------------
CSDLMusic g_SDLMusic;

//-----------------------------------------------------------------------------
// 생성자/소멸자
//-----------------------------------------------------------------------------

CSDLMusic::CSDLMusic()
{
	m_pDM = NULL;
	m_pDMPerformance = NULL;
	m_pDMPort = NULL;
	m_pDMLoader = NULL;
	m_pDMSegment = NULL;
	m_pDMSegmentState = NULL;

	m_mtStart = 0;
	m_mtOffset = 0;
	m_rtStart = 0;
	m_rtOffset = 0;

	m_bInit = false;
	m_bLoad = false;
	m_bPlay = false;

	m_bSoftwareSynth = true;
}

CSDLMusic::~CSDLMusic()
{
	// 스텁 - SDL mixer는 다른 곳에서 관리된다
}

//-----------------------------------------------------------------------------
// 초기화
//-----------------------------------------------------------------------------

bool CSDLMusic::Init(HWND hWnd, DIRECTMUSIC_TYPE type)
{
	// 구현되지 않음 - SDL_mixer의 Mix_OpenAudio()를 대신 사용하라
	(void)hWnd;
	(void)type;
	return false;
}

void CSDLMusic::Release()
{
	// 구현되지 않음 - SDL_mixer의 Mix_CloseAudio()를 대신 사용하라
}

bool CSDLMusic::CreatePerformance()
{
	// 구현되지 않음 - SDL mixer에는 별도의 퍼포먼스 객체가 없다
	return false;
}

bool CSDLMusic::CreatePort(DIRECTMUSIC_TYPE type)
{
	// 구현되지 않음 - SDL mixer는 시스템 오디오 출력을 사용한다
	(void)type;
	return false;
}

bool CSDLMusic::CreateLoader()
{
	// 구현되지 않음 - SDL_mixer의 Mix_LoadMUS()를 대신 사용하라
	return false;
}

//-----------------------------------------------------------------------------
// 재생 제어
//-----------------------------------------------------------------------------

bool CSDLMusic::Play(const char* filename, WORD repeat)
{
	// 구현되지 않음 - SDL_mixer의 Mix_PlayMusic()을 대신 사용하라
	(void)filename;
	(void)repeat;
	return false;
}

void CSDLMusic::Pause()
{
	// 구현되지 않음 - SDL_mixer의 Mix_PauseMusic()을 대신 사용하라
}

void CSDLMusic::Resume()
{
	// 구현되지 않음 - SDL_mixer의 Mix_ResumeMusic()을 대신 사용하라
}

void CSDLMusic::Stop()
{
	// 구현되지 않음 - SDL_mixer의 Mix_HaltMusic()을 대신 사용하라
}

//-----------------------------------------------------------------------------
// 템포 제어
//-----------------------------------------------------------------------------

void CSDLMusic::SetCurrentTempo(int tempo)
{
	// 구현되지 않음 - SDL_mixer는 템포 변경을 지원하지 않는다
	(void)tempo;
}
