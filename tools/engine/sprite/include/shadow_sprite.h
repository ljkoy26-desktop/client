/**
 * @file shadow_sprite.h
 * @brief 크리처 그림자 렌더링을 위한 ShadowSprite
 * 
 * 요구사항: 2.3, 2.4
 * 
 * ShadowSprite는 압축된 런렝스 포맷으로 그림자 데이터를 저장합니다.
 * 각 스캔라인은 (transparent_count, shadow_count) 쌍으로 구성됩니다.
 * 그림자 픽셀은 대상(화면) 픽셀을 어둡게 처리하여 렌더링됩니다.
 */

#ifndef SHADOW_SPRITE_H
#define SHADOW_SPRITE_H

#include <SDL.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * ShadowSprite 구조체
 * ============================================================================ */

/**
 * ShadowSprite 구조체
 * 
 * 각 스캔라인에 다음 정보가 포함되는 압축 포맷으로 그림자 데이터를 저장합니다:
 *   [count] - (transparent, shadow) 쌍의 개수
 *   각 쌍별:
 *     [transCount] - 투명 픽셀 수
 *     [shadowCount] - 그림자 픽셀 수
 * 
 * 그림자 픽셀은 대상 픽셀을 어둡게 만들어 렌더링됩니다.
 */
typedef struct {
    uint16_t width;          /**< 너비 (픽셀 단위) */
    uint16_t height;         /**< 높이 (픽셀 단위) */
    uint16_t** pixels;       /**< 스캔라인 데이터 배열 */
    int is_init;             /**< 초기화 여부 */
} ShadowSprite;

/* ============================================================================
 * 함수 선언
 * ============================================================================ */

/**
 * ShadowSprite 구조체 초기화
 * 
 * @param sprite ShadowSprite 포인터
 */
void shadow_sprite_init(ShadowSprite* sprite);

/**
 * ShadowSprite 리소스 해제
 * 
 * @param sprite ShadowSprite 포인터
 */
void shadow_sprite_release(ShadowSprite* sprite);

/**
 * 그림자 스프라이트 초기화 여부 확인
 * 
 * @param sprite ShadowSprite 포인터
 * @return 초기화되었으면 1, 그렇지 않으면 0
 */
int shadow_sprite_is_init(const ShadowSprite* sprite);

/**
 * 스프라이트 너비 반환
 * 
 * @param sprite ShadowSprite 포인터
 * @return 너비 (픽셀 단위)
 */
uint16_t shadow_sprite_get_width(const ShadowSprite* sprite);

/**
 * 스프라이트 높이 반환
 * 
 * @param sprite ShadowSprite 포인터
 * @return 높이 (픽셀 단위)
 */
uint16_t shadow_sprite_get_height(const ShadowSprite* sprite);

/**
 * 파일에서 ShadowSprite 로드
 * 
 * 스캔라인별 파일 포맷:
 *   [len: 2 bytes] - WORD 단위의 스캔라인 데이터 길이
 *   [data: len*2 bytes] - 스캔라인 데이터
 * 
 * 스캔라인 데이터 포맷:
 *   [count] - (trans, shadow) 쌍의 개수
 *   각 쌍별:
 *     [transCount] - 투명 픽셀 수
 *     [shadowCount] - 그림자 픽셀 수
 * 
 * @param sprite ShadowSprite 포인터
 * @param file 스프라이트 데이터 위치에 위치한 파일 핸들
 * @return 성공 시 1, 실패 시 0
 * 
 * 요구사항: 2.3
 */
int shadow_sprite_load_from_file(ShadowSprite* sprite, FILE* file);

/**
 * RGB565 픽셀 버퍼에 그림자 블릿 (완전 검은 그림자)
 * 
 * 그림자 픽셀을 검은색(0x0000)으로 설정합니다.
 * 
 * @param sprite ShadowSprite 포인터
 * @param dest 대상 RGB565 픽셀 버퍼
 * @param pitch 대상 버퍼 피치 (바이트 단위)
 * 
 * 요구사항: 2.4
 */
void shadow_sprite_blt(const ShadowSprite* sprite, uint16_t* dest, uint16_t pitch);

/**
 * 어두움 효과를 적용하여 그림자 블릿
 * 
 * 그림자 픽셀 위치의 대상 RGB 값을 비트 시프트하여 어둡게 만듭니다.
 * 
 * @param sprite ShadowSprite 포인터
 * @param dest 대상 RGB565 픽셀 버퍼
 * @param pitch 대상 버퍼 피치 (바이트 단위)
 * @param darkBits 어두움 레벨 (0-4), 클수록 더 어두움
 * 
 * 요구사항: 2.4
 */
void shadow_sprite_blt_darkness(const ShadowSprite* sprite, uint16_t* dest,
                                uint16_t pitch, uint8_t darkBits);

/**
 * 알파 블렌딩을 적용하여 그림자 블릿
 * 
 * 그림자 픽셀을 알파 값을 사용하여 대상과 블렌딩합니다.
 * 
 * @param sprite ShadowSprite 포인터
 * @param dest 대상 RGB565 픽셀 버퍼
 * @param pitch 대상 버퍼 피치 (바이트 단위)
 * @param alpha 알파 값 (0-255), 0 = 투명, 255 = 불투명 그림자
 * 
 * 요구사항: 2.4
 */
void shadow_sprite_blt_alpha(const ShadowSprite* sprite, uint16_t* dest,
                             uint16_t pitch, uint8_t alpha);

/**
 * SDL 렌더러로 그림자 렌더링
 * 
 * 그림자 마스크를 가진 텍스처를 생성하여 렌더링합니다.
 * 
 * @param sprite ShadowSprite 포인터
 * @param renderer SDL 렌더러
 * @param x X 좌표
 * @param y Y 좌표
 * @return 성공 시 0, 오류 시 음수
 */
int shadow_sprite_render(const ShadowSprite* sprite, SDL_Renderer* renderer,
                         int x, int y);

/**
 * 알파 값을 적용하여 SDL 렌더러로 그림자 렌더링
 * 
 * @param sprite ShadowSprite 포인터
 * @param renderer SDL 렌더러
 * @param x X 좌표
 * @param y Y 좌표
 * @param alpha 알파 값 (0-255)
 * @return 성공 시 0, 오류 시 음수
 */
int shadow_sprite_render_alpha(const ShadowSprite* sprite, SDL_Renderer* renderer,
                               int x, int y, uint8_t alpha);

/**
 * ShadowSprite로부터 SDL 텍스처 생성
 * 
 * @param sprite ShadowSprite 포인터
 * @param renderer SDL 렌더러
 * @param alpha 그림자의 알파 값 (0-255)
 * @return SDL_Texture 포인터, 실패 시 NULL
 */
SDL_Texture* shadow_sprite_create_texture(const ShadowSprite* sprite,
                                          SDL_Renderer* renderer,
                                          uint8_t alpha);

#ifdef __cplusplus
}
#endif

#endif /* SHADOW_SPRITE_H */
