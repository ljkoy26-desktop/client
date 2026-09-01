/*-----------------------------------------------------------------------------

	CDirectDraw.h

	SDL2 구현 (크로스플랫폼)
	Windows DirectDraw 구현은 제거됨 - 모든 플랫폼에서 SDL2를 사용한다
	참고: 이 클래스는 더 이상 사용되지 않는다(deprecated). SDL2 서페이스와 텍스처를 직접 사용하라.

	1999.12.28. sigi, KJTINC
	2025: SDL2 크로스플랫폼 지원을 위해 리팩토링됨

-----------------------------------------------------------------------------*/

#ifndef	__CSDLGRAPHICS_H__
#define	__CSDLGRAPHICS_H__

/* 크로스플랫폼 include */
#include "../basic/Platform.h"
#include <SDL.h>
#include <cstring>
#include <cstdint>

/* Windows 타입 호환성 - 모든 플랫폼에서 정의한다 */
#ifndef LPVOID_DEFINED
#define LPVOID_DEFINED
typedef void* LPVOID;
#endif

/* DirectDraw 타입 스텁 - 기존 코드와의 호환성을 위해 유지 */
typedef struct IDirectDraw* LPDIRECTDRAW7;
typedef struct IDirectDrawSurface* LPDIRECTDRAWSURFACE7;
typedef struct IDirectDrawGammaControl* LPDIRECTDRAWGAMMACONTROL;

/* DDPIXELFORMAT 구조체 스텁 */
#ifndef TAGPIXELFORMAT_DEFINED
#define TAGPIXELFORMAT_DEFINED
typedef struct tagPIXELFORMAT {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwRGBAlphaBitMask;
} DDPIXELFORMAT, *LPDDPIXELFORMAT;
#define DDPIXELFORMAT_DEFINED
#endif

/* DDSCAPS2 구조체 스텁 */
typedef struct {
	DWORD dwCaps;
	DWORD dwCaps2;
	DWORD dwCaps3;
	DWORD dwCaps4;
	DWORD dwVolumeDepth;
} DDSCAPS2;

/* DDSURFACEDESC2 구조체 스텁 */
typedef struct {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwHeight;
	DWORD dwWidth;
	LPVOID lpSurface;
	DDPIXELFORMAT ddpfPixelFormat;
	DDSCAPS2 ddsCaps;
	LONG lPitch;
} DDSURFACEDESC2;

/* DDGAMMARAMP 구조체 스텁 */
typedef struct {
	WORD red[256];
	WORD green[256];
	WORD blue[256];
} DDGAMMARAMP;

/* 공통 Windows 타입 - 아직 정의되지 않은 경우에만 정의한다.
   Windows에서는 (Platform.h를 통해) <windows.h>에서 이미 제공되므로,
   이를 재정의하면(특히 구조체는) 컴파일에 실패한다. */
#ifndef PLATFORM_WINDOWS
#ifndef RECT_DEFINED
#define RECT_DEFINED
typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT, *PRECT, *LPRECT;
#endif

#ifndef POINT_DEFINED
#define POINT_DEFINED
typedef struct tagPOINT {
	LONG x;
	LONG y;
} POINT, *PPOINT, *LPPOINT;
#endif

#ifndef COLORREF_DEFINED
#define COLORREF_DEFINED
typedef DWORD COLORREF;
#endif

#ifndef SIZE_DEFINED
#define SIZE_DEFINED
typedef struct tagSIZE {
	LONG cx;
	LONG cy;
} SIZE, *PSIZE, *LPSIZE;
#endif
#endif /* !PLATFORM_WINDOWS */

/* QWORD 타입 */
typedef uint64_t QWORD;

#ifndef PLATFORM_WINDOWS
/* HWND 타입 스텁 */
typedef void* HWND;

/* GDI 타입 */
#ifndef HBITMAP_DEFINED
#define HBITMAP_DEFINED
typedef void* HBITMAP;
#endif
#endif /* !PLATFORM_WINDOWS */

/* 메모리 유틸리티 */
#ifndef ZeroMemory
#define ZeroMemory(p, s) memset((p), 0, (s))
#endif

/* DirectDraw 상수 */
#define DDSD_CAPS 0x00000001
#define DDSD_HEIGHT 0x00000002
#define DDSD_WIDTH 0x00000004
#define DDSD_PITCH 0x00000008
#define DDSD_PIXELFORMAT 0x00001000
#define DDSD_LPSURFACE 0x00000800

/*-----------------------------------------------------------------------------
  CDirectDraw 클래스 - SDL2 크로스플랫폼 스텁
  참고: 대부분의 메서드는 스텁이다. SDL2 함수를 직접 사용하라.
-----------------------------------------------------------------------------*/

class CSDLGraphics
{
public:
	// 생성자/소멸자 (구현되지 않음 - 클래스가 더 이상 사용되지 않음)
	CSDLGraphics();
	~CSDLGraphics();

	enum SCREENMODE { FULLSCREEN, WINDOWMODE };

	// 색상 변환 유틸리티 (여전히 동작함)
	static inline WORD	Color(const BYTE& r, const BYTE& g, const BYTE& b)
	{
		// 5:6:5 포맷: R은 11-15비트, G는 5-10비트, B는 0-4비트
		return ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
	}

	static inline BYTE	Red(const WORD& c)
	{
		// R 성분 추출 (11-15비트)
		return (c >> 11) & 0x1F;
	}

	static inline BYTE	Green(const WORD& c)
	{
		// G 성분 추출 (5-10비트)
		return (c >> 5) & 0x3F;
	}

	static inline BYTE	Blue(const WORD& c)
	{
		// B 성분 추출 (0-4비트)
		return c & 0x1F;
	}

	// 색상 변환 (5:5:5 <-> 5:6:5)
	static inline WORD	Convert555to565(WORD pixel)
	{
		return ((pixel & 0x7FE0) << 1) | (pixel & 0x001F);
	}

	static inline WORD	Convert565to555(WORD pixel)
	{
		return (((pixel & 0xFFE0) >> 1) & 0x7FE0) | (pixel & 0x001F);
	}

	// 비트마스크 메서드
	static int		Get_Count_Rbit();
	static int		Get_Count_Gbit();
	static int		Get_Count_Bbit();
	static DWORD		Get_R_Bitmask();
	static DWORD		Get_G_Bitmask();
	static DWORD		Get_B_Bitmask();
	static DWORD		Get_BPP();

	// 속성 메서드
	static inline bool		IsFullscreen()			{ return m_bFullscreen; }
	static inline WORD		GetScreenWidth()		{ return m_ScreenWidth; }
	static inline WORD		GetScreenHeight()		{ return m_ScreenHeight; }
	static inline bool		IsMMX()	 				{ return false; }
	static inline bool		IsSupportGammaControl()	{ return false; }
	static inline bool		Is565()	 				{ return true; }
	static inline HWND		GetHwnd()				{ return m_hWnd; }
	static inline LPDIRECTDRAW7 GetDD()			{ return nullptr; }

	// Init - hWnd에 대한 실제 SDL 창/렌더러를 생성한다 (.cpp에 구현됨)
	static void		Init(HWND hWnd, WORD width, WORD height, SCREENMODE mode, bool bUseHAL = true, bool bUseIME = true);

	// 화면 표시 메서드 - Flip()/ReleaseAll()은 .cpp에 구현됨 (실제 SDL2
	// 프레젠테이션), 나머지는 스텁으로 남아있다 (대응하는 SDL2 개념이 필요 없음)
	static void		Flip();
	static inline void		FlipToGDISurface() { }
	static inline void		OnMove() { }
	static inline bool		RestoreAllSurfaces() { return true; }
	static inline void		ReleaseSurface() { }
	static void		ReleaseAll();
	static inline void		SetGammaRamp(WORD step = (WORD)-1) { }
	static inline void		RestoreGammaRamp() { }
	static inline void		SetAddGammaRamp(WORD rStep = 0, WORD gStep = 0, WORD bStep = 0) { }
	static inline void		SetDisplayMode(WORD width, WORD height, WORD bpp, DWORD flags1, DWORD flags2) { }
	static inline void		RestoreDisplayMode() { }

	// InitMask - 정적 배열을 초기화하기 위해 .cpp에 구현됨
	static void		InitMask(bool b565);

protected:
	// DirectDraw 서페이스 설명 - CDirectDrawSurface에서 접근 가능하도록 유지
	// (원본 VC6 CDirectDraw 베이스 클래스에서처럼, GetSurfacePointer()/
	// GetSurfacePitch()/GetDDSD()가 이를 직접 읽는다).
	static DDSURFACEDESC2					m_ddsd;

private:
	// DirectDraw 객체 (스텁 포인터)
	static LPDIRECTDRAW7					m_pDD;
	static LPDIRECTDRAWSURFACE7				m_pDDSPrimary;
	static LPDIRECTDRAWSURFACE7				m_pDDSBack;
	static LPDIRECTDRAWGAMMACONTROL			m_pDDGammaControl;

	// 이 클래스를 뒷받침하는 실제 SDL2 창/렌더러 (Init()에서 생성됨)
	static SDL_Window*						m_pSDLWindow;
	static SDL_Renderer*					m_pSDLRenderer;

	// 화면 속성
	static HWND								m_hWnd;
	static bool								m_bFullscreen;
	static WORD								m_ScreenWidth;
	static WORD								m_ScreenHeight;
	static bool								m_b565;
	static bool								m_b3D;
	static bool								m_bMMX;
	static bool								m_bGammaControl;
	static DDGAMMARAMP						m_DDGammaRamp;
	static WORD								m_GammaStep;
	static WORD								m_AddGammaStep[3];

	// 창 사각형
	static RECT								m_rcWindow;
	static RECT								m_rcViewport;
	static RECT								m_rcScreen;

public:
	// 정적 멤버 변수 (호환성을 위해 유지)
	static WORD		s_wMASK_SHIFT[5];
	static DWORD	s_dwMASK_SHIFT[5];
	static QWORD	s_qwMASK_SHIFT[5];

	static WORD		s_wMASK_RGB[6];
	static DWORD	s_dwMASK_RGB[6];
	static QWORD	s_qwMASK_RGB[6];

	static QWORD	s_qwMASK_ALPHA0;
	static QWORD	s_qwMASK_ALPHA1;
	static DWORD	s_dwMASK_ALPHA0;
	static DWORD	s_dwMASK_ALPHA1;
	static WORD		s_wMASK_ALPHA0;
	static WORD		s_wMASK_ALPHA1;

	static BYTE		s_bSHIFT_R;
	static BYTE		s_bSHIFT_G;
	static BYTE		s_bSHIFT_B;
	static BYTE		s_bSHIFT_A;  // 호환성을 위해 추가됨

	static BYTE		s_bSHIFT4_R;
	static BYTE		s_bSHIFT4_G;
	static BYTE		s_bSHIFT4_B;

	static DWORD	s_dwMASK_SHIFT_COUNT[5];  // 호환성을 위해 추가됨
	static DWORD	s_dwMASK_RGB_COUNT[6];   // 호환성을 위해 추가됨

	static WORD		RED;
	static WORD		GREEN;
	static WORD		BLUE;
	static WORD		WHITE;

	static bool		s_bUseIMEHandle;
};

#endif
