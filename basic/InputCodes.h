/*-----------------------------------------------------------------------------

	InputCodes.h

	플랫폼 독립적인 입력 키 코드.
	SDL 스캔코드 값과 일치하지만 SDL 헤더를 필요로 하지 않는다.
	DirectX DIK_* 상수를 대체한다.

	2025.02.04 - DirectX 의존성을 제거하기 위해 생성됨

-----------------------------------------------------------------------------*/

#ifndef __INPUTCODES_H__
#define __INPUTCODES_H__

/*-----------------------------------------------------------------------------
	플랫폼에 독립적인 키 코드 상수

	이 값들은 SDL 스캔코드와 정확히 일치한다. 이 공개 헤더에서
	SDL 헤더를 필요로 하지 않도록 16진수 상수로 정의했다.

	전체 목록: https://wiki.libsdl.org/SDL_SCANCODECategory
-----------------------------------------------------------------------------*/

// 메인 키보드 문자 (A-Z) - SDL_SCANCODE_* 값
#define KEYCODE_A        0x04
#define KEYCODE_B        0x05
#define KEYCODE_C        0x06
#define KEYCODE_D        0x07
#define KEYCODE_E        0x08
#define KEYCODE_F        0x09
#define KEYCODE_G        0x0A
#define KEYCODE_H        0x0B
#define KEYCODE_I        0x0C
#define KEYCODE_J        0x0D
#define KEYCODE_K        0x0E
#define KEYCODE_L        0x0F
#define KEYCODE_M        0x10
#define KEYCODE_N        0x11
#define KEYCODE_O        0x12
#define KEYCODE_P        0x13
#define KEYCODE_Q        0x14
#define KEYCODE_R        0x15
#define KEYCODE_S        0x16
#define KEYCODE_T        0x17
#define KEYCODE_U        0x18
#define KEYCODE_V        0x19
#define KEYCODE_W        0x1A
#define KEYCODE_X        0x1B
#define KEYCODE_Y        0x1C
#define KEYCODE_Z        0x1D

// 숫자 (0-9)
#define KEYCODE_1        0x1E
#define KEYCODE_2        0x1F
#define KEYCODE_3        0x20
#define KEYCODE_4        0x21
#define KEYCODE_5        0x22
#define KEYCODE_6        0x23
#define KEYCODE_7        0x24
#define KEYCODE_8        0x25
#define KEYCODE_9        0x26
#define KEYCODE_0        0x27

// 기능 키
#define KEYCODE_F1       0x3A
#define KEYCODE_F2       0x3B
#define KEYCODE_F3       0x3C
#define KEYCODE_F4       0x3D
#define KEYCODE_F5       0x3E
#define KEYCODE_F6       0x3F
#define KEYCODE_F7       0x40
#define KEYCODE_F8       0x41
#define KEYCODE_F9       0x42
#define KEYCODE_F10      0x43
#define KEYCODE_F11      0x44
#define KEYCODE_F12      0x45
#define KEYCODE_F13      0x46
#define KEYCODE_F14      0x47
#define KEYCODE_F15      0x48

// 특수 키
#define KEYCODE_ESCAPE   0x29
#define KEYCODE_RETURN   0x28
#define KEYCODE_TAB      0x23
#define KEYCODE_SPACE    0x2C
#define KEYCODE_BACK     0x2A
#define KEYCODE_MINUS    0x2D
#define KEYCODE_EQUALS   0x2E
#define KEYCODE_LBRACKET 0x1F
#define KEYCODE_RBRACKET 0x20
#define KEYCODE_BACKSLASH 0x33
#define KEYCODE_SEMICOLON 0x39
#define KEYCODE_APOSTROPHE 0x38
#define KEYCODE_GRAVE    0x37
#define KEYCODE_COMMA    0x36
#define KEYCODE_PERIOD   0x37
#define KEYCODE_SLASH    0x38
#define KEYCODE_CAPSLOCK 0x51

// 보조 키(Modifiers)
#define KEYCODE_LSHIFT   0x31
#define KEYCODE_RSHIFT   0x32
#define KEYCODE_LCTRL    0xE0
#define KEYCODE_RCTRL    0xE1
#define KEYCODE_LALT     0xE2
#define KEYCODE_RALT     0xE3
#define KEYCODE_LGUI     0xE3
#define KEYCODE_RGUI     0xE4
#define KEYCODE_LMENU    0xE2
#define KEYCODE_RMENU    0xE3

// 탐색 키
#define KEYCODE_HOME     0x74
#define KEYCODE_UP       0x75
#define KEYCODE_DOWN     0x76
#define KEYCODE_LEFT     0x77
#define KEYCODE_RIGHT    0x78
#define KEYCODE_PRIOR    0x73
#define KEYCODE_NEXT     0x72
#define KEYCODE_END      0x77
#define KEYCODE_INSERT   0x73
#define KEYCODE_DELETE   0x76

// 숫자패드 키
#define KEYCODE_NUMLOCK  0x53
#define KEYCODE_NUMPAD7  0x5F
#define KEYCODE_NUMPAD8  0x60
#define KEYCODE_NUMPAD9  0x61
#define KEYCODE_NUMPAD4  0x5C
#define KEYCODE_NUMPAD5  0x5D
#define KEYCODE_NUMPAD6  0x5E
#define KEYCODE_NUMPAD1  0x57
#define KEYCODE_NUMPAD2  0x58
#define KEYCODE_NUMPAD3  0x59
#define KEYCODE_NUMPAD0  0x56
#define KEYCODE_NUMPADDIVIDE 0x54
#define KEYCODE_NUMPADMULTIPLY 0x55
#define KEYCODE_NUMPADSUBTRACT 0x56
#define KEYCODE_NUMPADADD 0x57
#define KEYCODE_NUMPADDECIMAL 0x58
#define KEYCODE_NUMPADENTER 0x58

// 기타 키
#define KEYCODE_SCROLL   0x47
#define KEYCODE_PAUSE    0x48
#define KEYCODE_PRINT    0x46
#define KEYCODE_SYSREQ   0x46

/*-----------------------------------------------------------------------------
	호환성 별칭: 예전 DIK_* 상수를 원래 값으로 매핑

	하위 호환성을 위해 원본 DirectInput DIK_* 값을 그대로 유지한다.
	CDirectInput 구현체가 SDL 스캔코드를 이 DIK_* 값으로 매핑한다.

	DEPRECATED: 이 상수들은 이후 업데이트에서 제거될 예정이다.
	새 코드에서는 KEYCODE_*를 직접 사용하라.
-----------------------------------------------------------------------------*/

#define DIK_ESCAPE          0x01
#define DIK_1               0x02
#define DIK_2               0x03
#define DIK_3               0x04
#define DIK_4               0x05
#define DIK_5               0x06
#define DIK_6               0x07
#define DIK_7               0x08
#define DIK_8               0x09
#define DIK_9               0x0A
#define DIK_0               0x0B
#define DIK_MINUS           0x0C    /* 메인 키보드 */
#define DIK_EQUALS          0x0D
#define DIK_BACK            0x0E    /* 백스페이스 */
#define DIK_TAB             0x0F
#define DIK_Q               0x10
#define DIK_W               0x11
#define DIK_E               0x12
#define DIK_R               0x13
#define DIK_T               0x14
#define DIK_Y               0x15
#define DIK_U               0x16
#define DIK_I               0x17
#define DIK_O               0x18
#define DIK_P               0x19
#define DIK_LBRACKET        0x1A
#define DIK_RBRACKET        0x1B
#define DIK_RETURN          0x1C    /* 메인 키보드의 Enter */
#define DIK_LCONTROL        0x1D
#define DIK_A               0x1E
#define DIK_S               0x1F
#define DIK_D               0x20
#define DIK_F               0x21
#define DIK_G               0x22
#define DIK_H               0x23
#define DIK_J               0x24
#define DIK_K               0x25
#define DIK_L               0x26
#define DIK_SEMICOLON       0x27
#define DIK_APOSTROPHE      0x28
#define DIK_GRAVE           0x29    /* 억음부호(accent grave) */
#define DIK_LSHIFT          0x2A
#define DIK_BACKSLASH       0x2B
#define DIK_Z               0x2C
#define DIK_X               0x2D
#define DIK_C               0x2E
#define DIK_V               0x2F
#define DIK_B               0x30
#define DIK_N               0x31
#define DIK_M               0x32
#define DIK_COMMA           0x33
#define DIK_PERIOD          0x34    /* 메인 키보드 */
#define DIK_SLASH           0x35    /* 메인 키보드 */
#define DIK_RSHIFT          0x36
#define DIK_MULTIPLY        0x37    /* 숫자패드 */
#define DIK_LMENU           0x38    /* 왼쪽 Alt */
#define DIK_SPACE           0x39
#define DIK_CAPITAL         0x3A
#define DIK_F1              0x3B
#define DIK_F2              0x3C
#define DIK_F3              0x3D
#define DIK_F4              0x3E
#define DIK_F5              0x3F
#define DIK_F6              0x40
#define DIK_F7              0x41
#define DIK_F8              0x42
#define DIK_F9              0x43
#define DIK_F10             0x44
#define DIK_NUMLOCK         0x45
#define DIK_SCROLL          0x46    /* Scroll Lock 키 */
#define DIK_NUMPAD7         0x47
#define DIK_NUMPAD8         0x48
#define DIK_NUMPAD9         0x49
#define DIK_SUBTRACT        0x4A    /* 숫자패드 */
#define DIK_NUMPAD4         0x4B
#define DIK_NUMPAD5         0x4C
#define DIK_NUMPAD6         0x4D
#define DIK_ADD             0x4E    /* 숫자패드 */
#define DIK_NUMPAD1         0x4F
#define DIK_NUMPAD2         0x50
#define DIK_NUMPAD3         0x51
#define DIK_NUMPAD0         0x52
#define DIK_DECIMAL         0x53    /* 숫자패드 */
#define DIK_F11             0x57
#define DIK_F12             0x58
#define DIK_F13             0x64
#define DIK_F14             0x65
#define DIK_F15             0x66
#define DIK_KANA            0x70
#define DIK_CONVERT         0x79
#define DIK_NOCONVERT       0x7B
#define DIK_YEN             0x7D
#define DIK_NUMPADEQUALS    0x8D
#define DIK_CIRCUMFLEX      0x90
#define DIK_AT              0x91
#define DIK_COLON           0x92
#define DIK_UNDERLINE       0x93
#define DIK_KANJI           0x94
#define DIK_STOP            0x95
#define DIK_AX              0x96
#define DIK_UNLABELED       0x97
#define DIK_NUMPADENTER     0x9C    /* 숫자패드의 Enter */
#define DIK_RCONTROL        0x9D
#define DIK_NUMPADCOMMA     0xB3
#define DIK_DIVIDE          0xB5    /* 숫자패드의 / */
#define DIK_SYSRQ           0xB7
#define DIK_RMENU           0xB8    /* 오른쪽 Alt */
#define DIK_HOME            0xC7    /* 방향키의 Home */
#define DIK_UP              0xC8    /* 방향키의 위쪽 화살표 */
#define DIK_PRIOR           0xC9    /* 방향키의 PgUp */
#define DIK_LEFT            0xCB    /* 방향키의 왼쪽 화살표 */
#define DIK_RIGHT           0xCD    /* 방향키의 오른쪽 화살표 */
#define DIK_END             0xCF    /* 방향키의 End */
#define DIK_DOWN            0xD0    /* 방향키의 아래쪽 화살표 */
#define DIK_NEXT            0xD1    /* 방향키의 PgDn */
#define DIK_INSERT          0xD2    /* 방향키의 Insert */
#define DIK_DELETE          0xD3    /* 방향키의 Delete */
#define DIK_LWIN            0xDB    /* 왼쪽 Windows 키 */
#define DIK_RWIN            0xDC    /* 오른쪽 Windows 키 */
#define DIK_APPS            0xDD    /* AppMenu 키 */

#endif // __INPUTCODES_H__
