/*-----------------------------------------------------------------------------

	AudioTypes.h

	플랫폼 독립적인 오디오 타입 정의 및 상수.
	DirectX DSBVOLUME_*, DSBCAPS_* 상수를 대체한다.

	2025.02.04 - DirectX 오디오 의존성을 제거하기 위해 생성됨

-----------------------------------------------------------------------------*/

#ifndef __AUDIOTYPES_H__
#define __AUDIOTYPES_H__

/* 기본 타입 정의를 위해 Platform.h를 include한다 */
#include "Platform.h"

/*-----------------------------------------------------------------------------
	볼륨 상수

	DirectX는 -10000에서 0까지의 로그 스케일 볼륨을 사용했다.
	SDL_mixer는 0에서 MIX_MAX_VOLUME(128)까지의 선형 스케일을 사용한다.

	호환성을 위해 DirectX 상수는 그대로 유지하되, 구현부에서
	SDL 볼륨 값으로 매핑한다.
-----------------------------------------------------------------------------*/

/* 볼륨 범위 (DirectX 호환 값) */
#ifndef DSBVOLUME_MIN
#define DSBVOLUME_MIN            -10000    /* 무음 */
#endif

#ifndef DSBVOLUME_MAX
#define DSBVOLUME_MAX            0         /* 최대 볼륨 */
#endif

/* 새로운 플랫폼 독립적 이름 */
#define AUDIO_VOLUME_MIN          DSBVOLUME_MIN
#define AUDIO_VOLUME_MAX          DSBVOLUME_MAX

/*-----------------------------------------------------------------------------
	버퍼 기능(Capabilities)

	이 플래그들은 사운드 버퍼 속성을 제어한다.
	SDL에서는 다르게 처리되지만, 기존 코드와의 호환성을 위해
	인터페이스를 유지한다.
-----------------------------------------------------------------------------*/

#ifndef DSBCAPS_CTRLFREQUENCY
#define DSBCAPS_CTRLFREQUENCY     0x00000001  /* 주파수 제어 */
#endif

#ifndef DSBCAPS_CTRLPAN
#define DSBCAPS_CTRLPAN           0x00000002  /* 팬(pan) 제어 */
#endif

#ifndef DSBCAPS_CTRLVOLUME
#define DSBCAPS_CTRLVOLUME        0x00000004  /* 볼륨 제어 */
#endif

#ifndef DSBCAPS_GLOBALFOCUS
#define DSBCAPS_GLOBALFOCUS       0x00000080  /* 전역 포커스 */
#endif

/* 새로운 플랫폼 독립적 이름 */
#define AUDIO_BUFFER_CTRLFREQUENCY    DSBCAPS_CTRLFREQUENCY
#define AUDIO_BUFFER_CTRLPAN          DSBCAPS_CTRLPAN
#define AUDIO_BUFFER_CTRLVOLUME       DSBCAPS_CTRLVOLUME
#define AUDIO_BUFFER_GLOBALFOCUS      DSBCAPS_GLOBALFOCUS

/*-----------------------------------------------------------------------------
	멀티미디어 구조체

	멀티미디어 구조체에 대한 플랫폼 독립적 정의.
	DirectX 멀티미디어 타입을 대체한다.
-----------------------------------------------------------------------------*/

/* HMMIO/MMCKINFO: 이 번역 단위(translation unit)에서 실제 <mmsystem.h>/
   <mmiscapi.h>가 이미 include된 경우(예: CMP3.cpp/MMusic.cpp는 여전히
   실제 MCI/mmio API를 호출함) 여기서의 호환용 정의는 건너뛴다 - 둘 다
   정의하면 호환되지 않는 재정의 오류(C2371/C2011)가 발생한다.
   _INC_MMSYSTEM과 _MMISCAPI_H_는 해당 헤더들 자신의 include 가드다. */
#if !defined(_INC_MMSYSTEM) && !defined(_MMISCAPI_H_)
/* 전방 선언 */
typedef DWORD FOURCC;
typedef void* HMMIO;

/* 멀티미디어 RIFF 청크 정보 */
typedef struct _MMCKINFO {
    FOURCC  ckid;
    FOURCC  fccType;
    DWORD   dwDataOffset;
    DWORD   dwSize;
} MMCKINFO;
#endif /* !_INC_MMSYSTEM && !_MMISCAPI_H_ */

/* DSBPOSITIONNOTIFY: 같은 방식으로, 이 번역 단위에서 실제 <dsound.h>가
   이미 include되어 있으면 그쪽을 그대로 사용한다. __DSOUND_INCLUDED__는
   dsound.h 자신의 include 가드다. */
#ifndef __DSOUND_INCLUDED__
typedef struct _DSBPOSITIONNOTIFY {
    DWORD   dwFlags;
    DWORD   dwOffset;
    DWORD   dwCallback;
} DSBPOSITIONNOTIFY;
#endif /* !__DSOUND_INCLUDED__ */

/* 멀티미디어 상수 */
#ifndef MMIO_READ
#define MMIO_READ      0
#endif

#ifndef MMIO_ALLOCBUF
#define MMIO_ALLOCBUF  0x10000
#endif

#ifndef FOURCC
#define FOURCC(a,b,c,d) (((DWORD)(a)<<0)|((DWORD)(b)<<8)|((DWORD)(c)<<16)|((DWORD)(d)<<24))
#endif

#endif // __AUDIOTYPES_H__
