//----------------------------------------------------------------------
// CDirectSoundStream.h
//----------------------------------------------------------------------
// 우헤헤..
// DirectX 샘플을 긁어서 급조된 필살 허접 class.. - -;
//----------------------------------------------------------------------

#ifndef __CSDLSTREAM_H__
#define __CSDLSTREAM_H__


#define NUM_PLAY_NOTIFICATIONS  16

/* 플랫폼 독립적인 include (모든 플랫폼에서 SDL2 백엔드 사용) */
#include "../../basic/Platform.h"
#include "../../basic/AudioTypes.h"

/* DirectSound 타입에 대한 전방 선언 (불투명 포인터) */
/* 실제 구현은 모든 플랫폼에서 SDL_mixer를 사용한다 */
struct IDirectSound;
struct IDirectSoundBuffer;
struct IDirectSoundNotify;

/* 타입 정의 */
#ifndef LPDIRECTSOUNDBUFFER
typedef struct IDirectSoundBuffer* LPDIRECTSOUNDBUFFER;
#endif
typedef struct IDirectSoundNotify* LPDIRECTSOUNDNOTIFY;

class CSDLStream {
	public :
		CSDLStream();
		~CSDLStream();


		void					Release();

		BOOL					IsLoad() const			{ return m_bLoad; }
		BOOL					IsPlay() const			{ return m_bPlay; }

		void					Load(LPSTR filename);

		void					Play(BOOL bLooped);
		void					Stop();

		void					SetVolumeLimit(LONG volume);
		LONG					GetVolumeLimit() const	{ return m_MaxVolume; }

		// main loop에서 돌려줘야 한다.
		void					Update();

		// get
		LPDIRECTSOUNDBUFFER		GetBuffer() const	{ return m_pDSBuffer; }

	protected :
		BOOL					UpdateProgress();
		BOOL					HandleNotification( BOOL bLooped );
		BOOL					FillBuffer( BOOL bLooped );
		BOOL					ReadStream( BOOL bLooped, VOID* pbBuffer, DWORD dwBufferLength );
		BOOL					RestoreBuffers( BOOL bLooped );

		BOOL					Reset();
		HRESULT					WaveReadFile( HMMIO hmmioIn, UINT cbRead, BYTE* pbDest, MMCKINFO* pckIn, UINT* cbActualRead );

	protected :
		BOOL					m_bLoad;
		BOOL					m_bPlay;
		BOOL					m_bLoop;

		LPDIRECTSOUNDBUFFER		m_pDSBuffer;
		LPDIRECTSOUNDNOTIFY		m_pDSNotify;

		DSBPOSITIONNOTIFY		m_aPosNotify[ NUM_PLAY_NOTIFICATIONS + 1 ];

		HANDLE					m_hNotificationEvents[2];

		// 음헤헤...
		DWORD					m_dwBufferSize;
		DWORD					m_dwNotifySize;
		DWORD					m_dwNextWriteOffset;
		DWORD					m_dwProgress;
		DWORD					m_dwLastPos;
		BOOL					m_bFoundEnd;

		// 가장 최근에 load한 Wav에 대한 정보
		WAVEFORMATEX			m_wavefmt;        // WAVEFORMATEX 구조체에 대한 포인터
		HMMIO					m_hmmioIn;     // WAVE에 대한 MM I/O 핸들
		MMCKINFO				m_ckIn;        // 멀티미디어 RIFF 청크
		MMCKINFO				m_ckInRiff;    // WAVE 파일을 열 때 사용

		LONG					m_MaxVolume;		// 현재의 최대 소리 크기
};

#endif

