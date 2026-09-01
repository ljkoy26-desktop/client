//----------------------------------------------------------------------
// CDirectDrawSurface.cpp
//
// SDL2 구현 (크로스플랫폼)
// Windows DirectDraw 구현은 제거됨 - 모든 플랫폼에서 SDL2를 사용한다
// 참고: 이 클래스는 더 이상 사용되지 않는다(deprecated). SDL_Surface를 직접 사용하라.
//----------------------------------------------------------------------

#include "CDirectDrawSurface.h"

//-----------------------------------------------------------------------------
// SDL2를 위한 정적 멤버 초기화
//-----------------------------------------------------------------------------
void (*CDirectDrawSurface::s_GammaFunction)(void *pDest, int len, int p) = nullptr;

//-----------------------------------------------------------------------------
// 생성자/소멸자
//-----------------------------------------------------------------------------

CDirectDrawSurface::CDirectDrawSurface()
{
	m_pDDSurface = nullptr;
	m_Width = 0;
	m_Height = 0;
	m_ClipLeft = 0;
	m_ClipTop = 0;
	m_ClipRight = 0;
	m_ClipBottom = 0;
	m_bBackSurface = false;
	m_bLock = false;
}

CDirectDrawSurface::~CDirectDrawSurface()
{
	// 스텁 - SDL 서페이스는 다른 곳에서 관리된다
}

//-----------------------------------------------------------------------------
// 서페이스 초기화 (스텁 - 더 이상 사용되지 않음)
//-----------------------------------------------------------------------------

bool CDirectDrawSurface::InitBacksurface()
{
	// 구현되지 않음 - SDL 서페이스를 직접 사용하라
	return false;
}

bool CDirectDrawSurface::InitOffsurface(int wWidth, int wHeight, DWORD dwCaps)
{
	m_Width = wWidth;
	m_Height = wHeight;
	m_ClipRight = wWidth;
	m_ClipBottom = wHeight;
	(void)dwCaps;
	return true;
}

bool CDirectDrawSurface::InitTextureSurface(int wWidth, int wHeight, DWORD dwCaps, const LPDDPIXELFORMAT pDDPF)
{
	// 구현되지 않음 - SDL 텍스처를 직접 사용하라
	(void)wWidth;
	(void)wHeight;
	(void)dwCaps;
	(void)pDDPF;
	return false;
}

bool CDirectDrawSurface::InitTextureSurfaceFromBMP(LPCSTR szBitmap, DWORD dwCaps)
{
	// 구현되지 않음 - BMP 로드에는 SDL_image를 사용하라
	(void)szBitmap;
	(void)dwCaps;
	return false;
}

bool CDirectDrawSurface::InitFromBMP(LPCSTR szBitmap, DWORD dwCaps)
{
	// 구현되지 않음 - BMP 로드에는 SDL_image를 사용하라
	(void)szBitmap;
	(void)dwCaps;
	return false;
}

bool CDirectDrawSurface::ReLoadBMP(LPCSTR szBitmap)
{
	// 구현되지 않음
	(void)szBitmap;
	return false;
}

bool CDirectDrawSurface::Restore()
{
	// 구현되지 않음 - SDL 서페이스는 복원이 필요 없다
	return false;
}

//-----------------------------------------------------------------------------
// 서페이스 연산 (스텁 - 더 이상 사용되지 않음)
//-----------------------------------------------------------------------------

void CDirectDrawSurface::SetTransparency(DWORD dwValue)
{
	// 구현되지 않음 - SDL 블렌드 모드를 대신 사용하라
	(void)dwValue;
}

void CDirectDrawSurface::FillSurface(WORD color)
{
	// 구현되지 않음 - SDL_FillRect를 대신 사용하라
	(void)color;
}

void CDirectDrawSurface::FillRect(RECT* pRect, WORD color)
{
	// 구현되지 않음 - SDL_FillRect를 대신 사용하라
	(void)pRect;
	(void)color;
}

//-----------------------------------------------------------------------------
// Lock/Unlock (호환성을 위한 최소한의 스텁)
//-----------------------------------------------------------------------------

bool CDirectDrawSurface::Lock()
{
	ZeroMemory(&m_ddsd, sizeof(m_ddsd));
	m_ddsd.dwSize = sizeof(m_ddsd);
	m_ddsd.lPitch = m_Width * 2;  // 16비트 색상으로 가정한다
	m_ddsd.lpSurface = nullptr;  // 실제 서페이스 메모리 없음
	m_bLock = true;
	return true;
}

bool CDirectDrawSurface::Unlock()
{
	m_bLock = false;
	return true;
}

void CDirectDrawSurface::LockW(WORD*& lpSurface, WORD& lPitch)
{
	// 스텁 - 잠글 실제 서페이스가 없음
	lpSurface = nullptr;
	lPitch = m_Width * 2;
	m_bLock = true;
}

//-----------------------------------------------------------------------------
// 블리팅 (스텁 - 더 이상 사용되지 않음)
//-----------------------------------------------------------------------------

void CDirectDrawSurface::Blt(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT* pRect)
{
	// 구현되지 않음 - SDL_BlitSurface를 대신 사용하라
	(void)pPoint;
	(void)SourceSurface;
	(void)pRect;
}

void CDirectDrawSurface::BltNoColorkey(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT* pRect)
{
	// 구현되지 않음 - SDL_BlitSurface를 대신 사용하라
	(void)pPoint;
	(void)SourceSurface;
	(void)pRect;
}

void CDirectDrawSurface::Blt(RECT* pDestRect, CDirectDrawSurface* SourceSurface, RECT* pSourceRect)
{
	// 구현되지 않음 - SDL_BlitSurface를 대신 사용하라
	(void)pDestRect;
	(void)SourceSurface;
	(void)pSourceRect;
}

void CDirectDrawSurface::BltPrimarySurface(POINT* pPoint, RECT* pRect)
{
	// 구현되지 않음
	(void)pPoint;
	(void)pRect;
}

//-----------------------------------------------------------------------------
// 감마 보정 (구현되지 않음 - 셰이더 효과를 대신 사용하라)
//-----------------------------------------------------------------------------

void CDirectDrawSurface::GammaBox555(RECT* pRect, int p)
{
	// 구현되지 않음 - SDL2 셰이더 효과를 사용하라
	(void)pRect;
	(void)p;
}

void CDirectDrawSurface::GammaBox565(RECT* pRect, int p)
{
	// 구현되지 않음 - SDL2 셰이더 효과를 사용하라
	(void)pRect;
	(void)p;
}

void CDirectDrawSurface::Gamma4Pixel565(void *pDest, int len, int p)
{
	// 구현되지 않음 - SDL2 셰이더 효과를 사용하라
	(void)pDest;
	(void)len;
	(void)p;
}

void CDirectDrawSurface::Gamma4Pixel555(void *pDest, int len, int p)
{
	// 구현되지 않음 - SDL2 셰이더 효과를 사용하라
	(void)pDest;
	(void)len;
	(void)p;
}

//-----------------------------------------------------------------------------
// GDI 텍스트 (구현되지 않음 - SDL2 텍스트 렌더링을 대신 사용하라)
//-----------------------------------------------------------------------------

void CDirectDrawSurface::ShowFPS(int x, int y, COLORREF fcolor, COLORREF bcolor)
{
	// 구현되지 않음 - SDL2 텍스트 렌더링을 사용하라
	(void)x;
	(void)y;
	(void)fcolor;
	(void)bcolor;
}

void CDirectDrawSurface::GDI_Text(int x, int y, const char *str, COLORREF fcolor, COLORREF bcolor, bool option)
{
	// 구현되지 않음 - SDL2 텍스트 렌더링을 사용하라
	(void)x;
	(void)y;
	(void)str;
	(void)fcolor;
	(void)bcolor;
	(void)option;
}

//-----------------------------------------------------------------------------
// BMP 저장/로드 (구현되지 않음 - SDL_image를 대신 사용하라)
//-----------------------------------------------------------------------------

bool CDirectDrawSurface::SaveToBMP(const char* szFilename)
{
	// 구현되지 않음 - SDL_image의 PNG/BMP 저장 기능을 사용하라
	(void)szFilename;
	return false;
}

bool CDirectDrawSurface::CopyBitmap(HBITMAP hbm, int x, int y, int dx, int dy)
{
	// 구현되지 않음 - Windows GDI 전용
	(void)hbm;
	(void)x;
	(void)y;
	(void)dx;
	(void)dy;
	return false;
}
