/*-----------------------------------------------------------------------------

	ColorDraw.h

	CDirectDraw에서 추출한 색상 처리 유틸리티.
	RGB 5:6:5 포맷을 위한 순수 수학 연산이며,
	그래픽 API에 독립적이다.

	2025.02.04 - SpriteLib->DXLib 의존성을 제거하기 위해 분리됨

-----------------------------------------------------------------------------*/

#ifndef __COLORDRAW_H__
#define __COLORDRAW_H__

#include "Platform.h"

class ColorDraw
{
public:
	//----------------------------------------------------------------------
	// 색상 성분 추출 (RGB 5:6:5 포맷)
	// R: 11-15비트, G: 5-10비트, B: 0-4비트
	//----------------------------------------------------------------------
	static inline BYTE	Red(const WORD& c)
	{
		return (c >> 11) & 0x1F;
	}

	static inline BYTE	Green(const WORD& c)
	{
		return (c >> 5) & 0x3F;
	}

	static inline BYTE	Blue(const WORD& c)
	{
		return c & 0x1F;
	}

	//----------------------------------------------------------------------
	// 픽셀 포맷 감지
	//----------------------------------------------------------------------
	static inline bool	Is565()
	{
		return true;  // SDL2 백엔드는 항상 RGB 5:6:5를 사용한다
	}

	//----------------------------------------------------------------------
	// 색상 생성
	//----------------------------------------------------------------------
	static inline WORD	Color(BYTE r, BYTE g, BYTE b)
	{
		// 5:6:5 포맷: R은 11-15비트, G는 5-10비트, B는 0-4비트
		return ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
	}

	//----------------------------------------------------------------------
	// 색상 변환 (5:5:5 <-> 5:6:5)
	//----------------------------------------------------------------------
	static inline WORD	Convert555to565(WORD pixel)
	{
		return ((pixel & 0x7FE0) << 1) | (pixel & 0x001F);
	}

	static inline WORD	Convert565to555(WORD pixel)
	{
		return (pixel & 0xFFE0) >> 1;
	}

	//----------------------------------------------------------------------
	// 색상 재조합을 위한 정적 시프트 값
	// RGB 5:6:5 포맷과 일치: R@11, G@5, B@0
	//----------------------------------------------------------------------
	static BYTE	s_bSHIFT_R;	// RGB 5:6:5 기준 = 11
	static BYTE	s_bSHIFT_G;	// RGB 5:6:5 기준 = 5
	static BYTE	s_bSHIFT_B;	// RGB 5:6:5 기준 = 0
	static BYTE	s_bSHIFT_A;	// = 0 (미사용)

	//----------------------------------------------------------------------
	// ARGB 4:4:4:4 포맷을 위한 정적 시프트 값
	//----------------------------------------------------------------------
	static BYTE	s_bSHIFT4_R;	// ARGB 4:4:4:4 기준 = 12
	static BYTE	s_bSHIFT4_G;	// ARGB 4:4:4:4 기준 = 8
	static BYTE	s_bSHIFT4_B;	// ARGB 4:4:4:4 기준 = 4
	static BYTE	s_bSHIFT4_A;	// ARGB 4:4:4:4 기준 = 0

	//----------------------------------------------------------------------
	// 정적 마스크 배열 (원본 코드와의 호환성을 위함)
	//----------------------------------------------------------------------
	static WORD	s_wMASK_SHIFT[5];
	static DWORD	s_dwMASK_SHIFT[5];
	static uint64_t	s_qwMASK_SHIFT[5];

	static WORD	s_wMASK_RGB[6];
	static DWORD	s_dwMASK_RGB[6];
	static uint64_t	s_qwMASK_RGB[6];

	static uint64_t	s_qwMASK_ALPHA0;
	static uint64_t	s_qwMASK_ALPHA1;
	static DWORD	s_dwMASK_ALPHA0;
	static DWORD	s_dwMASK_ALPHA1;
	static WORD	s_wMASK_ALPHA0;
	static WORD	s_wMASK_ALPHA1;

	static DWORD	s_dwMASK_SHIFT_COUNT[5];
	static DWORD	s_dwMASK_RGB_COUNT[6];

	//----------------------------------------------------------------------
	// 표준 색상 상수
	//----------------------------------------------------------------------
	static WORD	RED;
	static WORD	GREEN;
	static WORD	BLUE;
	static WORD	WHITE;

	static bool	s_bUseIMEHandle;

	//----------------------------------------------------------------------
	// 초기화
	//----------------------------------------------------------------------
	static void InitMask(bool b565);
};

#endif // __COLORDRAW_H__
