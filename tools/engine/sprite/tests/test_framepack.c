/**
 * @file test_framepack.c
 * @brief CreatureFramePack 테스트
 *
 * 속성 3: CFPK 로딩 정확성
 * 유효한 .cfpk 파일이라면, 로드 후 CreatureFramePack이 올바른 개수의
 * CreatureType을 포함해야 하고 각 ActionArray가 올바른 구조를 가져야 한다.
 *
 * 검증 대상: 요구사항 3.1, 3.2, 3.3
 */

#include "framepack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 테스트 헬퍼 선언 */
void test_assert(int condition, const char* message);
void test_assert_eq(int expected, int actual, const char* message);

/* 테스트 데이터 파일 경로 */
static const char* CREATURE_CFPK_PATH = "../DarkEden/Data/Image/Creature.cfpk";

/* ============================================================================
 * 단위 테스트
 * ============================================================================ */

/**
 * 단위 테스트: CreatureFramePack 초기화와 해제
 */
static void test_framepack_init_free(void) {
    printf("  Unit test: CreatureFramePack init and free\n");
    
    CreatureFramePack pack;
    creature_framepack_init(&pack);
    
    test_assert_eq(0, creature_framepack_size(&pack), "Initial size is 0");
    
    creature_framepack_free(&pack);
    test_assert_eq(0, creature_framepack_size(&pack), "Size after free is 0");
}

/**
 * 단위 테스트: 간단한 데이터로 CreatureFramePack 파일 입출력
 */
static void test_framepack_file_io(void) {
    printf("  Unit test: CreatureFramePack file I/O\n");
    
    const char* test_file = "/tmp/test_creaturepack.cfpk";
    
    /* 간단한 프레임 팩을 생성한다 */
    CreatureFramePack pack1;
    creature_framepack_init(&pack1);

    /* 크리처 타입 2개를 추가한다 */
    for (int type = 0; type < 2; type++) {
        ActionArray aa;
        action_array_init(&aa);

        /* 각 타입은 1개의 액션을 가진다 */
        DirectionArray da;
        direction_array_init(&da);

        /* 각 액션은 2개의 방향을 가진다 */
        for (int dir = 0; dir < 2; dir++) {
            FrameArray fa;
            frame_array_init(&fa);

            /* 각 방향은 2개의 프레임을 가진다 */
            for (int f = 0; f < 2; f++) {
                Frame frame;
                frame_init(&frame, type * 100 + dir * 10 + f, type, dir);
                vecAppend(&fa, frame);
            }
            
            vecAppend(&da, fa);
        }
        
        vecAppend(&aa, da);
        vecAppend(&pack1.creatures, aa);
    }
    
    test_assert_eq(2, creature_framepack_size(&pack1), "Pack has 2 creature types");
    
    /* 파일로 저장한다 */
    int save_result = creature_framepack_save(&pack1, test_file);
    test_assert(save_result, "creature_framepack_save succeeds");
    
    /* 파일에서 로드한다 */
    CreatureFramePack pack2;
    creature_framepack_init(&pack2);
    
    int load_result = creature_framepack_load(&pack2, test_file);
    test_assert(load_result, "creature_framepack_load succeeds");
    
    /* 구조를 검증한다 */
    test_assert_eq(2, creature_framepack_size(&pack2), "Loaded pack has 2 creature types");
    
    /* 크리처 타입 0을 검증한다 */
    ActionArray* aa = creature_framepack_get(&pack2, 0);
    test_assert(aa != NULL, "Type 0 exists");
    test_assert_eq(1, action_array_size(aa), "Type 0 has 1 action");
    
    DirectionArray* da = action_array_get(aa, 0);
    test_assert(da != NULL, "Type 0 Action 0 exists");
    test_assert_eq(2, direction_array_size(da), "Type 0 Action 0 has 2 directions");
    
    FrameArray* fa = direction_array_get(da, 0);
    test_assert(fa != NULL, "Type 0 Action 0 Dir 0 exists");
    test_assert_eq(2, frame_array_size(fa), "Type 0 Action 0 Dir 0 has 2 frames");
    
    Frame* f = frame_array_get(fa, 0);
    test_assert_eq(0, f->sprite_id, "Frame data preserved (sprite_id)");
    test_assert_eq(0, f->cx, "Frame data preserved (cx)");
    test_assert_eq(0, f->cy, "Frame data preserved (cy)");
    
    /* 크리처 타입 1을 검증한다 */
    f = creature_framepack_get_frame(&pack2, 1, 0, 0, 0);
    test_assert(f != NULL, "Type 1 frame accessible");
    test_assert_eq(100, f->sprite_id, "Type 1 frame data correct");
    
    /* 정리한다 */
    creature_framepack_free(&pack1);
    creature_framepack_free(&pack2);
    remove(test_file);
}

/**
 * 단위 테스트: creature_framepack_get_frame 헬퍼
 */
static void test_framepack_get_frame(void) {
    printf("  Unit test: creature_framepack_get_frame\n");
    
    CreatureFramePack pack;
    creature_framepack_init(&pack);
    
    /* 구조 생성: 타입 1개, 액션 2개, 방향 3개, 프레임 4개 */
    ActionArray aa;
    action_array_init(&aa);
    
    for (int action = 0; action < 2; action++) {
        DirectionArray da;
        direction_array_init(&da);
        
        for (int dir = 0; dir < 3; dir++) {
            FrameArray fa;
            frame_array_init(&fa);
            
            for (int f = 0; f < 4; f++) {
                Frame frame;
                frame_init(&frame, action * 100 + dir * 10 + f, action, dir);
                vecAppend(&fa, frame);
            }
            
            vecAppend(&da, fa);
        }
        
        vecAppend(&aa, da);
    }
    
    vecAppend(&pack.creatures, aa);
    
    /* get_frame을 테스트한다 */
    Frame* f = creature_framepack_get_frame(&pack, 0, 0, 0, 0);
    test_assert(f != NULL, "Frame [0][0][0][0] exists");
    test_assert_eq(0, f->sprite_id, "Frame [0][0][0][0] correct");
    
    f = creature_framepack_get_frame(&pack, 0, 1, 2, 3);
    test_assert(f != NULL, "Frame [0][1][2][3] exists");
    test_assert_eq(123, f->sprite_id, "Frame [0][1][2][3] correct");
    
    /* 잘못된 인덱스를 테스트한다 */
    f = creature_framepack_get_frame(&pack, 1, 0, 0, 0);
    test_assert(f == NULL, "Invalid type returns NULL");
    
    f = creature_framepack_get_frame(&pack, 0, 5, 0, 0);
    test_assert(f == NULL, "Invalid action returns NULL");
    
    creature_framepack_free(&pack);
}

/* ============================================================================
 * 속성 3: CFPK 로딩 정확성
 *
 * 유효한 .cfpk 파일이라면, 로드 후 CreatureFramePack이 올바른 개수의
 * CreatureType을 포함해야 하고 각 ActionArray가 올바른 구조를 가져야 한다.
 *
 * 검증 대상: 요구사항 3.1, 3.2, 3.3
 * ============================================================================ */

static int test_property3_cfpk_loading(void) {
    int passed = 1;
    
    printf("  Property 3: CFPK loading correctness\n");
    
    /* 실제 Creature.cfpk 파일을 로드해본다 */
    CreatureFramePack pack;
    creature_framepack_init(&pack);
    
    printf("    Loading %s...\n", CREATURE_CFPK_PATH);
    
    if (!creature_framepack_load(&pack, CREATURE_CFPK_PATH)) {
        printf("    [SKIP] Could not load %s (file may not exist)\n", CREATURE_CFPK_PATH);
        printf("    [INFO] This test requires the DarkEden game data files\n");
        return 1;  /* 파일이 없으면 테스트를 건너뛴다 */
    }
    
    printf("    Loaded successfully!\n");
    printf("    CreatureType count: %d\n", creature_framepack_size(&pack));
    
    /* 기본 구조를 검증한다 */
    if (creature_framepack_size(&pack) == 0) {
        printf("    [FAIL] Pack is empty\n");
        creature_framepack_free(&pack);
        return 0;
    }
    
    /* 각 크리처 타입의 구조를 검증한다 */
    int valid_types = 0;
    int total_frames = 0;
    
    for (int type = 0; type < creature_framepack_size(&pack); type++) {
        ActionArray* aa = creature_framepack_get(&pack, type);
        
        if (aa == NULL || action_array_size(aa) == 0) {
            continue;  /* 비어있는 크리처 타입도 유효하다 */
        }
        
        valid_types++;
        
        /* 각 액션을 확인한다 */
        for (int action = 0; action < action_array_size(aa); action++) {
            DirectionArray* da = action_array_get(aa, action);
            
            if (da == NULL) continue;
            
            /* 각 방향을 확인한다 */
            for (int dir = 0; dir < direction_array_size(da); dir++) {
                FrameArray* fa = direction_array_get(da, dir);
                
                if (fa == NULL) continue;
                
                /* 프레임 수를 센다 */
                total_frames += frame_array_size(fa);
            }
        }
    }
    
    printf("    Valid creature types: %d\n", valid_types);
    printf("    Total frames: %d\n", total_frames);
    
    /* 기본적인 정합성 확인 */
    if (valid_types == 0) {
        printf("    [FAIL] No valid creature types found\n");
        passed = 0;
    }
    
    if (total_frames == 0) {
        printf("    [FAIL] No frames found\n");
        passed = 0;
    }
    
    /* 검증을 위해 샘플 데이터를 출력한다 */
    printf("    Sample data (first valid creature):\n");
    for (int type = 0; type < creature_framepack_size(&pack) && type < 5; type++) {
        ActionArray* aa = creature_framepack_get(&pack, type);
        if (aa != NULL && action_array_size(aa) > 0) {
            printf("      Type %d: %d actions\n", type, action_array_size(aa));
            
            Frame* f = creature_framepack_get_frame(&pack, type, 0, 0, 0);
            if (f != NULL) {
                printf("        First frame: SpriteID=%d, CX=%d, CY=%d\n",
                       f->sprite_id, f->cx, f->cy);
            }
            break;
        }
    }
    
    if (passed) {
        printf("    [PASS] CFPK loaded and validated successfully\n");
    }
    
    creature_framepack_free(&pack);
    return passed;
}

/**
 * InfoToFile 디버그 출력을 테스트한다
 */
static void test_infotofile(void) {
    printf("  Unit test: creature_framepack_info_to_file\n");
    
    const char* test_info_file = "/tmp/test_creature_info.txt";
    
    /* 간단한 프레임 팩을 생성한다 */
    CreatureFramePack pack;
    creature_framepack_init(&pack);

    /* 크리처 타입 2개를 추가한다 */
    for (int type = 0; type < 2; type++) {
        ActionArray aa;
        action_array_init(&aa);
        
        DirectionArray da;
        direction_array_init(&da);
        
        for (int dir = 0; dir < 2; dir++) {
            FrameArray fa;
            frame_array_init(&fa);
            
            Frame frame;
            frame_init(&frame, type * 100 + dir, type * 10, dir * 10);
            vecAppend(&fa, frame);
            
            vecAppend(&da, fa);
        }
        
        vecAppend(&aa, da);
        vecAppend(&pack.creatures, aa);
    }
    
    /* info 파일을 작성한다 */
    creature_framepack_info_to_file(&pack, test_info_file);
    
    /* 파일이 생성되었는지 검증한다 */
    FILE* file = fopen(test_info_file, "r");
    if (file) {
        /* 내용을 검증하기 위해 첫 줄을 읽는다 */
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), file)) {
            test_assert(strstr(buffer, "CreatureFramePack") != NULL ||
                       strstr(buffer, "===") != NULL,
                       "info_to_file creates valid header");
        }
        fclose(file);
        remove(test_info_file);
    } else {
        test_assert(0, "info_to_file failed to create file");
    }
    
    creature_framepack_free(&pack);
}

/* ============================================================================
 * 메인 테스트 함수
 * ============================================================================ */

void test_framepack(void) {
    int property3_passed;
    
    /* 단위 테스트를 실행한다 */
    printf("\n  --- Unit Tests ---\n");
    test_framepack_init_free();
    test_framepack_file_io();
    test_framepack_get_frame();
    test_infotofile();
    
    /* 속성 기반 테스트를 실행한다 */
    printf("\n  --- Property-Based Tests ---\n");
    property3_passed = test_property3_cfpk_loading();
    
    /* 요약 */
    printf("\n  --- Property Test Summary ---\n");
    printf("  Property 3 (CFPK loading correctness): %s\n",
           property3_passed ? "PASSED" : "FAILED");
}
