/**
 * @file color.h
 * @brief RGB565/RGB555 -> RGBA32 색상 변환
 * 
 * 요구사항: 5.1, 5.2, 5.3, 5.4, 5.5, 5.6
 */

#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * RGBA32 색상 구조체
 */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBA32;

/**
 * RGB565 색상을 RGBA32로 변환
 * @param color RGB565 색상 값
 * @param colorkey 투명 컬러키 (color == colorkey 일 때 alpha=0)
 * @return RGBA32 색상
 */
RGBA32 rgb565_to_rgba32(uint16_t color, uint16_t colorkey);

/**
 * RGB555 색상을 RGBA32로 변환
 * @param color RGB555 색상 값
 * @param colorkey 투명 컬러키 (color == colorkey 일 때 alpha=0)
 * @return RGBA32 색상
 */
RGBA32 rgb555_to_rgba32(uint16_t color, uint16_t colorkey);

/**
 * RGB565 색상 배열을 RGBA32로 일괄 변환
 * @param src 원본 RGB565 색상 배열
 * @param dst 대상 RGBA32 픽셀 배열 (uint32_t로 패킹됨)
 * @param count 변환할 픽셀 수
 * @param colorkey 투명 컬러키
 */
void rgb565_to_rgba32_batch(const uint16_t* src, uint32_t* dst, 
                            int count, uint16_t colorkey);

#ifdef __cplusplus
}
#endif

#endif /* COLOR_H */
