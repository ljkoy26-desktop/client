/*-----------------------------------------------------------------------------

	DXLibBackendSDL.cpp

	DXLib를 위한 SDL2 백엔드 구현.
	크로스플랫폼 지원을 위해 SDL2 기반 입력과 오디오를 제공한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#define DXLIB_BACKEND_SDL_IMPL

/* DIK 상수를 얻기 위해 CDirectInput.h를 가장 먼저 include한다 */
#include "CDirectInput.h"
#include "DXLibBackend.h"

#ifdef DXLIB_BACKEND_SDL

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 입력 포커스 관리자를 include한다 */
#include "../../VS_UI/src/InputFocusManager.h"

/* MP3/OGG 지원을 위함 */
#ifdef SDL_MIXER_MAJOR_VERSION
	#include <SDL_mixer.h>
#endif

/* 대체: DIK 상수가 정의되어 있지 않으면 여기서 정의한다 */
#ifndef DIK_ESCAPE
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
	#define DIK_MINUS           0x0C
	#define DIK_EQUALS          0x0D
	#define DIK_BACK            0x0E
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
	#define DIK_RETURN          0x1C
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
	#define DIK_GRAVE           0x29
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
	#define DIK_PERIOD          0x34
	#define DIK_SLASH           0x35
	#define DIK_RSHIFT          0x36
	#define DIK_MULTIPLY        0x37
	#define DIK_LMENU           0x38
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
	#define DIK_SCROLL          0x46
	#define DIK_NUMPAD7         0x47
	#define DIK_NUMPAD8         0x48
	#define DIK_NUMPAD9         0x49
	#define DIK_SUBTRACT        0x4A
	#define DIK_NUMPAD4         0x4B
	#define DIK_NUMPAD5         0x4C
	#define DIK_NUMPAD6         0x4D
	#define DIK_ADD             0x4E
	#define DIK_NUMPAD1         0x4F
	#define DIK_NUMPAD2         0x50
	#define DIK_NUMPAD3         0x51
	#define DIK_NUMPAD0         0x52
	#define DIK_DECIMAL         0x53
	#define DIK_F11             0x57
	#define DIK_F12             0x58
	#define DIK_F13             0x64
	#define DIK_F14             0x65
	#define DIK_F15             0x66
	#define DIK_KANA            0x70
	#define DIK_ABNT_C1         0x73
	#define DIK_CONVERT         0x79
	#define DIK_NOCONVERT       0x7B
	#define DIK_YEN             0x7D
	#define DIK_ABNT_C2         0x7E
	#define DIK_NUMPADEQUALS    0x8D
	#define DIK_PREVTRACK       0x90
	#define DIK_AT              0x91
	#define DIK_COLON           0x92
	#define DIK_UNDERLINE       0x93
	#define DIK_KANJI           0x94
	#define DIK_STOP            0x95
	#define DIK_AX              0x96
	#define DIK_UNLABELED       0x97
	#define DIK_NEXTTRACK       0x99
	#define DIK_NUMPADENTER     0x9C
	#define DIK_RCONTROL        0x9D
	#define DIK_MUTE            0xA0
	#define DIK_CALCULATOR      0xA1
	#define DIK_PLAYPAUSE       0xA2
	#define DIK_MEDIASTOP       0xA4
	#define DIK_VOLUMEDOWN      0xAE
	#define DIK_VOLUMEUP        0xB0
	#define DIK_WEBHOME         0xB2
	#define DIK_NUMPADCOMMA     0xB3
	#define DIK_DIVIDE          0xB5
	#define DIK_SYSRQ           0xB7
	#define DIK_RMENU           0xB8
	#define DIK_PAUSE           0xC5
	#define DIK_HOME            0xC7
	#define DIK_UP              0xC8
	#define DIK_PRIOR           0xC9
	#define DIK_LEFT            0xCB
	#define DIK_RIGHT           0xCD
	#define DIK_END             0xCF
	#define DIK_DOWN            0xD0
	#define DIK_NEXT            0xD1
	#define DIK_INSERT          0xD2
	#define DIK_DELETE          0xD3
	#define DIK_LWIN            0xDB
	#define DIK_RWIN            0xDC
	#define DIK_APPS            0xDD
	/* ALT 키 별칭 (DirectInput은 LMENU/RMENU를 사용) */
	#define DIK_LALT            DIK_LMENU
	#define DIK_RALT            DIK_RMENU
#endif

/* ============================================================================
 * 내부 상태
 * ============================================================================ */

static int g_input_initialized = 0;
static int g_sound_initialized = 0;
static int g_music_initialized = 0;
static int g_stream_initialized = 0;

/* 입력 상태 */
static Uint8 g_key_state[SDL_NUM_SCANCODES];
static int g_mouse_x = 0;
static int g_mouse_y = 0;
static int g_mouse_wheel = 0;
static int g_mouse_buttons[3] = {0, 0, 0};

/* 텍스트 입력 콜백 */
static dxlib_textinput_callback g_textinput_callback = NULL;

/* 텍스트 편집 콜백 */
static dxlib_textediting_callback g_textediting_callback = NULL;

/* 레거시 전역 마우스 좌표 (CWaitUIUpdate에서 사용) */
extern int g_x, g_y;

/* 전역 게임 상태. Windows가 아닌 환경에서는 이를 읽는 메인 루프가 있는
   SDLMain.cpp에서 정의된다; SDLMain.cpp는 Windows에서는 컴파일 대상에서
   제외되므로(macOS/Linux 진입점이기 때문) 대신 여기서 정의한다 - 이
   파일(dxlib)은 모든 플랫폼에서 빌드되며, PLATFORM_WINDOWS를 위해
   이를 제공하는 다른 곳이 없다. */
#ifdef PLATFORM_WINDOWS
bool g_bRunning = true;
#else
extern bool g_bRunning;
#endif
extern BOOL g_bActiveApp;

/* DIK를 SDL 스캔코드로 매핑하는 테이블 */
static SDL_Scancode g_dik_to_scancode[256] = {SDL_SCANCODE_UNKNOWN};

/* ============================================================================
 * 입력 백엔드 구현
 * ============================================================================ */

static void init_key_mapping(void) {
	/* DirectInput 키 코드를 SDL 스캔코드로 매핑하도록 초기화한다 */
	/* 알파벳 */
	g_dik_to_scancode[DIK_A] = SDL_SCANCODE_A;
	g_dik_to_scancode[DIK_B] = SDL_SCANCODE_B;
	g_dik_to_scancode[DIK_C] = SDL_SCANCODE_C;
	g_dik_to_scancode[DIK_D] = SDL_SCANCODE_D;
	g_dik_to_scancode[DIK_E] = SDL_SCANCODE_E;
	g_dik_to_scancode[DIK_F] = SDL_SCANCODE_F;
	g_dik_to_scancode[DIK_G] = SDL_SCANCODE_G;
	g_dik_to_scancode[DIK_H] = SDL_SCANCODE_H;
	g_dik_to_scancode[DIK_I] = SDL_SCANCODE_I;
	g_dik_to_scancode[DIK_J] = SDL_SCANCODE_J;
	g_dik_to_scancode[DIK_K] = SDL_SCANCODE_K;
	g_dik_to_scancode[DIK_L] = SDL_SCANCODE_L;
	g_dik_to_scancode[DIK_M] = SDL_SCANCODE_M;
	g_dik_to_scancode[DIK_N] = SDL_SCANCODE_N;
	g_dik_to_scancode[DIK_O] = SDL_SCANCODE_O;
	g_dik_to_scancode[DIK_P] = SDL_SCANCODE_P;
	g_dik_to_scancode[DIK_Q] = SDL_SCANCODE_Q;
	g_dik_to_scancode[DIK_R] = SDL_SCANCODE_R;
	g_dik_to_scancode[DIK_S] = SDL_SCANCODE_S;
	g_dik_to_scancode[DIK_T] = SDL_SCANCODE_T;
	g_dik_to_scancode[DIK_U] = SDL_SCANCODE_U;
	g_dik_to_scancode[DIK_V] = SDL_SCANCODE_V;
	g_dik_to_scancode[DIK_W] = SDL_SCANCODE_W;
	g_dik_to_scancode[DIK_X] = SDL_SCANCODE_X;
	g_dik_to_scancode[DIK_Y] = SDL_SCANCODE_Y;
	g_dik_to_scancode[DIK_Z] = SDL_SCANCODE_Z;

	/* 숫자 */
	g_dik_to_scancode[DIK_0] = SDL_SCANCODE_0;
	g_dik_to_scancode[DIK_1] = SDL_SCANCODE_1;
	g_dik_to_scancode[DIK_2] = SDL_SCANCODE_2;
	g_dik_to_scancode[DIK_3] = SDL_SCANCODE_3;
	g_dik_to_scancode[DIK_4] = SDL_SCANCODE_4;
	g_dik_to_scancode[DIK_5] = SDL_SCANCODE_5;
	g_dik_to_scancode[DIK_6] = SDL_SCANCODE_6;
	g_dik_to_scancode[DIK_7] = SDL_SCANCODE_7;
	g_dik_to_scancode[DIK_8] = SDL_SCANCODE_8;
	g_dik_to_scancode[DIK_9] = SDL_SCANCODE_9;

	/* 기능 키 */
	g_dik_to_scancode[DIK_F1] = SDL_SCANCODE_F1;
	g_dik_to_scancode[DIK_F2] = SDL_SCANCODE_F2;
	g_dik_to_scancode[DIK_F3] = SDL_SCANCODE_F3;
	g_dik_to_scancode[DIK_F4] = SDL_SCANCODE_F4;
	g_dik_to_scancode[DIK_F5] = SDL_SCANCODE_F5;
	g_dik_to_scancode[DIK_F6] = SDL_SCANCODE_F6;
	g_dik_to_scancode[DIK_F7] = SDL_SCANCODE_F7;
	g_dik_to_scancode[DIK_F8] = SDL_SCANCODE_F8;
	g_dik_to_scancode[DIK_F9] = SDL_SCANCODE_F9;
	g_dik_to_scancode[DIK_F10] = SDL_SCANCODE_F10;
	g_dik_to_scancode[DIK_F11] = SDL_SCANCODE_F11;
	g_dik_to_scancode[DIK_F12] = SDL_SCANCODE_F12;

	/* 특수 키 */
	g_dik_to_scancode[DIK_ESCAPE] = SDL_SCANCODE_ESCAPE;
	g_dik_to_scancode[DIK_TAB] = SDL_SCANCODE_TAB;
	g_dik_to_scancode[DIK_RETURN] = SDL_SCANCODE_RETURN;
	g_dik_to_scancode[DIK_SPACE] = SDL_SCANCODE_SPACE;
	g_dik_to_scancode[DIK_BACK] = SDL_SCANCODE_BACKSPACE;
	g_dik_to_scancode[DIK_LSHIFT] = SDL_SCANCODE_LSHIFT;
	g_dik_to_scancode[DIK_RSHIFT] = SDL_SCANCODE_RSHIFT;
	g_dik_to_scancode[DIK_LCONTROL] = SDL_SCANCODE_LCTRL;
	g_dik_to_scancode[DIK_RCONTROL] = SDL_SCANCODE_RCTRL;
	g_dik_to_scancode[DIK_LMENU] = SDL_SCANCODE_LALT;
	g_dik_to_scancode[DIK_RMENU] = SDL_SCANCODE_RALT;
	g_dik_to_scancode[DIK_CAPITAL] = SDL_SCANCODE_CAPSLOCK;
	g_dik_to_scancode[DIK_NUMLOCK] = SDL_SCANCODE_NUMLOCKCLEAR;
	g_dik_to_scancode[DIK_SCROLL] = SDL_SCANCODE_SCROLLLOCK;

	/* 방향키 */
	g_dik_to_scancode[DIK_UP] = SDL_SCANCODE_UP;
	g_dik_to_scancode[DIK_DOWN] = SDL_SCANCODE_DOWN;
	g_dik_to_scancode[DIK_LEFT] = SDL_SCANCODE_LEFT;
	g_dik_to_scancode[DIK_RIGHT] = SDL_SCANCODE_RIGHT;
	g_dik_to_scancode[DIK_HOME] = SDL_SCANCODE_HOME;
	g_dik_to_scancode[DIK_END] = SDL_SCANCODE_END;
	g_dik_to_scancode[DIK_PRIOR] = SDL_SCANCODE_PAGEUP;
	g_dik_to_scancode[DIK_NEXT] = SDL_SCANCODE_PAGEDOWN;
	g_dik_to_scancode[DIK_INSERT] = SDL_SCANCODE_INSERT;
	g_dik_to_scancode[DIK_DELETE] = SDL_SCANCODE_DELETE;

	/* 기호 */
	g_dik_to_scancode[DIK_EQUALS] = SDL_SCANCODE_EQUALS;
	g_dik_to_scancode[DIK_MINUS] = SDL_SCANCODE_MINUS;
	g_dik_to_scancode[DIK_LBRACKET] = SDL_SCANCODE_LEFTBRACKET;
	g_dik_to_scancode[DIK_RBRACKET] = SDL_SCANCODE_RIGHTBRACKET;
	g_dik_to_scancode[DIK_SEMICOLON] = SDL_SCANCODE_SEMICOLON;
	g_dik_to_scancode[DIK_APOSTROPHE] = SDL_SCANCODE_APOSTROPHE;
	g_dik_to_scancode[DIK_GRAVE] = SDL_SCANCODE_GRAVE;
	g_dik_to_scancode[DIK_BACKSLASH] = SDL_SCANCODE_BACKSLASH;
	g_dik_to_scancode[DIK_COMMA] = SDL_SCANCODE_COMMA;
	g_dik_to_scancode[DIK_PERIOD] = SDL_SCANCODE_PERIOD;
	g_dik_to_scancode[DIK_SLASH] = SDL_SCANCODE_SLASH;

	/* 숫자패드 */
	g_dik_to_scancode[DIK_NUMPAD0] = SDL_SCANCODE_KP_0;
	g_dik_to_scancode[DIK_NUMPAD1] = SDL_SCANCODE_KP_1;
	g_dik_to_scancode[DIK_NUMPAD2] = SDL_SCANCODE_KP_2;
	g_dik_to_scancode[DIK_NUMPAD3] = SDL_SCANCODE_KP_3;
	g_dik_to_scancode[DIK_NUMPAD4] = SDL_SCANCODE_KP_4;
	g_dik_to_scancode[DIK_NUMPAD5] = SDL_SCANCODE_KP_5;
	g_dik_to_scancode[DIK_NUMPAD6] = SDL_SCANCODE_KP_6;
	g_dik_to_scancode[DIK_NUMPAD7] = SDL_SCANCODE_KP_7;
	g_dik_to_scancode[DIK_NUMPAD8] = SDL_SCANCODE_KP_8;
	g_dik_to_scancode[DIK_NUMPAD9] = SDL_SCANCODE_KP_9;
	g_dik_to_scancode[DIK_NUMPADENTER] = SDL_SCANCODE_KP_ENTER;
	g_dik_to_scancode[DIK_ADD] = SDL_SCANCODE_KP_PLUS;
	g_dik_to_scancode[DIK_SUBTRACT] = SDL_SCANCODE_KP_MINUS;
	g_dik_to_scancode[DIK_MULTIPLY] = SDL_SCANCODE_KP_MULTIPLY;
	g_dik_to_scancode[DIK_DIVIDE] = SDL_SCANCODE_KP_DIVIDE;
	g_dik_to_scancode[DIK_DECIMAL] = SDL_SCANCODE_KP_PERIOD;
}

int dxlib_input_init(void* window_handle) {
	if (g_input_initialized) return 0;

	/* SDL 서브시스템을 초기화한다 (아직 초기화되지 않았다면) */
	if (SDL_WasInit(0) == 0) {
		if (SDL_Init(0) < 0) {
			fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
			return 1;
		}
	}

	/* 키 매핑을 초기화한다 */
	init_key_mapping();

	/* 키보드 상태를 초기화한다 */
	int num_keys;
	const Uint8* state = SDL_GetKeyboardState(&num_keys);
	memcpy(g_key_state, state, sizeof(g_key_state));

	g_input_initialized = 1;
	return 0;
}

void dxlib_input_release(void) {
	g_input_initialized = 0;
}

void dxlib_input_update(void) {
	if (!g_input_initialized) {
		static int notInitCount = 0;
		if (++notInitCount <= 3) {
			printf("WARNING: dxlib_input_update called but not initialized!\n");
			fflush(stdout);
		}
		return;
	}

	/* SDL 이벤트를 갱신한다 */
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				g_bRunning = false;
				break;

			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
					g_bActiveApp = TRUE;
				} else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
					// 비활성화하지 않는다 - 백그라운드에서도 게임을 계속 실행한다
					// g_bActiveApp = FALSE;
				}
				break;

			case SDL_KEYDOWN:
				/* 텍스트 입력을 위한 제어 키를 처리한다 */
				if (g_GetInputFocusManager().HasFocus()) {
					SDL_Keycode key = event.key.keysym.sym;
					unsigned int vk_code = 0;

					// SDL 키 코드를 Windows 가상 키 코드로 매핑한다
					switch (key) {
					case SDLK_BACKSPACE:	vk_code = 0x08; break; // VK_BACK
					case SDLK_TAB:		vk_code = 0x09; break; // VK_TAB
					case SDLK_RETURN:	vk_code = 0x0D; break; // VK_RETURN
					case SDLK_ESCAPE:	vk_code = 0x1B; break; // VK_ESCAPE
					case SDLK_LEFT:		vk_code = 0x25; break; // VK_LEFT
					case SDLK_UP:		vk_code = 0x26; break; // VK_UP
					case SDLK_RIGHT:	vk_code = 0x27; break; // VK_RIGHT
					case SDLK_DOWN:		vk_code = 0x28; break; // VK_DOWN
					case SDLK_DELETE:	vk_code = 0x2E; break; // VK_DELETE
					case SDLK_HOME:		vk_code = 0x24; break; // VK_HOME
					case SDLK_END:		vk_code = 0x23; break; // VK_END
					}

					if (vk_code != 0) {
						g_GetInputFocusManager().HandleKeyDown(vk_code);
						// 여기서 break하지 않는다 - 아래에서 키보드 상태가 갱신되도록 한다
						// 이는 dxlib_input_key_down()이 올바르게 동작하도록 보장한다
					}
				}
				/* 키보드 상태 갱신으로 fall through한다 */
				/* 중요: InputFocusManager가 포커스를 가지고 있어도 키보드 상태는 반드시 갱신되어야 한다 */

			case SDL_KEYUP:
				/* 키보드 상태는 아래의 SDL_GetKeyboardState에 의해 갱신된다 */
				/* 추가 처리가 필요 없다 */
				break;

			case SDL_MOUSEMOTION:
				g_mouse_x = event.motion.x;
				g_mouse_y = event.motion.y;
				// 레거시 코드를 위해 전역 g_x, g_y도 갱신한다
				g_x = event.motion.x;
				g_y = event.motion.y;
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				// 레거시 코드를 위해 전역 g_x, g_y도 갱신한다
				g_x = event.button.x;
				g_y = event.button.y;

				if (event.button.button == SDL_BUTTON_LEFT) {
					g_mouse_buttons[0] = (event.type == SDL_MOUSEBUTTONDOWN) ? 1 : 0;
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					g_mouse_buttons[1] = (event.type == SDL_MOUSEBUTTONDOWN) ? 1 : 0;
				} else if (event.button.button == SDL_BUTTON_MIDDLE) {
					g_mouse_buttons[2] = (event.type == SDL_MOUSEBUTTONDOWN) ? 1 : 0;
				}
				break;

			case SDL_MOUSEWHEEL:
				g_mouse_wheel += event.wheel.y;
				break;

			case SDL_TEXTINPUT:
				/* IME 및 텍스트 입력을 처리한다 */
				{
					// 콜백 대신 InputFocusManager로 전달한다
					if (event.text.text[0] != '\0') {
						g_GetInputFocusManager().HandleTextInput(event.text.text);
					}
				}
				break;

			case SDL_TEXTEDITING:
				/* IME 조합을 처리한다 (텍스트 편집이 진행 중) */
				{
					// 콜백 대신 InputFocusManager로 전달한다
					g_GetInputFocusManager().HandleTextEditing(event.edit.text,
					                                         event.edit.start,
					                                         event.edit.length);
				}
				break;
		}
	}

	/* 키보드 상태를 갱신한다 */
	const Uint8* state = SDL_GetKeyboardState(NULL);

	/* 이벤트가 없을 경우를 대비해 SDL에서 마우스 위치를 갱신한다 */
	SDL_GetMouseState(&g_mouse_x, &g_mouse_y);

	/* 레거시 코드를 위해 전역 g_x, g_y도 갱신한다 (중요!) */
	g_x = g_mouse_x;
	g_y = g_mouse_y;
}

int dxlib_input_key_down(int dik_key) {
	if (!g_input_initialized) return 0;

	if (dik_key < 0 || dik_key >= 256) return 0;

	SDL_Scancode scancode = g_dik_to_scancode[dik_key];
	if (scancode == SDL_SCANCODE_UNKNOWN) return 0;

	const Uint8* state = SDL_GetKeyboardState(NULL);
	return state[scancode] ? 1 : 0;
}

void dxlib_input_get_mouse_pos(int* x, int* y) {
	if (x) *x = g_mouse_x;
	if (y) *y = g_mouse_y;
}

int dxlib_input_get_mouse_wheel(void) {
	return g_mouse_wheel;
}

void dxlib_input_get_mouse_buttons(int* left, int* right, int* center) {
	if (left) *left = g_mouse_buttons[0];
	if (right) *right = g_mouse_buttons[1];
	if (center) *center = g_mouse_buttons[2];
}

void dxlib_input_set_mouse_pos(int x, int y) {
	SDL_WarpMouseInWindow(NULL, x, y);
}

void dxlib_input_set_textinput_callback(dxlib_textinput_callback callback) {
	g_textinput_callback = callback;
}

void dxlib_input_set_textediting_callback(dxlib_textediting_callback callback) {
	g_textediting_callback = callback;
}

void dxlib_input_start_text(void) {
	SDL_StartTextInput();
}

void dxlib_input_stop_text(void) {
	SDL_StopTextInput();
}

/* ============================================================================
 * 사운드 백엔드 구현 (SDL_mixer)
 * ============================================================================ */

#ifdef SDL_MIXER_MAJOR_VERSION

struct dxlib_sound_buffer {
	Mix_Chunk* chunk;
	int channel;
	int volume;
	int pan;
	int playing;
};

static int g_max_volume = 100;

int dxlib_sound_init(void* window_handle) {
	if (g_sound_initialized) return 0;

	/* SDL_mixer를 초기화한다 */
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
		return 1;
	}

	/* 채널을 할당한다 */
	Mix_AllocateChannels(32);

	g_sound_initialized = 1;
	return 0;
}

void dxlib_sound_release(void) {
	if (!g_sound_initialized) return;

	Mix_CloseAudio();
	g_sound_initialized = 0;
}

dxlib_sound_t dxlib_sound_load_wav(const char* filename) {
	if (!g_sound_initialized) return NULL;

	Mix_Chunk* chunk = Mix_LoadWAV(filename);
	if (!chunk) {
		fprintf(stderr, "Failed to load %s: %s\n", filename, Mix_GetError());
		return NULL;
	}

	struct dxlib_sound_buffer* sound = (struct dxlib_sound_buffer*)malloc(sizeof(struct dxlib_sound_buffer));
	if (!sound) {
		Mix_FreeChunk(chunk);
		return NULL;
	}

	sound->chunk = chunk;
	sound->channel = -1;
	sound->volume = 100;
	sound->pan = 0;
	sound->playing = 0;

	return sound;
}

dxlib_sound_t dxlib_sound_create_buffer(const void* data, int size,
                                       int channels, int sample_rate,
                                       int bits_per_sample) {
	if (!g_sound_initialized) return NULL;

	/* 원본 데이터를 SDL_RWops로 변환한다 */
	SDL_RWops* rw = SDL_RWFromConstMem(data, size);
	if (!rw) return NULL;

	Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 1); /* 1 = 자동 해제 */
	if (!chunk) return NULL;

	struct dxlib_sound_buffer* sound = (struct dxlib_sound_buffer*)malloc(sizeof(struct dxlib_sound_buffer));
	if (!sound) {
		Mix_FreeChunk(chunk);
		return NULL;
	}

	sound->chunk = chunk;
	sound->channel = -1;
	sound->volume = 100;
	sound->pan = 0;
	sound->playing = 0;

	return sound;
}

void dxlib_sound_free(dxlib_sound_t sound) {
	if (!sound) return;

	if (sound->playing) {
		Mix_HaltChannel(sound->channel);
	}

	Mix_FreeChunk(sound->chunk);
	free(sound);
}

int dxlib_sound_play(dxlib_sound_t sound, int loop) {
	if (!sound || !sound->chunk) return 1;

	int loops = loop ? -1 : 0; /* -1 = 무한 반복 */
	sound->channel = Mix_PlayChannel(-1, sound->chunk, loops);
	sound->playing = (sound->channel >= 0);

	return sound->playing ? 0 : 1;
}

int dxlib_sound_stop(dxlib_sound_t sound) {
	if (!sound) return 1;

	if (sound->channel >= 0) {
		Mix_HaltChannel(sound->channel);
		sound->channel = -1;
		sound->playing = 0;
	}

	return 0;
}

int dxlib_sound_is_playing(dxlib_sound_t sound) {
	if (!sound) return 0;
	return sound->playing && (sound->channel >= 0) && Mix_Playing(sound->channel);
}

int dxlib_sound_set_volume(dxlib_sound_t sound, int volume) {
	if (!sound) return 1;

	sound->volume = volume;
	int mix_volume = (volume * 128) / 100; /* SDL_mixer 범위로 변환한다 */

	if (sound->channel >= 0) {
		Mix_Volume(sound->channel, mix_volume);
	}

	return 0;
}

int dxlib_sound_set_pan(dxlib_sound_t sound, int pan) {
	if (!sound) return 1;

	/* SDL_mixer는 채널에서 직접적인 패닝을 지원하지 않는다 */
	/* 이를 위해서는 Mix_SetPosition을 사용해야 하는데(항상 사용 가능한 것은 아님) */
	/* 지금은 팬 값만 저장해 둔다 */
	sound->pan = pan;

	return 0;
}

int dxlib_sound_set_frequency(dxlib_sound_t sound, int frequency) {
	/* SDL_mixer는 주파수 변경을 지원하지 않는다 */
	/* 이를 위해서는 리샘플링된 데이터로 청크를 다시 생성해야 한다 */
	return 1;
}

dxlib_sound_t dxlib_sound_duplicate(dxlib_sound_t sound) {
	if (!sound) return NULL;

	struct dxlib_sound_buffer* duplicate = (struct dxlib_sound_buffer*)malloc(sizeof(struct dxlib_sound_buffer));
	if (!duplicate) return NULL;

	/* 동일한 청크를 참조한다 */
	duplicate->chunk = sound->chunk;
	duplicate->channel = -1;
	duplicate->volume = sound->volume;
	duplicate->pan = sound->pan;
	duplicate->playing = 0;

	return duplicate;
}

#else /* !SDL_MIXER_MAJOR_VERSION */

/* SDL_mixer를 사용할 수 없음 - 스텁 구현 */
int dxlib_sound_init(void* window_handle) { return 1; }
void dxlib_sound_release(void) {}
dxlib_sound_t dxlib_sound_load_wav(const char* filename) { return NULL; }
void dxlib_sound_free(dxlib_sound_t sound) {}
int dxlib_sound_play(dxlib_sound_t sound, int loop) { return 1; }
int dxlib_sound_stop(dxlib_sound_t sound) { return 1; }
int dxlib_sound_is_playing(dxlib_sound_t sound) { return 0; }
int dxlib_sound_set_volume(dxlib_sound_t sound, int volume) { return 1; }
int dxlib_sound_set_pan(dxlib_sound_t sound, int pan) { return 1; }
int dxlib_sound_set_frequency(dxlib_sound_t sound, int frequency) { return 1; }
dxlib_sound_t dxlib_sound_duplicate(dxlib_sound_t sound) { return NULL; }

#endif /* SDL_MIXER_MAJOR_VERSION */

/* ============================================================================
 * 음악 백엔드 구현 (SDL_mixer)
 * ============================================================================ */

#ifdef SDL_MIXER_MAJOR_VERSION

static Mix_Music* g_current_music = NULL;
static int g_music_playing = 0;
static int g_music_paused = 0;
static int g_music_volume = 100;

int dxlib_music_init(void* window_handle) {
	if (g_music_initialized) return 0;

	/* SDL_mixer는 dxlib_sound_init에 의해 이미 초기화되어 있어야 한다 */
	if (!g_sound_initialized) {
		if (dxlib_sound_init(window_handle) != 0) {
			return 1;
		}
	}

	g_music_initialized = 1;
	return 0;
}

void dxlib_music_release(void) {
	if (!g_music_initialized) return;

	if (g_current_music) {
		dxlib_music_free();
	}

	g_music_initialized = 0;
}

int dxlib_music_load(const char* filename) {
	if (!g_music_initialized) return 1;

	/* 이전 음악을 해제한다 */
	if (g_current_music) {
		Mix_FreeMusic(g_current_music);
		g_current_music = NULL;
	}

	g_current_music = Mix_LoadMUS(filename);
	if (!g_current_music) {
		fprintf(stderr, "Failed to load music %s: %s\n", filename, Mix_GetError());
		return 1;
	}

	return 0;
}

void dxlib_music_free(void) {
	if (g_current_music) {
		Mix_FreeMusic(g_current_music);
		g_current_music = NULL;
	}

	g_music_playing = 0;
	g_music_paused = 0;
}

int dxlib_music_play(int loop) {
	if (!g_current_music) return 1;

	int loops = loop ? -1 : 0; /* -1 = 무한 반복 */
	if (Mix_PlayMusic(g_current_music, loops) < 0) {
		return 1;
	}

	g_music_playing = 1;
	g_music_paused = 0;

	return 0;
}

void dxlib_music_stop(void) {
	Mix_HaltMusic();
	g_music_playing = 0;
	g_music_paused = 0;
}

void dxlib_music_pause(void) {
	if (g_music_playing && !g_music_paused) {
		Mix_PauseMusic();
		g_music_paused = 1;
	}
}

void dxlib_music_resume(void) {
	if (g_music_playing && g_music_paused) {
		Mix_ResumeMusic();
		g_music_paused = 0;
	}
}

int dxlib_music_is_playing(void) {
	return g_music_playing && !g_music_paused && Mix_PlayingMusic();
}

int dxlib_music_is_paused(void) {
	return g_music_paused;
}

int dxlib_music_set_volume(int volume) {
	g_music_volume = volume;
	int mix_volume = (volume * 128) / 100;
	Mix_VolumeMusic(mix_volume);
	return 0;
}

int dxlib_music_set_tempo(float tempo) {
	/* SDL_mixer는 템포 변경을 지원하지 않는다 */
	return 1;
}

#else /* !SDL_MIXER_MAJOR_VERSION */

/* SDL_mixer를 사용할 수 없음 - 스텁 구현 */
int dxlib_music_init(void* window_handle) { return 1; }
void dxlib_music_release(void) {}
int dxlib_music_load(const char* filename) { return 1; }
void dxlib_music_free(void) {}
int dxlib_music_play(int loop) { return 1; }
void dxlib_music_stop(void) {}
void dxlib_music_pause(void) {}
void dxlib_music_resume(void) {}
int dxlib_music_is_playing(void) { return 0; }
int dxlib_music_is_paused(void) { return 0; }
int dxlib_music_set_volume(int volume) { return 1; }
int dxlib_music_set_tempo(float tempo) { return 1; }

#endif /* SDL_MIXER_MAJOR_VERSION */

/* ============================================================================
 * 스트림 백엔드 (음악 백엔드를 사용)
 * ============================================================================ */

int dxlib_stream_init(void* window_handle) {
	return dxlib_music_init(window_handle);
}

void dxlib_stream_release(void) {
	dxlib_music_release();
}

dxlib_stream_t dxlib_stream_load(const char* filename) {
	/* 단순화를 위해 스트림은 음악 백엔드를 사용한다 */
	return (dxlib_stream_t)1; /* null이 아닌 값 */
}

void dxlib_stream_free(dxlib_stream_t stream) {
	dxlib_music_free();
}

int dxlib_stream_play(dxlib_stream_t stream, int loop) {
	return dxlib_music_play(loop);
}

void dxlib_stream_stop(dxlib_stream_t stream) {
	dxlib_music_stop();
}

int dxlib_stream_update(dxlib_stream_t stream) {
	/* SDL_mixer가 스트리밍을 자동으로 처리한다 */
	return 0;
}

int dxlib_stream_set_volume(dxlib_stream_t stream, int volume) {
	return dxlib_music_set_volume(volume);
}

int dxlib_stream_is_playing(dxlib_stream_t stream) {
	return dxlib_music_is_playing();
}

/* ============================================================================
 * 백엔드 정보
 * ============================================================================ */

const char* dxlib_get_backend_name(void) {
	return "SDL2";
}

int dxlib_get_capabilities(void) {
	int caps = 0;

	#ifdef SDL_MIXER_MAJOR_VERSION
		caps |= DXLIB_CAP_SOUND | DXLIB_CAP_MUSIC | DXLIB_CAP_STREAM | DXLIB_CAP_MP3 | DXLIB_CAP_OGG;
	#endif

	caps |= DXLIB_CAP_INPUT;

	return caps;
}

#endif /* DXLIB_BACKEND_SDL */
