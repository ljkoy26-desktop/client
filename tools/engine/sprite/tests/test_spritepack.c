/**
 * @file test_spritepack.c
 * @brief SpritePack 파싱 속성 기반 테스트
 *
 * 속성 4: SpritePack 파싱 완전성
 * 임의의 유효한 SpritePack 파일이라면, 로드 시 스프라이트 개수를 올바르게
 * 읽어야 하고, 0부터 count-1까지의 각 스프라이트 인덱스에 대해 스프라이트를
 * 로드할 수 있고 유효한 크기를 가져야 한다.
 *
 * 검증 대상: 요구사항 4.1, 4.2, 4.4
 */

#include "spritepack.h"
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
static unsigned int spritepack_pbt_seed = 0;

static void spritepack_pbt_init_seed(void) {
    spritepack_pbt_seed = (unsigned int)time(NULL);
}

static unsigned int spritepack_pbt_random(void) {
    spritepack_pbt_seed = spritepack_pbt_seed * 1103515245 + 12345;
    return spritepack_pbt_seed;
}

/**
 * 임의의 유효한 스프라이트를 생성해 파일에 기록한다
 * 기록한 바이트 수를 반환하거나 오류 시 -1을 반환한다
 */
static int write_random_sprite_to_file(FILE* file, int max_width, int max_height) {
    /* 임의의 크기를 생성한다 (1~max) */
    uint16_t width = 1 + (spritepack_pbt_random() % max_width);
    uint16_t height = 1 + (spritepack_pbt_random() % max_height);

    /* width와 height를 기록한다 */
    if (fwrite(&width, 2, 1, file) != 1) return -1;
    if (fwrite(&height, 2, 1, file) != 1) return -1;

    /* 각 스캔라인을 기록한다 */
    for (int y = 0; y < height; y++) {
        /* 이 스캔라인을 위한 간단한 RLE 데이터를 생성한다 */
        /* 형식: [segment_count] [trans_count, color_count, pixels...] */
        uint16_t temp[256];
        int temp_index = 1;  /* 세그먼트 개수를 위해 인덱스 0을 예약한다 */
        int segment_count = 0;
        int x = 0;

        while (x < width) {
            /* 임의의 투명 개수 */
            int remaining = width - x;
            int trans_count = spritepack_pbt_random() % (remaining + 1);
            if (trans_count > remaining) trans_count = remaining;
            x += trans_count;

            if (x >= width) break;

            /* 임의의 색상 개수 */
            remaining = width - x;
            int color_count = 1 + (spritepack_pbt_random() % remaining);

            temp[temp_index++] = (uint16_t)trans_count;
            temp[temp_index++] = (uint16_t)color_count;

            /* 임의의 픽셀을 생성한다 */
            for (int c = 0; c < color_count; c++) {
                uint16_t pixel = (uint16_t)(spritepack_pbt_random() >> 16);
                if (pixel == 0) pixel = 1;  /* colorkey를 피한다 */
                temp[temp_index++] = pixel;
            }

            x += color_count;
            segment_count++;
        }

        temp[0] = (uint16_t)segment_count;

        /* 스캔라인 길이와 데이터를 기록한다 */
        uint16_t len = (uint16_t)temp_index;
        if (fwrite(&len, 2, 1, file) != 1) return -1;
        if (fwrite(temp, 2, len, file) != len) return -1;
    }
    
    return 0;
}

/**
 * 임의의 스프라이트로 임시 SpritePack 파일을 생성한다
 * 성공 시 파일명을 반환하고 실패 시 NULL을 반환한다
 */
static char* create_temp_spritepack(uint16_t sprite_count, int max_width, int max_height) {
    static char filename[256];
    snprintf(filename, sizeof(filename), "/tmp/test_spritepack_%u.spk", spritepack_pbt_seed);

    FILE* file = fopen(filename, "wb");
    if (!file) return NULL;

    /* 스프라이트 개수를 기록한다 */
    if (fwrite(&sprite_count, 2, 1, file) != 1) {
        fclose(file);
        return NULL;
    }

    /* 각 스프라이트를 기록한다 */
    for (int i = 0; i < sprite_count; i++) {
        if (write_random_sprite_to_file(file, max_width, max_height) != 0) {
            fclose(file);
            return NULL;
        }
    }
    
    fclose(file);
    return filename;
}

/**
 * 인덱스 파일이 있는 임시 SpritePack 파일을 생성한다
 * 성공 시 파일명을 반환하고 실패 시 NULL을 반환한다
 */
static char* create_temp_spritepack_with_index(uint16_t sprite_count, int max_width, int max_height) {
    static char filename[256];
    static char index_filename[260];
    snprintf(filename, sizeof(filename), "/tmp/test_spritepack_idx_%u.spk", spritepack_pbt_seed);
    snprintf(index_filename, sizeof(index_filename), "%si", filename);
    
    FILE* data_file = fopen(filename, "wb");
    FILE* index_file = fopen(index_filename, "wb");
    if (!data_file || !index_file) {
        if (data_file) fclose(data_file);
        if (index_file) fclose(index_file);
        return NULL;
    }
    
    /* 두 파일 모두에 스프라이트 개수를 기록한다 */
    if (fwrite(&sprite_count, 2, 1, data_file) != 1 ||
        fwrite(&sprite_count, 2, 1, index_file) != 1) {
        fclose(data_file);
        fclose(index_file);
        return NULL;
    }
    
    /* 각 스프라이트를 기록하고 오프셋을 기록한다 */
    for (int i = 0; i < sprite_count; i++) {
        /* 현재 위치를 오프셋으로 기록한다 */
        uint32_t offset = (uint32_t)ftell(data_file);
        if (fwrite(&offset, 4, 1, index_file) != 1) {
            fclose(data_file);
            fclose(index_file);
            return NULL;
        }

        /* 스프라이트 데이터를 기록한다 */
        if (write_random_sprite_to_file(data_file, max_width, max_height) != 0) {
            fclose(data_file);
            fclose(index_file);
            return NULL;
        }
    }
    
    fclose(data_file);
    fclose(index_file);
    return filename;
}

/**
 * 임시 파일을 제거한다
 */
static void cleanup_temp_files(const char* filename) {
    if (filename) {
        remove(filename);

        /* 인덱스 파일이 있다면 함께 제거한다 */
        char index_filename[260];
        snprintf(index_filename, sizeof(index_filename), "%si", filename);
        remove(index_filename);
    }
}

/**
 * 속성 4: SpritePack 파싱 완전성
 *
 * 임의의 유효한 SpritePack 파일이라면, 로드 시 스프라이트 개수를 올바르게
 * 읽어야 하고, 0부터 count-1까지의 각 스프라이트 인덱스에 대해 스프라이트를
 * 로드할 수 있고 유효한 크기를 가져야 한다.
 *
 * 검증 대상: 요구사항 4.1, 4.2, 4.4
 */
static int test_property4_spritepack_completeness(void) {
    const int NUM_ITERATIONS = 100;
    const int MAX_SPRITES = 10;
    const int MAX_WIDTH = 32;
    const int MAX_HEIGHT = 32;
    int passed = 1;
    
    printf("  Property 4: SpritePack parsing completeness\n");
    printf("    Running %d iterations...\n", NUM_ITERATIONS);
    
    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        /* 임의의 스프라이트 개수를 생성한다 (1~MAX_SPRITES) */
        uint16_t expected_count = 1 + (spritepack_pbt_random() % MAX_SPRITES);

        /* 임시 spritepack 파일을 생성한다 */
        char* filename = create_temp_spritepack(expected_count, MAX_WIDTH, MAX_HEIGHT);
        if (!filename) {
            printf("    [FAIL] Iteration %d: Failed to create temp file\n", iter);
            passed = 0;
            break;
        }

        /* spritepack을 로드한다 */
        SpritePack pack;
        int result = spritepack_load(&pack, filename);

        if (result != 0) {
            printf("    [FAIL] Iteration %d: spritepack_load failed with code %d\n", iter, result);
            cleanup_temp_files(filename);
            passed = 0;
            break;
        }

        /* 스프라이트 개수를 검증한다 - 요구사항 4.1 */
        if (pack.count != expected_count) {
            printf("    [FAIL] Iteration %d: Count mismatch (expected %d, got %d)\n",
                   iter, expected_count, pack.count);
            spritepack_free(&pack);
            cleanup_temp_files(filename);
            passed = 0;
            break;
        }
        
        /* 각 스프라이트가 접근 가능하고 유효한지 검증한다 - 요구사항 4.2 */
        int sprite_error = 0;
        for (uint16_t i = 0; i < pack.count; i++) {
            Sprite* sprite = spritepack_get(&pack, i);
            if (!sprite) {
                printf("    [FAIL] Iteration %d: spritepack_get returned NULL for index %d\n",
                       iter, i);
                sprite_error = 1;
                break;
            }
            
            if (!sprite->is_valid) {
                printf("    [FAIL] Iteration %d: Sprite %d is not valid\n", iter, i);
                sprite_error = 1;
                break;
            }
            
            /* 크기가 예상 범위 내에 있는지 검증한다 */
            if (sprite->width == 0 || sprite->width > MAX_WIDTH ||
                sprite->height == 0 || sprite->height > MAX_HEIGHT) {
                printf("    [FAIL] Iteration %d: Sprite %d has invalid dimensions (%dx%d)\n",
                       iter, i, sprite->width, sprite->height);
                sprite_error = 1;
                break;
            }
        }
        
        spritepack_free(&pack);
        cleanup_temp_files(filename);
        
        if (sprite_error) {
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
 * 속성 4b: SpritePack 지연 로딩 완전성
 *
 * 인덱스가 있는 임의의 유효한 SpritePack 파일이라면, 지연 로딩이 스프라이트
 * 개수를 올바르게 읽어야 하고 임의의 스프라이트에 무작위 접근을 허용해야 한다.
 *
 * 검증 대상: 요구사항 4.3, 4.4
 */
static int test_property4b_spritepack_lazy_loading(void) {
    const int NUM_ITERATIONS = 50;
    const int MAX_SPRITES = 10;
    const int MAX_WIDTH = 32;
    const int MAX_HEIGHT = 32;
    int passed = 1;
    
    printf("  Property 4b: SpritePack lazy loading completeness\n");
    printf("    Running %d iterations...\n", NUM_ITERATIONS);
    
    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        /* 임의의 스프라이트 개수를 생성한다 (1~MAX_SPRITES) */
        uint16_t expected_count = 1 + (spritepack_pbt_random() % MAX_SPRITES);

        /* 인덱스가 있는 임시 spritepack 파일을 생성한다 */
        char* filename = create_temp_spritepack_with_index(expected_count, MAX_WIDTH, MAX_HEIGHT);
        if (!filename) {
            printf("    [FAIL] Iteration %d: Failed to create temp files\n", iter);
            passed = 0;
            break;
        }

        /* 지연 로딩으로 spritepack을 로드한다 */
        SpritePack pack;
        int result = spritepack_load_lazy(&pack, filename);

        if (result != 0) {
            printf("    [FAIL] Iteration %d: spritepack_load_lazy failed with code %d\n", iter, result);
            cleanup_temp_files(filename);
            passed = 0;
            break;
        }

        /* 스프라이트 개수를 검증한다 - 요구사항 4.4 */
        if (pack.count != expected_count) {
            printf("    [FAIL] Iteration %d: Count mismatch (expected %d, got %d)\n",
                   iter, expected_count, pack.count);
            spritepack_free(&pack);
            cleanup_temp_files(filename);
            passed = 0;
            break;
        }
        
        /* lazy_load 플래그가 설정되었는지 검증한다 */
        if (!pack.lazy_load) {
            printf("    [FAIL] Iteration %d: lazy_load flag not set\n", iter);
            spritepack_free(&pack);
            cleanup_temp_files(filename);
            passed = 0;
            break;
        }
        
        /* 지연 로딩을 테스트하기 위해 무작위 순서로 스프라이트에 접근한다 - 요구사항 4.3 */
        int sprite_error = 0;
        for (int access = 0; access < expected_count; access++) {
            /* 임의의 스프라이트 인덱스를 고른다 */
            uint16_t idx = spritepack_pbt_random() % expected_count;
            
            Sprite* sprite = spritepack_get(&pack, idx);
            if (!sprite) {
                printf("    [FAIL] Iteration %d: spritepack_get returned NULL for index %d\n",
                       iter, idx);
                sprite_error = 1;
                break;
            }
            
            if (!sprite->is_valid) {
                printf("    [FAIL] Iteration %d: Sprite %d is not valid after lazy load\n", iter, idx);
                sprite_error = 1;
                break;
            }
            
            /* 크기가 예상 범위 내에 있는지 검증한다 */
            if (sprite->width == 0 || sprite->width > MAX_WIDTH ||
                sprite->height == 0 || sprite->height > MAX_HEIGHT) {
                printf("    [FAIL] Iteration %d: Sprite %d has invalid dimensions (%dx%d)\n",
                       iter, idx, sprite->width, sprite->height);
                sprite_error = 1;
                break;
            }
        }
        
        spritepack_free(&pack);
        cleanup_temp_files(filename);
        
        if (sprite_error) {
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
 * 단위 테스트: 빈 spritepack 처리
 */
static void test_empty_spritepack(void) {
    printf("  Unit test: Empty spritepack handling\n");
    
    /* 빈 spritepack 파일을 생성한다 */
    const char* filename = "/tmp/test_empty_spritepack.spk";
    FILE* file = fopen(filename, "wb");
    if (file) {
        uint16_t count = 0;
        fwrite(&count, 2, 1, file);
        fclose(file);
        
        SpritePack pack;
        int result = spritepack_load(&pack, filename);
        
        test_assert(result == 0, "Empty spritepack loads successfully");
        test_assert_eq(0, pack.count, "Empty spritepack has zero count");
        
        spritepack_free(&pack);
        remove(filename);
    } else {
        test_assert(0, "Failed to create test file");
    }
}

/**
 * 단위 테스트: NULL 매개변수 처리
 */
static void test_null_parameters(void) {
    printf("  Unit test: NULL parameter handling\n");
    
    SpritePack pack;
    
    test_assert(spritepack_load(NULL, "test.spk") == -1,
                "spritepack_load returns -1 for NULL pack");
    test_assert(spritepack_load(&pack, NULL) == -1,
                "spritepack_load returns -1 for NULL filename");
    
    test_assert(spritepack_load_lazy(NULL, "test.spk") == -1,
                "spritepack_load_lazy returns -1 for NULL pack");
    test_assert(spritepack_load_lazy(&pack, NULL) == -1,
                "spritepack_load_lazy returns -1 for NULL filename");
    
    test_assert(spritepack_get(NULL, 0) == NULL,
                "spritepack_get returns NULL for NULL pack");
    
    /* NULL로 spritepack_free를 테스트한다 */
    spritepack_free(NULL);  /* 크래시가 발생하지 않아야 한다 */
    test_assert(1, "spritepack_free handles NULL gracefully");
}

/**
 * 단위 테스트: 잘못된 파일 처리
 */
static void test_invalid_file(void) {
    printf("  Unit test: Invalid file handling\n");
    
    SpritePack pack;
    
    test_assert(spritepack_load(&pack, "/nonexistent/path/file.spk") == -2,
                "spritepack_load returns -2 for nonexistent file");
    
    test_assert(spritepack_load_lazy(&pack, "/nonexistent/path/file.spk") == -5,
                "spritepack_load_lazy returns -5 for missing index file");
}

/**
 * 단위 테스트: 인덱스 범위 초과
 */
static void test_index_bounds(void) {
    printf("  Unit test: Index out of bounds\n");
    
    /* 작은 spritepack을 생성한다 */
    spritepack_pbt_seed = 12345;  /* 재현 가능하도록 고정 시드를 사용한다 */
    char* filename = create_temp_spritepack(3, 8, 8);
    
    if (filename) {
        SpritePack pack;
        int result = spritepack_load(&pack, filename);
        
        if (result == 0) {
            test_assert(spritepack_get(&pack, 0) != NULL, "Index 0 is valid");
            test_assert(spritepack_get(&pack, 2) != NULL, "Index 2 is valid");
            test_assert(spritepack_get(&pack, 3) == NULL, "Index 3 returns NULL (out of bounds)");
            test_assert(spritepack_get(&pack, 100) == NULL, "Index 100 returns NULL (out of bounds)");
            test_assert(spritepack_get(&pack, 0xFFFF) == NULL, "Index 0xFFFF returns NULL (out of bounds)");
            
            spritepack_free(&pack);
        } else {
            test_assert(0, "Failed to load test spritepack");
        }
        
        cleanup_temp_files(filename);
    } else {
        test_assert(0, "Failed to create test file");
    }
}

void test_spritepack_parsing(void) {
    int property4_passed;
    int property4b_passed;
    
    /* 속성 테스트를 위한 난수 시드를 초기화한다 */
    spritepack_pbt_init_seed();
    printf("  (PBT seed: %u)\n", spritepack_pbt_seed);
    
    /* 속성 기반 테스트를 실행한다 */
    printf("\n  --- Property-Based Tests ---\n");
    property4_passed = test_property4_spritepack_completeness();
    property4b_passed = test_property4b_spritepack_lazy_loading();
    
    /* 단위 테스트를 실행한다 */
    printf("\n  --- Unit Tests ---\n");
    test_empty_spritepack();
    test_null_parameters();
    test_invalid_file();
    test_index_bounds();
    
    /* 요약 */
    printf("\n  --- Property Test Summary ---\n");
    printf("  Property 4 (SpritePack parsing completeness): %s\n", 
           property4_passed ? "PASSED" : "FAILED");
    printf("  Property 4b (SpritePack lazy loading): %s\n", 
           property4b_passed ? "PASSED" : "FAILED");
}
