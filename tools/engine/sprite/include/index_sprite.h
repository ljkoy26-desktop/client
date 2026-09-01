/**
 * @file index_sprite.h
 * @brief 색상 매핑 크리처 렌더링을 위한 IndexedSprite
 * 
 * 요구사항: 1.4, 1.5, 1.6
 * 
 * IndexedSprite는 픽셀 데이터를 ColorSet 테이블의 인덱스로 저장하여,
 * 런타임에 크리처의 색상을 커스터마이징할 수 있도록 지원합니다.
 */

#ifndef INDEX_SPRITE_H
#define INDEX_SPRITE_H

#include <SDL.h>
#include <stdint.h>
#include <stdio.h>
#include "colorset.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * IndexSprite 구조체
 * ============================================================================ */

/**
 * IndexedSprite 구조체
 * 
 * 다음과 같은 압축 포맷으로 픽셀 데이터를 저장합니다:
 * - 투명 (INDEX_TRANS)
 * - 인덱스 색상 (setNumber << 8 | gradation)
 * - 직접 색상 (RGB565)
 * 
 * 데이터는 스캔라인별로 런렝스 인코딩(RLE) 형식으로 저장됩니다.
 */
typedef struct {
    uint16_t width;          /**< 너비 (픽셀 단위) */
    uint16_t height;         /**< 높이 (픽셀 단위) */
    uint16_t** pixels;       /**< 스캔라인 데이터 배열 */
    int is_init;             /**< 초기화 여부 */
} IndexSprite;

/* ============================================================================
 * 함수 선언
 * ============================================================================ */

/**
 * IndexSprite 구조체 초기화
 * 
 * @param sprite IndexSprite 포인터
 */
void index_sprite_init(IndexSprite* sprite);

/**
 * IndexSprite 리소스 해제
 * 
 * @param sprite IndexSprite 포인터
 */
void index_sprite_release(IndexSprite* sprite);

/**
 * 스프라이트 초기화 여부 확인
 * 
 * @param sprite IndexSprite 포인터
 * @return 초기화되었으면 1, 그렇지 않으면 0
 */
int index_sprite_is_init(const IndexSprite* sprite);

/**
 * 스프라이트 너비 반환
 * 
 * @param sprite IndexSprite 포인터
 * @return 너비 (픽셀 단위)
 */
uint16_t index_sprite_get_width(const IndexSprite* sprite);

/**
 * 스프라이트 높이 반환
 * 
 * @param sprite IndexSprite 포인터
 * @return 높이 (픽셀 단위)
 */
uint16_t index_sprite_get_height(const IndexSprite* sprite);

/**
 * 파일에서 IndexSprite 로드 (RGB565 포맷)
 * 
 * @param sprite IndexSprite 포인터
 * @param file 스프라이트 데이터 위치에 위치한 파일 핸들
 * @return 성공 시 1, 실패 시 0
 * 
 * 요구사항: 1.4
 */
int index_sprite_load_from_file(IndexSprite* sprite, FILE* file);

/**
 * 현재 ColorSet을 사용하여 픽셀 버퍼에 IndexSprite 블릿(렌더링)
 * 
 * @param sprite IndexSprite 포인터
 * @param dest 대상 RGB565 픽셀 버퍼
 * @param pitch 대상 버퍼 피치 (바이트 단위)
 * 
 * 요구사항: 1.5, 1.6
 */
void index_sprite_blt(const IndexSprite* sprite, uint16_t* dest, uint16_t pitch);

/**
 * 특정 ColorSet을 지정하여 IndexSprite 블릿(렌더링)
 * 
 * @param sprite IndexSprite 포인터
 * @param dest 대상 RGB565 픽셀 버퍼
 * @param pitch 대상 버퍼 피치 (바이트 단위)
 * @param colorSet 모든 인덱스 픽셀에 적용할 ColorSet 인덱스
 */
void index_sprite_blt_colorset(const IndexSprite* sprite, uint16_t* dest, 
                               uint16_t pitch, uint16_t colorSet);

/**
 * 어두움(Darkness) 효과를 적용하여 IndexSprite 블릿
 * 
 * @param sprite IndexSprite 포인터
 * @param dest 대상 RGB565 픽셀 버퍼
 * @param pitch 대상 버퍼 피치 (바이트 단위)
 * @param darkBits 어두움 레벨 (0-4)
 */
void index_sprite_blt_darkness(const IndexSprite* sprite, uint16_t* dest,
                               uint16_t pitch, uint8_t darkBits);

/**
 * 알파 블렌딩을 적용하여 IndexSprite 블릿
 * 
 * @param sprite IndexSprite 포인터
 * @param dest 대상 RGB565 픽셀 버퍼
 * @param pitch 대상 버퍼 피치 (바이트 단위)
 * @param alpha 알파 값 (0-255)
 */
void index_sprite_blt_alpha(const IndexSprite* sprite, uint16_t* dest,
                            uint16_t pitch, uint8_t alpha);

/**
 * 가산 혼합(Additive effect) 효과를 적용하여 IndexSprite 블릿
 * 
 * @param sprite IndexSprite 포인터
 * @param dest 대상 RGB565 픽셀 버퍼
 * @param pitch 대상 버퍼 피치 (바이트 단위)
 */
void index_sprite_blt_effect(const IndexSprite* sprite, uint16_t* dest,
                             uint16_t pitch);

/**
 * SDL 텍스처로 IndexSprite 렌더링
 * 
 * @param sprite IndexSprite 포인터
 * @param renderer SDL 렌더러
 * @param x X 좌표
 * @param y Y 좌표
 * @return 성공 시 0, 오류 시 음수
 */
int index_sprite_render(const IndexSprite* sprite, SDL_Renderer* renderer,
                        int x, int y);

/**
 * 특정 ColorSet을 적용하여 SDL 텍스처로 IndexSprite 렌더링
 * 
 * @param sprite IndexSprite 포인터
 * @param renderer SDL 렌더러
 * @param x X 좌표
 * @param y Y 좌표
 * @param colorSet 사용할 ColorSet 인덱스
 * @return 성공 시 0, 오류 시 음수
 */
int index_sprite_render_colorset(const IndexSprite* sprite, SDL_Renderer* renderer,
                                 int x, int y, uint16_t colorSet);

/**
 * IndexSprite로부터 SDL 텍스처 생성
 * 
 * @param sprite IndexSprite 포인터
 * @param renderer SDL 렌더러
 * @return SDL_Texture 포인터, 실패 시 NULL
 */
SDL_Texture* index_sprite_create_texture(const IndexSprite* sprite, 
                                         SDL_Renderer* renderer);

/**
 * 특정 ColorSet을 적용하여 IndexSprite로부터 SDL 텍스처 생성
 * 
 * @param sprite IndexSprite 포인터
 * @param renderer SDL 렌더러
 * @param colorSet 사용할 ColorSet 인덱스
 * @return SDL_Texture 포인터, 실패 시 NULL
 */
SDL_Texture* index_sprite_create_texture_colorset(const IndexSprite* sprite,
                                                  SDL_Renderer* renderer,
                                                  uint16_t colorSet);

#ifdef __cplusplus
}
#endif

#endif /* INDEX_SPRITE_H */
