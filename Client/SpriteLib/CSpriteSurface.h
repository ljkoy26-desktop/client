#include <fstream>
//----------------------------------------------------------------------
// CDirectDrawSurface.h
//----------------------------------------------------------------------
//
//
// 5:6:5 와  5:5:5의 차이는 
// Bit Mask에 의해 처리되는 부분에서 발생하는데
// CDirectDraw class의 InitMask()에서 Video Card에 맞는
// 적절한 Mask를 생성하므로 그냥~ CDirectDraw의 Mask를 사용하면 된다.
//
//
//
//----------------------------------------------------------------------
/*

  < DDSurface가 생성되는 MEMORY >

	- DDSCAPS_SYSTEMMEMORY는 Lock을 사용할 때 빠르다.
	- DDSCAPS_VIDEOMEMORY는 Blt를 사용할 때 빠르다.
	  단, VIDEOMEMORY가 부족하면 Surface를 생성하지 않으므로 error!
	- 지정을 안 해 주면 VIDEOMEMORY가 사용가능하면 사용하고
	  아니면 SYSTEMMEMORY를 사용한다.

  < Alpha Blending >
	- 사용하려면 DDSCAPS_SYSTEMMEMORY를 사용하는게 좋다.
	- Surface --> Surface로의 Alpha Blending보다
	  Memory(SpriteFile) --> Surface로의 Alpha Blending이 빠르다.

  < Sprite File >
	- 0번 압축 된 것과 안 된 것 두가지의 경우가 있다.
	- 0번 압축 된 것은 투명색이 있는 모든 경우에 빠르다.
	  단, MMX instruction으로 투명색을 제거할 경우가 더 빠르겠지...

  < 사용시 주의 사항 >
	- Clipping이 되는 함수와 안 되는 함수가 있는데
	  적절한 경우에 맞춰서 사용해야할 것이다. 속도 문제가 있으니까!


*/
//----------------------------------------------------------------------

#ifndef	__CSPRITESURFACE_H__
#define	__CSPRITESURFACE_H__

/* 백엔드 선택 */
#ifdef SPRITELIB_BACKEND_SDL
#include "SpriteLibBackend.h"
/* SDL 백엔드: CDirectDrawSurface를 상속받지 않는 독립 클래스 */
#define SPRITESURFACE_STANDALONE
#include "../../basic/2d.h"  // S_SURFACEINFO 구조체 정의용
#endif

#include "CSpritePal.h"
#include "CAlphaSpritePal.h"
#include "MPalettePack.h"

class CSprite;
class CAlphaSprite;
class CIndexSprite;
class CShadowSprite;
class CSpriteOutlineManager;

typedef void (*FUNCTION_MEMCPYEFFECT)(WORD*, WORD*, WORD);
typedef void (*FUNCTION_MEMCPYPALEFFECT)(WORD*, BYTE*, WORD, MPalette&);

#ifdef SPRITESURFACE_STANDALONE

/* ============================================================================
 * SDL 백엔드: 독립형 CSpriteSurface (CDirectDrawSurface를 상속받지 않음)
 * ============================================================================ */

class CSpriteSurface {
public:
	CSpriteSurface();
	~CSpriteSurface();

	//------------------------------------------------------------
	// 서피스 초기화
	//------------------------------------------------------------
	bool Init(int width, int height);
	bool InitFromFile(const char* filename);
	void Release();

	//------------------------------------------------------------
	// 그리기 함수
	//------------------------------------------------------------
	void	DrawRect(RECT* rect, WORD color);
	void	HLine(int x, int y, int length, WORD color);
	void	VLine(int x, int y, int length, WORD color);
	void	Line(int x1, int y1, int x2, int y2, WORD color);
	void	FillRect(RECT* rect, WORD color);



	//------------------------------------------------------------
	// BltFast 메서드(호환성을 위한 스텁)
	// 참고: SDL 백엔드에서는 CDirectDrawSurface* 대신 CSpriteSurface*를 사용하세요.
	//------------------------------------------------------------
	void	BltHalf(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect);
	void	BltNoColorkey(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect);
	void	BltDarkness(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect, BYTE DarkBits);
	void	BltBrightness(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect, BYTE BrightBits);
	void	BltDarknessFilter(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect, WORD TransColor = 0);

	// 밝기 변경
	void	ChangeBrightnessBit(RECT* pRect, BYTE DarkBits);
	void	BltColorAlpha(RECT* pRect, WORD color, BYTE alpha2);

	//------------------------------------------------------------
	// 스프라이트 블리팅 메서드
	//------------------------------------------------------------
	// 스프라이트
	void	BltSprite(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteNoClip(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteHalf(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteAlpha(POINT* pPoint, CSprite* pSprite, BYTE alphaDepth);
	void	BltSpriteColor(POINT* pPoint, CSprite* pSprite, BYTE rgb);
	void	BltSpriteDarkness(POINT* pPoint, CSprite* pSprite, BYTE DarkBits);
	void	BltSpriteColorSet(POINT* pPoint, CSprite* pSprite, WORD colorSet);
	void	BltSpriteEffect(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteScale(POINT* pPoint, CSprite* pSprite, int scale = 256);
	void	BltSpriteAlpha4444SmallNotTrans(POINT* pPoint, CSprite* pSprite, BYTE alpha, BYTE shift);
	void	BltSpriteAlpha4444NotTrans(POINT* pPoint, CSprite* pSprite, BYTE alpha);
	void	BltSprite1555SmallNotTrans(POINT* pPoint, CSprite* pSprite, BYTE shift);
	void	BltSprite1555NotTrans(POINT* pPoint, CSprite* pSprite);

	void	BltSpritePalEffect(POINT* pPoint, CSpritePal* pSprite, MPalette& pal);
	void	BltSpritePal1555SmallNotTrans(POINT* pPoint, CSpritePal* pSprite, BYTE shift, MPalette& pal);
	void	BltSpritePal1555NotTrans(POINT* pPoint, CSpritePal* pSprite, MPalette& pal);

	// 필터
	void	BltSpriteAlphaFilter(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteAlphaFilterDarkness(POINT* pPoint, CSprite* pSprite, BYTE DarkBits);
	void	BltSpriteDarkerFilter(POINT* pPoint, CSprite* pSprite);

	// 알파 스프라이트
	void	BltAlphaSprite(POINT* pPoint, CAlphaSprite* pSprite);
	void	BltAlphaSpriteAlpha(POINT* pPoint, CAlphaSprite* pSprite, BYTE alpha);
	void	BltAlphaSprite4444(POINT* pPoint, CAlphaSprite* pSprite);
	void	BltAlphaSprite4444NotTrans(POINT* pPoint, CAlphaSprite* pSprite);
	void	BltAlphaSprite4444SmallNotTrans(POINT* pPoint, CAlphaSprite* pSprite, BYTE shift);

	void	BltAlphaSpritePal(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette& pal);
	void	BltAlphaSpritePalAlpha(POINT* pPoint, CAlphaSpritePal* pSprite, BYTE alpha, MPalette& pal);
	void	BltAlphaSpritePal4444(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette& pal);
	void	BltAlphaSpritePal4444NotTrans(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette& pal);
	void	BltAlphaSpritePal4444SmallNotTrans(POINT* pPoint, CAlphaSpritePal* pSprite, BYTE shift, MPalette& pal);

	// 인덱스 스프라이트
	void	BltIndexSprite(POINT* pPoint, CIndexSprite* pSprite);
	void	BltIndexSpriteDarkness(POINT* pPoint, CIndexSprite* pSprite, BYTE DarkBits);
	void	BltIndexSpriteAlpha(POINT* pPoint, CIndexSprite* pSprite, BYTE alpha);
	void	BltIndexSpriteColor(POINT* pPoint, CIndexSprite* pSprite, BYTE rgb);
	void	BltIndexSpriteColorSet(POINT* pPoint, CIndexSprite* pSprite, WORD colorSet);
	void	BltIndexSpriteEffect(POINT* pPoint, CIndexSprite* pSprite);
	void	BltIndexSpriteBrightness(POINT* pPoint, CIndexSprite* pSprite, BYTE BrightBits);

	// 스프라이트 외곽선
	void	BltSpriteOutline(CSpriteOutlineManager* pSOM, WORD color);
	void	BltSpriteOutlineOnly(CSpriteOutlineManager* pSOM, WORD color);
	void	BltSpriteOutlineDarkness(CSpriteOutlineManager* pSOM, WORD color, BYTE DarkBits);

	// 그림자 스프라이트
	void	BltShadowSprite(POINT* pPoint, CShadowSprite* pSprite);
	void	BltShadowSpriteSmall(POINT* pPoint, CShadowSprite* pSprite, BYTE shift);
	void	BltShadowSpriteDarkness(POINT* pPoint, CShadowSprite* pSprite, BYTE DarkBits);
	void	BltShadowSprite4444(POINT* pPoint, CShadowSprite* pSprite, WORD pixel);
	void	BltShadowSpriteSmall4444(POINT* pPoint, CShadowSprite* pSprite, WORD pixel, BYTE shift);

	//------------------------------------------------------------
	// 정적 이펙트 함수 (호환성을 위한 스텁)
	//------------------------------------------------------------
	static void		InitEffectTable();
	static void		memcpyHalf(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyAlpha(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyColor(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyScale(WORD* pDest, WORD destPitch, WORD* pSource, WORD pixels);
	static void		memcpyDarkness(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyBrightness(WORD* pDest, WORD* pSource, WORD pixels);
	static void		Gamma4Pixel565(void* pDest, int len, int p);
	static void		Gamma4Pixel555(void* pDest, int len, int p);
	static WORD		memcpyAlpha1Pixel(WORD pDest, WORD pSource) { return 0; }

	// GammaBox - 이 서피스의 pRect 각 행에 Gamma4Pixel565/555를 적용
	void	GammaBox565(RECT* pRect, int p);
	void	GammaBox555(RECT* pRect, int p);

	static int		s_Value1;
	static int		s_Value2;
	static int		s_Value3;

public:
	enum FUNCTION_EFFECT
	{
		EFFECT_DARKER = 0,
		EFFECT_GRAY_SCALE,
		EFFECT_LIGHTEN,
		EFFECT_DARKEN,
		EFFECT_COLOR_DODGE,
		EFFECT_SCREEN,
		EFFECT_DODGE_BURN,
		EFFECT_DIFFERENT,
		EFFECT_GRADATION,
		EFFECT_SIMPLE_OUTLINE,
		EFFECT_WIPE_OUT,
		EFFECT_NET,
		EFFECT_GRAY_SCALE_VARIOUS,
		EFFECT_SCREEN_ALPHA,
		MAX_EFFECT
	};

	static void		SetEffect(enum FUNCTION_EFFECT func);
	static void		SetPalEffect(enum FUNCTION_EFFECT func);
	static void		memcpyEffect(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyPalEffect(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);

	// 이펙트 함수 - CSpriteSurface.cpp에 구현됨
	static void		memcpyEffectDarker(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectLighten(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectDarken(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectColorDodge(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectScreen(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectDodgeBurn(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectDifferent(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectGrayScale(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectSimpleOutline(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectWipeOut(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectNet(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectGrayScaleVarious(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectScreenAlpha(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectGradation(WORD* pDest, WORD* pSource, WORD pixels);

	static void		memcpyPalEffectDarker(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectGrayScale(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectLighten(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectDarken(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectColorDodge(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectScreen(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectDodgeBurn(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectDifferent(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectGradation(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectSimpleOutline(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectWipeOut(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectNet(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectGrayScaleVarious(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectScreenAlpha(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);

	static FUNCTION_MEMCPYEFFECT		s_pMemcpyEffectFunction;
	static FUNCTION_MEMCPYPALEFFECT		s_pMemcpyPalEffectFunction;
	static FUNCTION_MEMCPYEFFECT		s_pMemcpyEffectFunctionTable[MAX_EFFECT];
	static FUNCTION_MEMCPYPALEFFECT		s_pMemcpyPalEffectFunctionTable[MAX_EFFECT];

	static WORD		s_EffectScreenTableR[32][32];
	static WORD		s_EffectScreenTableG[32][32];
	static WORD		s_EffectScreenTableB[32][32];

	//------------------------------------------------------------
	// DirectX 호환성 메서드 (SDL 백엔드용 스텁)
	//------------------------------------------------------------
	bool				Restore();
	void* Lock(RECT* rect = NULL, DWORD* pitch = NULL);
	void				Unlock();
	int					GetSurfacePitch() const;
	void* GetSurfacePointer();
	int					GetWidth() const;
	int					GetHeight() const;

#ifdef SPRITELIB_BACKEND_SDL
	// SDL 백엔드 호환성을 위한 Lock/Unlock 메서드 (Windows와 시그니처가 다름)
	bool				LockSDL();  // Lock()과의 충돌을 방지하기 위해 이름 변경됨
	void				UnlockSDL();
	bool				IsLock();

#ifdef SPRITELIB_BACKEND_SDL
	// 신규: 실제 락 상태 조회 (디버깅용)
	bool				IsLocked() const { return m_lock_count > 0; }
	int					GetLockCount() const { return m_lock_count; }
#endif

	void				GetSurfaceInfo(S_SURFACEINFO* info);
	// GetDDSD 호환성 래퍼 - 내부 서피스 정보 포인터 반환
	S_SURFACEINFO* GetDDSD();

	// 백엔드 서피스 핸들 가져오기 (SDL 렌더러에 출력용)
	spritectl_surface_t	GetBackendSurface() const { return m_backend_surface; }
#endif
	bool				InitTextureSurface(int width, int height, void* pixels = NULL, void* pixelFormat = NULL);

	// 오프스크린 서피스 초기화 (CDirectDrawSurface와의 호환성)
	bool				InitOffsurface(int width, int height);
	void				SetTransparency(int value);
	int					GetTransparency() const;

	// GDI 텍스트 렌더링 (SDL 백엔드용 스텁 - 텍스트 렌더링 미구현)
	void				GDI_Text(int x, int y, const char* text, DWORD color);

	// FillSurface (CDirectDrawSurface와의 호환성)
	void				FillSurface(WORD color);

	// 클리핑 메서드 (CDirectDrawSurface와의 호환성)
	void				SetClip(RECT* rect);
	void				SetClipNULL();

	// Blt 메서드 (CDirectDrawSurface와의 호환성)
	void				Blt(POINT* pPoint, CSpriteSurface* SourceSurface, RECT* pRect);

	//------------------------------------------------------------
	// 클리핑 메서드 (CDirectDrawSurface와의 호환성)
	//------------------------------------------------------------
	inline int		GetClipRight() const { return m_width; }  // 스텁: 전체 너비 반환
	inline int		GetClipBottom() const { return m_height; } // 스텁: 전체 높이 반환
	inline void		SetClipRight(int Right) { /* 스텁: 아무 작업도 하지 않음 */ }
	inline void		SetClipRightBottom(int Right, int Bottom) { /* 스텁: 아무 작업도 하지 않음 */ }
	inline void		SetClipLeftTop(int Left, int Top) { /* 스텁: 아무 작업도 하지 않음 */ }

	// GetSurface 메서드 (호환성을 위한 스텁 - nullptr 반환)
	inline void* GetSurface() { return nullptr; }

protected:
	bool	ClippingRectToPoint(RECT*& pRect, POINT*& pPoint);

#ifdef SPRITELIB_BACKEND_SDL
	/* 백엔드 서피스 핸들 */
	spritectl_surface_t m_backend_surface;
	int m_width;
	int m_height;
	int m_transparency;  // 호환성을 위한 투명도 값

	// 이중 락 방지를 위한 락 상태 추적
	int m_lock_count;    // Lock()이 호출된 횟수

	// 클리핑 사각형 (CDirectDrawSurface와의 호환성)
	int m_ClipLeft;
	int m_ClipTop;
	int m_ClipRight;
	int m_ClipBottom;

	// 서피스 정보 (CDirectDrawSurface와의 호환성)
	S_SURFACEINFO m_ddsd;  // SDL 호환 서피스 정보 구조체 사용
#endif
};

#else

/* ============================================================================
 * Windows 백엔드: CDirectDrawSurface를 상속받는 원본 구현
 * ============================================================================ */

 // CDirectDraw include 제거됨 - 대신 ColorDraw 사용

class CSpriteSurface : public CDirectDrawSurface {
public:
	CSpriteSurface();
	~CSpriteSurface();

	//------------------------------------------------------------
	// BltFast
	//------------------------------------------------------------
	void	BltHalf(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT* pRect);
	void	BltDarkness(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT* pRect, BYTE DarkBits);
	void	BltBrightness(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT* pRect, BYTE BrightBits);
	void	BltDarknessFilter(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT* pRect, WORD TransColor = 0);

	//------------------------------------------------------------
	// Drawing 함수
	//------------------------------------------------------------
	void	DrawRect(RECT* rect, WORD color);
	void	HLine(int x, int y, int length, WORD color);		// 가로
	void	VLine(int x, int y, int length, WORD color);		// 세로
	void	Line(int x1, int y1, int x2, int y2, WORD color);	// 직선


	//------------------------------------------------------------
	//
	// 잠금 및 그리기 함수
	//
	//------------------------------------------------------------

	//------------------------------------------------------------
	// 어둡게 하기
	//------------------------------------------------------------
	void	ChangeBrightnessBit(RECT* pRect, BYTE DarkBits);

	//------------------------------------------------------------
	// 단색 Alpha blending (화면의 pRect를 color와 alpha로..)
	// alpha2가 0이면 color로.. 31이면.. 원래 surface
	//------------------------------------------------------------
	void	BltColorAlpha(RECT* pRect, WORD color, BYTE alpha2);

	//virtual void	BltHalf(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT*  pRect, DWORD ColorKey=0);
	//virtual void	BltDarkness(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT*  pRect, BYTE bits, WORD ColorKey=0);		

	//------------------------------------------------------------
	// AlphaDepth는 1~32이다.
	//------------------------------------------------------------		
	//virtual void	BltTransAlpha(POINT* pPoint, CDirectDrawSurface* SourceSurface, RECT*  pRect, int alphaDepth, DWORD ColorKey=0);

	//------------------------------------------------------------
	//
	// CSprite 관련 함수
	//
	//------------------------------------------------------------
	// 스프라이트
	void	BltSprite(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteNoClip(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteHalf(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteAlpha(POINT* pPoint, CSprite* pSprite, BYTE alphaDepth);
	void	BltSpriteColor(POINT* pPoint, CSprite* pSprite, BYTE rgb);
	void	BltSpriteDarkness(POINT* pPoint, CSprite* pSprite, BYTE DarkBits);
	void	BltSpriteColorSet(POINT* pPoint, CSprite* pSprite, WORD colorSet);
	void	BltSpriteEffect(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteScale(POINT* pPoint, CSprite* pSprite, int scale = 256);
	void	BltSpriteAlpha4444SmallNotTrans(POINT* pPoint, CSprite* pSprite, BYTE alpha, BYTE shift);
	void	BltSpriteAlpha4444NotTrans(POINT* pPoint, CSprite* pSprite, BYTE alpha);
	void	BltSprite1555SmallNotTrans(POINT* pPoint, CSprite* pSprite, BYTE shift);
	void	BltSprite1555NotTrans(POINT* pPoint, CSprite* pSprite);

	void	BltSpritePalEffect(POINT* pPoint, CSpritePal* pSprite, MPalette& pal);
	void	BltSpritePal1555SmallNotTrans(POINT* pPoint, CSpritePal* pSprite, BYTE shift, MPalette& pal);
	void	BltSpritePal1555NotTrans(POINT* pPoint, CSpritePal* pSprite, MPalette& pal);

	// 필터
	void	BltSpriteAlphaFilter(POINT* pPoint, CSprite* pSprite);
	void	BltSpriteAlphaFilterDarkness(POINT* pPoint, CSprite* pSprite, BYTE DarkBits);
	void	BltSpriteDarkerFilter(POINT* pPoint, CSprite* pSprite);
	//void	BltSpriteAlphaFilter(POINT* pPoint, CSprite* pSprite, CFilter* pFilter);

	// 알파 스프라이트
	void	BltAlphaSprite(POINT* pPoint, CAlphaSprite* pSprite);
	void	BltAlphaSpriteAlpha(POINT* pPoint, CAlphaSprite* pSprite, BYTE alpha);
	void	BltAlphaSprite4444(POINT* pPoint, CAlphaSprite* pSprite);
	void	BltAlphaSprite4444NotTrans(POINT* pPoint, CAlphaSprite* pSprite);
	void	BltAlphaSprite4444SmallNotTrans(POINT* pPoint, CAlphaSprite* pSprite, BYTE shift);

	void	BltAlphaSpritePal(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette& pal);
	void	BltAlphaSpritePalAlpha(POINT* pPoint, CAlphaSpritePal* pSprite, BYTE alpha, MPalette& pal);
	void	BltAlphaSpritePal4444(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette& pal);
	void	BltAlphaSpritePal4444NotTrans(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette& pal);
	void	BltAlphaSpritePal4444SmallNotTrans(POINT* pPoint, CAlphaSpritePal* pSprite, BYTE shift, MPalette& pal);

	// 인덱스 스프라이트
	void	BltIndexSprite(POINT* pPoint, CIndexSprite* pSprite);
	void	BltIndexSpriteDarkness(POINT* pPoint, CIndexSprite* pSprite, BYTE DarkBits);
	void	BltIndexSpriteAlpha(POINT* pPoint, CIndexSprite* pSprite, BYTE alpha);
	void	BltIndexSpriteColor(POINT* pPoint, CIndexSprite* pSprite, BYTE rgb);
	void	BltIndexSpriteColorSet(POINT* pPoint, CIndexSprite* pSprite, WORD colorSet);
	void	BltIndexSpriteEffect(POINT* pPoint, CIndexSprite* pSprite);
	void	BltIndexSpriteBrightness(POINT* pPoint, CIndexSprite* pSprite, BYTE BrightBits);

	// 스프라이트 외곽선
	void	BltSpriteOutline(CSpriteOutlineManager* pSOM, WORD color);
	void	BltSpriteOutlineOnly(CSpriteOutlineManager* pSOM, WORD color);
	void	BltSpriteOutlineDarkness(CSpriteOutlineManager* pSOM, WORD color, BYTE DarkBits);

	// 그림자 스프라이트
	void	BltShadowSprite(POINT* pPoint, CShadowSprite* pSprite);
	void	BltShadowSpriteSmall(POINT* pPoint, CShadowSprite* pSprite, BYTE shift);
	void	BltShadowSpriteDarkness(POINT* pPoint, CShadowSprite* pSprite, BYTE DarkBits);
	void	BltShadowSprite4444(POINT* pPoint, CShadowSprite* pSprite, WORD pixel);
	void	BltShadowSpriteSmall4444(POINT* pPoint, CShadowSprite* pSprite, WORD pixel, BYTE shift);

	// class 내부에서 쓰는 함수
protected:
	bool	ClippingRectToPoint(RECT*& pRect, POINT*& pPoint);

#ifdef SPRITELIB_BACKEND_SDL
	/* SDL2 구현용 백엔드 서피스 핸들 */
	spritectl_surface_t m_backend_surface;
#endif

public:
	static void		InitEffectTable();
	static void		memcpyHalf(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyAlpha(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyColor(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyScale(WORD* pDest, WORD destPitch, WORD* pSource, WORD pixels);
	static void		memcpyDarkness(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyBrightness(WORD* pDest, WORD* pSource, WORD pixels);
	static WORD		memcpyAlpha1Pixel(WORD pDest, WORD pSource)
	{
		int sr, sg, sb, dr, dg, db;

		// 한점 찍기
		sr = ColorDraw::Red(pSource);
		sg = ColorDraw::Green(pSource);
		sb = ColorDraw::Blue(pSource);

		dr = ColorDraw::Red(pDest);
		dg = ColorDraw::Green(pDest);
		db = ColorDraw::Blue(pDest);

		return ((s_Value1 * (sb - db) >> 5) + db |
			((s_Value1 * (sg - dg) >> 5) + dg) << ColorDraw::s_bSHIFT_G |
			((s_Value1 * (sr - dr) >> 5) + dr) << ColorDraw::s_bSHIFT_R);
	}

	// memcpy..()에서 이용하는 값들
	static int		s_Value1;	// DarkBits, Alpha
	static int		s_Value2;	// 32-Alpha
	static int		s_Value3;

public:
	// memcpyEffect의 종류
	enum FUNCTION_EFFECT
	{
		EFFECT_DARKER = 0,
		EFFECT_GRAY_SCALE,
		EFFECT_LIGHTEN,
		EFFECT_DARKEN,
		EFFECT_COLOR_DODGE,
		EFFECT_SCREEN,
		EFFECT_DODGE_BURN,
		EFFECT_DIFFERENT,
		EFFECT_GRADATION,
		EFFECT_SIMPLE_OUTLINE,
		EFFECT_WIPE_OUT,
		EFFECT_NET,
		EFFECT_GRAY_SCALE_VARIOUS,
		EFFECT_SCREEN_ALPHA,
		MAX_EFFECT
	};

public:
	// 특수효과 출력용
	static void		SetEffect(enum FUNCTION_EFFECT func) { s_pMemcpyEffectFunction = s_pMemcpyEffectFunctionTable[func]; }
	static void		SetPalEffect(enum FUNCTION_EFFECT func) { s_pMemcpyPalEffectFunction = s_pMemcpyPalEffectFunctionTable[func]; }
	static void		memcpyEffect(WORD* pDest, WORD* pSource, WORD pixels)
	{
		(*s_pMemcpyEffectFunction)(pDest, pSource, pixels);
	}

	static void		memcpyPalEffect(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal)
	{
		(*s_pMemcpyPalEffectFunction)(pDest, pSource, pixels, pal);
	}

	// memcpyEffect
	static void		memcpyEffectDarker(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectGrayScale(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectLighten(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectDarken(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectColorDodge(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectScreen(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectDodgeBurn(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectDifferent(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectGradation(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectSimpleOutline(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectWipeOut(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectNet(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectGrayScaleVarious(WORD* pDest, WORD* pSource, WORD pixels);
	static void		memcpyEffectScreenAlpha(WORD* pDest, WORD* pSource, WORD pixels);

	// memcpyEffect
	static void		memcpyPalEffectDarker(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectGrayScale(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectLighten(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectDarken(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectColorDodge(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectScreen(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectDodgeBurn(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectDifferent(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectGradation(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectSimpleOutline(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectWipeOut(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectNet(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectGrayScaleVarious(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);
	static void		memcpyPalEffectScreenAlpha(WORD* pDest, BYTE* pSource, WORD pixels, MPalette& pal);



	static FUNCTION_MEMCPYEFFECT		s_pMemcpyEffectFunction;
	static FUNCTION_MEMCPYPALEFFECT		s_pMemcpyPalEffectFunction;
	static FUNCTION_MEMCPYEFFECT		s_pMemcpyEffectFunctionTable[MAX_EFFECT];
	static FUNCTION_MEMCPYPALEFFECT		s_pMemcpyPalEffectFunctionTable[MAX_EFFECT];

	static WORD		s_EffectScreenTableR[32][32];
	static WORD		s_EffectScreenTableG[32][32];
	static WORD		s_EffectScreenTableB[32][32];
};

#endif /* SPRITESURFACE_STANDALONE */

#endif /* __CSPRITESURFACE_H__ */