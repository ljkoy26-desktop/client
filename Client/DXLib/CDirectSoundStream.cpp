//-----------------------------------------------------------------------------
// CDirectSoundStream.cpp
//
// SDL2 구현 (크로스플랫폼)
// Windows DirectSoundStream 구현은 제거됨 - 모든 플랫폼에서 SDL2_mixer를 사용한다
// 참고: 이 클래스는 더 이상 사용되지 않는다(deprecated). 스트리밍 오디오에는 SDL_mixer의 Mix_PlayChannel()을 사용하라.
//-----------------------------------------------------------------------------

#include "CDirectSoundStream.h"

// 호환성을 위해 E_FAIL을 정의한다 (Windows HRESULT 값)
#ifndef E_FAIL
#define E_FAIL 0x80004005L
#endif

//-----------------------------------------------------------------------------
// 생성자/소멸자
//-----------------------------------------------------------------------------

CSDLStream::CSDLStream()
{
	m_pDSBuffer = NULL;
	m_pDSNotify = NULL;

	m_bLoad = FALSE;
	m_bPlay = FALSE;
	m_bLoop = FALSE;

	m_hNotificationEvents[0] = NULL;
	m_hNotificationEvents[1] = NULL;

	m_MaxVolume = 0;
}

CSDLStream::~CSDLStream()
{
	// 스텁 - SDL mixer는 다른 곳에서 관리된다
}

//-----------------------------------------------------------------------------
// 스트림 연산
//-----------------------------------------------------------------------------

void CSDLStream::Release()
{
	// 구현되지 않음 - SDL_mixer의 Mix_FreeChunk()를 대신 사용하라
}

void CSDLStream::Load(LPSTR filename)
{
	// 구현되지 않음 - SDL_mixer의 Mix_LoadWAV()를 대신 사용하라
	(void)filename;
}

void CSDLStream::Play(BOOL bLooped)
{
	// 구현되지 않음 - SDL_mixer의 Mix_PlayChannel()을 대신 사용하라
	(void)bLooped;
}

void CSDLStream::Stop()
{
	// 구현되지 않음 - SDL_mixer의 Mix_HaltChannel()을 대신 사용하라
}

BOOL CSDLStream::FillBuffer(BOOL bLooped)
{
	// 구현되지 않음 - SDL_mixer가 버퍼링을 자동으로 처리한다
	(void)bLooped;
	return FALSE;
}

BOOL CSDLStream::ReadStream(BOOL bLooped, VOID* pbBuffer, DWORD dwBufferLength)
{
	// 구현되지 않음 - SDL_mixer가 스트리밍을 자동으로 처리한다
	(void)bLooped;
	(void)pbBuffer;
	(void)dwBufferLength;
	return FALSE;
}

BOOL CSDLStream::Reset()
{
	// 구현되지 않음 - SDL_mixer가 위치를 자동으로 처리한다
	return FALSE;
}

HRESULT CSDLStream::WaveReadFile(HMMIO hmmioIn, UINT cbRead, BYTE* pbDest,
	MMCKINFO* pckIn, UINT* cbActualRead)
{
	// 구현되지 않음 - Windows 멀티미디어 API 전용
	(void)hmmioIn;
	(void)cbRead;
	(void)pbDest;
	(void)pckIn;
	(void)cbActualRead;
	return E_FAIL;
}

//-----------------------------------------------------------------------------
// 알림 및 갱신
//-----------------------------------------------------------------------------

BOOL CSDLStream::HandleNotification(BOOL bLooped)
{
	// 구현되지 않음 - SDL_mixer는 알림 이벤트를 사용하지 않는다
	(void)bLooped;
	return FALSE;
}

BOOL CSDLStream::UpdateProgress()
{
	// 구현되지 않음 - SDL_mixer가 위치 추적을 처리한다
	return FALSE;
}

BOOL CSDLStream::RestoreBuffers(BOOL bLooped)
{
	// 구현되지 않음 - SDL_mixer가 버퍼 복원을 처리한다
	(void)bLooped;
	return FALSE;
}

void CSDLStream::Update()
{
	// 구현되지 않음 - SDL_mixer가 갱신을 자동으로 처리한다
}

//-----------------------------------------------------------------------------
// 볼륨 제어
//-----------------------------------------------------------------------------

void CSDLStream::SetVolumeLimit(LONG volume)
{
	// 구현되지 않음 - SDL_mixer의 Mix_Volume()을 대신 사용하라
	m_MaxVolume = volume;
}
