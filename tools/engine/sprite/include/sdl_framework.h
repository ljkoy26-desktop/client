/**
 * @file sdl_framework.h
 * @brief 창 관리 및 메인 루프를 위한 SDL2 게임 프레임워크
 * 
 * 요구사항: 1.1, 1.2, 1.3, 1.4, 1.5, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6
 */

#ifndef SDL_FRAMEWORK_H
#define SDL_FRAMEWORK_H

#include <SDL.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 프레임워크 설정 구조체
 */
typedef struct {
    int window_width;          /**< 창 너비 */
    int window_height;         /**< 창 높이 */
    const char* window_title;  /**< 창 제목 */
    int target_fps;            /**< 목표 FPS */
} SDLFrameworkConfig;

/**
 * 프레임워크 상태 구조체
 */
typedef struct SDLFramework SDLFramework;

struct SDLFramework {
    SDL_Window* window;        /**< SDL 윈도우 */
    SDL_Renderer* renderer;    /**< SDL 렌더러 */
    int running;               /**< 실행 중 여부 플래그 */
    uint32_t frame_delay;      /**< 프레임 지연 시간 (ms) */
};

/**
 * SDL 프레임워크 초기화
 * @param fw 프레임워크 상태 포인터
 * @param config 설정 포인터
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int sdl_framework_init(SDLFramework* fw, const SDLFrameworkConfig* config);

/**
 * 모든 SDL 리소스 해제
 * @param fw 프레임워크 상태 포인터
 */
void sdl_framework_cleanup(SDLFramework* fw);

/**
 * 대기 중인 SDL 이벤트 처리
 * @param fw 프레임워크 상태 포인터
 * @param event 이벤트 데이터를 저장할 SDL_Event 포인터
 * @return 계속 실행 시 0, 종료 시 0이 아닌 값
 */
int sdl_framework_poll_events(SDLFramework* fw, SDL_Event* event);

/**
 * 프레임 렌더링 시작 (화면 지우기)
 * @param fw 프레임워크 상태 포인터
 */
void sdl_framework_begin_frame(SDLFramework* fw);

/**
 * 프레임 렌더링 종료 (화면에 표시)
 * @param fw 프레임워크 상태 포인터
 */
void sdl_framework_end_frame(SDLFramework* fw);

/**
 * 프레임 레이트 제어 지연
 * @param fw 프레임워크 상태 포인터
 * @param frame_start 프레임 시작 시점의 틱 수
 */
void sdl_framework_delay(SDLFramework* fw, uint32_t frame_start);

#ifdef __cplusplus
}
#endif

#endif /* SDL_FRAMEWORK_H */
