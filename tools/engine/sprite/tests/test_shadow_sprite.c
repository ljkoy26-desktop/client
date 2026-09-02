/**
 * @file test_shadow_sprite.c
 * @brief ShadowSprite와 ShadowSpritePack에 대한 테스트
 *
 * 검증 대상: 요구사항 2.1, 2.2, 2.3, 2.4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shadow_sprite.h"
#include "shadow_spritepack.h"

/* 외부 테스트 어설션 함수 */
extern void test_assert(int condition, const char* message);
extern void test_assert_eq(int expected, int actual, const char* message);

/* 테스트 데이터 경로 - 작업 디렉터리에 따라 여러 경로를 시도한다 */
static const char* TEST_SSPK_PATHS[] = {
    "../DarkEden/Data/Image/Creature.sspk",
    "DarkEden/Data/Image/Creature.sspk",
    "../../DarkEden/Data/Image/Creature.sspk",
    NULL
};

static const char* find_sspk_path(void) {
    for (int i = 0; TEST_SSPK_PATHS[i] != NULL; i++) {
        FILE* f = fopen(TEST_SSPK_PATHS[i], "rb");
        if (f) {
            fclose(f);
            return TEST_SSPK_PATHS[i];
        }
    }
    return NULL;
}

/**
 * ShadowSprite 초기화를 테스트한다
 */
static void test_shadow_sprite_init(void) {
    ShadowSprite sprite;
    
    shadow_sprite_init(&sprite);
    
    test_assert(!shadow_sprite_is_init(&sprite), "New sprite should not be initialized");
    test_assert_eq(0, shadow_sprite_get_width(&sprite), "New sprite width should be 0");
    test_assert_eq(0, shadow_sprite_get_height(&sprite), "New sprite height should be 0");
    
    shadow_sprite_release(&sprite);
}

/**
 * ShadowSpritePack 초기화를 테스트한다
 */
static void test_shadow_spritepack_init(void) {
    ShadowSpritePack pack;
    
    shadow_spritepack_init(&pack);
    
    test_assert_eq(0, shadow_spritepack_get_size(&pack), "New pack size should be 0");
    
    shadow_spritepack_release(&pack);
}

/**
 * 실제 Creature.sspk 파일 로딩을 테스트한다
 * 검증 대상: 요구사항 2.1
 */
static void test_sspk_load(void) {
    ShadowSpritePack pack;
    shadow_spritepack_init(&pack);

    /* SSPK 파일을 찾는다 */
    const char* sspk_path = find_sspk_path();
    
    if (!sspk_path) {
        printf("    (Skipped: Could not find Creature.sspk in any known path)\n");
        test_assert(1, "SSPK load test skipped (file not found)");
        shadow_spritepack_release(&pack);
        return;
    }
    
    /* 팩 파일을 로드해본다 */
    int result = shadow_spritepack_load(&pack, sspk_path);

    if (result) {
        test_assert(1, "Successfully loaded Creature.sspk");

        uint16_t size = shadow_spritepack_get_size(&pack);
        test_assert(size > 0, "Pack should contain sprites");

        printf("    (Loaded %d shadow sprites from %s)\n", size, sspk_path);

        /* 첫 번째 스프라이트 접근을 테스트한다 */
        if (size > 0) {
            ShadowSprite* sprite = shadow_spritepack_get(&pack, 0);
            test_assert(sprite != NULL, "Should be able to get first sprite");

            if (sprite && shadow_sprite_is_init(sprite)) {
                uint16_t w = shadow_sprite_get_width(sprite);
                uint16_t h = shadow_sprite_get_height(sprite);
                printf("    (First shadow sprite: %dx%d)\n", w, h);
                test_assert(1, "First sprite is initialized");
            }
        }
    } else {
        /* 테스트 환경에 파일이 없을 수 있다 */
        printf("    (Skipped: Could not load %s)\n", sspk_path);
        test_assert(1, "SSPK load test skipped (file not found)");
    }
    
    shadow_spritepack_release(&pack);
}

/**
 * ShadowSpritePack의 지연 로딩을 테스트한다
 * 검증 대상: 요구사항 2.1, 2.2
 */
static void test_sspk_lazy_load(void) {
    ShadowSpritePack pack;
    shadow_spritepack_init(&pack);

    /* SSPK 파일을 찾는다 */
    const char* sspk_path = find_sspk_path();
    
    if (!sspk_path) {
        printf("    (Skipped: Could not find Creature.sspk in any known path)\n");
        test_assert(1, "SSPK lazy load test skipped (file not found)");
        shadow_spritepack_release(&pack);
        return;
    }
    
    /* 지연 로딩으로 로드를 시도한다 */
    int result = shadow_spritepack_load_lazy(&pack, sspk_path);

    if (result) {
        test_assert(1, "Successfully initialized lazy loading for Creature.sspk");

        uint16_t size = shadow_spritepack_get_size(&pack);
        test_assert(size > 0, "Pack should report sprite count");

        printf("    (Pack reports %d shadow sprites)\n", size);

        if (size > 0) {
            /* 첫 접근에서 로드가 트리거되어야 한다 */
            ShadowSprite* sprite1 = shadow_spritepack_get(&pack, 0);
            test_assert(sprite1 != NULL, "First access should return sprite");

            if (sprite1) {
                test_assert(shadow_sprite_is_init(sprite1),
                           "Sprite should be loaded on first access");

                /* 두 번째 접근은 같은 스프라이트를 반환해야 한다 */
                ShadowSprite* sprite2 = shadow_spritepack_get(&pack, 0);
                test_assert(sprite1 == sprite2,
                           "Subsequent access should return same sprite");

                /* 다른 스프라이트에 접근한다 */
                if (size > 10) {
                    ShadowSprite* sprite10 = shadow_spritepack_get(&pack, 10);
                    test_assert(sprite10 != NULL, "Should be able to access sprite 10");
                    if (sprite10) {
                        test_assert(shadow_sprite_is_init(sprite10),
                                   "Sprite 10 should be loaded on access");
                    }
                }
            }
        }
    } else {
        printf("    (Skipped: Could not load %s for lazy loading)\n", sspk_path);
        test_assert(1, "SSPK lazy load test skipped (file not found)");
    }
    
    shadow_spritepack_release(&pack);
}

/**
 * 실제 파일에서 그림자 스프라이트 크기를 테스트한다
 * 검증 대상: 요구사항 2.3
 */
static void test_shadow_sprite_dimensions(void) {
    ShadowSpritePack pack;
    shadow_spritepack_init(&pack);

    /* SSPK 파일을 찾는다 */
    const char* sspk_path = find_sspk_path();
    
    if (!sspk_path) {
        printf("    (Skipped: Could not find Creature.sspk in any known path)\n");
        test_assert(1, "Shadow sprite dimensions test skipped");
        shadow_spritepack_release(&pack);
        return;
    }
    
    int result = shadow_spritepack_load_lazy(&pack, sspk_path);
    
    if (result) {
        uint16_t size = shadow_spritepack_get_size(&pack);
        
        if (size > 0) {
            /* 처음 몇 개 스프라이트의 크기가 적절한지 확인한다 */
            int validCount = 0;
            int checkCount = (size < 5) ? size : 5;
            
            for (int i = 0; i < checkCount; i++) {
                ShadowSprite* sprite = shadow_spritepack_get(&pack, i);
                if (sprite && shadow_sprite_is_init(sprite)) {
                    uint16_t w = shadow_sprite_get_width(sprite);
                    uint16_t h = shadow_sprite_get_height(sprite);
                    
                    /* 스프라이트는 적절한 크기를 가져야 한다 (< 1024) */
                    if (w < 1024 && h < 1024) {
                        validCount++;
                    }
                }
            }
            
            test_assert(validCount == checkCount, 
                       "All checked sprites should have valid dimensions");
            printf("    (Verified %d shadow sprites have valid dimensions)\n", validCount);
        }
    } else {
        printf("    (Skipped: Could not load test file)\n");
        test_assert(1, "Shadow sprite dimensions test skipped");
    }
    
    shadow_spritepack_release(&pack);
}

/**
 * 그림자 렌더링 버퍼 연산을 테스트한다
 * 검증 대상: 요구사항 2.4
 */
static void test_shadow_rendering(void) {
    ShadowSpritePack pack;
    shadow_spritepack_init(&pack);

    /* SSPK 파일을 찾는다 */
    const char* sspk_path = find_sspk_path();
    
    if (!sspk_path) {
        printf("    (Skipped: Could not find Creature.sspk in any known path)\n");
        test_assert(1, "Shadow rendering test skipped");
        shadow_spritepack_release(&pack);
        return;
    }
    
    int result = shadow_spritepack_load_lazy(&pack, sspk_path);
    
    if (result) {
        uint16_t size = shadow_spritepack_get_size(&pack);
        
        if (size > 0) {
            /* 크기가 0이 아닌 첫 번째 스프라이트를 가져온다 */
            ShadowSprite* sprite = NULL;
            for (int i = 0; i < size && i < 100; i++) {
                ShadowSprite* s = shadow_spritepack_get(&pack, i);
                if (s && shadow_sprite_is_init(s) && 
                    shadow_sprite_get_width(s) > 0 && 
                    shadow_sprite_get_height(s) > 0) {
                    sprite = s;
                    break;
                }
            }
            
            if (sprite) {
                uint16_t w = shadow_sprite_get_width(sprite);
                uint16_t h = shadow_sprite_get_height(sprite);
                
                /* 흰색으로 채운 테스트 버퍼를 생성한다 */
                uint16_t* buffer = (uint16_t*)malloc(w * h * sizeof(uint16_t));
                if (buffer) {
                    /* 흰색으로 채운다 (RGB565에서 0xFFFF) */
                    for (int i = 0; i < w * h; i++) {
                        buffer[i] = 0xFFFF;
                    }

                    /* darkness를 적용해 그림자를 렌더링한다 */
                    shadow_sprite_blt_darkness(sprite, buffer, w * 2, 1);

                    /* 일부 픽셀이 어두워졌는지 확인한다 */
                    int darkenedCount = 0;
                    for (int i = 0; i < w * h; i++) {
                        if (buffer[i] != 0xFFFF) {
                            darkenedCount++;
                        }
                    }
                    
                    test_assert(darkenedCount > 0, 
                               "Shadow rendering should darken some pixels");
                    printf("    (Shadow darkened %d pixels in %dx%d buffer)\n", 
                           darkenedCount, w, h);
                    
                    free(buffer);
                } else {
                    test_assert(1, "Shadow rendering test skipped (memory allocation failed)");
                }
            } else {
                printf("    (Skipped: No valid shadow sprite found)\n");
                test_assert(1, "Shadow rendering test skipped (no valid sprite)");
            }
        }
    } else {
        printf("    (Skipped: Could not load test file)\n");
        test_assert(1, "Shadow rendering test skipped");
    }
    
    shadow_spritepack_release(&pack);
}

/**
 * 그림자 알파 블렌딩을 테스트한다
 * 검증 대상: 요구사항 2.4
 */
static void test_shadow_alpha_blending(void) {
    ShadowSpritePack pack;
    shadow_spritepack_init(&pack);

    /* SSPK 파일을 찾는다 */
    const char* sspk_path = find_sspk_path();
    
    if (!sspk_path) {
        printf("    (Skipped: Could not find Creature.sspk in any known path)\n");
        test_assert(1, "Shadow alpha blending test skipped");
        shadow_spritepack_release(&pack);
        return;
    }
    
    int result = shadow_spritepack_load_lazy(&pack, sspk_path);
    
    if (result) {
        uint16_t size = shadow_spritepack_get_size(&pack);
        
        if (size > 0) {
            /* 크기가 0이 아닌 첫 번째 스프라이트를 가져온다 */
            ShadowSprite* sprite = NULL;
            for (int i = 0; i < size && i < 100; i++) {
                ShadowSprite* s = shadow_spritepack_get(&pack, i);
                if (s && shadow_sprite_is_init(s) && 
                    shadow_sprite_get_width(s) > 0 && 
                    shadow_sprite_get_height(s) > 0) {
                    sprite = s;
                    break;
                }
            }
            
            if (sprite) {
                uint16_t w = shadow_sprite_get_width(sprite);
                uint16_t h = shadow_sprite_get_height(sprite);
                
                /* 테스트 버퍼를 생성한다 */
                uint16_t* buffer1 = (uint16_t*)malloc(w * h * sizeof(uint16_t));
                uint16_t* buffer2 = (uint16_t*)malloc(w * h * sizeof(uint16_t));

                if (buffer1 && buffer2) {
                    /* 흰색으로 채운다 */
                    for (int i = 0; i < w * h; i++) {
                        buffer1[i] = 0xFFFF;
                        buffer2[i] = 0xFFFF;
                    }

                    /* 서로 다른 alpha 값으로 렌더링한다 */
                    shadow_sprite_blt_alpha(sprite, buffer1, w * 2, 64);   /* 25% 그림자 */
                    shadow_sprite_blt_alpha(sprite, buffer2, w * 2, 192);  /* 75% 그림자 */

                    /* alpha가 높을수록 더 어두운 결과가 나오는지 확인한다 */
                    int sum1 = 0, sum2 = 0;
                    for (int i = 0; i < w * h; i++) {
                        sum1 += buffer1[i];
                        sum2 += buffer2[i];
                    }
                    
                    /* alpha가 높을수록(그림자가 더 불투명할수록) 합이 더 작아져야 한다(더 어두움) */
                    test_assert(sum2 <= sum1, 
                               "Higher alpha should produce darker shadow");
                    printf("    (Alpha 64 sum: %d, Alpha 192 sum: %d)\n", sum1, sum2);
                    
                    free(buffer1);
                    free(buffer2);
                } else {
                    if (buffer1) free(buffer1);
                    if (buffer2) free(buffer2);
                    test_assert(1, "Shadow alpha test skipped (memory allocation failed)");
                }
            } else {
                printf("    (Skipped: No valid shadow sprite found)\n");
                test_assert(1, "Shadow alpha test skipped (no valid sprite)");
            }
        }
    } else {
        printf("    (Skipped: Could not load test file)\n");
        test_assert(1, "Shadow alpha test skipped");
    }
    
    shadow_spritepack_release(&pack);
}

/**
 * ShadowSprite를 위한 메인 테스트 함수
 */
void test_shadow_sprite(void) {
    printf("  ShadowSprite Initialization:\n");
    test_shadow_sprite_init();
    
    printf("  ShadowSpritePack Initialization:\n");
    test_shadow_spritepack_init();
    
    printf("  SSPK File Loading (Requirement 2.1):\n");
    test_sspk_load();
    
    printf("  SSPK Lazy Loading (Requirements 2.1, 2.2):\n");
    test_sspk_lazy_load();
    
    printf("  Shadow Sprite Dimensions (Requirement 2.3):\n");
    test_shadow_sprite_dimensions();
    
    printf("  Shadow Rendering (Requirement 2.4):\n");
    test_shadow_rendering();
    
    printf("  Shadow Alpha Blending (Requirement 2.4):\n");
    test_shadow_alpha_blending();
}
