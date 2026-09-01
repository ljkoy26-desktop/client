/*-----------------------------------------------------------------------------

	DXLibBackend.h

	DXLib 플랫폼 백엔드 추상화.
	Windows DirectInput/DirectSound와 SDL 백엔드 양쪽에 대한 인터페이스를 정의한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#ifndef __DXLIB_BACKEND_H__
#define __DXLIB_BACKEND_H__

#include "../basic/Platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 백엔드 선택
 * ============================================================================ */

/* 백엔드 종류 선택 */
#ifdef PLATFORM_WINDOWS
	#ifndef DXLIB_USE_SDL_BACKEND
		#define DXLIB_BACKEND_WINDOWS  /* 네이티브 Windows API를 사용한다 */
	#else
		#define DXLIB_BACKEND_SDL      /* SDL2를 사용한다 */
	#endif
#else
	/* Windows가 아닌 플랫폼은 반드시 SDL 백엔드를 사용해야 한다 */
	#define DXLIB_BACKEND_SDL
#endif

/* ============================================================================
 * 입력 백엔드 인터페이스
 * ============================================================================ */

/**
 * 입력 백엔드를 초기화한다
 * @param window_handle 네이티브 창 핸들 (Windows에서는 HWND, SDL에서는 SDL_Window*)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_input_init(void* window_handle);

/**
 * 입력 백엔드를 해제한다
 */
void dxlib_input_release(void);

/**
 * 입력 상태를 갱신한다 (새 이벤트를 폴링한다)
 * 프레임마다 한 번씩 호출한다
 */
void dxlib_input_update(void);

/**
 * 키가 눌려 있는지 확인한다
 * @param dik_key DirectInput 키 코드 (DIK_*)
 * @return 눌려 있으면 1, 아니면 0
 */
int dxlib_input_key_down(int dik_key);

/**
 * 마우스 위치를 가져온다
 * @param x 출력 X 좌표
 * @param y 출력 Y 좌표
 */
void dxlib_input_get_mouse_pos(int* x, int* y);

/**
 * 마우스 휠 위치를 가져온다
 * @return 휠 위치 (z 좌표)
 */
int dxlib_input_get_mouse_wheel(void);

/**
 * 마우스 버튼 상태를 확인한다
 * @param left 출력: 왼쪽 버튼 상태 (1=눌림, 0=뗌)
 * @param right 출력: 오른쪽 버튼 상태
 * @param center 출력: 가운데 버튼 상태
 */
void dxlib_input_get_mouse_buttons(int* left, int* right, int* center);

/**
 * 마우스 위치를 설정한다 (상대 이동을 위함)
 * @param x X 좌표
 * @param y Y 좌표
 */
void dxlib_input_set_mouse_pos(int x, int y);

/**
 * 텍스트 입력 이벤트 콜백
 * @param text UTF-8로 인코딩된 입력 텍스트 (한 글자 이상)
 * @param window_coords 커서 위치를 위한 창 좌표 배열 [x, y]
 */
typedef void (*dxlib_textinput_callback)(const char* text, int* window_coords);

/**
 * 텍스트 입력 이벤트 수신 콜백을 설정한다
 * @param callback 텍스트가 입력될 때 호출할 함수
 */
void dxlib_input_set_textinput_callback(dxlib_textinput_callback callback);

/**
 * 텍스트 편집 이벤트 콜백 (IME 조합용)
 * @param text 조합 중인 UTF-8 인코딩 텍스트
 * @param start 조합 텍스트의 시작 위치
 * @param length 조합 텍스트의 길이
 * @param window_coords 커서 위치를 위한 창 좌표 배열 [x, y]
 */
typedef void (*dxlib_textediting_callback)(const char* text, int start, int length, int* window_coords);

/**
 * 텍스트 편집 이벤트 수신 콜백을 설정한다
 * @param callback 텍스트가 조합 중일 때(IME) 호출할 함수
 */
void dxlib_input_set_textediting_callback(dxlib_textediting_callback callback);

/**
 * 텍스트 입력을 시작한다 (SDL_TEXTINPUT 이벤트를 활성화한다)
 */
void dxlib_input_start_text(void);

/**
 * 텍스트 입력을 중지한다 (SDL_TEXTINPUT 이벤트를 비활성화한다)
 */
void dxlib_input_stop_text(void);

/* ============================================================================
 * 사운드 백엔드 인터페이스
 * ============================================================================ */

/**
 * 사운드 버퍼 핸들 (불투명 타입)
 */
typedef struct dxlib_sound_buffer* dxlib_sound_t;

/**
 * 사운드 백엔드를 초기화한다
 * @param window_handle 네이티브 창 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_sound_init(void* window_handle);

/**
 * 사운드 백엔드를 해제한다
 */
void dxlib_sound_release(void);

/**
 * WAV 파일을 메모리로 로드한다
 * @param filename WAV 파일 경로
 * @return 사운드 핸들, 실패 시 NULL
 */
dxlib_sound_t dxlib_sound_load_wav(const char* filename);

/**
 * 원본 데이터로부터 사운드 버퍼를 생성한다
 * @param data 원본 오디오 데이터
 * @param size 데이터 크기 (바이트)
 * @param channels 채널 수 (1=모노, 2=스테레오)
 * @param sample_rate 샘플링 레이트 (Hz)
 * @param bits_per_sample 샘플당 비트 수 (8 또는 16)
 * @return 사운드 핸들, 실패 시 NULL
 */
dxlib_sound_t dxlib_sound_create_buffer(const void* data, int size,
                                       int channels, int sample_rate,
                                       int bits_per_sample);

/**
 * 사운드 버퍼를 해제한다
 * @param sound 사운드 핸들
 */
void dxlib_sound_free(dxlib_sound_t sound);

/**
 * 사운드를 재생한다
 * @param sound 사운드 핸들
 * @param loop 반복 재생(1) 또는 한 번만 재생(0)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_sound_play(dxlib_sound_t sound, int loop);

/**
 * 사운드를 정지한다
 * @param sound 사운드 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_sound_stop(dxlib_sound_t sound);

/**
 * 사운드가 재생 중인지 확인한다
 * @param sound 사운드 핸들
 * @return 재생 중이면 1, 아니면 0
 */
int dxlib_sound_is_playing(dxlib_sound_t sound);

/**
 * 사운드 볼륨을 설정한다
 * @param sound 사운드 핸들
 * @param volume 볼륨 레벨 (0-100, 100이 최대)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_sound_set_volume(dxlib_sound_t sound, int volume);

/**
 * 사운드 팬(스테레오 위치)을 설정한다
 * @param sound 사운드 핸들
 * @param pan 팬 값 (-100 ~ 100, -100=왼쪽, 0=가운데, 100=오른쪽)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_sound_set_pan(dxlib_sound_t sound, int pan);

/**
 * 사운드 주파수(재생 속도)를 설정한다
 * @param sound 사운드 핸들
 * @param frequency 주파수 (Hz, 0 = 원래 주파수)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_sound_set_frequency(dxlib_sound_t sound, int frequency);

/**
 * 사운드 버퍼를 복제한다 (동시 재생을 위함)
 * @param sound 원본 사운드 핸들
 * @return 복제된 사운드 핸들, 실패 시 NULL
 */
dxlib_sound_t dxlib_sound_duplicate(dxlib_sound_t sound);

/* ============================================================================
 * 음악 백엔드 인터페이스
 * ============================================================================ */

/**
 * 음악 백엔드를 초기화한다
 * @param window_handle 네이티브 창 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_music_init(void* window_handle);

/**
 * 음악 백엔드를 해제한다
 */
void dxlib_music_release(void);

/**
 * 음악 파일을 로드한다
 * @param filename 음악 파일 경로 (MIDI, MP3, OGG 등)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_music_load(const char* filename);

/**
 * 현재 음악을 해제한다
 */
void dxlib_music_free(void);

/**
 * 음악을 재생한다
 * @param loop 반복 재생(1) 또는 한 번만 재생(0)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_music_play(int loop);

/**
 * 음악을 정지한다
 */
void dxlib_music_stop(void);

/**
 * 음악을 일시정지한다
 */
void dxlib_music_pause(void);

/**
 * 음악을 재개한다
 */
void dxlib_music_resume(void);

/**
 * 음악이 재생 중인지 확인한다
 * @return 재생 중이면 1, 아니면 0
 */
int dxlib_music_is_playing(void);

/**
 * 음악이 일시정지 상태인지 확인한다
 * @return 일시정지 상태면 1, 아니면 0
 */
int dxlib_music_is_paused(void);

/**
 * 음악 볼륨을 설정한다
 * @param volume 볼륨 레벨 (0-100)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_music_set_volume(int volume);

/**
 * 음악 템포(재생 속도)를 설정한다
 * @param tempo 템포 배율 (1.0 = 보통, 0.5 = 절반 속도, 2.0 = 두 배 속도)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_music_set_tempo(float tempo);

/* ============================================================================
 * 스트림 백엔드 인터페이스 (긴 오디오 파일용)
 * ============================================================================ */

/**
 * 스트림 핸들 (불투명 타입)
 */
typedef struct dxlib_stream* dxlib_stream_t;

/**
 * 스트림 백엔드를 초기화한다
 * @param window_handle 네이티브 창 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_stream_init(void* window_handle);

/**
 * 스트림 백엔드를 해제한다
 */
void dxlib_stream_release(void);

/**
 * 스트리밍용 오디오 파일을 로드한다
 * @param filename 오디오 파일 경로
 * @return 스트림 핸들, 실패 시 NULL
 */
dxlib_stream_t dxlib_stream_load(const char* filename);

/**
 * 스트림을 해제한다
 * @param stream 스트림 핸들
 */
void dxlib_stream_free(dxlib_stream_t stream);

/**
 * 스트림을 재생한다
 * @param stream 스트림 핸들
 * @param loop 반복 재생 여부
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_stream_play(dxlib_stream_t stream, int loop);

/**
 * 스트림을 정지한다
 * @param stream 스트림 핸들
 */
void dxlib_stream_stop(dxlib_stream_t stream);

/**
 * 스트림을 갱신한다 (버퍼를 다시 채우기 위해 주기적으로 호출)
 * @param stream 스트림 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_stream_update(dxlib_stream_t stream);

/**
 * 스트림 볼륨을 설정한다
 * @param stream 스트림 핸들
 * @param volume 볼륨 레벨 (0-100)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int dxlib_stream_set_volume(dxlib_stream_t stream, int volume);

/**
 * 스트림이 재생 중인지 확인한다
 * @param stream 스트림 핸들
 * @return 재생 중이면 1, 아니면 0
 */
int dxlib_stream_is_playing(dxlib_stream_t stream);

/* ============================================================================
 * 백엔드 정보
 * ============================================================================ */

/**
 * 백엔드 이름을 가져온다
 * @return 백엔드 이름 문자열 ("DirectInput/DirectSound" 또는 "SDL2")
 */
const char* dxlib_get_backend_name(void);

/**
 * 백엔드 기능 플래그를 가져온다
 */
#define DXLIB_CAP_INPUT           0x01
#define DXLIB_CAP_SOUND           0x02
#define DXLIB_CAP_MUSIC           0x04
#define DXLIB_CAP_STREAM          0x08
#define DXLIB_CAP_MIDI            0x10
#define DXLIB_CAP_MP3             0x20
#define DXLIB_CAP_OGG             0x40

int dxlib_get_capabilities(void);

#ifdef __cplusplus
}
#endif

#endif /* __DXLIB_BACKEND_H__ */
