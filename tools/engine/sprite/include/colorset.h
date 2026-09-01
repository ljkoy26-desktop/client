/**
 * @file colorset.h
 * @brief IndexedSprite 색상 매핑을 위한 ColorSet 시스템
 * 
 * 요구사항: 6.1, 6.2, 6.3, 6.4
 * 
 * ColorSet은 IndexedSprite 렌더링에 필요한 색상 조회 테이블(LUT)을 제공합니다.
 * 각 ColorSet은 30단계의 그라데이션 레벨을 포함하며, 총 495개의 ColorSet이 존재합니다
 * (33개 시드 색상 × 15개 변형).
 */

#ifndef COLORSET_H
#define COLORSET_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 상수 정의
 * ============================================================================ */

/**
 * ColorSet 시드(기본 색상)의 최대 개수
 */
#define MAX_COLORSET_SEED           33

/**
 * 시드당 변형(Modification) 개수
 */
#define MAX_COLORSET_SEED_MODIFY    15

/**
 * 전체 ColorSet 개수 (33 × 15 = 495)
 */
#define MAX_COLORSET                495

/**
 * ColorSet당 그라데이션 단계 수
 */
#define MAX_COLORGRADATION          30

/**
 * 그라데이션 단계 수의 절반
 */
#define MAX_COLORGRADATION_HALF     15

/**
 * 색상 -> 그라데이션 변환을 위한 최대값 (R+G+B 최대합)
 */
#define MAX_COLOR_TO_GRADATION      93

/**
 * 어두움(Darkness) 단계 수
 */
#define MAX_DARKBIT                 5

/**
 * 스프라이트당 사용할 수 있는 ColorSet 최대 개수
 */
#define MAX_COLORSET_USE            256

/**
 * 특수 픽셀에 대한 인덱스 값
 */
#define INDEX_NULL                  0xFF
#define INDEX_SELECT                0xFE
#define INDEX_TRANS                 0xFD
#define INDEXSET_NULL               0xFF

/* ============================================================================
 * ColorSet 테이블 (전역)
 * ============================================================================ */

/**
 * 메인 ColorSet 테이블 [495][30]
 * ColorSet[setIndex][gradation] = RGB565 색상
 */
extern uint16_t g_ColorSet[MAX_COLORSET][MAX_COLORGRADATION];

/**
 * 각 단계별 그라데이션 값
 */
extern uint16_t g_GradationValue[MAX_COLORGRADATION];

/**
 * 어두움(Darkness)이 적용된 ColorSet 테이블 [5][495][30]
 */
extern uint16_t g_ColorSetDarkness[MAX_DARKBIT][MAX_COLORSET][MAX_COLORGRADATION];

/**
 * 색상 -> 그라데이션 매핑 테이블
 */
extern uint8_t g_ColorToGradation[MAX_COLOR_TO_GRADATION];

/**
 * 렌더링에 현재 활성화된 ColorSet 인덱스
 * s_IndexValue[setNumber] = colorSetIndex
 */
extern int g_IndexValue[MAX_COLORSET_USE];

/* ============================================================================
 * 함수 선언
 * ============================================================================ */

/**
 * 계산된 값으로 ColorSet 테이블 초기화
 * 33개의 시드 색상으로부터 495개의 모든 ColorSet을 생성합니다.
 * 
 * 요구사항: 6.1, 6.2, 6.3
 */
void colorset_init(void);

/**
 * 렌더링에 사용할 활성 ColorSet 설정
 * 
 * @param set1 주 색상용 ColorSet 인덱스 (인덱스 0)
 * @param set2 보조 색상용 ColorSet 인덱스 (인덱스 1)
 * 
 * 요구사항: 6.4
 */
void colorset_set_using(int set1, int set2);

/**
 * 특정 ColorSet 슬롯 설정
 * 
 * @param setNumber 슬롯 번호 (0-255)
 * @param colorSet 사용할 ColorSet 인덱스
 */
void colorset_set_using_only(uint8_t setNumber, int colorSet);

/**
 * 슬롯의 ColorSet 인덱스 반환
 * 
 * @param n 슬롯 번호
 * @return ColorSet 인덱스
 */
int colorset_get_using(uint8_t n);

/**
 * 파일로부터 ColorSet 테이블 로드
 * 
 * @param file 읽어올 파일 핸들
 * @return 성공 시 1, 실패 시 0
 */
int colorset_load_from_file(FILE* file);

/**
 * ColorSet 테이블을 파일에 저장
 * 
 * @param file 저장할 파일 핸들
 * @return 성공 시 1, 실패 시 0
 */
int colorset_save_to_file(FILE* file);

/**
 * 색상 값을 그라데이션 인덱스로 변환
 * 
 * @param color RGB565 색상 값
 * @return 그라데이션 인덱스 (0 ~ MAX_COLORGRADATION-1)
 */
uint8_t colorset_get_index_color(uint16_t color);

/**
 * R+G+B 합을 그라데이션 인덱스로 변환
 * 
 * @param spriteGradation R+G+B 성분의 합 (0-93)
 * @return 그라데이션 인덱스
 */
uint8_t colorset_color_to_gradation(uint8_t spriteGradation);

/**
 * 5비트 성분들로부터 RGB565 색상 생성
 * 
 * @param r Red 성분 (0-31)
 * @param g Green 성분 (0-31, 6비트로 스케일링됨)
 * @param b Blue 성분 (0-31)
 * @return RGB565 색상 값
 */
uint16_t colorset_make_color(uint8_t r, uint8_t g, uint8_t b);

/**
 * RGB565에서 Red 성분 추출
 * 
 * @param color RGB565 색상
 * @return Red 성분 (0-31)
 */
uint8_t colorset_get_red(uint16_t color);

/**
 * RGB565에서 Green 성분 추출
 * 
 * @param color RGB565 색상
 * @return Green 성분 (0-31, 6비트에서 스케일링됨)
 */
uint8_t colorset_get_green(uint16_t color);

/**
 * RGB565에서 Blue 성분 추출
 * 
 * @param color RGB565 색상
 * @return Blue 성분 (0-31)
 */
uint8_t colorset_get_blue(uint16_t color);

#ifdef __cplusplus
}
#endif

#endif /* COLORSET_H */
