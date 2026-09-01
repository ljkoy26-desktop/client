/**
 * @file shadow_sprite.c
 * @brief ShadowSprite 구현부
 * 
 * 요구사항: 2.3, 2.4
 * 
 * ShadowSprite 파일 포맷 (스캔라인당):
 *   [len: 2 바이트] - WORD 단위의 스캔라인 데이터 길이
 *   [data: len*2 바이트] - 스캔라인 데이터
 * 
 * 스캔라인 데이터 포맷:
 *   [count] - (trans, shadow) 쌍의 개수
 *   각 쌍별:
 *     [transCount] - 투명 픽셀 수
 *     [shadowCount] - 그림자 픽셀 수
 * 
 * 그림자 렌더링은 색상을 직접 복사하지 않고 대상 픽셀을 어둡게 처리합니다.
 */

#include "shadow_sprite.h"
#include "color.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * 초기화 및 정리
 * ============================================================================ */

void shadow_sprite_init(ShadowSprite* sprite) {
    if (!sprite) return;
    
    sprite->width = 0;
    sprite->height = 0;
    sprite->pixels = NULL;
    sprite->is_init = 0;
}

void shadow_sprite_release(ShadowSprite* sprite) {
    if (!sprite) return;
    
    if (sprite->pixels) {
        for (int i = 0; i < sprite->height; i++) {
            if (sprite->pixels[i]) {
                free(sprite->pixels[i]);
            }
        }
        free(sprite->pixels);
        sprite->pixels = NULL;
    }
    
    sprite->width = 0;
    sprite->height = 0;
    sprite->is_init = 0;
}

int shadow_sprite_is_init(const ShadowSprite* sprite) {
    return sprite ? sprite->is_init : 0;
}

uint16_t shadow_sprite_get_width(const ShadowSprite* sprite) {
    return sprite ? sprite->width : 0;
}

uint16_t shadow_sprite_get_height(const ShadowSprite* sprite) {
    return sprite ? sprite->height : 0;
}

/* ============================================================================
 * 파일 입출력
 * ============================================================================ */

int shadow_sprite_load_from_file(ShadowSprite* sprite, FILE* file) {
    if (!sprite || !file) return 0;
    
    /* 기존 데이터가 있으면 해제 */
    shadow_sprite_release(sprite);
    
    /* 너비 및 높이 읽기 */
    if (fread(&sprite->width, 2, 1, file) != 1) return 0;
    if (fread(&sprite->height, 2, 1, file) != 1) return 0;
    
    /* 빈 스프라이트 처리 */
    if (sprite->width == 0 || sprite->height == 0) {
        sprite->is_init = 1;
        return 1;
    }
    
    /* 스캔라인 포인터 배열 할당 */
    sprite->pixels = (uint16_t**)malloc(sprite->height * sizeof(uint16_t*));
    if (!sprite->pixels) return 0;
    
    memset(sprite->pixels, 0, sprite->height * sizeof(uint16_t*));
    
    /* 각 스캔라인 읽기 */
    for (int i = 0; i < sprite->height; i++) {
        uint16_t len;
        
        /* 스캔라인 길이 읽기 (WORD 단위) */
        if (fread(&len, 2, 1, file) != 1) {
            shadow_sprite_release(sprite);
            return 0;
        }
        
        /* 스캔라인 데이터 할당 및 읽기 */
        sprite->pixels[i] = (uint16_t*)malloc(len * sizeof(uint16_t));
        if (!sprite->pixels[i]) {
            shadow_sprite_release(sprite);
            return 0;
        }
        
        if (fread(sprite->pixels[i], 2, len, file) != len) {
            shadow_sprite_release(sprite);
            return 0;
        }
    }
    
    sprite->is_init = 1;
    return 1;
}

/* ============================================================================
 * 렌더링 함수
 * ============================================================================ */

void shadow_sprite_blt(const ShadowSprite* sprite, uint16_t* dest, uint16_t pitch) {
    if (!sprite || !sprite->is_init || !sprite->pixels || !dest) return;
    
    int count, shadowCount;
    uint16_t* pDestTemp;
    uint16_t* pPixels;
    
    for (int i = 0; i < sprite->height; i++) {
        pPixels = sprite->pixels[i];
        pDestTemp = dest;
        
        /* (trans, shadow) 쌍의 개수 */
        count = *pPixels++;
        
        if (count > 0) {
            int j = count;
            do {
                /* 투명 픽셀 건너뛰기 */
                pDestTemp += *pPixels++;
                
                /* 그림자 픽셀 수 */
                shadowCount = *pPixels++;
                
                /* 그림자 픽셀을 검은색으로 설정 */
                memset(pDestTemp, 0, shadowCount * sizeof(uint16_t));
                
                pDestTemp += shadowCount;
            } while (--j);
        }
        
        dest = (uint16_t*)((uint8_t*)dest + pitch);
    }
}


void shadow_sprite_blt_darkness(const ShadowSprite* sprite, uint16_t* dest,
                                uint16_t pitch, uint8_t darkBits) {
    if (!sprite || !sprite->is_init || !sprite->pixels || !dest) return;
    if (darkBits > 4) darkBits = 4;
    
    int count, shadowCount;
    uint16_t* pDestTemp;
    uint16_t* pPixels;
    
    /* RGB565 값 시프트를 위한 마스크 */
    /* 우측 시프트 후 래핑된 비트를 마스킹 처리해야 함 */
    uint16_t mask;
    switch (darkBits) {
        case 0: mask = 0xFFFF; break;
        case 1: mask = 0x7BEF; break;  /* 0111 1011 1110 1111 */
        case 2: mask = 0x39E7; break;  /* 0011 1001 1110 0111 */
        case 3: mask = 0x18E3; break;  /* 0001 1000 1110 0011 */
        case 4: mask = 0x0861; break;  /* 0000 1000 0110 0001 */
        default: mask = 0x7BEF; break;
    }
    
    for (int i = 0; i < sprite->height; i++) {
        pPixels = sprite->pixels[i];
        pDestTemp = dest;
        
        /* (trans, shadow) 쌍의 개수 */
        count = *pPixels++;
        
        if (count > 0) {
            int j = count;
            do {
                /* 투명 픽셀 건너뛰기 */
                pDestTemp += *pPixels++;
                
                /* 그림자 픽셀 수 */
                shadowCount = *pPixels++;
                
                /* 대상 픽셀 어둡게 처리 */
                for (int k = 0; k < shadowCount; k++) {
                    *pDestTemp = (*pDestTemp >> darkBits) & mask;
                    pDestTemp++;
                }
            } while (--j);
        }
        
        dest = (uint16_t*)((uint8_t*)dest + pitch);
    }
}

void shadow_sprite_blt_alpha(const ShadowSprite* sprite, uint16_t* dest,
                             uint16_t pitch, uint8_t alpha) {
    if (!sprite || !sprite->is_init || !sprite->pixels || !dest) return;
    
    int count, shadowCount;
    uint16_t* pDestTemp;
    uint16_t* pPixels;
    int invAlpha = 255 - alpha;
    
    for (int i = 0; i < sprite->height; i++) {
        pPixels = sprite->pixels[i];
        pDestTemp = dest;
        
        /* (trans, shadow) 쌍의 개수 */
        count = *pPixels++;
        
        if (count > 0) {
            int j = count;
            do {
                /* 투명 픽셀 건너뛰기 */
                pDestTemp += *pPixels++;
                
                /* 그림자 픽셀 수 */
                shadowCount = *pPixels++;
                
                /* 대상과 그림자(검은색) 알파 블렌딩 */
                for (int k = 0; k < shadowCount; k++) {
                    uint16_t dstColor = *pDestTemp;
                    
                    /* RGB 성분 추출 (RGB565) */
                    int r = (dstColor >> 11) & 0x1F;
                    int g = (dstColor >> 5) & 0x3F;
                    int b = dstColor & 0x1F;
                    
                    /* 검은색과 블렌딩 (그림자 색상은 0,0,0) */
                    /* result = src * alpha + dst * (1-alpha) */
                    /* src가 검은색이므로 result = dst * invAlpha / 255 */
                    r = (r * invAlpha) / 255;
                    g = (g * invAlpha) / 255;
                    b = (b * invAlpha) / 255;
                    
                    /* RGB565로 다시 패킹 */
                    *pDestTemp = (uint16_t)((r << 11) | (g << 5) | b);
                    pDestTemp++;
                }
            } while (--j);
        }
        
        dest = (uint16_t*)((uint8_t*)dest + pitch);
    }
}

/* ============================================================================
 * SDL 렌더링 함수
 * ============================================================================ */

/**
 * ShadowSprite를 RGBA32 픽셀 버퍼로 디코딩
 */
static uint32_t* shadow_sprite_decode_rgba(const ShadowSprite* sprite, uint8_t alpha) {
    if (!sprite || !sprite->is_init || !sprite->pixels) return NULL;
    if (sprite->width == 0 || sprite->height == 0) return NULL;
    
    uint32_t* pixels = (uint32_t*)malloc(sprite->width * sprite->height * sizeof(uint32_t));
    if (!pixels) return NULL;
    
    /* 완전 투명으로 초기화 */
    memset(pixels, 0, sprite->width * sprite->height * sizeof(uint32_t));
    
    /* 그림자 색상: 지정된 알파를 가진 검은색 */
    uint32_t shadowColor = (uint32_t)alpha << 24;  /* RGBA: 0, 0, 0, alpha */
    
    for (int y = 0; y < sprite->height; y++) {
        uint16_t* pPixels = sprite->pixels[y];
        uint32_t* pDest = pixels + y * sprite->width;
        int x = 0;
        
        int count = *pPixels++;
        
        for (int j = 0; j < count && x < sprite->width; j++) {
            /* 투명 픽셀 건너뛰기 */
            int transCount = *pPixels++;
            x += transCount;
            pDest += transCount;
            
            /* 그림자 픽셀 */
            int shadowCount = *pPixels++;
            for (int k = 0; k < shadowCount && x < sprite->width; k++) {
                *pDest++ = shadowColor;
                x++;
            }
        }
    }
    
    return pixels;
}

SDL_Texture* shadow_sprite_create_texture(const ShadowSprite* sprite,
                                          SDL_Renderer* renderer,
                                          uint8_t alpha) {
    if (!sprite || !sprite->is_init || !renderer) return NULL;
    if (sprite->width == 0 || sprite->height == 0) return NULL;
    
    uint32_t* pixels = shadow_sprite_decode_rgba(sprite, alpha);
    if (!pixels) return NULL;
    
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STATIC,
                                             sprite->width, sprite->height);
    if (!texture) {
        free(pixels);
        return NULL;
    }
    
    SDL_UpdateTexture(texture, NULL, pixels, sprite->width * sizeof(uint32_t));
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    free(pixels);
    return texture;
}

int shadow_sprite_render(const ShadowSprite* sprite, SDL_Renderer* renderer,
                         int x, int y) {
    return shadow_sprite_render_alpha(sprite, renderer, x, y, 128);  /* 기본 50% 알파 */
}

int shadow_sprite_render_alpha(const ShadowSprite* sprite, SDL_Renderer* renderer,
                               int x, int y, uint8_t alpha) {
    if (!sprite || !sprite->is_init || !renderer) return -1;
    if (sprite->width == 0 || sprite->height == 0) return 0;
    
    SDL_Texture* texture = shadow_sprite_create_texture(sprite, renderer, alpha);
    if (!texture) return -1;
    
    SDL_Rect dstRect = { x, y, sprite->width, sprite->height };
    int result = SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    
    SDL_DestroyTexture(texture);
    return result;
}
