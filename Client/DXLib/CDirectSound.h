//----------------------------------------------------------------------
// CDirectSound.h
//----------------------------------------------------------------------

#ifndef __CDirectSound_H__
#define __CDirectSound_H__

#pragma warning(disable:4786)

/* 플랫폼 독립적인 include (모든 플랫폼에서 SDL2 백엔드 사용) */
#include "../../basic/Platform.h"
#include "../../basic/AudioTypes.h"
#include <list>
#include <cstring>

/* DirectSound 타입에 대한 전방 선언 (불투명 포인터) */
/* 실제 구현은 모든 플랫폼에서 SDL_mixer를 사용한다 */
typedef struct IDirectSound* LPDIRECTSOUND;
#ifndef LPDIRECTSOUNDBUFFER
typedef struct IDirectSoundBuffer* LPDIRECTSOUNDBUFFER;
#endif
typedef std::list<LPDIRECTSOUNDBUFFER>	LPDIRECTSOUNDBUFFER_LIST;

class CSDLAudio
{

	public:		// 함수
		CSDLAudio();
		~CSDLAudio();

		//---------------------------------------------------------
		// 초기화 / 해제
		//---------------------------------------------------------
		bool					Init(HWND);									// 초기화
		void					Release();									// 제거
		void					ReleaseDuplicateBuffer();

	bool					IsInit() const;

		//---------------------------------------------------------
		// 로드 / 해제 / 복사
		//---------------------------------------------------------
		LPDIRECTSOUNDBUFFER		LoadWav(LPSTR filename);					// 화일 로드(*,wav)
		LPDIRECTSOUNDBUFFER		CreateBuffer(LPVOID sdat, DWORD size, DWORD caps, LPWAVEFORMATEX wfx);
		void					Release(LPDIRECTSOUNDBUFFER);				// 사운드 버퍼 제거
		LPDIRECTSOUNDBUFFER		DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, bool bAutoRelease=true);	// 사운드 버퍼 복사	

		//---------------------------------------------------------
		// 재생 / 정지
		//---------------------------------------------------------
		bool					IsPlay(LPDIRECTSOUNDBUFFER) const;								// 연주중인가?
		bool					NewPlay(LPDIRECTSOUNDBUFFER, bool bLoop=false);	// 사운드 플레이(처음부터 다시 시작)
		bool					Play(LPDIRECTSOUNDBUFFER, bool bLoop=false, bool bDuplicate=true);		// 사운드 플레이(끝날때까지 기다림)
		bool					Stop(LPDIRECTSOUNDBUFFER);					// 사운드 멈춤
		void					ReleaseTerminatedDuplicateBuffer();

		//---------------------------------------------------------
		// 음소거
		//---------------------------------------------------------
		bool					IsMute() const;
		void					SetMute();
		void					UnSetMute();

		//---------------------------------------------------------
		// 주파수
		//---------------------------------------------------------
		bool					AddFrequency(LPDIRECTSOUNDBUFFER, int);		// 주파수 올리기				
		bool					SubFrequency(LPDIRECTSOUNDBUFFER, int);		// 주파수 내리기				
	
		//---------------------------------------------------------
		// 볼륨
		//---------------------------------------------------------
		bool					SetMaxVolume(LPDIRECTSOUNDBUFFER buffer);
		bool					AddVolume(LPDIRECTSOUNDBUFFER, int);		// 볼륨 높임
		bool					SubVolume(LPDIRECTSOUNDBUFFER, int);		// 볼륨 내림				
		bool					SubVolumeFromMax(LPDIRECTSOUNDBUFFER, int);	// Max부터 볼륨 내림
		void					SetVolumeLimit(LONG volume);
		LONG					GetVolumeLimit() const;

		//---------------------------------------------------------
		// Pan
		//---------------------------------------------------------
		bool					RightPan(LPDIRECTSOUNDBUFFER, int);			// 오른쪽 팬
		bool					LeftPan(LPDIRECTSOUNDBUFFER, int);			// 왼쪽 팬
		bool					CenterToRightPan(LPDIRECTSOUNDBUFFER, int);			// 가운데부터 오른쪽 팬
		bool					CenterToLeftPan(LPDIRECTSOUNDBUFFER, int);			// 가운데부터 왼쪽 팬
		bool					CenterPan(LPDIRECTSOUNDBUFFER);				// 가운데 팬
		bool					ChangePan(LPDIRECTSOUNDBUFFER buffer, int pan);	// -10000 ~ 10000

		LPDIRECTSOUND			GetDS() const;

		

	protected:
		bool					DirectSoundFailed(const char *str);		


	protected :
		LPDIRECTSOUND			m_pDS;										// 다이렉트 사운드 오브젝트
		bool					m_bInit;

		bool					m_bMute;			// 소리를 내지 않는다.

		LONG					m_MaxVolume;		// 현재의 최대 소리 크기

		LPDIRECTSOUNDBUFFER_LIST	m_listDuplicatedBuffer;


	friend class CSDLMusic;
	friend class CSDLStream;
};

extern	CSDLAudio		g_SDLAudio;

#endif