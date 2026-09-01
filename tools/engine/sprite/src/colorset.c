/**
 * @file colorset.c
 * @brief ColorSet 시스템 구현부
 *
 * 요구사항: 6.1, 6.2, 6.3, 6.4
 *
 * IndexedSprite가 크리처 색상 변화를 표현할 때 사용하는 ColorSet 색상
 * 조회 시스템을 구현한다. 이 시스템은 495개의 ColorSet
 * (33개 시드 × 15개 변형)을 지원하며 각각 30단계의 그라데이션을 갖는다.
 */

#include "colorset.h"
#include <string.h>

/* ============================================================================
 * 전역 테이블
 * ============================================================================ */

uint16_t g_ColorSet[MAX_COLORSET][MAX_COLORGRADATION];
uint16_t g_GradationValue[MAX_COLORGRADATION];
uint16_t g_ColorSetDarkness[MAX_DARKBIT][MAX_COLORSET][MAX_COLORGRADATION];
uint8_t g_ColorToGradation[MAX_COLOR_TO_GRADATION];
int g_IndexValue[MAX_COLORSET_USE];

/* ============================================================================
 * RGB565 헬퍼 함수
 * ============================================================================ */

uint16_t colorset_make_color(uint8_t r, uint8_t g, uint8_t b) {
    /* RGB565: RRRRR GGGGGG BBBBB
     * Red:   11-15번 비트에 5비트
     * Green: 5-10번 비트에 6비트 (5비트를 6비트로 스케일: g * 2)
     * Blue:  0-4번 비트에 5비트
     */
    return ((uint16_t)(r & 0x1F) << 11) |
           ((uint16_t)(g & 0x1F) << 6) |  /* 참고: 5비트 green을 6비트 위치로 시프트해서 사용 */
           ((uint16_t)(b & 0x1F));
}

uint8_t colorset_get_red(uint16_t color) {
    return (color >> 11) & 0x1F;
}

uint8_t colorset_get_green(uint16_t color) {
    /* 6비트 green을 추출해서 일관성을 위해 5비트로 변환 */
    return (color >> 6) & 0x1F;
}

uint8_t colorset_get_blue(uint16_t color) {
    return color & 0x1F;
}

/* ============================================================================
 * 내부 헬퍼 함수
 * ============================================================================ */

/**
 * 두 RGB 지점 사이의 그라데이션 색상을 생성한다
 *
 * @param pColor 색상을 저장할 출력 배열
 * @param step 그라데이션 단계 수
 * @param r0,g0,b0 시작 색상 (5비트 성분)
 * @param r1,g1,b1 끝 색상 (5비트 성분)
 */
static void get_index_color(uint16_t* pColor, int step,
                           int r0, int g0, int b0,
                           int r1, int g1, int b1) {
    float r = (float)r0;
    float g = (float)g0;
    float b = (float)b0;
    
    float step_1 = (float)step - 1.0f;
    float sr = (r1 - r0) / step_1;
    float sg = (g1 - g0) / step_1;
    float sb = (b1 - b0) / step_1;
    
    for (int i = 0; i < step; i++) {
        uint8_t red   = (uint8_t)r;
        uint8_t green = (uint8_t)g;
        uint8_t blue  = (uint8_t)b;
        
        *pColor++ = colorset_make_color(red, green, blue);
        
        r += sr;
        g += sg;
        b += sb;
    }
}

/* ============================================================================
 * 공개 함수
 * ============================================================================ */

void colorset_init(void) {
    int i, j, k, set;
    uint16_t color;

    /* 33개 시드 색상 (RGB 5비트 성분) */
    static const uint8_t rgbPoint[MAX_COLORSET_SEED][3] = {
        { 0, 0, 31 },       /* 파랑 */
        { 0, 31, 0 },       /* 초록 */
        { 31, 0, 0 },       /* 빨강 */
        { 0, 31, 31 },      /* 청록 */
        { 31, 0, 31 },      /* 자홍 */
        { 31, 31, 0 },      /* 노랑 */

        { 0, 0, 16 },       /* 어두운 파랑 */
        { 0, 16, 0 },       /* 어두운 초록 */
        { 16, 0, 0 },       /* 어두운 빨강 */
        { 0, 16, 16 },      /* 어두운 청록 */
        { 16, 0, 16 },      /* 어두운 자홍 */
        { 16, 16, 0 },      /* 어두운 노랑 */

        { 16, 31, 0 },      /* 라임 */
        { 16, 0, 31 },      /* 보라 */
        { 31, 16, 0 },      /* 주황 */
        { 0, 16, 31 },      /* 하늘색 */
        { 31, 0, 16 },      /* 분홍 */
        { 0, 31, 16 },      /* 스프링 그린 */

        { 16, 31, 16 },     /* 연한 초록 */
        { 16, 16, 31 },     /* 연한 파랑 */
        { 31, 16, 16 },     /* 연한 빨강 */

        { 16, 31, 31 },     /* 연한 청록 */
        { 31, 16, 31 },     /* 연한 자홍 */
        { 31, 31, 16 },     /* 연한 노랑 */

        { 16, 16, 16 },     /* 회색 */
        { 24, 24, 24 },     /* 밝은 회색 */
        { 8, 8, 8 },        /* 어두운 회색 */

        { 30, 24, 18 },     /* 피부색 1 */
        { 25, 15, 11 },     /* 피부색 2 */
        { 21, 12, 11 },     /* 피부색 3 */
        { 19, 15, 13 },     /* 피부색 4 */

        { 21, 18, 11 },     /* 갈색 1 */
        { 22, 16, 9 }       /* 갈색 2 */
    };

    /* index 값들을 0으로 초기화 */
    memset(g_IndexValue, 0, sizeof(g_IndexValue));

    /* ColorSet 테이블 생성 */
    set = 0;

    for (i = 0; i < MAX_COLORSET_SEED; i++) {
        int r = rgbPoint[i][0];
        int g = rgbPoint[i][1];
        int b = rgbPoint[i][2];

        /* 시드당 MAX_COLORSET_SEED_MODIFY개의 변형을 생성 */
        for (j = MAX_COLORGRADATION_HALF; j >= 1; j--) {
            /* 첫 번째 행: 흰색에서 시드 색상까지의 그라데이션 */
            if (j == MAX_COLORGRADATION_HALF) {
                get_index_color(g_ColorSet[set], j,
                               31, 31, 31,  /* 흰색 */
                               r, g, b);    /* 시드 색상 */
            } else {
                /* 더 부드러운 그라데이션을 위해 기본 세트의 색상을 사용 */
                color = g_ColorSet[i * MAX_COLORSET_SEED_MODIFY][MAX_COLORGRADATION_HALF - j];
                int r0 = colorset_get_red(color);
                int g0 = colorset_get_green(color);
                int b0 = colorset_get_blue(color);

                get_index_color(g_ColorSet[set], j,
                               r0, g0, b0,
                               r, g, b);
            }

            /* 두 번째 절반: 시드 색상에서 검정까지의 그라데이션 */
            get_index_color(g_ColorSet[set] + j, MAX_COLORGRADATION - j,
                           r, g, b,
                           0, 0, 0);  /* 검정 */

            set++;
        }
    }

    /* GradationValue 테이블 생성 */
    for (j = 0; j < MAX_COLORGRADATION; j++) {
        color = g_ColorSet[0][j];
        g_GradationValue[j] = colorset_get_red(color) +
                              colorset_get_green(color) +
                              colorset_get_blue(color);
    }

    /* Darkness 테이블 생성 */
    for (i = 0; i < MAX_COLORSET; i++) {
        for (j = 0; j < MAX_COLORGRADATION; j++) {
            color = g_ColorSet[i][j];

            for (k = 0; k < MAX_DARKBIT; k++) {
                int dr = (colorset_get_red(color) >> k);
                int dg = (colorset_get_green(color) >> k);
                int db = (colorset_get_blue(color) >> k);
                g_ColorSetDarkness[k][i][j] = colorset_make_color(dr, dg, db);
            }
        }
    }

    /* ColorToGradation 테이블 생성 */
    for (uint8_t cg = 0; cg < MAX_COLOR_TO_GRADATION; cg++) {
        g_ColorToGradation[cg] = colorset_color_to_gradation(cg);
    }
}

void colorset_set_using(int set1, int set2) {
    g_IndexValue[0] = set1;
    g_IndexValue[1] = set2;
}

void colorset_set_using_only(uint8_t setNumber, int colorSet) {
    g_IndexValue[setNumber] = colorSet;
}

int colorset_get_using(uint8_t n) {
    return g_IndexValue[n];
}

uint8_t colorset_get_index_color(uint16_t color) {
    /* R+G+B 합으로 그라데이션 계산 */
    uint16_t spriteGradation = colorset_get_red(color) +
                               colorset_get_green(color) +
                               colorset_get_blue(color);
    
    if (spriteGradation >= MAX_COLOR_TO_GRADATION) {
        spriteGradation = MAX_COLOR_TO_GRADATION - 1;
    }
    
    return g_ColorToGradation[spriteGradation];
}

uint8_t colorset_color_to_gradation(uint8_t spriteGradation) {
    int g;

    /* 가장 가까운 그라데이션 값을 찾는다 */
    for (g = 0; g < MAX_COLORGRADATION; g++) {
        if (spriteGradation > g_GradationValue[g]) {
            break;
        }
    }

    /* 경계 케이스 처리 */
    if (g == 0 || g == MAX_COLORGRADATION - 1) {
        return (uint8_t)g;
    }

    /* 더 가까운 값 찾기 */
    uint16_t value1 = g_GradationValue[g - 1] - spriteGradation;
    uint16_t value2 = spriteGradation - g_GradationValue[g];
    
    if (value1 < value2) {
        return (uint8_t)(g - 1);
    } else if (value1 > value2) {
        return (uint8_t)g;
    }
    
    return (uint8_t)(g - 1);
}

int colorset_load_from_file(FILE* file) {
    int i, j, k;
    int cg, cs, db;
    
    if (!file) {
        return 0;
    }
    
    /* 헤더 값 읽기 */
    if (fread(&cg, 4, 1, file) != 1) return 0;
    if (fread(&cs, 4, 1, file) != 1) return 0;
    if (fread(&db, 4, 1, file) != 1) return 0;

    /* 헤더 검증 (호환성을 위해 불일치 허용) */
    /* 원본 코드도 불일치 시 실패하지 않는다 */

    /* ColorSet 테이블 읽기 */
    for (i = 0; i < MAX_COLORSET; i++) {
        for (j = 0; j < MAX_COLORGRADATION; j++) {
            if (fread(&g_ColorSet[i][j], 2, 1, file) != 1) return 0;
        }
    }

    /* GradationValue 테이블 읽기 (참고: 원본은 MAX_COLORSET개 항목을 읽는다) */
    for (i = 0; i < MAX_COLORSET; i++) {
        uint16_t dummy;
        if (fread(&dummy, 2, 1, file) != 1) return 0;
        if (i < MAX_COLORGRADATION) {
            g_GradationValue[i] = dummy;
        }
    }

    /* ColorSetDarkness 테이블 읽기 */
    for (k = 0; k < MAX_DARKBIT; k++) {
        for (i = 0; i < MAX_COLORSET; i++) {
            for (j = 0; j < MAX_COLORGRADATION; j++) {
                if (fread(&g_ColorSetDarkness[k][i][j], 2, 1, file) != 1) return 0;
            }
        }
    }
    
    return 1;
}

int colorset_save_to_file(FILE* file) {
    int i, j, k;
    
    if (!file) {
        return 0;
    }
    
    int cg = MAX_COLORGRADATION;
    int cs = MAX_COLORSET;
    int db = MAX_DARKBIT;
    
    /* 헤더 값 쓰기 */
    if (fwrite(&cg, 4, 1, file) != 1) return 0;
    if (fwrite(&cs, 4, 1, file) != 1) return 0;
    if (fwrite(&db, 4, 1, file) != 1) return 0;

    /* ColorSet 테이블 쓰기 */
    for (i = 0; i < MAX_COLORSET; i++) {
        for (j = 0; j < MAX_COLORGRADATION; j++) {
            if (fwrite(&g_ColorSet[i][j], 2, 1, file) != 1) return 0;
        }
    }

    /* GradationValue 테이블 쓰기 (참고: 원본은 MAX_COLORSET개 항목을 쓴다) */
    for (i = 0; i < MAX_COLORSET; i++) {
        uint16_t value = (i < MAX_COLORGRADATION) ? g_GradationValue[i] : 0;
        if (fwrite(&value, 2, 1, file) != 1) return 0;
    }

    /* ColorSetDarkness 테이블 쓰기 */
    for (k = 0; k < MAX_DARKBIT; k++) {
        for (i = 0; i < MAX_COLORSET; i++) {
            for (j = 0; j < MAX_COLORGRADATION; j++) {
                if (fwrite(&g_ColorSetDarkness[k][i][j], 2, 1, file) != 1) return 0;
            }
        }
    }
    
    return 1;
}
