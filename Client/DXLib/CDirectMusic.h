//----------------------------------------------------------------------
// CSDLMusic.h
//----------------------------------------------------------------------

#ifndef __CSDLMUSIC_H__
#define __CSDLMUSIC_H__

/* 플랫폼 독립적인 include (모든 플랫폼에서 SDL2 백엔드 사용) */
#include "../../basic/Platform.h"

/* DirectMusic 타입에 대한 전방 선언 (불투명 포인터) */
/* 실제 구현은 모든 플랫폼에서 SDL_mixer를 사용한다 */
struct IDirectMusic;
struct IDirectMusicPerformance;
struct IDirectMusicPort;
struct IDirectMusicLoader;
struct IDirectMusicSegment;
struct IDirectMusicSegmentState;

/* DirectMusic 타입 정의 */
typedef long MUSIC_TIME;
typedef long long REFERENCE_TIME;

typedef enum DIRECTMUSIC_TYPE
{
	DIRECTMUSIC_TYPE_HW,	// 하드웨어 미디
	DIRECTMUSIC_TYPE_SW		// 마이크로소프트 소프트웨어 신디사이저
};

class CSDLMusic {
	public :
		CSDLMusic();
		~CSDLMusic();

		//-----------------------------------------------------------
		// 초기화 / 해제
		//-----------------------------------------------------------
		bool		Init(HWND hWnd, DIRECTMUSIC_TYPE type=DIRECTMUSIC_TYPE_SW);
		void		Release();


		//-----------------------------------------------------------
		// 재생 / 정지		일시정지 / 재개
		//-----------------------------------------------------------
		bool		Play(const char* filename, WORD repeat=0xFFFF);
		void		Stop();
		void		Pause();
		void		Resume();

		//-----------------------------------------------------------
		// 템포
		//-----------------------------------------------------------
		int			GetCurrentTempo() const		{ return m_CurrentTempo; }
		int			GetOriginalTempo() const	{ return m_OriginalTempo; }
		void		SetOriginalTempo()			{ SetCurrentTempo(m_OriginalTempo); }
		void		SetCurrentTempo(int t);

		//-----------------------------------------------------------
		// 볼륨
		//-----------------------------------------------------------
		//void		AddVolume(long hdec);
		//void		SubVolume(long hdec);

		//-----------------------------------------------------------
		// 상태
		//-----------------------------------------------------------
		bool		IsInit() const		{ return m_bInit; }
		bool		IsLoad() const		{ return m_bInit && m_bLoad; }
		bool		IsPlay() const		{ return m_bInit && m_bPlay; }
		bool		IsPause() const		{ return m_bInit && m_bLoad && !m_bPlay; }

		// 포트 타입
		bool		IsSoftwareSynth() const	{ return m_bInit && m_bSoftwareSynth; }

	protected :
		//-----------------------------------------------------------
		// Protected 함수
		//-----------------------------------------------------------
		bool		CreatePerformance();
		bool		CreatePort(DIRECTMUSIC_TYPE type);
		bool		CreateLoader();


	protected :
		IDirectMusic*				m_pDM;
		IDirectMusicPerformance*	m_pDMPerformance;
		IDirectMusicPort*			m_pDMPort;
		IDirectMusicLoader*			m_pDMLoader;
		IDirectMusicSegment*		m_pDMSegment;
		IDirectMusicSegmentState*	m_pDMSegmentState;

		MUSIC_TIME					m_mtStart;
		MUSIC_TIME                  m_mtOffset;
		REFERENCE_TIME              m_rtStart;
		REFERENCE_TIME              m_rtOffset;

		bool						m_bSoftwareSynth;

		bool						m_bInit;	// 초기화 되었는가?
		bool						m_bLoad;	// 화일이 Load되었는가?
		bool						m_bPlay;	// 연주 중인가?

		int							m_OriginalTempo;	// 원본 템포
		int							m_CurrentTempo;		// 현재 템포
		//long						m_MasterVolume;
};


extern CSDLMusic		g_SDLMusic;


#endif

