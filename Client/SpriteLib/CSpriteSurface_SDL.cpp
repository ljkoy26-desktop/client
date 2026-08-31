/*-----------------------------------------------------------------------------

	CSpriteSurface_SDL.cpp

	CSpriteSurface에 대한 SDL2 백엔드 구현부.
	SpriteLibBackend를 이용해 구현한 모든 CSpriteSurface 메소드를 포함한다.
	CDirectDraw나 Windows 전용 코드에는 의존하지 않는다.

	2025.01.14

-----------------------------------------------------------------------------*/

#ifdef SPRITELIB_BACKEND_SDL

#include "Client_PCH.h"
#include "CSprite.h"
#include "CAlphaSprite.h"
#include "CIndexSprite.h"
#include "CShadowSprite.h"
#include "CSpriteOutlineManager.h"
#include "CFilter.h"
#include "CSpriteSurface.h"
#include "SpriteLibBackend.h"
#include "SpriteLibBackendSDL.h"

/* ============================================================================
 * 정적 멤버 초기화
 * ============================================================================ */

int CSpriteSurface::s_Value1 = 1;
int CSpriteSurface::s_Value2 = 31;
int CSpriteSurface::s_Value3 = 1;

FUNCTION_MEMCPYEFFECT CSpriteSurface::s_pMemcpyEffectFunction = NULL;
FUNCTION_MEMCPYEFFECT CSpriteSurface::s_pMemcpyEffectFunctionTable[MAX_EFFECT] = {0};
FUNCTION_MEMCPYPALEFFECT CSpriteSurface::s_pMemcpyPalEffectFunction = NULL;
FUNCTION_MEMCPYPALEFFECT CSpriteSurface::s_pMemcpyPalEffectFunctionTable[MAX_EFFECT] = {0};

WORD CSpriteSurface::s_EffectScreenTableR[32][32] = {0};
WORD CSpriteSurface::s_EffectScreenTableG[32][32] = {0};
WORD CSpriteSurface::s_EffectScreenTableB[32][32] = {0};

/* ============================================================================
 * 생성자 / 소멸자
 * ============================================================================ */

CSpriteSurface::CSpriteSurface()
	: m_backend_surface(SPRITECTL_INVALID_SURFACE)
	, m_width(0)
	, m_height(0)
	, m_transparency(0)
	, m_lock_count(0)  // 잠금 카운터 초기화
{
}

CSpriteSurface::~CSpriteSurface()
{
	Release();
}

/* ============================================================================
 * 서페이스 초기화
 * ============================================================================ */

bool CSpriteSurface::Init(int width, int height)
{
	/* 기존 서페이스 정리 */
	Release();

	/* 백엔드 서페이스 생성 */
	m_backend_surface = spritectl_create_surface(width, height, SPRITECTL_FORMAT_RGB565);
	if (m_backend_surface == SPRITECTL_INVALID_SURFACE) {
		return false;
	}

	m_width = width;
	m_height = height;
	return true;
}

bool CSpriteSurface::InitFromFile(const char* filename)
{
	/* TODO: BMP 파일로부터 로드 */
	return false;
}

void CSpriteSurface::Release()
{
	if (m_backend_surface != SPRITECTL_INVALID_SURFACE) {
		spritectl_destroy_surface(m_backend_surface);
		m_backend_surface = SPRITECTL_INVALID_SURFACE;
	}
	m_width = 0;
	m_height = 0;
	m_transparency = 0;
}

/* ============================================================================
 * DirectX 호환용 메소드
 * ============================================================================ */

bool CSpriteSurface::InitOffsurface(int width, int height)
{
	/* SDL2에서는 오프스크린 서페이스가 일반 서페이스와 동일하다 */
	return Init(width, height);
}

void CSpriteSurface::SetTransparency(int value)
{
	m_transparency = value;
}

int CSpriteSurface::GetTransparency() const
{
	return m_transparency;
}

void CSpriteSurface::GDI_Text(int x, int y, const char* text, DWORD color)
{
	/* TODO: SDL2_ttf 등을 이용해 텍스트 렌더링 구현
	 * 현재는 스텁 상태 - SDL 백엔드에서 텍스트 렌더링은 미구현
	 */
	(void)x; (void)y; (void)text; (void)color;
}

/* ============================================================================
 * 그리기 함수
 * ============================================================================ */

void CSpriteSurface::DrawRect(RECT* rect, WORD color)
{
	if (!rect || m_backend_surface == SPRITECTL_INVALID_SURFACE) {
		return;
	}

	spritectl_surface_s* backend = (spritectl_surface_s*)m_backend_surface;
	SDL_Surface* surf = backend->surface;
	if (!surf) {
		return;
	}

	/* 백엔드를 이용해 사각형 채우기 */
	SDL_Rect sdl_rect;
	sdl_rect.x = rect->left;
	sdl_rect.y = rect->top;
	sdl_rect.w = rect->right - rect->left;
	sdl_rect.h = rect->bottom - rect->top;

	// 서페이스 포맷을 확인하여 그에 맞게 처리
	Uint32 pixel = 0;

	// 16비트 서페이스라면, 포맷이 일치할 경우 색상 값을 그대로 사용 시도
	if (SDL_ISPIXELFORMAT_INDEXED(surf->format->format) ||
	    surf->format->BitsPerPixel == 16) {
		// RGB565 포맷인지 확인
		if (surf->format->Rmask == 0xF800 &&
		    surf->format->Gmask == 0x07E0 &&
		    surf->format->Bmask == 0x001F) {
			// RGB565 포맷 - 색상 값을 그대로 사용
			pixel = (Uint32)color;

			// DEBUG: HP 바 색상(회색 계열) 확인용
			if ((color & 0xF800) != 0 && ((color & 0xF800) >> 11) < 25) {
				static int debug_count = 0;
				if (debug_count < 10) {
					uint8_t r = (color >> 11) & 0x1F;
					uint8_t g = (color >> 5) & 0x3F;
					uint8_t b = color & 0x1F;
					fprintf(stderr, "DrawRect HP color: input=0x%04X, RGB565=(%d,%d,%d), pixel=0x%08X, surface_format=%s\n",
						(uint16_t)color, r, g, b, pixel, SDL_GetPixelFormatName(surf->format->format));
					debug_count++;
				}
			}
		} else if (surf->format->Rmask == 0x7C00 &&
		           surf->format->Gmask == 0x03E0 &&
		           surf->format->Bmask == 0x001F) {
			// RGB555 포맷 - RGB565로부터 변환
			uint16_t rgb565 = (uint16_t)color;
			// RGB565를 RGB555로 변환
			pixel = ((rgb565 & 0xF800) >> 1) | ((rgb565 & 0x0600) >> 1) |  // R (5비트)
			        ((rgb565 & 0x07E0) >> 1) |  // G (5비트)
			        (rgb565 & 0x001F);         // B (5비트)
			static int rgb555_count = 0;
			if (rgb555_count < 3 && (color & 0xF800) != 0) {
				fprintf(stderr, "DrawRect: Converting RGB565 0x%04X to RGB555 0x%04X\n", rgb565, (uint16_t)pixel);
				rgb555_count++;
			}
		} else {
			// 알 수 없는 16비트 포맷 - SDL_MapRGB로 대체 처리
			uint8_t r = 0, g = 0, b = 0;
			if (backend->format == SPRITECTL_FORMAT_RGB555) {
				spritectl_555_to_rgb((uint16_t)color, &r, &g, &b);
			} else {
				spritectl_565_to_rgb((uint16_t)color, &r, &g, &b);
			}
			pixel = SDL_MapRGB(surf->format, r, g, b);
			static int fallback_count = 0;
			if (fallback_count < 3 && r > 100) {
				fprintf(stderr, "DrawRect: Fallback to SDL_MapRGB: format=%s, Rmask=0x%08X, Gmask=0x%08X, Bmask=0x%08X, RGB=(%d,%d,%d), pixel=0x%08X\n",
					SDL_GetPixelFormatName(surf->format->format),
					surf->format->Rmask, surf->format->Gmask, surf->format->Bmask,
					r, g, b, pixel);
				fallback_count++;
			}
		}
	} else {
		// 16비트가 아닌 서페이스는 SDL_MapRGB 사용
		uint8_t r = 0, g = 0, b = 0;
		if (backend->format == SPRITECTL_FORMAT_RGB555) {
			spritectl_555_to_rgb((uint16_t)color, &r, &g, &b);
		} else {
			spritectl_565_to_rgb((uint16_t)color, &r, &g, &b);
		}
		pixel = SDL_MapRGB(surf->format, r, g, b);
	}

	spritectl_surface_info_t info;
	if (spritectl_lock_surface(m_backend_surface, &info) == 0) {
		SDL_FillRect(surf, &sdl_rect, pixel);
		spritectl_unlock_surface(m_backend_surface);
	}
}

void CSpriteSurface::FillRect(RECT* rect, WORD color)
{
	DrawRect(rect, color);
}

void CSpriteSurface::HLine(int x, int y, int length, WORD color)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + length;
	rect.bottom = y + 1;
	DrawRect(&rect, color);
}

void CSpriteSurface::VLine(int x, int y, int length, WORD color)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + 1;
	rect.bottom = y + length;
	DrawRect(&rect, color);
}

void CSpriteSurface::Line(int x1, int y1, int x2, int y2, WORD color)
{
	/* TODO: 선 그리기 구현 */
	/* 현재는 수평/수직선만 그린다 */
	if (y1 == y2) {
		int x = (x1 < x2) ? x1 : x2;
		int len = (x2 - x1);
		if (len < 0) len = -len;
		HLine(x, y1, len, color);
	} else if (x1 == x2) {
		int y = (y1 < y2) ? y1 : y2;
		int len = (y2 - y1);
		if (len < 0) len = -len;
		VLine(x1, y, len, color);
	}
}

/* ============================================================================
 * 스텁 구현 (TODO)
 * ============================================================================ */

void CSpriteSurface::BltHalf(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltNoColorkey(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect)
{
	/* 컬러키 투명 처리 없이 소스 서페이스를 이 서페이스로 Blt */
	/* SDL 백엔드에서는 Blt()와 동일 - 둘 다 픽셀을 그대로 복사한다 */
	if (!pPoint || !SourceSurface) {
		return;
	}

	/* 소스/대상 정보 획득 */
	S_SURFACEINFO src_info, dst_info;
	SourceSurface->GetSurfaceInfo(&src_info);
	this->GetSurfaceInfo(&dst_info);

	if (!src_info.p_surface || !dst_info.p_surface) {
		return;
	}

	/* 크기 계산 */
	int src_x = pRect ? pRect->left : 0;
	int src_y = pRect ? pRect->top : 0;
	int src_w = pRect ? (pRect->right - pRect->left) : src_info.width;
	int src_h = pRect ? (pRect->bottom - pRect->top) : src_info.height;

	/* 소스 서페이스 범위로 클램프 */
	if (src_x + src_w > src_info.width) src_w = src_info.width - src_x;
	if (src_y + src_h > src_info.height) src_h = src_info.height - src_y;

	/* 대상 서페이스 범위로 클램프 */
	if (pPoint->x + src_w > dst_info.width) src_w = dst_info.width - pPoint->x;
	if (pPoint->y + src_h > dst_info.height) src_h = dst_info.height - pPoint->y;

	if (src_w <= 0 || src_h <= 0) {
		return;
	}

	/* 한 줄씩 픽셀 복사 (자기 자신에게 Blt하는 경우의 겹침을 처리) */
	WORD* src_pixels = (WORD*)src_info.p_surface;
	WORD* dst_pixels = (WORD*)dst_info.p_surface;
	const int src_pitch_words = src_info.pitch / 2;
	const int dst_pitch_words = dst_info.pitch / 2;
	const bool same_surface = (SourceSurface == this) || (src_info.p_surface == dst_info.p_surface);

	if (same_surface && pPoint->y > src_y) {
		for (int y = src_h - 1; y >= 0; --y) {
			WORD* src_row = src_pixels + (src_y + y) * src_pitch_words + src_x;
			WORD* dst_row = dst_pixels + (pPoint->y + y) * dst_pitch_words + pPoint->x;
			memmove(dst_row, src_row, src_w * sizeof(WORD));
		}
	} else {
		for (int y = 0; y < src_h; y++) {
			WORD* src_row = src_pixels + (src_y + y) * src_pitch_words + src_x;
			WORD* dst_row = dst_pixels + (pPoint->y + y) * dst_pitch_words + pPoint->x;
			if (same_surface) {
				memmove(dst_row, src_row, src_w * sizeof(WORD));
			} else {
				memcpy(dst_row, src_row, src_w * sizeof(WORD));
			}
		}
	}
}

void CSpriteSurface::BltDarkness(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect, BYTE DarkBits)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltBrightness(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect, BYTE BrightBits)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltDarknessFilter(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect, WORD TransColor)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::ChangeBrightnessBit(RECT* pRect, BYTE DarkBits)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::BltColorAlpha(RECT* pRect, WORD color, BYTE alpha2)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::InitEffectTable()
{
	/* 필요 시 이펙트 테이블 초기화 */
}

void CSpriteSurface::memcpyHalf(WORD* pDest, WORD* pSource, WORD pixels)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::memcpyAlpha(WORD* pDest, WORD* pSource, WORD pixels)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::memcpyColor(WORD* pDest, WORD* pSource, WORD pixels)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::memcpyScale(WORD* pDest, WORD destPitch, WORD* pSource, WORD pixels)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::memcpyDarkness(WORD* pDest, WORD* pSource, WORD pixels)
{
	/* TODO: 구현 필요 */
}

void CSpriteSurface::memcpyBrightness(WORD* pDest, WORD* pSource, WORD pixels)
{
	/* TODO: 구현 필요 */
}

/* ============================================================================
 * 클리핑 헬퍼
 * ============================================================================ */

bool CSpriteSurface::ClippingRectToPoint(RECT*& pRect, POINT*& pPoint)
{
	/* TODO: 클리핑 구현 */
	return true;
}

/* ============================================================================
 * 어댑터 구현 포함 (BltSprite 계열 메소드)
 * ============================================================================ */

/* BltSprite 계열 메소드용 어댑터 코드 포함 */
#include "CSpriteSurface_Adapter.cpp"

/* ============================================================================
 * Lock/Unlock 메소드 (호환성을 위한 스텁 구현)
 * ============================================================================ */

bool CSpriteSurface::LockSDL()
{
	/* 스텁: SDL 백엔드에서는 명시적인 잠금이 필요 없다
	 * 필요할 때 spritectl_lock_surface가 내부적으로 호출된다
	 */
	return true;
}

void CSpriteSurface::UnlockSDL()
{
	/* 스텁: SDL 백엔드에서는 명시적인 잠금 해제가 필요 없다 */
}

bool CSpriteSurface::IsLock()
{
	return m_lock_count > 0;
}

/* ============================================================================
 * 서페이스 정보 획득 (Windows 코드와의 호환용)
 * ============================================================================ */

int CSpriteSurface::GetSurfacePitch() const
{
	/* 서페이스의 피치(한 행당 바이트 수)를 반환 */
	if (m_backend_surface == SPRITECTL_INVALID_SURFACE) {
		return 0;
	}

	/* 피치를 얻기 위해 서페이스를 잠금 */
	spritectl_surface_info_t info;
	if (spritectl_lock_surface((spritectl_surface_t)m_backend_surface, &info) == 0) {
		int pitch = info.pitch;
		spritectl_unlock_surface((spritectl_surface_t)m_backend_surface);
		return pitch;
	}

	/* 기본값: RGB565 포맷 기준 width * 2 */
	return m_width * 2;
}

int CSpriteSurface::GetWidth() const
{
	return m_width;
}

int CSpriteSurface::GetHeight() const
{
	return m_height;
}

void CSpriteSurface::GetSurfaceInfo(S_SURFACEINFO* info)
{
	/* SDL 백엔드용 서페이스 정보 구조체 채우기 */
	if (m_backend_surface == SPRITECTL_INVALID_SURFACE) {
		info->p_surface = nullptr;
		info->width = 0;
		info->height = 0;
		info->pitch = 0;
		return;
	}

	/* 정보를 얻기 위해 서페이스를 잠금 */
	spritectl_surface_info_t sdl_info;
	if (spritectl_lock_surface(m_backend_surface, &sdl_info) == 0) {
		info->p_surface = sdl_info.pixels;
		info->width = sdl_info.width;
		info->height = sdl_info.height;
		info->pitch = sdl_info.pitch;
		spritectl_unlock_surface(m_backend_surface);
	} else {
		info->p_surface = nullptr;
		info->width = m_width;
		info->height = m_height;
		info->pitch = m_width * 2; /* RGB565 = 픽셀당 2바이트 */
	}
}

/* ============================================================================
 * GetDDSD 호환용 래퍼 (Windows API 호환용)
 * ============================================================================ */

S_SURFACEINFO* CSpriteSurface::GetDDSD()
{
	/* 서페이스 정보를 담는 정적 버퍼 - 호환성을 위해 포인터로 반환 */
	static S_SURFACEINFO ddsd_buffer;
	GetSurfaceInfo(&ddsd_buffer);
	return &ddsd_buffer;
}

/* ============================================================================
 * 클리핑 메소드 (CDirectDrawSurface와의 호환용)
 * ============================================================================ */

void CSpriteSurface::SetClip(RECT* rect)
{
	/* 스텁: SDL 백엔드는 동일한 방식의 클리핑 사각형을 사용하지 않는다 */
	/* SDL은 SDL_RenderSetClipRect와 함께 SDL_Rect로 클리핑을 처리한다 */
}

void CSpriteSurface::SetClipNULL()
{
	/* 스텁: 클리핑 초기화 - SDL 백엔드에서는 해당 없음 */
}

/* ============================================================================
 * Blt 메소드 (CDirectDrawSurface와의 호환용)
 * ============================================================================ */

void CSpriteSurface::Blt(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect)
{
	/* 스텁: 소스 서페이스로부터 이 서페이스로의 기본 블릿 */
	/* 실제로는 SourceSurface의 픽셀을 이 서페이스로 복사해야 한다 */
	if (!pPoint || !SourceSurface) {
		return;
	}

	/* 소스/대상 정보 획득 */
	S_SURFACEINFO src_info, dst_info;
	SourceSurface->GetSurfaceInfo(&src_info);
	this->GetSurfaceInfo(&dst_info);

	if (!src_info.p_surface || !dst_info.p_surface) {
		return;
	}

	/* 크기 계산 */
	int src_x = pRect ? pRect->left : 0;
	int src_y = pRect ? pRect->top : 0;
	int src_w = pRect ? (pRect->right - pRect->left) : src_info.width;
	int src_h = pRect ? (pRect->bottom - pRect->top) : src_info.height;

	/* 소스 서페이스 범위로 클램프 */
	if (src_x + src_w > src_info.width) src_w = src_info.width - src_x;
	if (src_y + src_h > src_info.height) src_h = src_info.height - src_y;

	/* 대상 서페이스 범위로 클램프 */
	if (pPoint->x + src_w > dst_info.width) src_w = dst_info.width - pPoint->x;
	if (pPoint->y + src_h > dst_info.height) src_h = dst_info.height - pPoint->y;

	if (src_w <= 0 || src_h <= 0) {
		return;
	}

	/* 한 줄씩 픽셀 복사 (자기 자신에게 Blt하는 경우의 겹침을 처리) */
	WORD* src_pixels = (WORD*)src_info.p_surface;
	WORD* dst_pixels = (WORD*)dst_info.p_surface;
	const int src_pitch_words = src_info.pitch / 2;
	const int dst_pitch_words = dst_info.pitch / 2;
	const bool same_surface = (SourceSurface == this) || (src_info.p_surface == dst_info.p_surface);

	if (same_surface && pPoint->y > src_y) {
		for (int y = src_h - 1; y >= 0; --y) {
			WORD* src_row = src_pixels + (src_y + y) * src_pitch_words + src_x;
			WORD* dst_row = dst_pixels + (pPoint->y + y) * dst_pitch_words + pPoint->x;
			memmove(dst_row, src_row, src_w * sizeof(WORD));
		}
	} else {
		for (int y = 0; y < src_h; y++) {
			WORD* src_row = src_pixels + (src_y + y) * src_pitch_words + src_x;
			WORD* dst_row = dst_pixels + (pPoint->y + y) * dst_pitch_words + pPoint->x;
			if (same_surface) {
				memmove(dst_row, src_row, src_w * sizeof(WORD));
			} else {
				memcpy(dst_row, src_row, src_w * sizeof(WORD));
			}
		}
	}
}

/* ============================================================================
 * FillSurface 메소드 (CDirectDrawSurface와의 호환용)
 * ============================================================================ */

void CSpriteSurface::FillSurface(WORD color)
{
	/* 서페이스 전체를 지정한 색상으로 채운다 */
	if (m_backend_surface == SPRITECTL_INVALID_SURFACE) {
		return;
	}

	/* 픽셀 데이터를 얻기 위해 서페이스를 잠금 */
	spritectl_surface_info_t info;
	if (spritectl_lock_surface(m_backend_surface, &info) == 0) {
		WORD* pixels = (WORD*)info.pixels;
		int pixel_count = info.width * info.height;

		/* 모든 픽셀을 지정한 색상으로 채움 */
		for (int i = 0; i < pixel_count; i++) {
			pixels[i] = color;
		}

		spritectl_unlock_surface(m_backend_surface);
	}
}

/* ============================================================================
 * 감마 보정
 * ============================================================================ */

void CSpriteSurface::Gamma4Pixel565(void *pDest, int len, int p)
{
	// TODO: [SDL_BACKEND] 최적화된 감마 보정 알고리즘 구현
	// 현재 구현은 기본적인 RGB 스케일링을 사용한다
	// 원본 Windows 구현은 x86 어셈블리를 사용한다
	WORD* dest = (WORD*)pDest;
	int light = p;

	for (int i = 0; i < len; i++) {
		WORD pixel = dest[i];

		// RGB565 성분 추출
		int r = (pixel >> 11) & 0x1F;
		int g = (pixel >> 5) & 0x3F;
		int b = pixel & 0x1F;

		// 감마 보정 적용
		r = (r * light) >> 5;
		g = (g * light) >> 5;
		b = (b * light) >> 5;

		// 값 클램프
		if (r > 31) r = 31;
		if (g > 63) g = 63;
		if (b > 31) b = 31;

		// RGB565로 재조합
		dest[i] = (r << 11) | (g << 5) | b;
	}
}

/* ============================================================================
 * Gamma4Pixel555 - 호환용으로 Gamma4Pixel565를 그대로 사용
 * RGB555와 RGB565는 구조가 동일하다 (5-6-5 vs 5-5-5)
 * SDL 백엔드는 RGB565만 사용하므로 Gamma4Pixel555는 Gamma4Pixel565로 매핑된다
 * ============================================================================ */
void CSpriteSurface::Gamma4Pixel555(void *pDest, int len, int p)
{
	// RGB555와 RGB565는 구조적으로 유사하다
	// SDL 백엔드에서는 항상 RGB565를 사용하므로 Gamma4Pixel565를 그대로 호출
	Gamma4Pixel565(pDest, len, p);
}

/* ============================================================================
 * GammaBox565/555 - pRect의 모든 행에 Gamma4Pixel565/555를 적용
 * (원본 CDirectDrawSurface::GammaBox565/555를 이식)
 * ============================================================================ */
void CSpriteSurface::GammaBox565(RECT* pRect, int p)
{
	if (!pRect)
	{
		return;
	}

	S_SURFACEINFO info;
	GetSurfaceInfo(&info);
	if (info.p_surface == NULL)
	{
		return;
	}

	// SDL 백엔드는 DirectDraw 방식의 클립 영역 추적이 없으므로,
	// 대신 서페이스 자체의 경계로 클리핑한다 (위쪽의 GetClipRight()/
	// GetClipBottom() 스텁이 m_width/m_height를 반환하는 것과 동일한 방식).
	if (pRect->bottom < 0 || pRect->top > info.height
		|| pRect->right < 0 || pRect->left > info.width)
	{
		return;
	}

	if (pRect->left < 0) pRect->left = 0;
	if (pRect->right > info.width) pRect->right = info.width;
	if (pRect->top < 0) pRect->top = 0;
	if (pRect->bottom > info.height) pRect->bottom = info.height;

	if (pRect->left >= pRect->right || pRect->top >= pRect->bottom)
	{
		return;
	}

	WORD* pDest = (WORD*)((BYTE*)info.p_surface + pRect->top * info.pitch + (pRect->left << 1));
	int dLen = pRect->right - pRect->left;
	int rows = pRect->bottom - pRect->top;

	for (int i = 0; i < rows; i++)
	{
		Gamma4Pixel565(pDest, dLen, p);
		pDest = (WORD*)((BYTE*)pDest + info.pitch);
	}
}

void CSpriteSurface::GammaBox555(RECT* pRect, int p)
{
	// SDL 백엔드는 항상 RGB565 서페이스를 사용한다 (위의 Gamma4Pixel555 참조)
	GammaBox565(pRect, p);
}

/* ============================================================================
 * 링커 호환을 위해 누락된 메소드
 * ============================================================================ */

void* CSpriteSurface::GetSurfacePointer()
{
	if (m_backend_surface == SPRITECTL_INVALID_SURFACE) return NULL;

#ifdef _DEBUG
	static bool warning_shown = false;
	if (!warning_shown) {
		fprintf(stderr, "WARNING: GetSurfacePointer() is deprecated and leaks locks!\n");
		fprintf(stderr, "         Use Lock() + GetSurfacePointer() + Unlock() instead\n");
		warning_shown = true;
	}
#endif

	spritectl_surface_info_t info;
	if (spritectl_lock_surface(m_backend_surface, &info) == 0) {
		m_lock_count++;  // 이 잠금을 추적
		return info.pixels;  // 호출자가 반드시 Unlock()을 호출해야 함!
	}
	return NULL;
}

void* CSpriteSurface::Lock(RECT* rect, DWORD* pitch)
{
	(void)rect;  // SDL 백엔드에서는 사용하지 않음
	if (m_backend_surface == SPRITECTL_INVALID_SURFACE) return NULL;

#ifdef _DEBUG
	// 이미 잠긴 상태라면 경고 (이중 잠금 가능성)
	if (m_lock_count > 0) {
		fprintf(stderr, "WARNING: CSpriteSurface::Lock() called while already locked (lock_count=%d)\n", m_lock_count);
	}
#endif

	spritectl_surface_info_t info;
	if (spritectl_lock_surface(m_backend_surface, &info) == 0) {
		if (pitch != NULL) {
			*pitch = info.pitch;
		}
		m_lock_count++;  // 잠금 추적
		return info.pixels;
	}
	return NULL;
}

void CSpriteSurface::Unlock()
{
	if (m_backend_surface != SPRITECTL_INVALID_SURFACE) {
#ifdef _DEBUG
		// 잠기지 않은 상태라면 경고 (이중 잠금 해제 가능성)
		if (m_lock_count <= 0) {
			fprintf(stderr, "WARNING: CSpriteSurface::Unlock() called but not locked!\n");
		} else {
			m_lock_count--;  // 잠금 해제 추적
		}
#else
		m_lock_count--;
#endif

		spritectl_unlock_surface(m_backend_surface);
	}
}

bool CSpriteSurface::InitTextureSurface(int width, int height, void* tex1, void* tex2)
{
	// OpenGL 텍스처 서페이스 초기화
	// SDL 백엔드에서는 일반 서페이스를 그대로 생성
	(void)tex1; (void)tex2; // 사용하지 않는 매개변수
	return Init(width, height);
}

bool CSpriteSurface::Restore()
{
	// 디바이스 손실 후 서페이스 복구 (Windows 전용)
	// SDL 백엔드에서는 아무 동작도 하지 않음
	return true;
}

void CSpriteSurface::SetEffect(FUNCTION_EFFECT effect)
{
	s_pMemcpyEffectFunction = s_pMemcpyEffectFunctionTable[effect];
}

void CSpriteSurface::SetPalEffect(FUNCTION_EFFECT effect)
{
	s_pMemcpyPalEffectFunction = s_pMemcpyPalEffectFunctionTable[effect];
}

// 정적 이펙트 메소드
void CSpriteSurface::memcpyEffect(unsigned short* dest, unsigned short* src, unsigned short pixels)
{
	if (s_pMemcpyEffectFunction != NULL) {
		s_pMemcpyEffectFunction(dest, src, pixels);
	} else {
		// 기본값: 단순 복사
		for (int i = 0; i < pixels; i++) {
			dest[i] = src[i];
		}
	}
}

void CSpriteSurface::memcpyEffectGradation(unsigned short* dest, unsigned short* src, unsigned short pixels)
{
	// 그라데이션 이펙트 - 현재는 단순 복사
	memcpyEffect(dest, src, pixels);
}

#endif /* SPRITELIB_BACKEND_SDL */
