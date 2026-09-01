//----------------------------------------------------------------------
// CDirectDraw.cpp
//
// SDL2 구현 (크로스플랫폼)
// Windows DirectX 구현은 제거됨 - 모든 플랫폼에서 SDL2를 사용한다
// 참고: 정적 멤버 정의는 CDirectDraw_StaticMembers.cpp에 있다
//----------------------------------------------------------------------

#include "CDirectDraw.h"
// spritectl_init()만을 위함; CSDLGraphics::Flip()(전체 CSpriteSurface 정의가
// 필요함)은 대신 Client/CSDLGraphicsFlip.cpp에 구현되어 있다. 이 파일은
// 독립적인 dxlib 라이브러리로 컴파일되므로(/IClient 없음, SPRITELIB_BACKEND_SDL
// 없음) DarkEden.exe 자체 소스처럼 SpriteLib/CSpriteSurface.h를 안전하게
// 끌어올 수 없기 때문이다.
#include "../SpriteLib/SpriteLibBackend.h"

//-----------------------------------------------------------------------------
// DirectDraw 객체를 위한 정적 멤버 초기화
// 참고: SDL2 백엔드를 위한 불투명 포인터/스텁이다
//-----------------------------------------------------------------------------
LPDIRECTDRAW7					CSDLGraphics::m_pDD					= NULL;
LPDIRECTDRAWSURFACE7			CSDLGraphics::m_pDDSPrimary			= NULL;
LPDIRECTDRAWSURFACE7			CSDLGraphics::m_pDDSBack				= NULL;
LPDIRECTDRAWGAMMACONTROL	CSDLGraphics::m_pDDGammaControl		= NULL;

DDSURFACEDESC2					CSDLGraphics::m_ddsd;

SDL_Window*						CSDLGraphics::m_pSDLWindow			= NULL;
SDL_Renderer*					CSDLGraphics::m_pSDLRenderer			= NULL;

HWND								CSDLGraphics::m_hWnd					= NULL;

bool								CSDLGraphics::m_bFullscreen			= true;
WORD								CSDLGraphics::m_ScreenWidth			= 0;
WORD								CSDLGraphics::m_ScreenHeight			= 0;
bool								CSDLGraphics::m_b565					= true;
bool								CSDLGraphics::m_b3D					= true;
bool								CSDLGraphics::m_bMMX					= false;
bool								CSDLGraphics::m_bGammaControl		= false;
DDGAMMARAMP						CSDLGraphics::m_DDGammaRamp;
WORD								CSDLGraphics::m_GammaStep				= 0;
WORD								CSDLGraphics::m_AddGammaStep[3];

RECT								CSDLGraphics::m_rcWindow;
RECT								CSDLGraphics::m_rcScreen;
RECT								CSDLGraphics::m_rcViewport;

// 참고: 색상 마스크 정적 멤버는 CDirectDraw_StaticMembers.cpp에 정의되어 있다

//-----------------------------------------------------------------------------
// 생성자/소멸자 (스텁 - 구현되지 않음)
//-----------------------------------------------------------------------------
CSDLGraphics::CSDLGraphics()
{
}

CSDLGraphics::~CSDLGraphics()
{
}

//-----------------------------------------------------------------------------
// Init
//
// hWnd는 이 함수가 호출되기 전에 CreateWindowEx()로 이미 생성된 실제
// 네이티브 창이다; SDL_CreateWindowFrom()은 새 창을 만드는 대신 이를
// 감싸므로, SDL은 Win32 메시지 처리가 사용하는 것과 동일한 창에 렌더링한다.
//-----------------------------------------------------------------------------
void CSDLGraphics::Init(HWND hWnd, WORD width, WORD height, SCREENMODE mode, bool bUseHAL, bool bUseIME)
{
	(void)bUseHAL;
	(void)bUseIME;

	ReleaseAll();

	spritectl_init();

	m_pSDLWindow = SDL_CreateWindowFrom((void*)hWnd);
	if (m_pSDLWindow == NULL)
	{
		return;
	}

	m_pSDLRenderer = SDL_CreateRenderer(m_pSDLWindow, -1, SDL_RENDERER_ACCELERATED);
	if (m_pSDLRenderer == NULL)
	{
		m_pSDLRenderer = SDL_CreateRenderer(m_pSDLWindow, -1, 0);
	}

	if (m_pSDLRenderer != NULL)
	{
		SDL_SetRenderDrawColor(m_pSDLRenderer, 0, 0, 0, 255);
	}

	m_hWnd = hWnd;
	m_ScreenWidth = width;
	m_ScreenHeight = height;
	m_bFullscreen = (mode == FULLSCREEN);
}

// CSDLGraphics::Flip()은 Client/CSDLGraphicsFlip.cpp에 정의되어 있다
// (여기 없는 이유는 위 SpriteLibBackend.h include에 대한 주석 참고).

//-----------------------------------------------------------------------------
// ReleaseAll
//-----------------------------------------------------------------------------
void CSDLGraphics::ReleaseAll()
{
	if (m_pSDLRenderer != NULL)
	{
		SDL_DestroyRenderer(m_pSDLRenderer);
		m_pSDLRenderer = NULL;
	}

	if (m_pSDLWindow != NULL)
	{
		// SDL_CreateWindowFrom()은 외부에서 소유한 네이티브 창을 감싸므로,
		// 여기서 파괴해도 SDL의 래퍼만 해제될 뿐 hWnd 자체는 해제되지 않는다.
		SDL_DestroyWindow(m_pSDLWindow);
		m_pSDLWindow = NULL;
	}
}

//-----------------------------------------------------------------------------
// InitMask
//-----------------------------------------------------------------------------
void CSDLGraphics::InitMask(bool b565)
{
	// SDL2를 위한 5:6:5 포맷
	s_wMASK_SHIFT[0] = 11;
	s_wMASK_SHIFT[1] = 5;
	s_wMASK_SHIFT[2] = 0;
	s_wMASK_SHIFT[3] = 0;
	s_wMASK_SHIFT[4] = 0;

	s_dwMASK_SHIFT[0] = 0xF800;
	s_dwMASK_SHIFT[1] = 0x07E0;
	s_dwMASK_SHIFT[2] = 0x001F;
	s_dwMASK_SHIFT[3] = 0;
	s_dwMASK_SHIFT[4] = 0;

	s_wMASK_RGB[0] = 0;
	s_wMASK_RGB[1] = 11;
	s_wMASK_RGB[2] = 5;
	s_wMASK_RGB[3] = 0;
	s_wMASK_RGB[4] = 0;
	s_wMASK_RGB[5] = 0;

	s_dwMASK_RGB[0] = 0x0000F800;
	s_dwMASK_RGB[1] = 0x000007E0;
	s_dwMASK_RGB[2] = 0x0000001F;
	s_dwMASK_RGB[3] = 0;
	s_dwMASK_RGB[4] = 0;
	s_dwMASK_RGB[5] = 0;

	s_qwMASK_RGB[0] = 0x000000000000F800;
	s_qwMASK_RGB[1] = 0x0000000000007E0;
	s_qwMASK_RGB[2] = 0x00000000000001F;
	s_qwMASK_RGB[3] = 0;
	s_qwMASK_RGB[4] = 0;
	s_qwMASK_RGB[5] = 0;

	s_bSHIFT_R = 3;
	s_bSHIFT_G = 2;
	s_bSHIFT_B = 3;
	s_bSHIFT_A = 4;

	s_dwMASK_SHIFT_COUNT[0] = 5;
	s_dwMASK_SHIFT_COUNT[1] = 6;
	s_dwMASK_SHIFT_COUNT[2] = 5;
	s_dwMASK_SHIFT_COUNT[3] = 0;
	s_dwMASK_SHIFT_COUNT[4] = 0;

	s_dwMASK_RGB_COUNT[0] = 0;
	s_dwMASK_RGB_COUNT[1] = 5;
	s_dwMASK_RGB_COUNT[2] = 6;
	s_dwMASK_RGB_COUNT[3] = 0;
	s_dwMASK_RGB_COUNT[4] = 0;
	s_dwMASK_RGB_COUNT[5] = 0;

	(void)b565;  // 호환성을 위해 파라미터를 유지한다
}

//-----------------------------------------------------------------------------
// SDL2를 위한 비트마스크 메서드
//-----------------------------------------------------------------------------

int CSDLGraphics::Get_Count_Rbit()
{
	// 5:6:5 포맷에서 R은 5비트를 사용한다
	return 5;
}

int CSDLGraphics::Get_Count_Gbit()
{
	// 5:6:5 포맷에서 G는 6비트를 사용한다
	return 6;
}

int CSDLGraphics::Get_Count_Bbit()
{
	// 5:6:5 포맷에서 B는 5비트를 사용한다
	return 5;
}

DWORD CSDLGraphics::Get_R_Bitmask()
{
	// 5:6:5 포맷: R은 11-15비트
	return 0xF800;
}

DWORD CSDLGraphics::Get_G_Bitmask()
{
	// 5:6:5 포맷: G는 5-10비트
	return 0x07E0;
}

DWORD CSDLGraphics::Get_B_Bitmask()
{
	// 5:6:5 포맷: B는 0-4비트
	return 0x001F;
}

DWORD CSDLGraphics::Get_BPP()
{
	// SDL2는 일반적으로 16비트 색상을 사용한다
	return 16;
}
