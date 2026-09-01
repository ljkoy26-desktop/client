/**
 * @file sprite.h
 * @brief 스프라이트 파일 포맷 파싱 및 렌더링
 * 
 * 요구사항: 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 6.1, 6.2, 6.3, 6.4, 6.5, 6.6
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;

/**
 * 원시(Raw) 스프라이트 데이터 구조체 (RLE 압축됨)
 */
typedef struct {
    uint16_t width;          /**< 너비 */
    uint16_t height;         /**< 높이 */
    uint16_t** scanlines;    /* 각 스캔라인별 RLE 데이터 */
    uint16_t* scanline_lens; /* 각 스캔라인별 RLE 데이터 길이 */
    int is_valid;            /**< 유효성 여부 */
} Sprite;

typedef struct DecodedSprite DecodedSprite;
/**
 * 디코딩된 스프라이트 구조체 (렌더링 준비 완료)
 */
struct DecodedSprite {
    uint16_t width;          /**< 너비 */
    uint16_t height;         /**< 높이 */
    uint32_t* pixels;        /* RGBA32 픽셀 데이터 */
    SDL_Texture* texture;    /* SDL 텍스처 (선택적) */
};

/**
 * 파일 스트림에서 스프라이트 로드
 * @param sprite Sprite 구조체 포인터
 * @param file 스프라이트 데이터 위치에 위치한 파일 스트림
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int sprite_load(Sprite* sprite, FILE* file);

/**
 * 스프라이트 리소스 해제
 * @param sprite Sprite 구조체 포인터
 */
void sprite_free(Sprite* sprite);

/**
 * 스프라이트 RLE 데이터를 RGBA32 픽셀로 디코딩
 * @param sprite RLE 데이터를 가진 원본 스프라이트
 * @param decoded 대상 디코딩된 스프라이트
 * @param colorkey 투명 컬러키
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int sprite_decode(const Sprite* sprite, DecodedSprite* decoded, 
                  uint16_t colorkey);

/**
 * 디코딩된 스프라이트 리소스 해제
 * @param decoded DecodedSprite 구조체 포인터
 */
void decoded_sprite_free(DecodedSprite* decoded);

struct SDL_Renderer;

/**
 * 디코딩된 스프라이트로부터 SDL 텍스처 생성
 * @param decoded DecodedSprite 포인터
 * @param renderer SDL 렌더러
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int decoded_sprite_create_texture(DecodedSprite* decoded, 
                                  SDL_Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif /* SPRITE_H */
