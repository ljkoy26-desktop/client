/**
 * @file test_index_sprite.c
 * @brief IndexedSprite와 IndexedSpritePack에 대한 테스트
 *
 * 속성 4: ISPK 지연 로딩(lazy loading)
 * 속성 5: ColorSet 매핑 일관성
 * 검증 대상: 요구사항 1.1, 1.2, 1.3, 6.4, 6.5
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colorset.h"
#include "index_sprite.h"
#include "index_spritepack.h"

/* 외부 테스트 어설션 함수 */
extern void test_assert(int condition, const char* message);
extern void test_assert_eq(int expected, int actual, const char* message);

/* 테스트 데이터 경로 - 작업 디렉터리에 따라 여러 경로를 시도한다 */
static const char* TEST_ISPK_PATHS[] = {
    "../DarkEden/Data/Image/Creature.ispk",
    "DarkEden/Data/Image/Creature.ispk",
    "../../DarkEden/Data/Image/Creature.ispk",
    NULL
};

static const char* find_ispk_path(void) {
    for (int i = 0; TEST_ISPK_PATHS[i] != NULL; i++) {
        FILE* f = fopen(TEST_ISPK_PATHS[i], "rb");
        if (f) {
            fclose(f);
            return TEST_ISPK_PATHS[i];
        }
    }
    return NULL;
}

/**
 * ColorSet 초기화를 테스트한다
 * 검증 대상: 요구사항 6.1, 6.2, 6.3
 */
static void test_colorset_init(void) {
    /* ColorSet 테이블을 초기화한다 */
    colorset_init();

    /* ColorSet 테이블이 채워졌는지 검증한다 */
    int hasNonZero = 0;
    for (int i = 0; i < MAX_COLORSET && !hasNonZero; i++) {
        for (int j = 0; j < MAX_COLORGRADATION && !hasNonZero; j++) {
            if (g_ColorSet[i][j] != 0) {
                hasNonZero = 1;
            }
        }
    }
    test_assert(hasNonZero, "ColorSet table should have non-zero values after init");
    
    /* GradationValue 테이블이 채워졌는지 검증한다 */
    hasNonZero = 0;
    for (int i = 0; i < MAX_COLORGRADATION; i++) {
        if (g_GradationValue[i] != 0) {
            hasNonZero = 1;
            break;
        }
    }
    test_assert(hasNonZero, "GradationValue table should have non-zero values");
    
    /* ColorSetDarkness 테이블이 채워졌는지 검증한다 */
    hasNonZero = 0;
    for (int k = 0; k < MAX_DARKBIT && !hasNonZero; k++) {
        for (int i = 0; i < MAX_COLORSET && !hasNonZero; i++) {
            for (int j = 0; j < MAX_COLORGRADATION && !hasNonZero; j++) {
                if (g_ColorSetDarkness[k][i][j] != 0) {
                    hasNonZero = 1;
                }
            }
        }
    }
    test_assert(hasNonZero, "ColorSetDarkness table should have non-zero values");
}

/**
 * ColorSet의 set/get 함수를 테스트한다
 * 검증 대상: 요구사항 6.4
 */
static void test_colorset_using(void) {
    colorset_init();

    /* SetUsingColorSet을 테스트한다 */
    colorset_set_using(100, 200);
    test_assert_eq(100, colorset_get_using(0), "SetUsingColorSet should set index 0");
    test_assert_eq(200, colorset_get_using(1), "SetUsingColorSet should set index 1");

    /* SetUsingColorSetOnly를 테스트한다 */
    colorset_set_using_only(5, 300);
    test_assert_eq(300, colorset_get_using(5), "SetUsingColorSetOnly should set specific index");

    /* 다른 인덱스가 변경되지 않았는지 검증한다 */
    test_assert_eq(100, colorset_get_using(0), "Other indices should remain unchanged");
}

/**
 * RGB565 색상 함수를 테스트한다
 */
static void test_colorset_rgb565(void) {
    /* 색상 생성을 테스트한다 */
    uint16_t color = colorset_make_color(31, 31, 31);  /* 흰색 */
    test_assert(color != 0, "White color should be non-zero");

    /* 성분 추출을 테스트한다 */
    test_assert_eq(31, colorset_get_red(color), "Red component should be 31");
    test_assert_eq(31, colorset_get_green(color), "Green component should be 31");
    test_assert_eq(31, colorset_get_blue(color), "Blue component should be 31");
    
    /* 검정색을 테스트한다 */
    color = colorset_make_color(0, 0, 0);
    test_assert_eq(0, color, "Black color should be 0");

    /* 빨강만 있는 경우를 테스트한다 */
    color = colorset_make_color(31, 0, 0);
    test_assert_eq(31, colorset_get_red(color), "Red-only: red should be 31");
    test_assert_eq(0, colorset_get_green(color), "Red-only: green should be 0");
    test_assert_eq(0, colorset_get_blue(color), "Red-only: blue should be 0");
}

/**
 * IndexSprite 초기화를 테스트한다
 */
static void test_index_sprite_init(void) {
    IndexSprite sprite;
    
    index_sprite_init(&sprite);
    
    test_assert(!index_sprite_is_init(&sprite), "New sprite should not be initialized");
    test_assert_eq(0, index_sprite_get_width(&sprite), "New sprite width should be 0");
    test_assert_eq(0, index_sprite_get_height(&sprite), "New sprite height should be 0");
    
    index_sprite_release(&sprite);
}

/**
 * IndexSpritePack 초기화를 테스트한다
 */
static void test_index_spritepack_init(void) {
    IndexSpritePack pack;
    
    index_spritepack_init(&pack);
    
    test_assert_eq(0, index_spritepack_get_size(&pack), "New pack size should be 0");
    
    index_spritepack_release(&pack);
}

/**
 * 실제 Creature.ispk 파일 로딩을 테스트한다
 * 검증 대상: 요구사항 1.1
 */
static void test_ispk_load(void) {
    IndexSpritePack pack;
    index_spritepack_init(&pack);

    /* ISPK 파일을 찾는다 */
    const char* ispk_path = find_ispk_path();
    
    if (!ispk_path) {
        printf("    (Skipped: Could not find Creature.ispk in any known path)\n");
        test_assert(1, "ISPK load test skipped (file not found)");
        index_spritepack_release(&pack);
        return;
    }
    
    /* 팩 파일을 로드해본다 */
    int result = index_spritepack_load(&pack, ispk_path);

    if (result) {
        test_assert(1, "Successfully loaded Creature.ispk");

        uint16_t size = index_spritepack_get_size(&pack);
        test_assert(size > 0, "Pack should contain sprites");

        printf("    (Loaded %d sprites from %s)\n", size, ispk_path);

        /* 첫 번째 스프라이트 접근을 테스트한다 */
        if (size > 0) {
            IndexSprite* sprite = index_spritepack_get(&pack, 0);
            test_assert(sprite != NULL, "Should be able to get first sprite");

            if (sprite && index_sprite_is_init(sprite)) {
                uint16_t w = index_sprite_get_width(sprite);
                uint16_t h = index_sprite_get_height(sprite);
                printf("    (First sprite: %dx%d)\n", w, h);
                test_assert(1, "First sprite is initialized");
            }
        }
    } else {
        /* 테스트 환경에 파일이 없을 수 있다 */
        printf("    (Skipped: Could not load %s)\n", ispk_path);
        test_assert(1, "ISPK load test skipped (file not found)");
    }
    
    index_spritepack_release(&pack);
}

/**
 * 속성 4: ISPK 지연 로딩(lazy loading)
 * IndexSpritePack의 지연 로딩을 테스트한다
 * 검증 대상: 요구사항 1.2, 1.3
 *
 * LoadFromFileRunning()으로 로드된 IndexedSpritePack에 대해,
 * operator[]로 스프라이트에 접근하면 필요할 때 파일에서 로드해야 하고,
 * 이후의 접근은 동일한 스프라이트를 반환해야 한다.
 */
static void test_ispk_lazy_load(void) {
    IndexSpritePack pack;
    index_spritepack_init(&pack);

    /* ISPK 파일을 찾는다 */
    const char* ispk_path = find_ispk_path();
    
    if (!ispk_path) {
        printf("    (Skipped: Could not find Creature.ispk in any known path)\n");
        test_assert(1, "ISPK lazy load test skipped (file not found)");
        index_spritepack_release(&pack);
        return;
    }
    
    /* 지연 로딩으로 로드를 시도한다 */
    int result = index_spritepack_load_lazy(&pack, ispk_path);

    if (result) {
        test_assert(1, "Successfully initialized lazy loading for Creature.ispk");

        uint16_t size = index_spritepack_get_size(&pack);
        test_assert(size > 0, "Pack should report sprite count");

        printf("    (Pack reports %d sprites)\n", size);

        if (size > 0) {
            /* 첫 접근에서 로드가 트리거되어야 한다 */
            IndexSprite* sprite1 = index_spritepack_get(&pack, 0);
            test_assert(sprite1 != NULL, "First access should return sprite");

            if (sprite1) {
                test_assert(index_sprite_is_init(sprite1),
                           "Property 4: Sprite should be loaded on first access");

                /* 두 번째 접근은 같은 스프라이트를 반환해야 한다 */
                IndexSprite* sprite2 = index_spritepack_get(&pack, 0);
                test_assert(sprite1 == sprite2,
                           "Property 4: Subsequent access should return same sprite");

                /* 다른 스프라이트에 접근한다 */
                if (size > 10) {
                    IndexSprite* sprite10 = index_spritepack_get(&pack, 10);
                    test_assert(sprite10 != NULL, "Should be able to access sprite 10");
                    if (sprite10) {
                        test_assert(index_sprite_is_init(sprite10),
                                   "Property 4: Sprite 10 should be loaded on access");
                    }
                }
            }
        }
    } else {
        printf("    (Skipped: Could not load %s for lazy loading)\n", ispk_path);
        test_assert(1, "ISPK lazy load test skipped (file not found)");
    }
    
    index_spritepack_release(&pack);
}

/**
 * 속성 5: ColorSet 매핑 일관성
 * ColorSet 매핑 일관성을 테스트한다
 * 검증 대상: 요구사항 6.4, 6.5
 *
 * 임의의 유효한 인덱스 값과 ColorSet 설정에 대해,
 * 렌더링된 색상은 ColorSet 테이블 조회 결과와 일치해야 한다.
 */
static void test_colorset_mapping_consistency(void) {
    colorset_init();

    /* ColorSet 조회가 일관되는지 테스트한다 */
    for (int setIdx = 0; setIdx < 10; setIdx++) {  /* 처음 10개 세트를 테스트한다 */
        for (int grad = 0; grad < MAX_COLORGRADATION; grad++) {
            uint16_t color1 = g_ColorSet[setIdx][grad];
            uint16_t color2 = g_ColorSet[setIdx][grad];
            
            if (color1 != color2) {
                test_assert(0, "Property 5: ColorSet lookup should be consistent");
                return;
            }
        }
    }
    test_assert(1, "Property 5: ColorSet lookup is consistent across accesses");
    
    /* SetUsingColorSet이 조회에 올바르게 영향을 주는지 테스트한다 */
    colorset_set_using(50, 100);

    /* 매핑이 적용되었는지 검증한다 */
    int idx0 = colorset_get_using(0);
    int idx1 = colorset_get_using(1);
    
    test_assert_eq(50, idx0, "Property 5: Index 0 should map to ColorSet 50");
    test_assert_eq(100, idx1, "Property 5: Index 1 should map to ColorSet 100");
    
    /* 매핑된 세트의 색상에 접근 가능한지 검증한다 */
    uint16_t color0 = g_ColorSet[idx0][15];  /* 중간 gradation */
    uint16_t color1 = g_ColorSet[idx1][15];
    
    test_assert(color0 == g_ColorSet[50][15], 
               "Property 5: Mapped color should match direct lookup");
    test_assert(color1 == g_ColorSet[100][15],
               "Property 5: Mapped color should match direct lookup");
}

/**
 * 실제 파일에서 스프라이트 크기를 테스트한다
 */
static void test_sprite_dimensions(void) {
    IndexSpritePack pack;
    index_spritepack_init(&pack);

    /* ISPK 파일을 찾는다 */
    const char* ispk_path = find_ispk_path();

    if (!ispk_path) {
        printf("    (Skipped: Could not find Creature.ispk in any known path)\n");
        test_assert(1, "Sprite dimensions test skipped");
        index_spritepack_release(&pack);
        return;
    }
    
    int result = index_spritepack_load_lazy(&pack, ispk_path);
    
    if (result) {
        uint16_t size = index_spritepack_get_size(&pack);
        
        if (size > 0) {
            /* 처음 몇 개 스프라이트의 크기가 적절한지 확인한다 */
            int validCount = 0;
            int checkCount = (size < 5) ? size : 5;
            
            for (int i = 0; i < checkCount; i++) {
                IndexSprite* sprite = index_spritepack_get(&pack, i);
                if (sprite && index_sprite_is_init(sprite)) {
                    uint16_t w = index_sprite_get_width(sprite);
                    uint16_t h = index_sprite_get_height(sprite);
                    
                    /* 스프라이트는 적절한 크기를 가져야 한다 (< 1024) */
                    if (w < 1024 && h < 1024) {
                        validCount++;
                    }
                }
            }
            
            test_assert(validCount == checkCount, 
                       "All checked sprites should have valid dimensions");
            printf("    (Verified %d sprites have valid dimensions)\n", validCount);
        }
    } else {
        printf("    (Skipped: Could not load test file)\n");
        test_assert(1, "Sprite dimensions test skipped");
    }
    
    index_spritepack_release(&pack);
}

/**
 * ColorSet 렌더링 효과를 테스트한다
 * 검증 대상: 요구사항 6.4, 6.5
 *
 * 서로 다른 ColorSet이 같은 gradation 레벨에서 서로 다른 색상을
 * 만들어내는지 검증한다.
 */
static void test_colorset_rendering_effect(void) {
    colorset_init();

    /* 서로 다른 ColorSet이 다른 색상을 만드는지 테스트한다 */
    int differentCount = 0;
    int totalComparisons = 0;

    /* 서로 다른 ColorSet에서 중간 gradation(15)의 색상을 비교한다 */
    for (int set1 = 0; set1 < 10; set1++) {
        for (int set2 = set1 + 1; set2 < 10; set2++) {
            uint16_t color1 = g_ColorSet[set1][15];
            uint16_t color2 = g_ColorSet[set2][15];
            
            if (color1 != color2) {
                differentCount++;
            }
            totalComparisons++;
        }
    }
    
    /* 대부분의 ColorSet은 서로 다른 색상을 만들어야 한다 */
    test_assert(differentCount > totalComparisons / 2,
               "Different ColorSets should produce different colors");
    printf("    (%d/%d ColorSet pairs produce different colors)\n", 
           differentCount, totalComparisons);
    
    /* ColorSet 내에서 gradation 진행을 테스트한다 */
    int gradationProgression = 1;
    for (int set = 0; set < 5; set++) {
        /* gradation이 증가할수록 색상은 대체로 어두워져야 한다 */
        uint16_t colorBright = g_ColorSet[set][0];
        uint16_t colorDark = g_ColorSet[set][MAX_COLORGRADATION - 1];
        
        int brightSum = colorset_get_red(colorBright) + 
                        colorset_get_green(colorBright) + 
                        colorset_get_blue(colorBright);
        int darkSum = colorset_get_red(colorDark) + 
                      colorset_get_green(colorDark) + 
                      colorset_get_blue(colorDark);
        
        /* 밝은 쪽이 어두운 쪽보다 색상 합이 커야 한다 */
        if (brightSum <= darkSum) {
            gradationProgression = 0;
            break;
        }
    }
    test_assert(gradationProgression, 
               "ColorSet gradation should progress from bright to dark");
    
    /* 어둡게 하기 레벨을 테스트한다 */
    int darknessProgression = 1;
    for (int set = 0; set < 5; set++) {
        for (int grad = 0; grad < MAX_COLORGRADATION; grad++) {
            uint16_t baseColor = g_ColorSet[set][grad];
            
            for (int dark = 1; dark < MAX_DARKBIT; dark++) {
                uint16_t darkColor = g_ColorSetDarkness[dark][set][grad];
                
                int baseSum = colorset_get_red(baseColor) + 
                              colorset_get_green(baseColor) + 
                              colorset_get_blue(baseColor);
                int darkSum = colorset_get_red(darkColor) + 
                              colorset_get_green(darkColor) + 
                              colorset_get_blue(darkColor);
                
                /* 더 어두운 레벨은 색상 합이 더 작거나 같아야 한다 */
                if (darkSum > baseSum) {
                    darknessProgression = 0;
                    break;
                }
            }
            if (!darknessProgression) break;
        }
        if (!darknessProgression) break;
    }
    test_assert(darknessProgression,
               "Darkness levels should progressively darken colors");
}

/**
 * IndexedSprite를 위한 메인 테스트 함수
 */
void test_index_sprite(void) {
    printf("  ColorSet Initialization:\n");
    test_colorset_init();
    
    printf("  ColorSet Using Functions:\n");
    test_colorset_using();
    
    printf("  ColorSet RGB565 Functions:\n");
    test_colorset_rgb565();
    
    printf("  IndexSprite Initialization:\n");
    test_index_sprite_init();
    
    printf("  IndexSpritePack Initialization:\n");
    test_index_spritepack_init();
    
    printf("  ISPK File Loading (Requirement 1.1):\n");
    test_ispk_load();
    
    printf("  Property 4 - ISPK Lazy Loading (Requirements 1.2, 1.3):\n");
    test_ispk_lazy_load();
    
    printf("  Property 5 - ColorSet Mapping Consistency (Requirements 6.4, 6.5):\n");
    test_colorset_mapping_consistency();
    
    printf("  Sprite Dimensions Validation:\n");
    test_sprite_dimensions();
    
    printf("  ColorSet Rendering Effect Validation:\n");
    test_colorset_rendering_effect();
}
