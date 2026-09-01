/*-----------------------------------------------------------------------------

	Platform.h

	Dark Eden 클라이언트를 위한 크로스플랫폼 추상화 계층.
	Windows, Linux, macOS를 위한 통합 API를 제공한다.

	원본 Windows API 의존성을 여기서 추상화한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Windows가 아닌 플랫폼을 위해 assert 매크로를 정의한다 */
#ifndef PLATFORM_WINDOWS
#ifndef assert
#define assert(e) ((void)(e))
#endif
#endif

/* ============================================================================
 * 플랫폼 감지
 * ============================================================================ */

/* 플랫폼을 감지한다 */
#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WINDOWS
#elif defined(__linux__)
	#define PLATFORM_LINUX
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_OS_MAC
		#ifndef PLATFORM_MACOS
			#define PLATFORM_MACOS
		#endif
	#endif
#else
	#define PLATFORM_UNKNOWN
#endif

/* ============================================================================
 * 플랫폼 선택
 * ============================================================================ */

/* Windows가 아닌 플랫폼에서는 SDL 백엔드를 강제한다 */
#ifndef PLATFORM_WINDOWS
	#ifndef PLATFORM_USE_SDL
		#define PLATFORM_USE_SDL
	#endif
#endif

/* Windows에서 명시적으로 SDL을 선택할 수 있도록 한다 */
#ifdef PLATFORM_USE_SDL
	#if __has_include(<SDL2/SDL.h>)
		#include <SDL2/SDL.h>
	#elif __has_include(<SDL.h>)
		#include <SDL.h>
	#else
		#include <SDL2/SDL.h>  // 어쨌든 시도한다 - include 경로는 CMake를 통해 설정되어야 한다
	#endif
#endif

/* Windows 빌드에서는 windows.h를 최대한 일찍 include하여, 이 헤더의 나머지
   부분이 Win32 타입/구조체/함수를 재정의(컴파일 실패나 조용한 불일치를
   유발함)하지 않고 실제 것을 그대로 사용하도록 한다. */
#ifdef PLATFORM_WINDOWS
	#ifndef _WINDOWS_
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
#endif

/* ============================================================================
 * 호출 규약
 * ============================================================================ */

/* Windows가 아닌 플랫폼을 위해 호출 규약을 정의한다 */
#ifndef PLATFORM_WINDOWS
	#ifndef __cdecl
		#define __cdecl
	#endif
	#ifndef __stdcall
		#define __stdcall
	#endif
	#ifndef WINAPI
		#define WINAPI
	#endif
	#ifndef APIENTRY
		#define APIENTRY
	#endif
	#ifndef CALLBACK
		#define CALLBACK
	#endif
	#ifndef INLINE
		#define INLINE inline
	#endif
#endif

/* ============================================================================
 * 기본 타입 정의 (Typedef.h에서 가져옴)
 * ============================================================================ */

#ifndef NULL
	#define NULL 0
#endif

#define NOT_SELECTED						-1

/* 타입 정의 (원본 Typedef.h와 동일)
   Windows에서는 위에서 include한 <windows.h>의 타입을 대신 사용한다.
   해당 타입들(DWORD, LONG 등)은 여기의 고정폭 대응 타입과
   서로 바꿔 쓸 수 없기 때문이다. */
#ifndef PLATFORM_WINDOWS
typedef uint8_t			BYTE;
typedef uint16_t		WORD;
typedef uint32_t		UINT;
typedef uint32_t		DWORD;
typedef uint32_t		ULONG;
typedef uint64_t		DWORD64;
typedef uint64_t		ULONGLONG;
typedef int64_t			LONGLONG;
typedef void*			PVOID;
typedef void*			ADDRESS_MODE;
typedef uintptr_t		ULONG_PTR;
typedef intptr_t		LONG_PTR;
typedef uintptr_t		DWORD_PTR;
typedef int32_t			LONG;
typedef int				BOOL;

	/* 크로스플랫폼 호환성을 위해 id_t를 정의한다 (모든 플랫폼에서 unsigned int) */
	typedef unsigned int   id_t;
#endif /* !PLATFORM_WINDOWS */

/* QWORD는 실제 Win32 API에 속하지 않는 프로젝트 전용 타입이므로
   <windows.h>는 이를 정의하지 않는다 - 모든 플랫폼에서 사용 가능하도록 유지한다. */
#ifndef QWORD_DEFINED
#define QWORD_DEFINED
typedef uint64_t		QWORD;
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* 오디오 포맷을 위한 WAVEFORMATEX 구조체 */
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct _WAVEFORMATEX {
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX, *PWAVEFORMATEX;
#endif

/* WAVE 포맷 상수 */
#define WAVE_FORMAT_PCM		1
#define WAVE_FORMAT_ADPCM	2

/* DirectSound 버퍼 기능 */
#ifndef PLATFORM_WINDOWS
#define DSBCAPS_PRIMARYBUFFER		0x00000001
#define DSBCAPS_STATIC			0x00000002
#define DSBCAPS_LOCHARDWARE		0x00000004
#define DSBCAPS_LOCSOFTWARE		0x00000008
#define DSBCAPS_CTRL3D			0x00000010
#define DSBCAPS_CTRLFREQUENCY		0x00000020
#define DSBCAPS_CTRLPAN			0x00000040
#define DSBCAPS_CTRLVOLUME		0x00000080
#define DSBCAPS_CTRLPOSITIONNOTIFY	0x00000100
#define DSBCAPS_CTRLFX			0x00000200
#define DSBCAPS_STICKYFOCUS		0x00004000
#define DSBCAPS_GLOBALFOCUS		0x00008000
#define DSBCAPS_GETCURRENTPOSITION2	0x00010000
#define DSBCAPS_MUTE3DATMAX		0x00020000
#define DSBCAPS_MIXIN			0x00040000
#define DSBCAPS_TRUEPLAYPOSITION	0x00080000
#endif

/* Windows가 아닌 플랫폼을 위한 DirectSound 타입 */
#ifndef PLATFORM_WINDOWS
struct IDirectSound;
struct IDirectSoundBuffer;
struct IDirectSoundNotify;

#ifndef LPDIRECTSOUNDBUFFER
typedef struct IDirectSoundBuffer* LPDIRECTSOUNDBUFFER;
#endif
typedef struct IDirectSound* LPDIRECTSOUND;
typedef struct IDirectSoundNotify* LPDIRECTSOUNDNOTIFY;
#endif

/* 스레드 동기화를 위한 CRITICAL_SECTION */
#if !defined(_CRITICAL_SECTION_DEFINED) && !defined(PLATFORM_WINDOWS)
#define _CRITICAL_SECTION_DEFINED
#include <pthread.h>

typedef struct _CRITICAL_SECTION {
	pthread_mutex_t mutex;
	int initialized;
} CRITICAL_SECTION, *PCRITICAL_SECTION, *LPCRITICAL_SECTION;

/* 임계 구역(critical section) 함수 - macOS를 위한 pthread 기반 구현 */
/* 참고: Windows CRITICAL_SECTION 동작과 맞추기 위해 재귀 뮤텍스를 사용한다 */
static inline void InitializeCriticalSection(CRITICAL_SECTION* cs) {
	if (cs != NULL) {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);  // 재귀 락
		pthread_mutex_init(&cs->mutex, &attr);
		pthread_mutexattr_destroy(&attr);
		cs->initialized = 1;
	}
}

static inline void EnterCriticalSection(CRITICAL_SECTION* cs) {
	if (cs != NULL && cs->initialized) {
		pthread_mutex_lock(&cs->mutex);
	}
}

static inline void LeaveCriticalSection(CRITICAL_SECTION* cs) {
	if (cs != NULL && cs->initialized) {
		pthread_mutex_unlock(&cs->mutex);
	}
}

static inline void DeleteCriticalSection(CRITICAL_SECTION* cs) {
	if (cs != NULL && cs->initialized) {
		pthread_mutex_destroy(&cs->mutex);
		cs->initialized = 0;
	}
}

/* GDI 객체 관리 함수 - 스텁 구현 */
static inline int DeleteObject(void* hObject) {
	(void)hObject;
	/* 스텁 - Windows GDI 객체 삭제 */
	return 1; /* TRUE를 반환한다 */
}

/* 폰트 굵기 상수 */
#define FW_NORMAL 400
#define FW_BOLD 700
#define FW_THIN 100
#define FW_MEDIUM 500
#define FW_HEAVY 900
#define FW_EXTRABOLD 800
#define FW_LIGHT 300

/* 폰트 생성을 위한 LOGFONT 구조체 */
#ifndef LOGFONT_DEFINED
#define LOGFONT_DEFINED
typedef struct tagLOGFONT {
	long lfHeight;
	long lfWidth;
	long lfEscapement;
	long lfOrientation;
	long lfWeight;
	unsigned char lfItalic;
	unsigned char lfUnderline;
	unsigned char lfStrikeOut;
	unsigned char lfCharSet;
	unsigned char lfOutPrecision;
	unsigned char lfClipPrecision;
	unsigned char lfQuality;
	unsigned char lfPitchAndFamily;
	char lfFaceName[32];
} LOGFONT, *PLOGFONT, *LPLOGFONT;
#endif

/* 문자 집합 상수 */
#define ANSI_CHARSET 0
#define DEFAULT_CHARSET 1
#define SYMBOL_CHARSET 2
#define SHIFTJIS_CHARSET 128
#define HANGUL_CHARSET 129
#define GB2312_CHARSET 134
#define OEM_CHARSET 255

/* 출력 정밀도 상수 */
#define OUT_DEFAULT_PRECIS 0
#define OUT_STRING_PRECIS 1
#define OUT_CHARACTER_PRECIS 2
#define OUT_STROKE_PRECIS 3
#define OUT_TT_PRECIS 4
#define OUT_DEVICE_PRECIS 5
#define OUT_RASTER_PRECIS 6
#define OUT_TT_ONLY_PRECIS 7
#define OUT_OUTLINE_PRECIS 8
#define OUT_SCREEN_OUTLINE_PRECIS 9
#define OUT_PS_ONLY_PRECIS 10

/* 클립 정밀도 상수 */
#define CLIP_DEFAULT_PRECIS 0
#define CLIP_CHARACTER_PRECIS 1
#define CLIP_STROKE_PRECIS 2
#define CLIP_MASK 0xf
#define CLIP_LH_ANGLES (1<<4)
#define CLIP_TT_ALWAYS (2<<4)
#define CLIP_EMBEDDED (8<<4)

/* 폰트 품질 상수 */
#define DEFAULT_QUALITY 0
#define DRAFT_QUALITY 1
#define PROOF_QUALITY 2
#define NONANTIALIASED_QUALITY 3
#define ANTIALIASED_QUALITY 4
#define CLEARTYPE_QUALITY 5

/* 폰트 피치 및 계열 상수 */
#define DEFAULT_PITCH 0
#define FIXED_PITCH 1
#define VARIABLE_PITCH 2
#define FF_DONTCARE 0
#define FF_ROMAN 1
#define FF_SWISS 2
#define FF_MODERN 3
#define FF_SCRIPT 4
#define FF_DECORATIVE 5

/* 배경 모드 상수 */
#define TRANSPARENT 1
#define OPAQUE 2

/* 텍스트 정렬 상수 */
#define TA_NOUPDATECP 0
#define TA_LEFT 0
#define TA_TOP 0
#define TA_UPDATECP 1
#define TA_RIGHT 2
#define TA_CENTER 6
#define TA_BASELINE 24

/* DirectDraw 서페이스 기능 */
#define DDSCAPS_SYSTEMMEMORY 0x00000800L

/* GDI 폰트 생성 함수 - 스텁 구현 */
static inline void* CreateFontIndirect(LOGFONT* lplf) {
	(void)lplf;
	/* 스텁 - Windows에서는 폰트를 생성했을 것이다 */
	return (void*)1; /* null이 아닌 핸들을 반환한다 */
}
#endif

/* DirectDraw 서페이스 기능 - 실제 Win32 API에 속하지 않는 프로젝트 전용
   상수이므로 <windows.h>는 이를 정의하지 않는다 - 모든 플랫폼에서 사용
   가능하도록 유지한다 (위 QWORD와 같은 이유). */
#ifndef DDSCAPS_SYSTEMMEMORY
#define DDSCAPS_SYSTEMMEMORY 0x00000800L
#endif

/* Windows 경로 상수 */
#ifndef _MAX_PATH
	#define _MAX_PATH	260
#endif

/* 호환성을 위해 밑줄 없는 MAX_PATH를 정의한다 */
#ifndef MAX_PATH
	#define MAX_PATH _MAX_PATH
#endif

/* 색상 타입 정의 */
typedef DWORD			COLORREF;
#define RGB(r,g,b)		((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

/* id_t는 macOS/Linux에서 POSIX와 충돌하므로 Windows에서만 정의한다 */
#ifdef PLATFORM_WINDOWS
typedef DWORD			id_t;
#endif

typedef WORD			char_t;

/* ============================================================================
 * 공통 Windows 타입 정의 (크로스플랫폼 호환성을 위함)
 * ============================================================================ */

#ifndef PLATFORM_WINDOWS
	/* Windows가 아닌 플랫폼을 위한 Windows 호환 타입 정의 */
	typedef int				BOOL;
	/* 크로스플랫폼 호환성을 위해 id_t를 정의한다 (모든 플랫폼에서 unsigned int) */
	typedef unsigned int   id_t;
	#ifndef TRUE
		#define TRUE	1
	#endif
	#ifndef FALSE
		#define FALSE	0
	#endif

	typedef int32_t			HRESULT;
	typedef intptr_t		LRESULT;
	typedef uintptr_t		UINT_PTR;
	#ifndef S_OK
		#define S_OK		0
	#endif
	#ifndef S_FALSE
		#define S_FALSE		1
	#endif

	/* HRESULT 매크로 */
	#ifndef SUCCEEDED
		#define SUCCEEDED(hr)	(((HRESULT)(hr)) >= 0)
	#endif
	#ifndef FAILED
		#define FAILED(hr)		(((HRESULT)(hr)) < 0)
	#endif

	/* VOID 타입 */
	#ifndef VOID
		typedef void		VOID;
	#endif

	/* 추가 Windows 타입 */
	typedef int32_t			LONG;
	typedef void*			LPVOID;
	typedef void*			HWND;
	typedef void*			HDC;
	typedef void*			HFONT;
	typedef void*			HINSTANCE;
	typedef void*			HANDLE;
	typedef DWORD*			LPDWORD;
	typedef const char*		LPCSTR;
	typedef char*			LPSTR;
	typedef const char*		LPCTSTR;
	typedef char*			LPTSTR;
	typedef const wchar_t*	LPCWSTR;
	typedef wchar_t*		LPWSTR;
	typedef unsigned char*	LPBYTE;
	typedef intptr_t		LPARAM;
	typedef intptr_t		WPARAM;
	typedef uint32_t			UINT;

	/* MessageBox 상수 */
	#define MB_OK			0x00000000L
	#define MB_ICONERROR		0x00000010L

	/* Windows 파라미터 주석 매크로 (콜백 함수 시그니처용) */
	#ifndef IN
		#define IN
	#endif
	#ifndef OUT
		#define OUT
	#endif
	#ifndef OPTIONAL
		#define OPTIONAL
	#endif

	/* 문자 타입 매크로 */
	#ifndef _T
		#define _T(x)		x
	#endif
	#ifndef TEXT
		#define TEXT(x)	x
	#endif
	#ifndef _L
		#define _L(x)		x
	#endif

	/* TCHAR 및 관련 타입 */
	#ifndef UNICODE
		typedef char			TCHAR;
		#define _tcscat		strcat
		#define _tcscpy		strcpy
		#define _tcslen		strlen
		#define _tcschr		strchr
		#define _tcsrchr		strrchr
		#define _stprintf	sprintf
		#define _tprintf		printf
		#define _tmain		main
	#else
		typedef wchar_t		TCHAR;
		#define _tcscat		wcscat
		#define _tcscpy		wcscpy
		#define _tcslen		wcslen
		#define _tcschr		wcschr
		#define _tcsrchr		wcsrchr
		#define _stprintf	swprintf
		#define _tprintf		wprintf
		#define _tmain		wmain
	#endif

	typedef TCHAR*			LPTSTR;
	typedef const TCHAR*	LPCTSTR;

	/* 이전 코드와의 호환성을 위한 _TCHAR 별칭 */
	#ifndef _TCHAR
		#define _TCHAR	TCHAR
	#endif

	/* MessageBox 스텁 - stderr에 출력만 한다 */
	static inline int MessageBox(void* hWnd, const char* lpText, const char* lpCaption, unsigned int uType) {
		(void)hWnd; (void)uType;
		fprintf(stderr, "[%s] %s\n", lpCaption, lpText);
		return 1;
	}

	/* SystemParametersInfo 상수 */
	#define SPI_GETMOUSE			0x0003
	#define SPI_SETMOUSE			0x0004

	/* SystemParametersInfo 스텁 */
	static inline BOOL SystemParametersInfo(UINT uiAction, UINT uiParam, void* pvParam, UINT fWinIni) {
		(void)uiAction; (void)uiParam; (void)pvParam; (void)fWinIni;
		// Windows가 아닌 플랫폼에는 동일한 방식의 마우스 가속 설정이 없다
		return FALSE;
	}

	/* 참고: GetCursorPos와 ScreenToClient는 아래에 인라인 함수로 정의되어 있지만,
	   먼저 POINT가 완전히 정의되어 있어야 한다 (Client_PCH.h에서 include됨) */

	/* GetCursorPos 스텁 - SDL에서 마우스 위치를 가져온다 */
	static inline BOOL GetCursorPos(void* lpPoint) {
		if (lpPoint) {
			typedef struct { LONG x; LONG y; } POINT;
			POINT* p = (POINT*)lpPoint;
			int x, y;
			SDL_GetMouseState(&x, &y);
			p->x = x;
			p->y = y;
			return TRUE;
		}
		return FALSE;
	}

	/* ScreenToClient 스텁 - SDL에서는 아무 동작도 하지 않는다 (좌표가 이미 창 기준 상대 좌표임) */
	static inline BOOL ScreenToClient(void* hWnd, void* lpPoint) {
		(void)hWnd;
		// SDL은 이미 창 기준 상대 좌표를 제공한다
		return lpPoint ? TRUE : FALSE;
	}

	/* 문자열 비교 (대소문자 무시) - Windows stricmp와 동일 */
	#define stricmp strcasecmp

	/* Microsoft 전용 문자열 함수 - 표준 대응 함수를 사용한다 */
	#define _stscanf sscanf

	/* tlhelp32.h 함수를 위한 프로세스 및 스레드 스텁 */
	#define INVALID_HANDLE_VALUE ((HANDLE)-1)
	#define TH32CS_SNAPPROCESS 0x00000002
	#define CloseHandle(handle) /* Windows가 아닌 곳에서는 아무 동작도 하지 않는다 */

	typedef struct {
		DWORD dwSize;              // 구조체 길이(바이트)
		DWORD cntUsage;
		DWORD th32ProcessID;
		ULONG_PTR th32DefaultHeapID;
		DWORD th32ModuleID;
		DWORD cntThreads;
		DWORD th32ParentProcessID;
		LONG pcPriClassBase;
		DWORD dwFlags;
		char szExeFile[MAX_PATH];
	} PROCESSENTRY32;

	/* CreateToolhelp32Snapshot 스텁 - Windows가 아닌 곳에서는 잘못된 핸들을 반환한다 */
	static inline void* CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID) {
		(void)dwFlags;
		(void)th32ProcessID;
		return INVALID_HANDLE_VALUE;
	}

	/* Process32First 스텁 - Windows가 아닌 곳에서는 항상 실패한다 */
	static inline BOOL Process32First(void* hSnapshot, PROCESSENTRY32* lppe) {
		(void)hSnapshot;
		(void)lppe;
		return FALSE;
	}

	/* Process32Next 스텁 - Windows가 아닌 곳에서는 항상 실패한다 */
	static inline BOOL Process32Next(void* hSnapshot, PROCESSENTRY32* lppe) {
		(void)hSnapshot;
		(void)lppe;
		return FALSE;
	}

	/* FindWindow 스텁 - Windows가 아닌 곳에서는 구현되지 않는다 */
	static inline void* FindWindow(const char* lpClassName, const char* lpWindowName) {
		(void)lpClassName;
		(void)lpWindowName;
		return NULL; // Windows가 아닌 플랫폼에서는 창을 찾을 수 없다
	}

	/* ShowCursor 스텁 - Windows가 아닌 곳에서는 항상 0을 반환한다 (커서 숨김) */
	static inline int ShowCursor(BOOL bShow) {
		(void)bShow;
		return 0;
	}

	/* InitCommonControls 스텁 - Windows가 아닌 곳에서는 아무 동작도 하지 않는다 */
	#define InitCommonControls()

	/* GetSystemMetrics 상수 */
	#define SM_CXSCREEN 0
	#define SM_CYSCREEN 1
	#define SM_CYVSCROLL 20
	#define SM_CYSIZEFRAME 33
	#define SM_CYMENU 15
	#define SM_CXSIZEFRAME 32

	/* GetSystemMetrics 스텁 - Windows가 아닌 곳에서는 기본값을 반환한다 */
	static inline int GetSystemMetrics(int nIndex) {
		switch(nIndex) {
			case SM_CXSCREEN: return 1024;
			case SM_CYSCREEN: return 768;
			case SM_CYVSCROLL: return 16;
			default: return 0;
		}
	}

	/* 윈도우 스타일 상수 */
	#define WS_EX_TOPMOST 0x00000008
	#define WS_EX_APPWINDOW 0x00040000
	#define WS_VISIBLE 0x10000000
	#define WS_POPUP 0x80000000L
	#define WS_OVERLAPPED 0x00000000L
	#define WS_CLIPCHILDREN 0x02000000L
	#define WS_THICKFRAME 0x00040000L
	#define WS_MINIMIZEBOX 0x00020000L
	#define WS_SYSMENU 0x00080000L
	#define SW_HIDE 0

	/* 진행 표시줄 상수 */
	#define PROGRESS_CLASS "PROGRESS_CLASS"
	#define PBS_SMOOTH 0x01
	#define PBM_SETRANGE (WM_USER+1)
	#define PBM_SETSTEP (WM_USER+4)
	#define PBM_SETPOS (WM_USER+2)
	#define PBM_STEPIT (WM_USER+5)

	/* 윈도우 클래스 스타일 */
	#define CS_HREDRAW 0x0001
	#define CS_VREDRAW 0x0002
	#define CS_DBLCLKS 0x0008

	/* WNDCLASS 구조체 (Windows 윈도우 클래스 등록) */
	typedef struct tagWNDCLASS {
		UINT style;
		void* lpfnWndProc;
		int cbClsExtra;
		int cbWndExtra;
		void* hInstance;
		void* hIcon;
		void* hCursor;
		void* hbrBackground;
		const char* lpszMenuName;
		const char* lpszClassName;
	} WNDCLASS, *PWNDCLASS, *LPWNDCLASS;

	/* 메시지 매크로 */
	#define MAKELPARAM(l, h) ((LPARAM)(((DWORD_PTR)(l) & 0xFFFF) | ((DWORD_PTR)(h) << 16)))
	#define WM_USER 0x0400
	#define WM_TIMER 0x0113
	#define WM_CHAR 0x0102
	#define WM_KEYUP 0x0101
	#define WM_IME_COMPOSITION 0x010F
	#define WM_IME_STARTCOMPOSITION 0x010D
	#define WM_IME_ENDCOMPOSITION 0x010E

	/* SDL 텍스트 입력 메시지 */
	#define WM_TEXTINPUT 0x0111  /* SDL 텍스트 입력 이벤트 (확정된 텍스트) */
	#define WM_TEXTEDITING 0x0110  /* SDL 텍스트 편집 이벤트 (IME 조합 중) */

	/* 윈도우 메시지 */
	#define WM_DESTROY 0x0002
	#define WM_CLOSE 0x0010
	#define WM_QUIT 0x0012
	#define WM_SYSCOMMAND 0x0112
	#define WM_MOVE 0x0003
	#define WM_KEYDOWN 0x0100
	#define WM_GETMINMAXINFO 0x0024
	#define WM_ACTIVATEAPP 0x001C

	/* 가상 키 코드 */
	#define VK_SPACE 0x20
	#define VK_RETURN 0x0D
	#define VK_ESCAPE 0x1B
	#define VK_SCROLL 0x91

	/* 코드 페이지 상수 */
	#define CP_ACP 0
	#define CP_OEMCP 1
	#define CP_UTF8 65001
	#define WC_COMPOSITECHECK 0x00000200

	/* 시스템 명령 값 */
	#define SC_HOTKEY 0xF150
	#define SC_KEYMENU 0xF100
	#define SC_TASKLIST 0xF140
	#define SC_PREVWINDOW 0xF050
	#define SC_NEXTWINDOW 0xF040
	#define SC_CLOSE 0xF060
	#define SC_MOVE 0xF010
	#define SC_SIZE 0xF000
	#define SC_SCREENSAVE 0xF140  // 참고: SC_TASKLIST와 같은 값
	#define SC_MONITORPOWER 0xF170
	#define SC_MAXIMIZE 0xF030

	/* MCI 메시지 - 일부 코드에서 여전히 필요함 */
	#define MM_MCINOTIFY 0x3D9
	#define MCI_NOTIFY_SUCCESSFUL 0x0001
	#define MCI_NOTIFY_SUPERCEDED 0x0002
	#define MCI_NOTIFY_ABORTED 0x0004
	#define MCI_NOTIFY_FAILURE 0x0008

	/* SendMessage 스텁 - Windows가 아닌 곳에서는 아무 동작도 하지 않는다 */
	static inline LRESULT SendMessage(void* hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		(void)hWnd; (void)Msg; (void)wParam; (void)lParam;
		return 0;
	}

	/* WideCharToMultiByte 스텁 - Windows가 아닌 곳을 위한 기본 변환 */
	static inline int WideCharToMultiByte(UINT CodePage, DWORD dwFlags,
		LPCWSTR lpWideCharStr, int cchWideChar,
		LPSTR lpMultiByteStr, int cbMultiByte,
		LPCSTR lpDefaultChar, BOOL* lpUsedDefaultChar) {
		(void)CodePage; (void)dwFlags; (void)lpDefaultChar; (void)lpUsedDefaultChar;
		/* Windows가 아닌 플랫폼을 위한 기본적인 UTF-16 -> UTF-8 변환 */
		if (cchWideChar == -1) {
			/* null 종료 문자를 찾는다 */
			int len = 0;
			while (lpWideCharStr[len]) len++;
			cchWideChar = len;
		}
		/* 단순 변환 - 하위 바이트만 복사한다 (ASCII에서 동작) */
		for (int i = 0; i < cchWideChar && i < cbMultiByte - 1; i++) {
			lpMultiByteStr[i] = (char)(lpWideCharStr[i] & 0xFF);
		}
		lpMultiByteStr[cchWideChar < cbMultiByte ? cchWideChar : cbMultiByte - 1] = '\0';
		return cchWideChar;
	}

	/* SetWindowText 스텁 - Windows가 아닌 곳에서는 아무 동작도 하지 않는다 */
	static inline BOOL SetWindowText(void* hWnd, const char* lpText) {
		(void)hWnd; (void)lpText;
		return TRUE;
	}

	/* ShowWindow 스텁 - Windows가 아닌 곳에서는 아무 동작도 하지 않는다 */
	static inline BOOL ShowWindow(void* hWnd, int nCmdShow) {
		(void)hWnd; (void)nCmdShow;
		return TRUE;
	}

	/* CreateWindowEx 스텁 - Windows가 아닌 곳에서는 NULL을 반환한다 (창 생성 없음) */
	static inline void* CreateWindowEx(DWORD dwExStyle, const char* lpClassName,
	                                    const char* lpWindowName, DWORD dwStyle,
	                                    int X, int Y, int nWidth, int nHeight,
	                                    void* hWndParent, void* hMenu, void* hInstance, void* lpParam) {
		(void)dwExStyle; (void)lpClassName; (void)lpWindowName; (void)dwStyle;
		(void)X; (void)Y; (void)nWidth; (void)nHeight;
		(void)hWndParent; (void)hMenu; (void)hInstance; (void)lpParam;
		return NULL; // Windows가 아닌 플랫폼에서는 창을 생성하지 않는다
	}

	/* RegisterClass 스텁 - Windows가 아닌 곳에서는 0(atom)을 반환한다 */
	static inline unsigned short RegisterClass(const WNDCLASS* lpWndClass) {
		(void)lpWndClass;
		return 0;
	}

	/* 스톡 오브젝트 상수 */
	#define BLACK_BRUSH 4
	#define WHITE_BRUSH 0
	#define DC_BRUSH 18

	/* GetStockObject 스텁 - Windows가 아닌 곳에서는 NULL을 반환한다 */
	static inline void* GetStockObject(int nIndex) {
		(void)nIndex;
		return NULL;
	}

	/* LoadIcon 스텁 - Windows가 아닌 곳에서는 NULL을 반환한다 */
	static inline void* LoadIcon(void* hInstance, const char* lpIconName) {
		(void)hInstance; (void)lpIconName;
		return NULL;
	}

	/* LoadCursor 스텁 - Windows가 아닌 곳에서는 NULL을 반환한다 */
	static inline void* LoadCursor(void* hInstance, const char* lpCursorName) {
		(void)hInstance; (void)lpCursorName;
		return NULL;
	}

	/* SetCursor 스텁 - Windows가 아닌 곳에서는 NULL을 반환한다 */
	static inline void* SetCursor(void* hCursor) {
		(void)hCursor;
		return NULL;
	}

	/* UpdateWindow 스텁 - Windows가 아닌 곳에서는 아무 동작도 하지 않는다 */
	static inline BOOL UpdateWindow(void* hWnd) {
		(void)hWnd;
		return TRUE;
	}

	/* SetFocus 스텁 - Windows가 아닌 곳에서는 NULL을 반환한다 */
	static inline void* SetFocus(void* hWnd) {
		(void)hWnd;
		return NULL;
	}

	/* DefWindowProc 스텁 - 기본 윈도우 프로시저 */
	static inline LRESULT DefWindowProc(void* hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		(void)hWnd; (void)Msg; (void)wParam; (void)lParam;
		return 0;
	}

	/* PostQuitMessage 스텁 - Windows가 아닌 곳에서는 아무 동작도 하지 않는다 */
	static inline void PostQuitMessage(int nExitCode) {
		(void)nExitCode;
	}

	/* GetDoubleClickTime 스텁 - Windows가 아닌 곳에서는 기본값 500ms를 반환한다 */
	static inline int GetDoubleClickTime() {
		return 500; // 기본 더블클릭 시간(밀리초)
	}

	/* HMENU 타입 정의 */
	typedef void* HMENU;
	typedef void* HBRUSH;
	typedef void* HICON;
	typedef void* HCURSOR;

	/* 리소스 관리 매크로 */
	#define MAKEINTRESOURCE(i) (LPCTSTR)((DWORD_PTR)((WORD)(i)))

	/* 표준 커서 */
	#define IDC_ARROW ((LPCTSTR)"MAKEINTRESOURCE(32512)")

	/* Windows 콜백 규약을 위한 FAR PASCAL 매크로 */
	#ifndef FAR
		#define FAR
	#endif

	#ifndef PASCAL
		#define PASCAL
	#endif

	/* FARPROC - 함수 포인터 (Windows 콜백 타입) */
	typedef int (*FARPROC)();

	/* 콜백 함수 타입 */
	typedef long (__cdecl *WNDPROC)(void*, unsigned int, unsigned long, long long);

	/* IWebBrowser2 스텁 - 웹 브라우저 제어용 COM 인터페이스 */
	#ifndef IWebBrowser2_DEFINED
	#define IWebBrowser2_DEFINED
	typedef void* IWebBrowser2;
	#endif

	/* Windows 타이밍 함수 */
	#define GetTickCount()		platform_get_ticks()
	#define timeGetTime()		platform_get_ticks()
#endif

#ifdef PLATFORM_WINDOWS
	/* timeGetTime()은 원래 <mmsystem.h>(winmm)에 있지만, 이 프로젝트는
	   HMMIO/MMCKINFO 등 basic/AudioTypes.h의 SDL 대체 타입과 충돌하는
	   요소들을 함께 끌어오기 때문에 더 이상 이를 include하지 않는다.
	   이 함수 하나만을 위해 mmsystem.h를 다시 끌어오는 대신, 다른 모든
	   플랫폼에서 쓰는 것과 동일한 platform_get_ticks()를 거치도록 한다. */
	#ifndef timeGetTime
	#define timeGetTime()		platform_get_ticks()
	#endif
#endif

/* ============================================================================
 * 플랫폼별 타입
 * ============================================================================ */

#ifdef PLATFORM_WINDOWS
	/* Windows 타입 */
	#ifndef _WINDOWS_
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif

	typedef HANDLE	platform_thread_t;
	typedef HANDLE	platform_mutex_t;
	typedef HANDLE	platform_event_t;
	typedef HMODULE	platform_lib_t;

	#define PLATFORM_INVALID_THREAD	NULL
	#define PLATFORM_INVALID_MUTEX	NULL
	#define PLATFORM_INVALID_EVENT	NULL
	#define PLATFORM_INVALID_LIB		NULL

#else
	/* SDL/POSIX 타입 */
	typedef SDL_Thread*	platform_thread_t;
	typedef SDL_mutex*	platform_mutex_t;

	/* 이벤트 구조체 전방 선언 */
	typedef struct platform_event_s* platform_event_t;

	typedef void*	platform_lib_t;

	#define PLATFORM_INVALID_THREAD	NULL
	#define PLATFORM_INVALID_MUTEX	NULL
	#define PLATFORM_INVALID_EVENT	NULL
	#define PLATFORM_INVALID_LIB		NULL

#endif /* PLATFORM_WINDOWS */

/* ============================================================================
 * 시간 함수
 * ============================================================================ */

/**
 * 현재 시간을 밀리초 단위로 가져온다 (timeGetTime/GetTickCount와 유사)
 * @return 밀리초 단위 시간
 */
DWORD platform_get_ticks(void);

/**
 * 고성능 카운터 값을 가져온다 (QueryPerformanceCounter와 유사)
 * @return 카운터 값
 */
uint64_t platform_get_performance_counter(void);

/**
 * 고성능 카운터 주파수를 가져온다 (QueryPerformanceFrequency와 유사)
 * @return 카운터 주파수 (초당 카운트 수)
 */
uint64_t platform_get_performance_frequency(void);

/**
 * 지정한 밀리초만큼 대기한다 (Sleep과 유사)
 * @param ms 대기할 밀리초
 */
void platform_sleep(DWORD ms);

/* ============================================================================
 * 스레드 함수
 * ============================================================================ */

/**
 * 스레드 진입점 타입
 */
typedef DWORD (*platform_thread_func_t)(void* param);

/**
 * 새 스레드를 생성한다 (CreateThread와 유사)
 * @param func 스레드 함수
 * @param param 스레드 함수에 전달할 파라미터
 * @return 스레드 핸들, 실패 시 PLATFORM_INVALID_THREAD
 */
platform_thread_t platform_thread_create(platform_thread_func_t func, void* param);

/**
 * 스레드가 종료될 때까지 대기한다 (스레드에 대한 WaitForSingleObject와 유사)
 * @param thread 스레드 핸들
 * @return 대기 결과 (0 = 성공, 0이 아니면 실패)
 */
int platform_thread_wait(platform_thread_t thread);

/**
 * 스레드 핸들을 닫는다 (스레드에 대한 CloseHandle과 유사)
 * @param thread 스레드 핸들
 */
void platform_thread_close(platform_thread_t thread);

/* ============================================================================
 * 뮤텍스 함수
 * ============================================================================ */

/**
 * 뮤텍스를 생성한다 (CreateMutex와 유사)
 * @param initial_locked 뮤텍스를 잠긴 상태로 시작할지 여부
 * @return 뮤텍스 핸들, 실패 시 PLATFORM_INVALID_MUTEX
 */
platform_mutex_t platform_mutex_create(int initial_locked);

/**
 * 뮤텍스를 잠근다 (뮤텍스에 대한 WaitForSingleObject와 유사)
 * @param mutex 뮤텍스 핸들
 * @return 잠금 결과 (0 = 성공, 0이 아니면 실패)
 */
int platform_mutex_lock(platform_mutex_t mutex);

/**
 * 뮤텍스를 해제한다 (ReleaseMutex와 유사)
 * @param mutex 뮤텍스 핸들
 * @return 해제 결과 (0 = 성공, 0이 아니면 실패)
 */
int platform_mutex_unlock(platform_mutex_t mutex);

/**
 * 뮤텍스 핸들을 닫는다 (뮤텍스에 대한 CloseHandle과 유사)
 * @param mutex 뮤텍스 핸들
 */
void platform_mutex_close(platform_mutex_t mutex);

/* ============================================================================
 * 이벤트 함수
 * ============================================================================ */

/**
 * 이벤트 객체를 생성한다 (CreateEvent와 유사)
 * @param manual_reset 수동 리셋이 필요한지 여부
 * @param initial_state 초기 상태 (TRUE = 시그널됨, FALSE = 시그널되지 않음)
 * @return 이벤트 핸들, 실패 시 PLATFORM_INVALID_EVENT
 */
platform_event_t platform_event_create(int manual_reset, int initial_state);

/**
 * 이벤트가 시그널될 때까지 대기한다 (이벤트에 대한 WaitForSingleObject와 유사)
 * @param event 이벤트 핸들
 * @param timeout 타임아웃(밀리초), 무한 대기는 PLATFORM_INFINITE
 * @return 대기 결과 (0 = 성공, 0이 아니면 타임아웃/실패)
 */
int platform_event_wait(platform_event_t event, DWORD timeout);

/**
 * 이벤트를 시그널한다 (SetEvent와 유사)
 * @param event 이벤트 핸들
 * @return 시그널 결과 (0 = 성공, 0이 아니면 실패)
 */
int platform_event_signal(platform_event_t event);

/**
 * 이벤트를 시그널되지 않은 상태로 초기화한다 (ResetEvent와 유사)
 * @param event 이벤트 핸들
 * @return 초기화 결과 (0 = 성공, 0이 아니면 실패)
 */
int platform_event_reset(platform_event_t event);

/**
 * 이벤트 핸들을 닫는다 (이벤트에 대한 CloseHandle과 유사)
 * @param event 이벤트 핸들
 */
void platform_event_close(platform_event_t event);

#define PLATFORM_INFINITE	((DWORD)-1)

/* ============================================================================
 * 동적 라이브러리 함수
 * ============================================================================ */

/**
 * 동적 라이브러리를 로드한다 (LoadLibrary와 유사)
 * @param filename 라이브러리 파일 이름/경로
 * @return 라이브러리 핸들, 실패 시 PLATFORM_INVALID_LIB
 */
platform_lib_t platform_lib_load(const char* filename);

/**
 * 라이브러리에서 함수 주소를 가져온다 (GetProcAddress와 유사)
 * @param lib 라이브러리 핸들
 * @param symbol 함수 심볼 이름
 * @return 함수 포인터, 실패 시 NULL
 */
void* platform_lib_get_symbol(platform_lib_t lib, const char* symbol);

/**
 * 동적 라이브러리를 언로드한다 (FreeLibrary와 유사)
 * @param lib 라이브러리 핸들
 */
void platform_lib_free(platform_lib_t lib);

/* ============================================================================
 * 파일/경로 함수
 * ============================================================================ */

/**
 * 현재 플랫폼의 경로 구분자를 가져온다
 * @return 경로 구분자 문자 (Windows는 '\\', POSIX는 '/')
 */
char platform_get_path_separator(void);

/**
 * 파일이 존재하는지 확인한다
 * @param filename 확인할 파일 경로
 * @return 존재하면 1, 아니면 0
 */
int platform_file_exists(const char* filename);

/**
 * 현재 실행 파일의 디렉토리를 가져온다
 * @param buffer 경로를 저장할 버퍼
 * @param size 버퍼 크기
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int platform_get_executable_dir(char* buffer, size_t size);

/**
 * 디렉토리가 없으면 생성한다
 * @param path 디렉토리 경로
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int platform_create_directory(const char* path);

/* ============================================================================
 * 키보드 함수 (PlatformUtil.h에서 가져옴)
 * ============================================================================ */

/**
 * Control 키가 현재 눌려 있는지 확인한다
 * @return 눌려 있으면 1, 아니면 0
 */
int platform_is_ctrl_pressed(void);

/**
 * lParam(Windows 메시지 파라미터)에서 키보드 스캔 코드를 가져온다
 * @param lParam 키보드 메시지의 LPARAM
 * @return 스캔 코드
 */
BYTE platform_get_scan_code(DWORD lParam);

/* ============================================================================
 * 레지스트리/설정 함수 (Windows 전용 추상화)
 * ============================================================================ */

/**
 * 설정에서 문자열 값을 가져온다 (RegQueryValueEx 대체)
 * @param key 설정 키 이름 (예: "SOFTWARE\\Netmarble\\NetmarbleDarkEden")
 * @param value 값 이름
 * @param buffer 값을 저장할 버퍼
 * @param size 버퍼 크기 (입출력)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int platform_config_get_string(const char* key, const char* value,
                               char* buffer, DWORD* size);

/**
 * 설정에 문자열 값을 설정한다 (RegSetValueEx 대체)
 * @param key 설정 키 이름
 * @param value 값 이름
 * @param data 설정할 문자열 데이터
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int platform_config_set_string(const char* key, const char* value,
                               const char* data);

/* ============================================================================
 * 오류 보고
 * ============================================================================ */

/**
 * 오류 메시지 박스를 표시한다 (MessageBox와 유사)
 * @param title 메시지 박스 제목
 * @param message 오류 메시지
 */
void platform_show_error(const char* title, const char* message);

/* ============================================================================
 * 초기화
 * ============================================================================ */

/**
 * 플랫폼 추상화 계층을 초기화한다
 * 프로그램 시작 시 호출한다
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int platform_init(void);

/**
 * 플랫폼 추상화 계층을 정리한다
 * 프로그램 종료 시 호출한다
 */
void platform_shutdown(void);

/* ============================================================================
 * Windows 호환성 매크로
 * ============================================================================ */

// Windows가 아닌 플랫폼에서는 DLLIFC를 빈 값으로 정의한다 (Immersion 라이브러리 호환용)
#ifndef PLATFORM_WINDOWS
#ifndef DLLIFC
#define DLLIFC
#endif
#endif

/* 아래 남은 Windows API 셰임(shim)들(타입, 구조체, 스텁 함수)은
   <windows.h> 없이 컴파일할 때만 필요하다; 실제 Windows 빌드에서는
   (위에서 include한) <windows.h>가 이미 이들을 모두 제공한다. */
#ifndef PLATFORM_WINDOWS

// 필요할 수 있는 Windows 상수
#ifndef MAXLONG
#define MAXLONG 2147483647L  // 0x7FFFFFFF
#endif

#ifndef MAXDWORD
#define MAXDWORD 0xFFFFFFFF
#endif

/* ============================================================================
 * 사각형 구조체 (Windows RECT에 대응)
 * ============================================================================ */

#ifndef RECT_DEFINED
#define RECT_DEFINED

/**
 * 점 구조체 (Windows POINT에 대응)
 * 2D 좌표를 정의하는 데 사용한다
 */
#ifndef POINT_DEFINED
#define POINT_DEFINED
typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT, *PPOINT, *LPPOINT;
#endif

/**
 * 사각형 구조체 (Windows RECT에 대응)
 * 사각 영역을 정의하는 데 사용한다
 */
typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT, *LPRECT;

/**
 * MINMAXINFO 구조체 (WM_GETMINMAXINFO에서 사용)
 * 창의 최대화 크기와 위치에 대한 정보를 담는다
 */
typedef struct tagMINMAXINFO {
    POINT ptReserved;
    POINT ptMaxSize;
    POINT ptMaxPosition;
    POINT ptMinTrackSize;
    POINT ptMaxTrackSize;
} MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;

/* SYSTEMTIME 구조체 (날짜와 시간) */
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

/* DEVMODE 구조체 (디스플레이 모드 설정) */
#define ENUM_CURRENT_SETTINGS ((DWORD)-1)
#define DM_BITSPERPEL 0x00040000
#define DM_PELSWIDTH 0x00080000
#define DM_PELSHEIGHT 0x00100000
#define DM_DISPLAYFREQUENCY 0x00400000

typedef struct _devicemode {
    char   dmDeviceName[32];
    WORD   dmSpecVersion;
    WORD   dmDriverVersion;
    WORD   dmSize;
    WORD   dmDriverExtra;
    DWORD  dmFields;
    union {
        struct {
            short dmOrientation;
            short dmPaperSize;
            short dmPaperLength;
            short dmPaperWidth;
            short dmScale;
            short dmCopies;
            short dmDefaultSource;
            short dmPrintQuality;
        };
        POINT dmPosition;
    };
    short  dmColor;
    short  dmDuplex;
    short  dmYResolution;
    short  dmTTOption;
    short  dmCollate;
    char   dmFormName[32];
    WORD   dmLogPixels;
    DWORD  dmBitsPerPel;
    DWORD  dmPelsWidth;
    DWORD  dmPelsHeight;
    DWORD  dmDisplayFlags;
    DWORD  dmDisplayFrequency;
} DEVMODE, *PDEVMODE, *LPDEVMODE;

#endif /* RECT_DEFINED */

/* ============================================================================
 * Windows 파일 및 프로세스 API 스텁
 * ============================================================================ */

/* 파일 시간 구조체 (WIN32_FIND_DATA보다 먼저 정의해야 함) */
#ifndef FILETIME_DEFINED
#define FILETIME_DEFINED
typedef struct _FILETIME {
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#endif

/* 보안 속성 구조체 */
#ifndef SECURITY_ATTRIBUTES_DEFINED
#define SECURITY_ATTRIBUTES_DEFINED
typedef struct _SECURITY_ATTRIBUTES {
	DWORD nLength;
	LPVOID lpSecurityDescriptor;
	BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
#endif

/* 파일 찾기 데이터 구조체 */
#ifndef WIN32_FIND_DATA_DEFINED
#define WIN32_FIND_DATA_DEFINED
typedef struct _WIN32_FIND_DATAA {
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	DWORD dwReserved0;
	DWORD dwReserved1;
	char cFileName[MAX_PATH];
	char cAlternateFileName[14];
} WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;
#endif

/* 프로세스 접근 권한 */
#ifndef PROCESS_ALL_ACCESS
	#define PROCESS_ALL_ACCESS (0xFFFF)
#endif

/* 디스플레이 설정 상수 */
#ifndef CDS_RESET
	#define CDS_RESET 0x40000000
#endif
#ifndef CDS_UPDATEREGISTRY
	#define CDS_UPDATEREGISTRY 0x00000001
#endif
#ifndef DISP_CHANGE_SUCCESSFUL
	#define DISP_CHANGE_SUCCESSFUL 0
#endif
#ifndef DISP_CHANGE_RESTART
	#define DISP_CHANGE_RESTART 1
#endif
#ifndef DISP_CHANGE_FAILED
	#define DISP_CHANGE_FAILED -1
#endif

/* 파일 찾기 핸들 */
typedef void* HANDLE;
typedef void* HWND;
typedef void* HINSTANCE;
typedef void* HMODULE;
typedef void* HKEY;

/* 레지스트리 상수 */
#ifndef HKEY_LOCAL_MACHINE
	#define HKEY_LOCAL_MACHINE ((HKEY)0x80000002)
#endif

#ifndef KEY_ALL_ACCESS
	#define KEY_ALL_ACCESS (0xF003F)
#endif

#ifndef REG_SZ
	#define REG_SZ 1
#endif

#ifndef ERROR_SUCCESS
	#define ERROR_SUCCESS 0
#endif

/* 파일 및 프로세스 작업을 위한 스텁 구현 */
#ifndef DeleteFile
static inline BOOL DeleteFileA(LPCSTR lpFileName) {
	(void)lpFileName;
	return FALSE;
}
#define DeleteFile DeleteFileA
#endif

#ifndef CopyFile
static inline BOOL CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists) {
	(void)lpExistingFileName; (void)lpNewFileName; (void)bFailIfExists;
	return FALSE;
}
#define CopyFile CopyFileA
#endif

#ifndef SetCurrentDirectory
static inline BOOL SetCurrentDirectoryA(LPCSTR lpPathName) {
	(void)lpPathName;
	return FALSE;
}
#define SetCurrentDirectory SetCurrentDirectoryA
#endif

#ifndef GetModuleFileName
static inline DWORD GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize) {
	(void)hModule;
	if (lpFilename && nSize > 0) {
		lpFilename[0] = '\0';
		return 0;
	}
	return 0;
}
#define GetModuleFileName GetModuleFileNameA
#endif

#ifndef GetWindowThreadProcessId
static inline DWORD GetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessId) {
	(void)hWnd;
	if (lpdwProcessId) *lpdwProcessId = 0;
	return 0;
}
#endif

#ifndef TerminateProcess
static inline BOOL TerminateProcess(HANDLE hProcess, UINT uExitCode) {
	(void)hProcess; (void)uExitCode;
	return FALSE;
}
#endif

#ifndef FindClose
static inline BOOL FindClose(HANDLE hFindFile) {
	(void)hFindFile;
	return FALSE;
}
#endif

#ifndef FindFirstFileA
static inline HANDLE FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData) {
	(void)lpFileName; (void)lpFindFileData;
	return (HANDLE)INVALID_HANDLE_VALUE;
}
#define FindFirstFile FindFirstFileA
#endif

#ifndef FindNextFileA
static inline BOOL FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData) {
	(void)hFindFile; (void)lpFindFileData;
	return FALSE;
}
#define FindNextFile FindNextFileA
#endif

#ifndef INVALID_HANDLE_VALUE
	#define INVALID_HANDLE_VALUE ((HANDLE)(-1))
#endif

#ifndef CreateMutexA
static inline HANDLE CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName) {
	(void)lpMutexAttributes; (void)bInitialOwner; (void)lpName;
	return (HANDLE)NULL;
}
#define CreateMutex CreateMutexA
#endif

#ifndef ReleaseMutex
static inline BOOL ReleaseMutex(HANDLE hMutex) {
	(void)hMutex;
	return FALSE;
}
#endif

#ifndef OpenProcess
static inline HANDLE OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
	(void)dwDesiredAccess; (void)bInheritHandle; (void)dwProcessId;
	return (HANDLE)NULL;
}
#endif

#ifndef ChangeDisplaySettingsA
static inline LONG ChangeDisplaySettingsA(LPDEVMODE lpDevMode, DWORD dwflags) {
	(void)lpDevMode; (void)dwflags;
	return DISP_CHANGE_FAILED;
}
#define ChangeDisplaySettings ChangeDisplaySettingsA
#endif

#ifndef GetLastError
static inline DWORD GetLastError() {
	return 0;
}
#endif

#ifndef _chdir
	#define _chdir chdir
#endif

/* Windows 호환을 위한 Unix 스타일 함수 이름 매핑 */
#ifndef _access
	#define _access access
#endif

#ifndef _getcwd
	#define _getcwd getcwd
#endif

#ifndef _rmdir
	#define _rmdir rmdir
#endif

/* spawn 함수를 위한 _P_OVERLAY */
#ifndef _P_OVERLAY
	#define _P_OVERLAY 2
#endif

/* 레지스트리 타입 */
#ifndef REGSAM
typedef DWORD REGSAM;
#endif

#ifndef PHKEY
typedef HKEY* PHKEY;
#endif

/* 레지스트리 함수 */
#ifndef RegOpenKeyExA
static inline LONG RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
	(void)hKey; (void)lpSubKey; (void)ulOptions; (void)samDesired;
	if (phkResult) *phkResult = NULL;
	return ERROR_SUCCESS;
}
#define RegOpenKeyEx RegOpenKeyExA
#endif

#ifndef RegQueryValueExA
static inline LONG RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
	(void)hKey; (void)lpValueName; (void)lpReserved; (void)lpType;
	if (lpData && lpcbData && *lpcbData > 0) {
		lpData[0] = '\0';
		*lpcbData = 1;
	}
	return ERROR_SUCCESS;
}
#define RegQueryValueEx RegQueryValueExA
#endif

#ifndef RegSetValueExA
static inline LONG RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData) {
	(void)hKey; (void)lpValueName; (void)Reserved; (void)dwType; (void)lpData; (void)cbData;
	return ERROR_SUCCESS;
}
#define RegSetValueEx RegSetValueExA
#endif

#ifndef RegCloseKey
static inline LONG RegCloseKey(HKEY hKey) {
	(void)hKey;
	return ERROR_SUCCESS;
}
#endif

/* 레지스트리 접근 마스크 타입 */
#ifndef REGSAM_DEFINED
	#define REGSAM_DEFINED
#endif

#ifndef EnumDisplaySettingsA
static inline BOOL EnumDisplaySettingsA(LPCSTR lpszDeviceName, DWORD iModeNum, LPDEVMODE lpDevMode) {
	(void)lpszDeviceName; (void)iModeNum;
	if (lpDevMode) {
		lpDevMode->dmBitsPerPel = 32;
		lpDevMode->dmPelsWidth = 1024;
		lpDevMode->dmPelsHeight = 768;
		lpDevMode->dmDisplayFrequency = 60;
	}
	return FALSE;
}
#define EnumDisplaySettings EnumDisplaySettingsA
#endif

#ifndef Sleep
	/* 지정한 밀리초만큼 대기한다 */
	#ifdef PLATFORM_WINDOWS
		/* Windows Sleep을 사용한다 */
	#else
		/* Unix: usleep을 사용한다 */
		#include <unistd.h>
		static inline void Sleep(DWORD dwMilliseconds) {
			usleep(dwMilliseconds * 1000);
		}
	#endif
#endif

/* Spawn 함수 */
#ifndef _spawnl
static inline intptr_t _spawnl(int mode, const char* cmdname, const char* arg0, ...) {
	(void)mode; (void)cmdname; (void)arg0;
	return -1;
}
#endif

/* 파일 찾기를 위한 _finddata_t 구조체 */
#ifndef _FINDDATA_T_DEFINED
#define _FINDDATA_T_DEFINED
struct _finddata_t {
	unsigned attrib;
	time_t time_create;
	time_t time_access;
	time_t time_write;
	long size;
	char name[512];
};
#endif

/* 파일 찾기 함수 (Unix 스타일) */
#ifndef _findfirst
static inline long _findfirst(const char* filename, struct _finddata_t* finddata) {
	(void)filename; (void)finddata;
	return -1;
}
#endif

#ifndef _findnext
static inline int _findnext(long handle, struct _finddata_t* finddata) {
	(void)handle; (void)finddata;
	return -1;
}
#endif

#ifndef _findclose
static inline int _findclose(long handle) {
	(void)handle;
	return 0;
}
#endif

/* Windows 메시지를 위한 MSG 구조체 */
#ifndef tagMSG_DEFINED
#define tagMSG_DEFINED
typedef struct tagMSG {
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	DWORD time;
	POINT pt;
} MSG, *PMSG, *LPMSG;
#endif

/* 메시지 처리 함수 */
#ifndef GetMessage
static inline BOOL GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax) {
	(void)hWnd; (void)wMsgFilterMin; (void)wMsgFilterMax;
	if (lpMsg) {
		lpMsg->message = WM_QUIT;
		return FALSE;
	}
	return FALSE;
}
#endif

#ifndef TranslateMessage
static inline BOOL TranslateMessage(const MSG* lpMsg) {
	(void)lpMsg;
	return FALSE;
}
#endif

#ifndef DispatchMessage
static inline LRESULT DispatchMessage(const MSG* lpMsg) {
	(void)lpMsg;
	return 0;
}
#endif

#ifndef WaitMessage
static inline BOOL WaitMessage() {
	return FALSE;
}
#endif

/* PeekMessage 플래그 */
#ifndef PM_NOREMOVE
	#define PM_NOREMOVE 0x0000
#endif

/* 콘솔 상수 */
#ifndef MIN_CLRSCR
	#define MIN_CLRSCR 0
#endif

#ifndef MIN_SHOWWND
	#define MIN_SHOWWND 1
#endif

#ifndef MIN_HIDEWND
	#define MIN_HIDEWND 2
#endif

/* PeekMessage 함수 */
#ifndef PeekMessageA
static inline BOOL PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
	(void)hWnd; (void)wMsgFilterMin; (void)wMsgFilterMax; (void)wRemoveMsg;
	return FALSE;
}
#define PeekMessage PeekMessageA
#endif

/* 시스템 파라미터 정보 */
#ifndef SPI_SETSCREENSAVERRUNNING
	#define SPI_SETSCREENSAVERRUNNING 17
#endif

/* OS 버전 정보 */
#ifndef OSVERSIONINFO_DEFINED
#define OSVERSIONINFO_DEFINED
typedef struct _OSVERSIONINFOA {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	char szCSDVersion[128];
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;
#define OSVERSIONINFO OSVERSIONINFOA
#endif

#ifndef VER_PLATFORM_WIN32_WINDOWS
	#define VER_PLATFORM_WIN32_WINDOWS 1
#endif

#ifndef VER_PLATFORM_WIN32_NT
	#define VER_PLATFORM_WIN32_NT 2
#endif

/* DirectInput 키 코드 */
#ifndef DIK_NUMPADENTER
	#define DIK_NUMPADENTER 0x9C
#endif

/* 버전 확인 */
#ifndef GetVersionExA
static inline BOOL GetVersionExA(LPOSVERSIONINFOA lpVersionInformation) {
	if (lpVersionInformation) {
		lpVersionInformation->dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
		lpVersionInformation->dwMajorVersion = 5;
		lpVersionInformation->dwMinorVersion = 1;
		lpVersionInformation->dwBuildNumber = 2600;
		lpVersionInformation->dwPlatformId = VER_PLATFORM_WIN32_NT;
		lpVersionInformation->szCSDVersion[0] = '\0';
	}
	return TRUE;
}
#define GetVersionEx GetVersionExA
#endif

/* 예외 처리 */
/* 예외 필터 타입 (EXCEPTION_POINTERS보다 먼저 정의해야 함) */
#ifndef LPTOP_LEVEL_EXCEPTION_FILTER
typedef LONG (*LPTOP_LEVEL_EXCEPTION_FILTER)(struct _EXCEPTION_POINTERS*);
#endif

#ifndef EXCEPTION_POINTERS_DEFINED
#define EXCEPTION_POINTERS_DEFINED
typedef struct _EXCEPTION_POINTERS {
	DWORD ExceptionCode;
	DWORD ExceptionFlags;
	void* ExceptionRecord;
	void* ExceptionAddress;
	DWORD NumberParameters;
	void* ExceptionInformation[15];
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
#endif

#ifndef SetUnhandledExceptionFilter
static inline LPTOP_LEVEL_EXCEPTION_FILTER SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) {
	(void)lpTopLevelExceptionFilter;
	return NULL;
}
#endif

/* DirectDraw 기능 */
#ifndef DDSCAPS_VIDEOMEMORY
	#define DDSCAPS_VIDEOMEMORY 0x00000040
#endif

typedef struct _DDCAPS {
	DWORD dwSize;
	DWORD dwCaps;
	DWORD dwCaps2;
	DWORD dwCKeyCaps;
	DWORD dwFXCaps;
	DWORD dwFXAlphaCaps;
	DWORD dwPalCaps;
	DWORD dwSVCaps;
	DWORD dwAlphaCaps;
	DWORD dwVideoPortCaps;
	DWORD dwVideoPortCaps2;
	DWORD dwVidMemTotal;
	DWORD dwVidMemFree;
	DWORD dwMaxVisibleOverhead;
} DDCAPS;

/* SetRect 함수 */
static inline void SetRect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom) {
    if (lprc) {
        lprc->left = xLeft;
        lprc->top = yTop;
        lprc->right = xRight;
        lprc->bottom = yBottom;
    }
}

#endif /* !PLATFORM_WINDOWS (위에서 시작한 Windows API 셰임의 끝) */

/* SDL 텍스트 입력 중계 메시지 (실제 Win32 메시지 ID가 아니며, 이 코드베이스의
   메시지 디스패치를 통해 SDL_TEXTINPUT/SDL_TEXTEDITING 이벤트를 전달하는 데
   사용된다). Windows가 아닌 경우는 위에서 정의했다; 이제 모든 플랫폼(Windows
   포함)에서 USE_SDL_BACKEND가 필수이므로, <windows.h>가 정의하지 않은 경우
   여기서도 정의한다 (실제 Win32에는 WM_TEXTINPUT/WM_TEXTEDITING이 없다). */
#ifndef WM_TEXTINPUT
#define WM_TEXTINPUT 0x0111
#define WM_TEXTEDITING 0x0110
#endif

/* MCI 알림 상수 (원래 <mmsystem.h>/<digitalv.h>에 있지만, 이 프로젝트는
   더 이상 이를 include하지 않는다 - CWinUpdate.h 참고). Client.cpp의
   WindowProc에서 AVI 인트로 재생 완료를 감지하는 데 사용된다. 값은
   수십 년간 안정적으로 유지된 Win32 SDK 상수다. */
#ifndef MM_MCINOTIFY
#define MM_MCINOTIFY 0x03B9
#define MCI_NOTIFY_SUCCESSFUL 0x0001
#endif

/* Windows 코드와의 호환성을 위한 max, min 매크로 */
#ifndef PLATFORM_WINDOWS
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
/* __int64 Windows 타입 - macOS에서는 long long을 사용한다 */
typedef long long __int64;
/* _atoi64 Windows 함수 - macOS에서는 atoll을 사용한다 */
#define _atoi64(x) atoll(x)
#endif

/* macOS용 wsprintf 스텁 - 서식화된 출력을 문자열에 기록한다 */
#ifndef PLATFORM_WINDOWS
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
static inline int wsprintf(char* buf, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
    int result = vsprintf(buf, fmt, args);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    va_end(args);
    return result;
}

/* 파일 작업을 위한 Windows API 스텁 */
#define FILE_ATTRIBUTE_DIRECTORY (0x00000010)

/* 키보드 입력을 위한 Windows 가상 키 코드 */
#ifndef VK_UP
#define VK_UP    0x26
#endif
#ifndef VK_DOWN
#define VK_DOWN  0x28
#endif
#ifndef VK_LEFT
#define VK_LEFT  0x25
#endif
#ifndef VK_RIGHT
#define VK_RIGHT 0x27
#endif
#ifndef VK_RETURN
#define VK_RETURN 0x0D
#endif
#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif
#ifndef VK_TAB
#define VK_TAB 0x09
#endif
#ifndef VK_BACK
#define VK_BACK 0x08
#endif
#ifndef VK_SPACE
#define VK_SPACE  0x20
#endif
#ifndef VK_SHIFT
#define VK_SHIFT  0x10
#endif
#ifndef VK_CONTROL
#define VK_CONTROL 0x11
#endif
#ifndef VK_HOME
#define VK_HOME 0x24
#endif
#ifndef VK_END
#define VK_END 0x23
#endif
#ifndef VK_DELETE
#define VK_DELETE 0x2E
#endif
#ifndef VK_INSERT
#define VK_INSERT 0x2D
#endif

/* DirectInput 키 코드 */
#ifndef DIK_LCONTROL
#define DIK_LCONTROL 0x1D
#endif
#ifndef DIK_RCONTROL
#define DIK_RCONTROL 0x9D
#endif
#ifndef DIK_LSHIFT
#define DIK_LSHIFT 0x2A
#endif
#ifndef DIK_RSHIFT
#define DIK_RSHIFT 0x36
#endif

/* 워드(word) 조작을 위한 Windows 매크로 */
#ifndef LOWORD
#define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#endif
#ifndef HIWORD
#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#endif
#ifndef LOBYTE
#define LOBYTE(w) ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#endif
#ifndef HIBYTE
#define HIBYTE(w) ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#endif

static inline DWORD GetLogicalDrives() {
    /* macOS 스텁 - 드라이브 없음을 반환한다 */
    return 0;
}

static inline DWORD GetCurrentDirectory(DWORD nBufferLength, LPSTR lpBuffer) {
    /* macOS 스텁 - 현재 작업 디렉토리를 가져온다 */
    if (getcwd(lpBuffer, nBufferLength) != NULL) {
        return (DWORD)strlen(lpBuffer);
    }
    return 0;
}

/* GetLocalTime - 현재 로컬 시간으로 SYSTEMTIME 구조체를 채운다 */
static inline void GetLocalTime(LPSYSTEMTIME lpSystemTime) {
    /* macOS 스텁 - 현재 로컬 시간을 가져온다 */
    if (lpSystemTime) {
        struct tm* now;
        time_t aclock;
        time(&aclock);
        now = localtime(&aclock);

        lpSystemTime->wYear = now->tm_year + 1900;
        lpSystemTime->wMonth = now->tm_mon + 1;
        lpSystemTime->wDayOfWeek = now->tm_wday;
        lpSystemTime->wDay = now->tm_mday;
        lpSystemTime->wHour = now->tm_hour;
        lpSystemTime->wMinute = now->tm_min;
        lpSystemTime->wSecond = now->tm_sec;
        lpSystemTime->wMilliseconds = 0;
    }
}
#endif

/* SDL 백엔드를 위한 SetSurfaceInfo - S_SURFACEINFO를 복사한다. Windows에서도
   필요하다: SPRITELIB_BACKEND_SDL(이 프로젝트가 빌드하는 유일한 백엔드이며
   Windows도 포함)에서 CSpriteSurface는 독립적인 SDL 클래스로, 자체
   GetDDSD()가 (CSpriteSurface.h의 SPRITESURFACE_STANDALONE 분기 참고)
   S_SURFACEINFO*를 반환하며, basic/GL_import.h의 Windows 전용
   SetSurfaceInfo() 오버로드가 기대하는 DDSURFACEDESC2*가 아니다(그 오버로드
   자체는 예전 GL_import DLL에서 남은, 구현되지 않은 죽은 코드다 -
   이 프로젝트의 어떤 .cpp도 이를 정의하지 않는다). */
#include "2d.h"
static inline void SetSurfaceInfo(S_SURFACEINFO* dest, const S_SURFACEINFO* src) {
    if (dest && src) {
        dest->p_surface = src->p_surface;
        dest->width = src->width;
        dest->height = src->height;
        dest->pitch = src->pitch;
    }
}

/* Windows가 아닌 플랫폼을 위한 DirectInput 키 코드 */
#ifndef PLATFORM_WINDOWS
/* DIK_LMENU와 DIK_RMENU는 좌/우 ALT에 대한 DirectInput 이름이다 */
#define DIK_LMENU           0x38
#define DIK_RMENU           0xB8
/* ALT 키의 대체 이름 */
#define DIK_LALT            DIK_LMENU
#define DIK_RALT            DIK_RMENU

/* 값으로부터 LONG/LPARAM을 만드는 Windows 매크로 */
#ifndef MAKELONG
#define MAKELONG(a, b) ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD_PTR)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#endif
#ifndef MAKEWPARAM
#define MAKEWPARAM(l, h) ((WPARAM)(DWORD)MAKELONG(l, h))
#endif
#ifndef MAKELPARAM
#define MAKELPARAM(l, h) ((LPARAM)(DWORD)MAKELONG(l, h))
#endif
#ifndef MAKELRESULT
#define MAKELRESULT(l, h) ((LRESULT)(DWORD)MAKELONG(l, h))
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_H__ */
