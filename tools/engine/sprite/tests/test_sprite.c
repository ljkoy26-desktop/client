/**
 * @file test_sprite.c
 * @brief Sprite 파싱 속성 기반 테스트
 *
 * 속성 3: Sprite 파싱 왕복 일관성(round-trip consistency)
 * 임의의 유효한 스프라이트 데이터에 대해, RLE 데이터를 디코딩하면 올바른
 * 픽셀 출력을 만들어야 한다. 디코딩된 픽셀 수는 width * height와 같아야 하고
 * 투명/색상 구간이 올바르게 재구성되어야 한다.
 *
 * 검증 대상: 요구사항 3.1, 3.2, 3.4, 3.5, 3.6, 3.7
 */

#include "sprite.h"
#include "color.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 테스트 헬퍼 선언 */
extern void test_assert(int condition, const char* message);
extern void test_assert_eq(int expected, int actual, const char* message);

/* 속성 테스트를 위한 간단한 의사난수 생성기 */
static unsigned int sprite_pbt_seed = 0;

static void sprite_pbt_init_seed(void) {
    sprite_pbt_seed = (unsigned int)time(NULL);
}

static unsigned int sprite_pbt_random(void) {
    sprite_pbt_seed = sprite_pbt_seed * 1103515245 + 12345;
    return sprite_pbt_seed;
}

static uint16_t sprite_pbt_random_uint16(void) {
    return (uint16_t)(sprite_pbt_random() >> 16);
}

/**
 * RLE 데이터를 가진 임의의 유효한 스프라이트를 생성한다
 * 올바른 형식의 RLE 스캔라인을 가진 스프라이트를 반환한다
 */
static int generate_random_sprite(Sprite* sprite, int max_width, int max_height) {
    /* 임의의 크기를 생성한다 (1~max) */
    sprite->width = 1 + (sprite_pbt_random() % max_width);
    sprite->height = 1 + (sprite_pbt_random() % max_height);
    sprite->is_valid = 1;
    
    /* 스캔라인 배열을 할당한다 */
    sprite->scanlines = (uint16_t**)calloc(sprite->height, sizeof(uint16_t*));
    sprite->scanline_lens = (uint16_t*)calloc(sprite->height, sizeof(uint16_t));
    
    if (!sprite->scanlines || !sprite->scanline_lens) {
        return -1;
    }
    
    /* 각 스캔라인의 RLE 데이터를 생성한다 */
    for (int y = 0; y < sprite->height; y++) {
        /* RLE 데이터를 구성하기 위한 임시 버퍼 */
        uint16_t temp[1024];
        int temp_index = 1;  /* 세그먼트 개수를 위해 인덱스 0을 예약한다 */
        int segment_count = 0;
        int x = 0;

        /* width를 채울 때까지 세그먼트를 생성한다 */
        while (x < sprite->width) {
            /* 임의의 투명 개수 (0 ~ 남은 width) */
            int remaining = sprite->width - x;
            int trans_count = sprite_pbt_random() % (remaining + 1);

            /* width를 넘지 않도록 보장한다 */
            if (trans_count > remaining) {
                trans_count = remaining;
            }

            x += trans_count;

            /* 투명으로 끝에 도달했다면 완료된 것이다 */
            if (x >= sprite->width) {
                /* 기록할 투명 구간이 있을 때만 세그먼트를 추가한다 */
                if (trans_count > 0 && segment_count == 0) {
                    /* 빈 줄 - 세그먼트가 필요 없다 */
                }
                break;
            }

            /* 임의의 색상 개수 (1 ~ 남은 width) */
            remaining = sprite->width - x;
            int color_count = 1 + (sprite_pbt_random() % remaining);

            /* 세그먼트를 저장한다 */
            temp[temp_index++] = (uint16_t)trans_count;
            temp[temp_index++] = (uint16_t)color_count;

            /* 임의의 색상 픽셀을 생성한다 (colorkey를 피하기 위해 0이 아니게) */
            for (int c = 0; c < color_count; c++) {
                uint16_t pixel = sprite_pbt_random_uint16();
                /* 0이 아니도록 보장한다 (colorkey는 보통 0이다) */
                if (pixel == 0) pixel = 1;
                temp[temp_index++] = pixel;
            }

            x += color_count;
            segment_count++;
        }

        /* 인덱스 0에 세그먼트 개수를 저장한다 */
        temp[0] = (uint16_t)segment_count;

        /* RLE 데이터를 할당하고 복사한다 */
        sprite->scanline_lens[y] = (uint16_t)temp_index;
        sprite->scanlines[y] = (uint16_t*)malloc(temp_index * sizeof(uint16_t));
        if (!sprite->scanlines[y]) {
            return -1;
        }
        memcpy(sprite->scanlines[y], temp, temp_index * sizeof(uint16_t));
    }
    
    return 0;
}

/**
 * sprite_decode와 비교 검증하기 위해 스프라이트를 수동으로 디코딩한다
 * 픽셀 배열을 반환하거나 오류 시 NULL을 반환한다
 */
static uint32_t* manual_decode_sprite(const Sprite* sprite, uint16_t colorkey) {
    if (!sprite || sprite->width == 0 || sprite->height == 0) {
        return NULL;
    }
    
    size_t pixel_count = (size_t)sprite->width * sprite->height;
    uint32_t* pixels = (uint32_t*)calloc(pixel_count, sizeof(uint32_t));
    if (!pixels) {
        return NULL;
    }
    
    for (int y = 0; y < sprite->height; y++) {
        uint32_t* row = pixels + (y * sprite->width);
        
        if (!sprite->scanlines[y] || sprite->scanline_lens[y] == 0) {
            continue;
        }
        
        uint16_t* rle = sprite->scanlines[y];
        int count = rle[0];
        int idx = 1;
        int x = 0;
        
        for (int seg = 0; seg < count && x < sprite->width; seg++) {
            int trans = rle[idx++];
            int colors = rle[idx++];
            
            x += trans;
            
            for (int c = 0; c < colors && x < sprite->width; c++) {
                uint16_t pixel = rle[idx++];
                RGBA32 rgba = rgb565_to_rgba32(pixel, colorkey);
                row[x] = ((uint32_t)rgba.r) |
                         ((uint32_t)rgba.g << 8) |
                         ((uint32_t)rgba.b << 16) |
                         ((uint32_t)rgba.a << 24);
                x++;
            }
        }
    }
    
    return pixels;
}

/**
 * 속성 3: Sprite 파싱 왕복 일관성(round-trip consistency)
 *
 * 임의의 유효한 스프라이트 데이터에 대해, RLE 데이터를 디코딩하면 올바른
 * 픽셀 출력을 만들어야 한다. 다음과 같은 방식으로 검증한다:
 * 1. 임의의 유효한 RLE 스프라이트 데이터를 생성한다
 * 2. sprite_decode로 디코딩한다
 * 3. 같은 데이터를 수동으로 디코딩한다
 * 4. 결과를 비교한다
 *
 * 검증 대상: 요구사항 3.1, 3.2, 3.4, 3.5, 3.6, 3.7
 */
static int test_property3_sprite_decode_consistency(void) {
    const int NUM_ITERATIONS = 100;
    const int MAX_WIDTH = 64;
    const int MAX_HEIGHT = 64;
    const uint16_t COLORKEY = 0;
    int passed = 1;
    
    printf("  Property 3: Sprite decode consistency\n");
    printf("    Running %d iterations...\n", NUM_ITERATIONS);
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        Sprite sprite;
        memset(&sprite, 0, sizeof(sprite));
        
        /* 임의의 스프라이트를 생성한다 */
        if (generate_random_sprite(&sprite, MAX_WIDTH, MAX_HEIGHT) != 0) {
            printf("    [FAIL] Iteration %d: Failed to generate sprite\n", i);
            passed = 0;
            break;
        }
        
        /* sprite_decode를 이용해 디코딩한다 */
        DecodedSprite decoded;
        if (sprite_decode(&sprite, &decoded, COLORKEY) != 0) {
            printf("    [FAIL] Iteration %d: sprite_decode failed\n", i);
            sprite_free(&sprite);
            passed = 0;
            break;
        }
        
        /* 비교를 위해 수동으로 디코딩한다 */
        uint32_t* expected = manual_decode_sprite(&sprite, COLORKEY);
        if (!expected) {
            printf("    [FAIL] Iteration %d: manual decode failed\n", i);
            decoded_sprite_free(&decoded);
            sprite_free(&sprite);
            passed = 0;
            break;
        }
        
        /* 크기를 비교한다 */
        if (decoded.width != sprite.width || decoded.height != sprite.height) {
            printf("    [FAIL] Iteration %d: Dimension mismatch\n", i);
            printf("           Expected: %dx%d, Got: %dx%d\n",
                   sprite.width, sprite.height, decoded.width, decoded.height);
            free(expected);
            decoded_sprite_free(&decoded);
            sprite_free(&sprite);
            passed = 0;
            break;
        }
        
        /* 픽셀 데이터를 비교한다 */
        size_t pixel_count = (size_t)sprite.width * sprite.height;
        int mismatch = 0;
        for (size_t p = 0; p < pixel_count; p++) {
            if (decoded.pixels[p] != expected[p]) {
                int x = p % sprite.width;
                int y = p / sprite.width;
                printf("    [FAIL] Iteration %d: Pixel mismatch at (%d, %d)\n", i, x, y);
                printf("           Expected: 0x%08X, Got: 0x%08X\n",
                       expected[p], decoded.pixels[p]);
                mismatch = 1;
                break;
            }
        }
        
        free(expected);
        decoded_sprite_free(&decoded);
        sprite_free(&sprite);
        
        if (mismatch) {
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
 * 단위 테스트: 빈 스프라이트 처리
 */
static void test_empty_sprite(void) {
    printf("  Unit test: Empty sprite handling\n");
    
    Sprite sprite;
    memset(&sprite, 0, sizeof(sprite));
    sprite.width = 0;
    sprite.height = 0;
    sprite.is_valid = 1;
    sprite.scanlines = NULL;
    sprite.scanline_lens = NULL;
    
    DecodedSprite decoded;
    int result = sprite_decode(&sprite, &decoded, 0);
    
    test_assert(result == 0, "Empty sprite decodes successfully");
    test_assert(decoded.width == 0 && decoded.height == 0, 
                "Empty sprite has zero dimensions");
    test_assert(decoded.pixels == NULL, "Empty sprite has NULL pixels");
    
    decoded_sprite_free(&decoded);
}

/**
 * 단위 테스트: 단일 픽셀 스프라이트
 */
static void test_single_pixel_sprite(void) {
    printf("  Unit test: Single pixel sprite\n");
    
    Sprite sprite;
    memset(&sprite, 0, sizeof(sprite));
    sprite.width = 1;
    sprite.height = 1;
    sprite.is_valid = 1;
    
    /* 단일 픽셀에 대한 RLE 데이터 생성: [세그먼트 1개] [투명 0] [색상 1] [픽셀] */
    uint16_t rle_data[] = {1, 0, 1, 0xF800};  /* 순수 빨강 */
    
    sprite.scanlines = (uint16_t**)malloc(sizeof(uint16_t*));
    sprite.scanline_lens = (uint16_t*)malloc(sizeof(uint16_t));
    sprite.scanlines[0] = (uint16_t*)malloc(sizeof(rle_data));
    memcpy(sprite.scanlines[0], rle_data, sizeof(rle_data));
    sprite.scanline_lens[0] = 4;
    
    DecodedSprite decoded;
    int result = sprite_decode(&sprite, &decoded, 0);
    
    test_assert(result == 0, "Single pixel sprite decodes successfully");
    test_assert(decoded.width == 1 && decoded.height == 1,
                "Single pixel sprite has correct dimensions");
    
    if (decoded.pixels) {
        /* 픽셀이 완전 불투명한 빨강인지 확인한다 */
        uint32_t pixel = decoded.pixels[0];
        uint8_t r = pixel & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = (pixel >> 16) & 0xFF;
        uint8_t a = (pixel >> 24) & 0xFF;
        
        test_assert(r == 255 && g == 0 && b == 0 && a == 255,
                    "Single red pixel has correct RGBA values");
    }
    
    decoded_sprite_free(&decoded);
    sprite_free(&sprite);
}

/**
 * 단위 테스트: 투명 줄 처리
 */
static void test_transparent_line(void) {
    printf("  Unit test: Transparent line handling\n");
    
    Sprite sprite;
    memset(&sprite, 0, sizeof(sprite));
    sprite.width = 10;
    sprite.height = 1;
    sprite.is_valid = 1;
    
    /* 완전 투명한 줄에 대한 RLE 데이터 생성: [세그먼트 0개] */
    uint16_t rle_data[] = {0};
    
    sprite.scanlines = (uint16_t**)malloc(sizeof(uint16_t*));
    sprite.scanline_lens = (uint16_t*)malloc(sizeof(uint16_t));
    sprite.scanlines[0] = (uint16_t*)malloc(sizeof(rle_data));
    memcpy(sprite.scanlines[0], rle_data, sizeof(rle_data));
    sprite.scanline_lens[0] = 1;
    
    DecodedSprite decoded;
    int result = sprite_decode(&sprite, &decoded, 0);
    
    test_assert(result == 0, "Transparent line decodes successfully");
    
    if (decoded.pixels) {
        int all_transparent = 1;
        for (int x = 0; x < 10; x++) {
            if (decoded.pixels[x] != 0) {
                all_transparent = 0;
                break;
            }
        }
        test_assert(all_transparent, "All pixels in transparent line are zero");
    }
    
    decoded_sprite_free(&decoded);
    sprite_free(&sprite);
}

/**
 * 단위 테스트: NULL 매개변수 처리
 */
static void test_null_parameters(void) {
    printf("  Unit test: NULL parameter handling\n");
    
    Sprite sprite;
    DecodedSprite decoded;
    
    test_assert(sprite_decode(NULL, &decoded, 0) == -1,
                "sprite_decode returns -1 for NULL sprite");
    test_assert(sprite_decode(&sprite, NULL, 0) == -1,
                "sprite_decode returns -1 for NULL decoded");
    
    /* NULL로 sprite_free를 테스트한다 */
    sprite_free(NULL);  /* 크래시가 발생하지 않아야 한다 */
    test_assert(1, "sprite_free handles NULL gracefully");

    /* NULL로 decoded_sprite_free를 테스트한다 */
    decoded_sprite_free(NULL);  /* 크래시가 발생하지 않아야 한다 */
    test_assert(1, "decoded_sprite_free handles NULL gracefully");
}

void test_sprite_parsing(void) {
    int property3_passed;
    
    /* 속성 테스트를 위한 난수 시드를 초기화한다 */
    sprite_pbt_init_seed();
    printf("  (PBT seed: %u)\n", sprite_pbt_seed);
    
    /* 속성 기반 테스트를 실행한다 */
    printf("\n  --- Property-Based Tests ---\n");
    property3_passed = test_property3_sprite_decode_consistency();
    
    /* 단위 테스트를 실행한다 */
    printf("\n  --- Unit Tests ---\n");
    test_empty_sprite();
    test_single_pixel_sprite();
    test_transparent_line();
    test_null_parameters();
    
    /* 요약 */
    printf("\n  --- Property Test Summary ---\n");
    printf("  Property 3 (Sprite decode consistency): %s\n", 
           property3_passed ? "PASSED" : "FAILED");
}
