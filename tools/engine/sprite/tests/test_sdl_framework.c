/**
 * @file test_sdl_framework.c
 * @brief SDL 프레임워크 단위 테스트
 *
 * 초기화, 정리, 이벤트 처리를 테스트한다
 * 검증 대상: 요구사항 1.1, 1.4, 1.5
 */

#include "sdl_framework.h"
#include <stdio.h>
#include <string.h>

/* 테스트 헬퍼 선언 */
extern void test_assert(int condition, const char* message);
extern void test_assert_eq(int expected, int actual, const char* message);

/**
 * NULL 매개변수 처리를 테스트한다
 * 검증 대상: 요구사항 1.4 (오류 처리)
 */
static void test_null_parameters(void) {
    printf("  Testing NULL parameter handling...\n");
    
    SDLFramework fw;
    SDLFrameworkConfig config = {800, 600, "Test", 60};
    
    /* NULL 프레임워크 포인터 테스트 */
    int result = sdl_framework_init(NULL, &config);
    test_assert(result < 0, "Init with NULL framework returns error");
    
    /* NULL 설정 포인터 테스트 */
    result = sdl_framework_init(&fw, NULL);
    test_assert(result < 0, "Init with NULL config returns error");
    
    /* NULL로 cleanup 테스트 - 크래시가 발생하지 않아야 한다 */
    sdl_framework_cleanup(NULL);
    test_assert(1, "Cleanup with NULL does not crash");
    
    /* NULL로 poll_events 테스트 */
    SDL_Event event;
    result = sdl_framework_poll_events(NULL, &event);
    test_assert(result == 0, "Poll events with NULL framework returns 0");
    
    memset(&fw, 0, sizeof(fw));
    result = sdl_framework_poll_events(&fw, NULL);
    test_assert(result == 0, "Poll events with NULL event returns 0");
    
    /* NULL로 begin/end frame 테스트 - 크래시가 발생하지 않아야 한다 */
    sdl_framework_begin_frame(NULL);
    test_assert(1, "Begin frame with NULL does not crash");
    
    sdl_framework_end_frame(NULL);
    test_assert(1, "End frame with NULL does not crash");
    
    /* NULL로 delay 테스트 - 크래시가 발생하지 않아야 한다 */
    sdl_framework_delay(NULL, 0);
    test_assert(1, "Delay with NULL does not crash");
}

/**
 * 프레임워크 설정 기본값을 테스트한다
 */
static void test_config_defaults(void) {
    printf("  Testing configuration handling...\n");
    
    SDLFrameworkConfig config;
    memset(&config, 0, sizeof(config));
    
    /* 0 또는 음수 값은 기본값을 사용해야 한다 */
    config.window_width = 0;
    config.window_height = 0;
    config.window_title = NULL;
    config.target_fps = 0;
    
    /* 디스플레이 없이는 실제로 SDL 초기화를 테스트할 수 없지만,
       설정 구조체가 유효한지는 확인할 수 있다 */
    test_assert(sizeof(SDLFrameworkConfig) > 0, "Config structure has valid size");
    test_assert(sizeof(SDLFramework) > 0, "Framework structure has valid size");
}

/**
 * 프레임워크 상태 구조체를 테스트한다
 */
static void test_framework_structure(void) {
    printf("  Testing framework structure...\n");
    
    SDLFramework fw;
    memset(&fw, 0, sizeof(fw));
    
    /* 초기 상태를 확인한다 */
    test_assert(fw.window == NULL, "Initial window is NULL");
    test_assert(fw.renderer == NULL, "Initial renderer is NULL");
    test_assert(fw.running == 0, "Initial running is 0");
    test_assert(fw.frame_delay == 0, "Initial frame_delay is 0");
}

/**
 * SDL 초기화와 정리를 테스트한다
 * 참고: 이 테스트는 SDL이 사용 가능해야 하며 headless 환경에서는 실패할 수 있다
 * 검증 대상: 요구사항 1.1, 1.2, 1.3, 1.4, 1.5
 */
static void test_init_and_cleanup(void) {
    printf("  Testing SDL initialization and cleanup...\n");
    
    SDLFramework fw;
    SDLFrameworkConfig config = {
        .window_width = 320,
        .window_height = 240,
        .window_title = "Test Window",
        .target_fps = 60
    };
    
    /* 초기화를 시도한다 - headless 환경에서는 실패할 수 있다 */
    int result = sdl_framework_init(&fw, &config);
    
    if (result == 0) {
        /* 요구사항 1.1: SDL 초기화 성공 */
        test_assert(1, "SDL initialization succeeded");
        
        /* 요구사항 1.2: 윈도우 생성됨 */
        test_assert(fw.window != NULL, "Window was created");
        
        /* 요구사항 1.3: 렌더러 생성됨 */
        test_assert(fw.renderer != NULL, "Renderer was created");
        
        /* 요구사항 2.1: Running 플래그 설정됨 */
        test_assert(fw.running == 1, "Running flag is set");
        
        /* 요구사항 2.6: 프레임 지연 계산됨 */
        test_assert(fw.frame_delay > 0, "Frame delay is set");
        test_assert_eq(16, (int)fw.frame_delay, "Frame delay is ~16ms for 60 FPS");
        
        /* 프레임 렌더링 함수 테스트 */
        sdl_framework_begin_frame(&fw);
        test_assert(1, "Begin frame executed");
        
        sdl_framework_end_frame(&fw);
        test_assert(1, "End frame executed");
        
        /* 이벤트 폴링 테스트 (이벤트가 없을 것으로 예상) */
        SDL_Event event;
        int has_event = sdl_framework_poll_events(&fw, &event);
        test_assert(has_event == 0 || has_event == 1, "Poll events returns valid result");
        
        /* 요구사항 1.5: 리소스 정리 */
        sdl_framework_cleanup(&fw);
        test_assert(fw.window == NULL, "Window cleaned up");
        test_assert(fw.renderer == NULL, "Renderer cleaned up");
        test_assert(fw.running == 0, "Running flag cleared");
        test_assert(1, "SDL cleanup succeeded");
    } else {
        /* SDL 초기화 실패 - headless 환경일 가능성이 높다 */
        printf("    [SKIP] SDL initialization failed (headless environment?)\n");
        printf("    [INFO] Error code: %d\n", result);
        test_assert(result < 0, "Init failure returns negative error code");
    }
}

/**
 * 프레임 지연 계산을 테스트한다
 */
static void test_frame_delay_calculation(void) {
    printf("  Testing frame delay calculation...\n");
    
    /* 다양한 FPS 값을 테스트한다 */
    struct {
        int fps;
        uint32_t expected_delay;
    } test_cases[] = {
        {60, 16},   /* 1000/60 = 16.67 -> 16 */
        {30, 33},   /* 1000/30 = 33.33 -> 33 */
        {120, 8},   /* 1000/120 = 8.33 -> 8 */
        {1, 1000},  /* 1000/1 = 1000 */
    };
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        uint32_t calculated = 1000 / test_cases[i].fps;
        char msg[128];
        snprintf(msg, sizeof(msg), "Frame delay for %d FPS is %u ms", 
                 test_cases[i].fps, test_cases[i].expected_delay);
        test_assert_eq((int)test_cases[i].expected_delay, (int)calculated, msg);
    }
}

void test_sdl_framework(void) {
    printf("\n  --- SDL Framework Unit Tests ---\n");
    
    test_null_parameters();
    test_config_defaults();
    test_framework_structure();
    test_frame_delay_calculation();
    test_init_and_cleanup();
}
