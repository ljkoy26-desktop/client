/**
 * @file test_frame.c
 * @brief Frame과 FrameArray의 단위 테스트 및 속성 기반 테스트
 *
 * 속성 1: FrameArray 데이터 일관성
 * 임의의 FrameArray에 대해, 프레임을 추가한 뒤 크기가 일치해야 하고
 * 모든 프레임을 올바른 값으로 조회할 수 있어야 한다.
 *
 * 속성 2: Frame 데이터 저장
 * 임의의 Frame에 대해, 값을 설정한 뒤 getter가 동일한 값을 반환해야 한다.
 *
 * 검증 대상: 요구사항 3.4, 3.5, 4.6
 */

#include "frame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 테스트 헬퍼 선언 */
void test_assert(int condition, const char* message);
void test_assert_eq(int expected, int actual, const char* message);

/* 속성 테스트를 위한 간단한 의사난수 생성기 */
static unsigned int frame_pbt_seed = 0;

static void frame_pbt_init_seed(void) {
    frame_pbt_seed = (unsigned int)time(NULL);
}

static unsigned int frame_pbt_random(void) {
    frame_pbt_seed = frame_pbt_seed * 1103515245 + 12345;
    return frame_pbt_seed;
}

static uint16_t frame_pbt_random_uint16(void) {
    return (uint16_t)(frame_pbt_random() >> 16);
}

static int16_t frame_pbt_random_int16(void) {
    return (int16_t)(frame_pbt_random() >> 16);
}

/* ============================================================================
 * 속성 1: FrameArray 데이터 일관성
 *
 * 임의의 FrameArray에 대해, 프레임을 추가한 뒤 크기가 일치해야 하고
 * 모든 프레임을 올바른 값으로 조회할 수 있어야 한다.
 *
 * 검증 대상: 요구사항 4.6
 * ============================================================================ */

static int test_property1_framearray_consistency(void) {
    const int NUM_ITERATIONS = 100;
    const int MAX_SIZE = 256;
    int passed = 1;
    
    printf("  Property 1: FrameArray data consistency\n");
    printf("    Running %d iterations...\n", NUM_ITERATIONS);
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        /* 임의의 크기를 생성한다 (1~MAX_SIZE) */
        int size = 1 + (frame_pbt_random() % MAX_SIZE);
        
        /* FrameArray를 생성한다 */
        FrameArray arr;
        frame_array_init(&arr);
        vecInit(&arr, size);
        
        /* 임의의 Frame 값을 생성하고 저장한다 */
        TYPE_SPRITEID* expected_ids = (TYPE_SPRITEID*)malloc(size * sizeof(TYPE_SPRITEID));
        int16_t* expected_cx = (int16_t*)malloc(size * sizeof(int16_t));
        int16_t* expected_cy = (int16_t*)malloc(size * sizeof(int16_t));
        
        for (int j = 0; j < size; j++) {
            expected_ids[j] = frame_pbt_random_uint16();
            expected_cx[j] = frame_pbt_random_int16();
            expected_cy[j] = frame_pbt_random_int16();
            
            Frame f;
            frame_init(&f, expected_ids[j], expected_cx[j], expected_cy[j]);
            vecAppend(&arr, f);
        }
        
        /* 크기를 검증한다 */
        if (frame_array_size(&arr) != size) {
            printf("    [FAIL] Iteration %d: Size mismatch\n", i);
            printf("           Expected: %d, Got: %d\n", size, frame_array_size(&arr));
            passed = 0;
            free(expected_ids);
            free(expected_cx);
            free(expected_cy);
            frame_array_free(&arr);
            break;
        }
        
        /* 모든 값을 올바르게 조회할 수 있는지 검증한다 */
        int mismatch = 0;
        for (int j = 0; j < size; j++) {
            Frame* f = frame_array_get(&arr, j);
            if (f == NULL ||
                f->sprite_id != expected_ids[j] ||
                f->cx != expected_cx[j] ||
                f->cy != expected_cy[j]) {
                printf("    [FAIL] Iteration %d: Value mismatch at index %d\n", i, j);
                if (f != NULL) {
                    printf("           Expected: ID=%d, CX=%d, CY=%d\n",
                           expected_ids[j], expected_cx[j], expected_cy[j]);
                    printf("           Got: ID=%d, CX=%d, CY=%d\n",
                           f->sprite_id, f->cx, f->cy);
                }
                mismatch = 1;
                break;
            }
        }
        
        free(expected_ids);
        free(expected_cx);
        free(expected_cy);
        frame_array_free(&arr);
        
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

/* ============================================================================
 * 속성 2: Frame 데이터 저장
 *
 * 임의의 Frame에 대해, 값을 설정한 뒤 getter가 동일한 값을 반환해야 한다.
 *
 * 검증 대상: 요구사항 3.4, 3.5
 * ============================================================================ */

static int test_property2_frame_storage(void) {
    const int NUM_ITERATIONS = 100;
    int passed = 1;
    
    printf("  Property 2: Frame data storage\n");
    printf("    Running %d iterations...\n", NUM_ITERATIONS);
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        /* 임의의 값을 생성한다 */
        TYPE_SPRITEID sprite_id = frame_pbt_random_uint16();
        int16_t cx = frame_pbt_random_int16();
        int16_t cy = frame_pbt_random_int16();
        
        /* frame_init을 테스트한다 */
        Frame frame;
        frame_init(&frame, sprite_id, cx, cy);
        
        if (frame.sprite_id != sprite_id ||
            frame.cx != cx ||
            frame.cy != cy) {
            printf("    [FAIL] Iteration %d: frame_init mismatch\n", i);
            printf("           Expected: ID=%d, CX=%d, CY=%d\n", sprite_id, cx, cy);
            printf("           Got: ID=%d, CX=%d, CY=%d\n",
                   frame.sprite_id, frame.cx, frame.cy);
            passed = 0;
            break;
        }
    }
    
    if (passed) {
        printf("    [PASS] All %d iterations passed\n", NUM_ITERATIONS);
    }
    
    return passed;
}

/* ============================================================================
 * 단위 테스트
 * ============================================================================ */

/**
 * 단위 테스트: Frame 초기화
 */
static void test_frame_init(void) {
    printf("  Unit test: Frame init\n");
    
    Frame frame;
    frame_init(&frame, 12345, -100, 200);
    
    test_assert_eq(12345, frame.sprite_id, "SpriteID set correctly");
    test_assert_eq(-100, frame.cx, "CX set correctly");
    test_assert_eq(200, frame.cy, "CY set correctly");
}

/**
 * 단위 테스트: Frame 파일 입출력
 */
static void test_frame_file_io(void) {
    printf("  Unit test: Frame file I/O\n");
    
    const char* test_file = "/tmp/test_frame.bin";
    
    /* Frame을 생성하고 저장한다 */
    Frame frame1;
    frame_init(&frame1, 12345, -100, 200);
    
    FILE* file = fopen(test_file, "wb");
    if (file) {
        int save_result = frame_save(&frame1, file);
        fclose(file);
        test_assert(save_result, "frame_save succeeds");
        
        /* Frame을 로드한다 */
        Frame frame2;
        file = fopen(test_file, "rb");
        if (file) {
            int load_result = frame_load(&frame2, file);
            fclose(file);
            test_assert(load_result, "frame_load succeeds");
            
            test_assert_eq(12345, frame2.sprite_id, "Loaded sprite_id matches");
            test_assert_eq(-100, frame2.cx, "Loaded cx matches");
            test_assert_eq(200, frame2.cy, "Loaded cy matches");
        } else {
            test_assert(0, "Failed to open file for reading");
        }
        
        /* 정리한다 */
        remove(test_file);
    } else {
        test_assert(0, "Failed to open file for writing");
    }
}

/**
 * 단위 테스트: FrameArray 초기화와 해제
 */
static void test_framearray_init_free(void) {
    printf("  Unit test: FrameArray init and free\n");
    
    FrameArray arr;
    frame_array_init(&arr);
    
    test_assert_eq(0, frame_array_size(&arr), "Initial size is 0");
    
    /* 몇 개의 Frame을 추가한다 */
    Frame f1, f2, f3;
    frame_init(&f1, 100, 10, 20);
    frame_init(&f2, 200, 30, 40);
    frame_init(&f3, 300, 50, 60);
    
    vecAppend(&arr, f1);
    vecAppend(&arr, f2);
    vecAppend(&arr, f3);
    
    test_assert_eq(3, frame_array_size(&arr), "Size after adding 3 frames");
    
    Frame* pf = frame_array_get(&arr, 1);
    test_assert(pf != NULL, "frame_array_get returns non-NULL");
    test_assert_eq(200, pf->sprite_id, "Frame at index 1 has correct sprite_id");
    
    frame_array_free(&arr);
    test_assert_eq(0, frame_array_size(&arr), "Size after free is 0");
}

/**
 * 단위 테스트: FrameArray 파일 입출력
 */
static void test_framearray_file_io(void) {
    printf("  Unit test: FrameArray file I/O\n");
    
    const char* test_file = "/tmp/test_framearray.bin";
    
    /* 배열을 생성하고 저장한다 */
    FrameArray arr1;
    frame_array_init(&arr1);
    
    Frame f1, f2, f3;
    frame_init(&f1, 100, 10, 20);
    frame_init(&f2, 200, 30, 40);
    frame_init(&f3, 300, 50, 60);
    
    vecAppend(&arr1, f1);
    vecAppend(&arr1, f2);
    vecAppend(&arr1, f3);
    
    FILE* file = fopen(test_file, "wb");
    if (file) {
        int save_result = frame_array_save(&arr1, file);
        fclose(file);
        test_assert(save_result, "frame_array_save succeeds");
        
        /* 배열을 로드한다 */
        FrameArray arr2;
        file = fopen(test_file, "rb");
        if (file) {
            int load_result = frame_array_load(&arr2, file);
            fclose(file);
            test_assert(load_result, "frame_array_load succeeds");
            
            test_assert_eq(3, frame_array_size(&arr2), "Loaded array has correct size");
            
            Frame* pf = frame_array_get(&arr2, 0);
            test_assert_eq(100, pf->sprite_id, "Element 0 loaded correctly");
            
            pf = frame_array_get(&arr2, 1);
            test_assert_eq(200, pf->sprite_id, "Element 1 loaded correctly");
            
            pf = frame_array_get(&arr2, 2);
            test_assert_eq(300, pf->sprite_id, "Element 2 loaded correctly");
            
            frame_array_free(&arr2);
        } else {
            test_assert(0, "Failed to open file for reading");
        }
        
        /* 정리한다 */
        remove(test_file);
    } else {
        test_assert(0, "Failed to open file for writing");
    }
    
    frame_array_free(&arr1);
}

/**
 * 단위 테스트: 중첩 배열 (DirectionArray)
 */
static void test_nested_arrays(void) {
    printf("  Unit test: Nested arrays (DirectionArray)\n");
    
    /* DirectionArray를 생성한다 (8방향) */
    DirectionArray dir_arr;
    direction_array_init(&dir_arr);
    
    /* 8개 방향을 추가한다 */
    for (int d = 0; d < 8; d++) {
        FrameArray fa;
        frame_array_init(&fa);
        
        /* 각 방향은 d+1개의 프레임을 가진다 */
        for (int f = 0; f < d + 1; f++) {
            Frame frame;
            frame_init(&frame, d * 100 + f, d, f);
            vecAppend(&fa, frame);
        }
        
        vecAppend(&dir_arr, fa);
    }
    
    test_assert_eq(8, direction_array_size(&dir_arr), "Direction array has 8 elements");
    
    /* 구조를 검증한다 */
    FrameArray* fa0 = direction_array_get(&dir_arr, 0);
    test_assert_eq(1, frame_array_size(fa0), "Direction 0 has 1 frame");
    
    FrameArray* fa7 = direction_array_get(&dir_arr, 7);
    test_assert_eq(8, frame_array_size(fa7), "Direction 7 has 8 frames");
    
    Frame* f = frame_array_get(fa0, 0);
    test_assert_eq(0, f->sprite_id, "Direction 0, Frame 0 has correct ID");
    
    f = frame_array_get(fa7, 5);
    test_assert_eq(705, f->sprite_id, "Direction 7, Frame 5 has correct ID");
    
    direction_array_free(&dir_arr);
}

/* ============================================================================
 * 메인 테스트 함수
 * ============================================================================ */

void test_frame_and_tarray(void) {
    int property1_passed;
    int property2_passed;
    
    /* 속성 테스트를 위한 난수 시드를 초기화한다 */
    frame_pbt_init_seed();
    printf("  (PBT seed: %u)\n", frame_pbt_seed);
    
    /* 속성 기반 테스트를 실행한다 */
    printf("\n  --- Property-Based Tests ---\n");
    property1_passed = test_property1_framearray_consistency();
    property2_passed = test_property2_frame_storage();
    
    /* 단위 테스트를 실행한다 */
    printf("\n  --- Unit Tests ---\n");
    test_frame_init();
    test_frame_file_io();
    test_framearray_init_free();
    test_framearray_file_io();
    test_nested_arrays();
    
    /* 요약 */
    printf("\n  --- Property Test Summary ---\n");
    printf("  Property 1 (FrameArray data consistency): %s\n", 
           property1_passed ? "PASSED" : "FAILED");
    printf("  Property 2 (Frame data storage): %s\n", 
           property2_passed ? "PASSED" : "FAILED");
}
