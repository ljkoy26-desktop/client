//----------------------------------------------------------------------
// CDirectSound.cpp
//
// SDL2 구현 (크로스플랫폼)
// Windows DirectSound 구현은 제거됨 - 모든 플랫폼에서 SDL2_mixer를 사용한다
// 참고: 이 클래스는 더 이상 사용되지 않는다(deprecated). SDL_mixer 함수를 직접 사용하라.
//----------------------------------------------------------------------

#include "CDirectSound.h"

//-----------------------------------------------------------------------------
// 전역 인스턴스
//-----------------------------------------------------------------------------
CSDLAudio g_SDLAudio;

//-----------------------------------------------------------------------------
// 생성자/소멸자
//-----------------------------------------------------------------------------

CSDLAudio::CSDLAudio()
{
	m_pDS = NULL;
	m_bInit = false;
	m_bMute = false;
	m_MaxVolume = 0;
}

CSDLAudio::~CSDLAudio()
{
	// 스텁 - SDL mixer는 다른 곳에서 관리된다
}

//-----------------------------------------------------------------------------
// 초기화
//-----------------------------------------------------------------------------

bool CSDLAudio::Init(HWND hwnd)
{
	// 구현되지 않음 - SDL_mixer의 Mix_OpenAudio()를 대신 사용하라
	(void)hwnd;
	return false;
}

void CSDLAudio::Release()
{
	// 구현되지 않음 - SDL_mixer의 Mix_CloseAudio()를 대신 사용하라
}

bool CSDLAudio::IsInit() const
{
	return m_bInit;
}

//-----------------------------------------------------------------------------
// 음소거 제어
//-----------------------------------------------------------------------------

bool CSDLAudio::IsMute() const
{
	return m_bMute;
}

void CSDLAudio::SetMute()
{
	m_bMute = true;
}

void CSDLAudio::UnSetMute()
{
	m_bMute = false;
}

//-----------------------------------------------------------------------------
// 볼륨 제어
//-----------------------------------------------------------------------------

LONG CSDLAudio::GetVolumeLimit() const
{
	return m_MaxVolume;
}

void CSDLAudio::SetVolumeLimit(LONG volume)
{
	m_MaxVolume = volume;
}

bool CSDLAudio::SetMaxVolume(LPDIRECTSOUNDBUFFER buffer)
{
	// 구현되지 않음 - SDL_mixer의 Mix_Volume()을 대신 사용하라
	(void)buffer;
	return false;
}

bool CSDLAudio::AddVolume(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer의 Mix_Volume()을 대신 사용하라
	(void)buffer;
	(void)step;
	return false;
}

bool CSDLAudio::SubVolume(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer의 Mix_Volume()을 대신 사용하라
	(void)buffer;
	(void)step;
	return false;
}

bool CSDLAudio::SubVolumeFromMax(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer의 Mix_Volume()을 대신 사용하라
	(void)buffer;
	(void)step;
	return false;
}

//-----------------------------------------------------------------------------
// 주파수 제어
//-----------------------------------------------------------------------------

bool CSDLAudio::AddFrequency(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer는 실시간 주파수 변경을 지원하지 않는다
	(void)buffer;
	(void)step;
	return false;
}

bool CSDLAudio::SubFrequency(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer는 실시간 주파수 변경을 지원하지 않는다
	(void)buffer;
	(void)step;
	return false;
}

//-----------------------------------------------------------------------------
// 팬 제어
//-----------------------------------------------------------------------------

bool CSDLAudio::RightPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer는 실시간 패닝을 지원하지 않는다
	(void)buffer;
	(void)step;
	return false;
}

bool CSDLAudio::LeftPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer는 실시간 패닝을 지원하지 않는다
	(void)buffer;
	(void)step;
	return false;
}

bool CSDLAudio::CenterToRightPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer는 실시간 패닝을 지원하지 않는다
	(void)buffer;
	(void)step;
	return false;
}

bool CSDLAudio::CenterToLeftPan(LPDIRECTSOUNDBUFFER buffer, int step)
{
	// 구현되지 않음 - SDL_mixer는 실시간 패닝을 지원하지 않는다
	(void)buffer;
	(void)step;
	return false;
}

bool CSDLAudio::CenterPan(LPDIRECTSOUNDBUFFER buffer)
{
	// 구현되지 않음 - SDL_mixer는 실시간 패닝을 지원하지 않는다
	(void)buffer;
	return false;
}

bool CSDLAudio::ChangePan(LPDIRECTSOUNDBUFFER buffer, int pan)
{
	// 구현되지 않음 - SDL_mixer는 실시간 패닝을 지원하지 않는다
	(void)buffer;
	(void)pan;
	return false;
}

//-----------------------------------------------------------------------------
// 사운드 버퍼 연산
//-----------------------------------------------------------------------------

LPDIRECTSOUNDBUFFER CSDLAudio::LoadWav(LPSTR filename)
{
	// 구현되지 않음 - SDL_mixer의 Mix_LoadWAV()를 대신 사용하라
	(void)filename;
	return NULL;
}

LPDIRECTSOUNDBUFFER CSDLAudio::CreateBuffer(LPVOID sdat, DWORD size, DWORD caps, LPWAVEFORMATEX wfx)
{
	// 구현되지 않음 - SDL_mixer 사운드 함수를 대신 사용하라
	(void)sdat;
	(void)size;
	(void)caps;
	(void)wfx;
	return NULL;
}

void CSDLAudio::Release(LPDIRECTSOUNDBUFFER buffer)
{
	// 구현되지 않음 - SDL_mixer의 Mix_FreeChunk()를 대신 사용하라
	(void)buffer;
}

LPDIRECTSOUNDBUFFER CSDLAudio::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER buffer, bool bAutoRelease)
{
	// 구현되지 않음 - SDL_mixer는 버퍼 복제가 필요 없다
	(void)buffer;
	(void)bAutoRelease;
	return NULL;
}

void CSDLAudio::ReleaseDuplicateBuffer()
{
	// 구현되지 않음 - SDL_mixer에서는 필요 없다
}

void CSDLAudio::ReleaseTerminatedDuplicateBuffer()
{
	// 구현되지 않음 - SDL_mixer에서는 필요 없다
}

//-----------------------------------------------------------------------------
// 재생 제어
//-----------------------------------------------------------------------------

bool CSDLAudio::IsPlay(LPDIRECTSOUNDBUFFER buffer) const
{
	// 구현되지 않음 - SDL_mixer의 Mix_Playing()을 대신 사용하라
	(void)buffer;
	return false;
}

bool CSDLAudio::NewPlay(LPDIRECTSOUNDBUFFER buffer, bool loop)
{
	// 구현되지 않음 - SDL_mixer의 Mix_PlayChannel()을 대신 사용하라
	(void)buffer;
	(void)loop;
	return false;
}

bool CSDLAudio::Play(LPDIRECTSOUNDBUFFER buffer, bool loop, bool duplicate)
{
	// 구현되지 않음 - SDL_mixer의 Mix_PlayChannel()을 대신 사용하라
	(void)buffer;
	(void)loop;
	(void)duplicate;
	return false;
}

bool CSDLAudio::Stop(LPDIRECTSOUNDBUFFER buffer)
{
	// 구현되지 않음 - SDL_mixer의 Mix_HaltChannel()을 대신 사용하라
	(void)buffer;
	return false;
}

//-----------------------------------------------------------------------------
// DirectSound 접근
//-----------------------------------------------------------------------------

LPDIRECTSOUND CSDLAudio::GetDS() const
{
	return m_pDS;
}

//-----------------------------------------------------------------------------
// 오류 처리
//-----------------------------------------------------------------------------

bool CSDLAudio::DirectSoundFailed(const char* str)
{
	// 구현되지 않음 - 대신 콘솔에 오류를 기록하라
	(void)str;
	return false;
}
