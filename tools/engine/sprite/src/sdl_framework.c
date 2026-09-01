/**
 * @file sdl_framework.c
 * @brief SDL2 게임 프레임워크 구현부
 *
 * 요구사항: 1.1, 1.2, 1.3, 1.4, 1.5, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6
 */

#include "sdl_framework.h"
#include <stdio.h>

/**
 * SDL 프레임워크 초기화
 * 요구사항 1.1: 비디오 서브시스템을 포함하여 SDL2 초기화
 * 요구사항 1.2: 설정 가능한 크기로 창 생성
 * 요구사항 1.3: 하드웨어 가속 렌더러 생성
 * 요구사항 1.4: 실패 시 에러 코드 반환 및 로깅
 */
int sdl_framework_init(SDLFramework* fw, const SDLFrameworkConfig* config) {
    if (!fw || !config) {
        fprintf(stderr, "Error: NULL pointer passed to sdl_framework_init\n");
        return -1;
    }

    /* 프레임워크 상태 초기화 */
    fw->window = NULL;
    fw->renderer = NULL;
    fw->running = 0;
    fw->frame_delay = 0;

    /* 요구사항 1.1: 비디오 서브시스템을 포함하여 SDL2 초기화 */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error: SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    /* 요구사항 1.2: 설정 가능한 너비와 높이로 창 생성 */
    fw->window = SDL_CreateWindow(
        config->window_title ? config->window_title : "SDL Framework",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config->window_width > 0 ? config->window_width : 800,
        config->window_height > 0 ? config->window_height : 600,
        SDL_WINDOW_SHOWN
    );

    if (!fw->window) {
        fprintf(stderr, "Error: SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -2;
    }

    /* 요구사항 1.3: 하드웨어 가속 렌더러 생성 */
    fw->renderer = SDL_CreateRenderer(
        fw->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!fw->renderer) {
        fprintf(stderr, "Error: SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(fw->window);
        fw->window = NULL;
        SDL_Quit();
        return -3;
    }

    /* 요구사항 2.6: 설정 가능한 목표 프레임 레이트 지원 */
    if (config->target_fps > 0) {
        fw->frame_delay = 1000 / config->target_fps;
    } else {
        fw->frame_delay = 16; /* 기본값 약 60 FPS */
    }

    /* 요구사항 2.1: 메인 루프를 위한 실행 중 플래그 설정 */
    fw->running = 1;

    return 0;
}

/**
 * 모든 SDL 리소스 해제
 * 요구사항 1.5: 종료 시 모든 SDL 리소스를 올바르게 해제
 */
void sdl_framework_cleanup(SDLFramework* fw) {
    if (!fw) {
        return;
    }

    if (fw->renderer) {
        SDL_DestroyRenderer(fw->renderer);
        fw->renderer = NULL;
    }

    if (fw->window) {
        SDL_DestroyWindow(fw->window);
        fw->window = NULL;
    }

    fw->running = 0;

    SDL_Quit();
}

/**
 * 대기 중인 SDL 이벤트 처리
 * 요구사항 2.2: 대기 중인 모든 SDL 이벤트 처리
 * 요구사항 2.3: quit 이벤트 발생 시 종료 플래그 설정
 *
 * @return 처리할 이벤트가 있으면 1, 없으면 0
 */
int sdl_framework_poll_events(SDLFramework* fw, SDL_Event* event) {
    if (!fw || !event) {
        return 0;
    }

    /* 요구사항 2.2: 대기 중인 SDL 이벤트 처리 */
    if (SDL_PollEvent(event)) {
        /* 요구사항 2.3: quit 이벤트 발생 시 종료 플래그 설정 */
        if (event->type == SDL_QUIT) {
            fw->running = 0;
        }
        return 1;
    }

    return 0;
}

/**
 * 프레임 렌더링 시작
 * 요구사항 2.4: 프레임 시작 시 렌더 타겟 지우기
 */
void sdl_framework_begin_frame(SDLFramework* fw) {
    if (!fw || !fw->renderer) {
        return;
    }

    /* 지우기 색상을 흰색으로 설정 */
    SDL_SetRenderDrawColor(fw->renderer, 255, 255, 255, 255);

    /* 요구사항 2.4: 렌더 타겟 지우기 */
    SDL_RenderClear(fw->renderer);
}

/**
 * 프레임 렌더링 종료
 * 요구사항 2.5: 렌더링된 내용을 화면에 표시
 */
void sdl_framework_end_frame(SDLFramework* fw) {
    if (!fw || !fw->renderer) {
        return;
    }

    /* 요구사항 2.5: 렌더링된 내용 표시 */
    SDL_RenderPresent(fw->renderer);
}

/**
 * 프레임 레이트 제어 지연
 * 요구사항 2.6: 설정 가능한 목표 프레임 레이트 지원
 */
void sdl_framework_delay(SDLFramework* fw, uint32_t frame_start) {
    if (!fw) {
        return;
    }

    uint32_t frame_time = SDL_GetTicks() - frame_start;

    if (frame_time < fw->frame_delay) {
        SDL_Delay(fw->frame_delay - frame_time);
    }
}
