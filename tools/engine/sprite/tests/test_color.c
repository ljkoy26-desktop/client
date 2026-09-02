/**
 * @file test_color.c
 * @brief 색상 변환 속성 기반(property-based) 테스트
 *
 * 속성 1: RGB565 색상 변환 정확성
 * 속성 2: Colorkey 투명도 처리
 * 검증 대상: 요구사항 5.1, 5.2, 5.3, 5.4, 5.5, 5.6
 */

#include "color.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 테스트 헬퍼 선언 */
extern void test_assert(int condition, const char* message);
extern void test_assert_eq(int expected, int actual, const char* message);

/* 속성 테스트를 위한 간단한 의사난수 생성기 */
static unsigned int pbt_seed = 0;

static void pbt_init_seed(void) {
    pbt_seed = (unsigned int)time(NULL);
}

static uint16_t pbt_random_uint16(void) {
    pbt_seed = pbt_seed * 1103515245 + 12345;
    return (uint16_t)(pbt_seed >> 16);
}

/**
 * 속성 1: RGB565 색상 변환 정확성
 *
 * 임의의 RGB565 색상값을 RGBA32로 변환할 때, red(11-15비트), green(5-10비트),
 * blue(0-4비트)를 올바르게 추출하고 각 성분을 8비트 범위로 스케일링해야 한다.
 *
 * 검증 대상: 요구사항 5.1, 5.2, 5.3, 5.4
 */
static int test_property1_rgb565_conversion_correctness(void) {
    const int NUM_ITERATIONS = 100;
    int passed = 1;
    
    printf("  Property 1: RGB565 color conversion correctness\n");
    printf("    Running %d iterations...\n", NUM_ITERATIONS);
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        uint16_t color = pbt_random_uint16();
        uint16_t colorkey = pbt_random_uint16();
        
        /* 이 속성 테스트에서는 color != colorkey를 보장한다 (alpha 처리는 속성 2에서 다룬다) */
        while (color == colorkey) {
            colorkey = pbt_random_uint16();
        }
        
        RGBA32 result = rgb565_to_rgba32(color, colorkey);
        
        /* RGB565에서 예상되는 성분을 추출한다 */
        uint8_t expected_r5 = (color >> 11) & 0x1F;
        uint8_t expected_g6 = (color >> 5) & 0x3F;
        uint8_t expected_b5 = color & 0x1F;
        
        /* 예상되는 8비트 값을 계산한다 */
        uint8_t expected_r = (expected_r5 * 255) / 31;
        uint8_t expected_g = (expected_g6 * 255) / 63;
        uint8_t expected_b = (expected_b5 * 255) / 31;
        
        /* 변환 결과를 검증한다 */
        if (result.r != expected_r || result.g != expected_g || result.b != expected_b) {
            printf("    [FAIL] Iteration %d: color=0x%04X\n", i, color);
            printf("           Expected: R=%d, G=%d, B=%d\n", expected_r, expected_g, expected_b);
            printf("           Got:      R=%d, G=%d, B=%d\n", result.r, result.g, result.b);
            passed = 0;
            break;
        }
    }
    
    if (passed) {
        printf("    [PASS] All %d iterations passed\n", NUM_ITERATIONS);
    }
    
    return passed;
}

/**
 * 속성 2: Colorkey 투명도 처리
 *
 * 임의의 픽셀 값과 colorkey에 대해, 픽셀이 colorkey와 같으면 alpha는 0이어야 하고
 * 그렇지 않으면 alpha는 255여야 한다.
 *
 * 검증 대상: 요구사항 5.5, 5.6
 */
static int test_property2_colorkey_transparency(void) {
    const int NUM_ITERATIONS = 100;
    int passed = 1;
    
    printf("  Property 2: Colorkey transparency handling\n");
    printf("    Running %d iterations...\n", NUM_ITERATIONS);
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        uint16_t color = pbt_random_uint16();
        uint16_t colorkey = pbt_random_uint16();
        
        RGBA32 result = rgb565_to_rgba32(color, colorkey);
        
        /* colorkey 비교 결과에 따라 alpha를 검증한다 */
        uint8_t expected_alpha = (color == colorkey) ? 0 : 255;
        
        if (result.a != expected_alpha) {
            printf("    [FAIL] Iteration %d: color=0x%04X, colorkey=0x%04X\n", i, color, colorkey);
            printf("           Expected alpha=%d, Got alpha=%d\n", expected_alpha, result.a);
            passed = 0;
            break;
        }
    }
    
    /* colorkey가 명시적으로 일치하는 경우도 테스트한다 */
    if (passed) {
        for (int i = 0; i < 10; i++) {
            uint16_t color = pbt_random_uint16();
            RGBA32 result = rgb565_to_rgba32(color, color);  /* color == colorkey인 경우 */
            
            if (result.a != 0) {
                printf("    [FAIL] Colorkey match test: color=0x%04X should have alpha=0, got %d\n", 
                       color, result.a);
                passed = 0;
                break;
            }
        }
    }
    
    if (passed) {
        printf("    [PASS] All iterations passed\n");
    }
    
    return passed;
}

/**
 * 경계 조건과 알려진 값에 대한 추가 단위 테스트
 */
static void test_rgb565_known_values(void) {
    printf("  Unit tests: RGB565 known values\n");
    
    /* 검정색(0x0000) 테스트 */
    RGBA32 black = rgb565_to_rgba32(0x0000, 0xFFFF);
    test_assert(black.r == 0 && black.g == 0 && black.b == 0, 
                "Black (0x0000) converts to R=0, G=0, B=0");
    
    /* 흰색(0xFFFF) 테스트 */
    RGBA32 white = rgb565_to_rgba32(0xFFFF, 0x0000);
    test_assert(white.r == 255 && white.g == 255 && white.b == 255, 
                "White (0xFFFF) converts to R=255, G=255, B=255");
    
    /* 순수 빨강(0xF800) 테스트 - 11-15비트 모두 설정 */
    RGBA32 red = rgb565_to_rgba32(0xF800, 0x0000);
    test_assert(red.r == 255 && red.g == 0 && red.b == 0, 
                "Pure red (0xF800) converts to R=255, G=0, B=0");
    
    /* 순수 초록(0x07E0) 테스트 - 5-10비트 모두 설정 */
    RGBA32 green = rgb565_to_rgba32(0x07E0, 0x0000);
    test_assert(green.r == 0 && green.g == 255 && green.b == 0, 
                "Pure green (0x07E0) converts to R=0, G=255, B=0");
    
    /* 순수 파랑(0x001F) 테스트 - 0-4비트 모두 설정 */
    RGBA32 blue = rgb565_to_rgba32(0x001F, 0x0000);
    test_assert(blue.r == 0 && blue.g == 0 && blue.b == 255, 
                "Pure blue (0x001F) converts to R=0, G=0, B=255");
    
    /* colorkey 투명도 테스트 */
    RGBA32 transparent = rgb565_to_rgba32(0x1234, 0x1234);
    test_assert(transparent.a == 0, "Colorkey match results in alpha=0");
    
    RGBA32 opaque = rgb565_to_rgba32(0x1234, 0x5678);
    test_assert(opaque.a == 255, "Non-colorkey results in alpha=255");
}

/**
 * 일괄 변환 함수를 테스트한다
 */
static void test_rgb565_batch_conversion(void) {
    printf("  Unit tests: RGB565 batch conversion\n");
    
    uint16_t src[5] = {0x0000, 0xFFFF, 0xF800, 0x07E0, 0x001F};
    uint32_t dst[5] = {0};
    uint16_t colorkey = 0xDEAD;  /* 어떤 소스와도 일치하지 않음 */
    
    rgb565_to_rgba32_batch(src, dst, 5, colorkey);
    
    /* 각 변환을 검증한다 */
    for (int i = 0; i < 5; i++) {
        RGBA32 expected = rgb565_to_rgba32(src[i], colorkey);
        uint32_t expected_packed = ((uint32_t)expected.r) |
                                   ((uint32_t)expected.g << 8) |
                                   ((uint32_t)expected.b << 16) |
                                   ((uint32_t)expected.a << 24);
        
        if (dst[i] != expected_packed) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Batch conversion index %d matches single conversion", i);
            test_assert(0, msg);
            return;
        }
    }
    test_assert(1, "Batch conversion matches single conversion for all elements");
    
    /* NULL 처리를 테스트한다 */
    rgb565_to_rgba32_batch(NULL, dst, 5, colorkey);
    rgb565_to_rgba32_batch(src, NULL, 5, colorkey);
    rgb565_to_rgba32_batch(src, dst, 0, colorkey);
    rgb565_to_rgba32_batch(src, dst, -1, colorkey);
    test_assert(1, "Batch conversion handles NULL/invalid inputs gracefully");
}

void test_color_conversion(void) {
    int property1_passed, property2_passed;
    
    /* 속성 테스트를 위한 난수 시드를 초기화한다 */
    pbt_init_seed();
    printf("  (PBT seed: %u)\n", pbt_seed);
    
    /* 속성 기반 테스트를 실행한다 */
    printf("\n  --- Property-Based Tests ---\n");
    property1_passed = test_property1_rgb565_conversion_correctness();
    property2_passed = test_property2_colorkey_transparency();
    
    /* 단위 테스트를 실행한다 */
    printf("\n  --- Unit Tests ---\n");
    test_rgb565_known_values();
    test_rgb565_batch_conversion();
    
    /* 요약 */
    printf("\n  --- Property Test Summary ---\n");
    printf("  Property 1 (RGB565 conversion): %s\n", property1_passed ? "PASSED" : "FAILED");
    printf("  Property 2 (Colorkey handling): %s\n", property2_passed ? "PASSED" : "FAILED");
}
