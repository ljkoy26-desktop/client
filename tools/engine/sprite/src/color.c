/**
 * @file color.c
 * @brief RGB565/RGB555 -> RGBA32 색상 변환 구현부
 *
 * RGB565 포맷 (16비트):
 *   비트 15-11: Red (5비트)
 *   비트 10-5:  Green (6비트)
 *   비트 4-0:   Blue (5비트)
 *
 * RGB555 포맷 (16비트):
 *   비트 15:     사용 안 함
 *   비트 14-10: Red (5비트)
 *   비트 9-5:   Green (5비트)
 *   비트 4-0:   Blue (5비트)
 */

#include "color.h"

RGBA32 rgb565_to_rgba32(uint16_t color, uint16_t colorkey) {
    RGBA32 result;
    
    /* RGB565 성분 추출
     * Red:   비트 11-15 (5비트)
     * Green: 비트 5-10  (6비트)
     * Blue:  비트 0-4   (5비트)
     */
    uint8_t r5 = (color >> 11) & 0x1F;  /* 5비트 */
    uint8_t g6 = (color >> 5) & 0x3F;   /* 6비트 */
    uint8_t b5 = color & 0x1F;          /* 5비트 */

    /* 8비트 범위로 스케일링
     * 5비트 -> 8비트: value * 255 / 31
     * 6비트 -> 8비트: value * 255 / 63
     */
    result.r = (r5 * 255) / 31;
    result.g = (g6 * 255) / 63;
    result.b = (b5 * 255) / 31;

    /* colorkey와 비교해서 alpha 설정 */
    result.a = (color == colorkey) ? 0 : 255;

    return result;
}

RGBA32 rgb555_to_rgba32(uint16_t color, uint16_t colorkey) {
    RGBA32 result;

    /* RGB555 성분 추출
     * Red:   비트 10-14 (5비트)
     * Green: 비트 5-9   (5비트)
     * Blue:  비트 0-4   (5비트)
     * 비트 15는 사용하지 않음
     */
    uint8_t r5 = (color >> 10) & 0x1F;  /* 5비트 */
    uint8_t g5 = (color >> 5) & 0x1F;   /* 5비트 */
    uint8_t b5 = color & 0x1F;          /* 5비트 */

    /* 8비트 범위로 스케일링
     * 5비트 -> 8비트: value * 255 / 31
     */
    result.r = (r5 * 255) / 31;
    result.g = (g5 * 255) / 31;
    result.b = (b5 * 255) / 31;

    /* colorkey와 비교해서 alpha 설정 */
    result.a = (color == colorkey) ? 0 : 255;
    
    return result;
}

void rgb565_to_rgba32_batch(const uint16_t* src, uint32_t* dst, 
                            int count, uint16_t colorkey) {
    if (!src || !dst || count <= 0) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        RGBA32 rgba = rgb565_to_rgba32(src[i], colorkey);
        /* RGBA32를 uint32_t로 묶는다 (RGBA 순서) */
        dst[i] = ((uint32_t)rgba.r) |
                 ((uint32_t)rgba.g << 8) |
                 ((uint32_t)rgba.b << 16) |
                 ((uint32_t)rgba.a << 24);
    }
}
