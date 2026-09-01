/*-----------------------------------------------------------------------------

	ColorDraw.cpp

	ColorDraw 클래스의 구현 파일.

	2025.02.04 - SpriteLib->DXLib 의존성을 제거하기 위해 CDirectDraw에서 분리됨

-----------------------------------------------------------------------------*/

#include "ColorDraw.h"

//----------------------------------------------------------------------
// 정적 멤버 정의 (CDirectDraw와 동일)
//----------------------------------------------------------------------
BYTE ColorDraw::s_bSHIFT_R = 11;
BYTE ColorDraw::s_bSHIFT_G = 5;
BYTE ColorDraw::s_bSHIFT_B = 0;
BYTE ColorDraw::s_bSHIFT_A = 0;

// ARGB 4:4:4:4 포맷을 위함
BYTE ColorDraw::s_bSHIFT4_R = 12;
BYTE ColorDraw::s_bSHIFT4_G = 8;
BYTE ColorDraw::s_bSHIFT4_B = 4;
BYTE ColorDraw::s_bSHIFT4_A = 0;

WORD ColorDraw::s_wMASK_SHIFT[5] = {0, 0, 0, 0, 0};
DWORD ColorDraw::s_dwMASK_SHIFT[5] = {0, 0, 0, 0, 0};
uint64_t ColorDraw::s_qwMASK_SHIFT[5] = {0, 0, 0, 0, 0};

WORD ColorDraw::s_wMASK_RGB[6] = {0, 0, 0, 0, 0, 0};
DWORD ColorDraw::s_dwMASK_RGB[6] = {0, 0, 0, 0, 0, 0};
uint64_t ColorDraw::s_qwMASK_RGB[6] = {0, 0, 0, 0, 0, 0};

uint64_t ColorDraw::s_qwMASK_ALPHA0 = 0;
uint64_t ColorDraw::s_qwMASK_ALPHA1 = 0;
DWORD ColorDraw::s_dwMASK_ALPHA0 = 0;
DWORD ColorDraw::s_dwMASK_ALPHA1 = 0;
WORD ColorDraw::s_wMASK_ALPHA0 = 0;
WORD ColorDraw::s_wMASK_ALPHA1 = 0;

DWORD ColorDraw::s_dwMASK_SHIFT_COUNT[5] = {0, 0, 0, 0, 0};
DWORD ColorDraw::s_dwMASK_RGB_COUNT[6] = {0, 0, 0, 0, 0, 0};

WORD ColorDraw::RED = 0xF800;
WORD ColorDraw::GREEN = 0x07E0;
WORD ColorDraw::BLUE = 0x001F;
WORD ColorDraw::WHITE = 0xFFFF;

bool ColorDraw::s_bUseIMEHandle = false;

//----------------------------------------------------------------------
// InitMask - RGB 5:6:5 포맷을 위한 색상 마스크를 초기화한다
// CSDLGraphics::InitMask와 동일한 로직이다
//----------------------------------------------------------------------
void ColorDraw::InitMask(bool b565)
{
	// SDL2 RGB 5:6:5 포맷을 위함
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

	// 중요: 이 값들은 RGB 5:6:5 포맷을 위해 반드시 (11, 5, 0)을 유지해야 한다
	s_bSHIFT_R = 11;
	s_bSHIFT_G = 5;
	s_bSHIFT_B = 0;
	s_bSHIFT_A = 0;

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
