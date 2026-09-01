/**
 * @file index_sprite.c
 * @brief IndexedSprite 구현부
 *
 * 요구사항: 1.4, 1.5, 1.6
 *
 * IndexedSprite는 압축된 scanline 포맷을 사용한다:
 *
 * scanline마다:
 *   [transPairCount] - (trans, index, normal) 그룹의 개수
 *   각 그룹마다:
 *     [transCount]   - 투명 픽셀의 개수
 *     [indexCount]   - 인덱스 픽셀의 개수
 *     [indexData...] - 인덱스 데이터 (setNumber<<8 | gradation) × indexCount개
 *     [normalCount]  - 일반(직접 색상) 픽셀의 개수
 *     [normalData...] - RGB565 색상 × normalCount개
 */

#include "index_sprite.h"
#include "color.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * 초기화 및 정리
 * ============================================================================ */

void index_sprite_init(IndexSprite* sprite) {
    if (!sprite) return;
    
    sprite->width = 0;
    sprite->height = 0;
    sprite->pixels = NULL;
    sprite->is_init = 0;
}

void index_sprite_release(IndexSprite* sprite) {
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

int index_sprite_is_init(const IndexSprite* sprite) {
    return sprite ? sprite->is_init : 0;
}

uint16_t index_sprite_get_width(const IndexSprite* sprite) {
    return sprite ? sprite->width : 0;
}

uint16_t index_sprite_get_height(const IndexSprite* sprite) {
    return sprite ? sprite->height : 0;
}

/* ============================================================================
 * 파일 입출력
 * ============================================================================ */

int index_sprite_load_from_file(IndexSprite* sprite, FILE* file) {
    if (!sprite || !file) return 0;

    /* 기존 데이터가 있으면 해제한다 */
    index_sprite_release(sprite);

    /* width, height 읽기 */
    if (fread(&sprite->width, 2, 1, file) != 1) return 0;
    if (fread(&sprite->height, 2, 1, file) != 1) return 0;

    /* 빈 스프라이트 처리 */
    if (sprite->width == 0 || sprite->height == 0) {
        sprite->is_init = 1;
        return 1;
    }

    /* scanline 포인터 배열 할당 */
    sprite->pixels = (uint16_t**)malloc(sprite->height * sizeof(uint16_t*));
    if (!sprite->pixels) return 0;

    memset(sprite->pixels, 0, sprite->height * sizeof(uint16_t*));

    /* 각 scanline을 읽는다 */
    for (int i = 0; i < sprite->height; i++) {
        uint16_t len;

        /* scanline 길이 읽기 */
        if (fread(&len, 2, 1, file) != 1) {
            index_sprite_release(sprite);
            return 0;
        }

        /* scanline 데이터 할당 및 읽기 */
        sprite->pixels[i] = (uint16_t*)malloc(len * sizeof(uint16_t));
        if (!sprite->pixels[i]) {
            index_sprite_release(sprite);
            return 0;
        }
        
        if (fread(sprite->pixels[i], 2, len, file) != len) {
            index_sprite_release(sprite);
            return 0;
        }
    }
    
    sprite->is_init = 1;
    return 1;
}

/* ============================================================================
 * 렌더링 함수
 * ============================================================================ */

void index_sprite_blt(const IndexSprite* sprite, uint16_t* dest, uint16_t pitch) {
    if (!sprite || !sprite->is_init || !sprite->pixels || !dest) return;
    
    int transPair, indexCount, colorCount;
    int colorSet, colorGradation;
    uint16_t* pDestTemp;
    uint16_t* pPixels;
    
    /* 아래에서 위로 렌더링 (원래 동작) */
    int rectBottom = sprite->height;
    
    if (rectBottom > 0) {
        int i = rectBottom - 1;
        dest = (uint16_t*)((uint8_t*)dest + i * pitch);
        
        do {
            pPixels = sprite->pixels[i];
            pDestTemp = dest;
            
            /* (trans, index, normal) 그룹 수 */
            transPair = *pPixels++;
            
            if (transPair > 0) {
                int j = transPair;
                do {
                    /* 투명 픽셀 건너뛰기 */
                    pDestTemp += *pPixels++;
                    
                    /* 인덱스 픽셀 수 */
                    indexCount = *pPixels++;
                    
                    /* 인덱스 픽셀 렌더링 */
                    if (indexCount > 0) {
                        int k = indexCount;
                        do {
                            colorSet = (*pPixels >> 8) & 0xFF;
                            colorGradation = (*pPixels & 0xFF);
                            pPixels++;
                            
                            /* ColorSet 테이블에서 색상 조회 */
                            *pDestTemp = g_ColorSet[g_IndexValue[colorSet]][colorGradation];
                            pDestTemp++;
                        } while (--k);
                    }
                    
                    /* 일반(직접 색상) 픽셀 수 */
                    colorCount = *pPixels++;
                    
                    /* 직접 색상 복사 */
                    memcpy(pDestTemp, pPixels, colorCount * 2);
                    
                    pDestTemp += colorCount;
                    pPixels += colorCount;
                } while (--j);
            }
            
            dest = (uint16_t*)((uint8_t*)dest - pitch);
        } while (i--);
    }
}

void index_sprite_blt_colorset(const IndexSprite* sprite, uint16_t* dest,
                               uint16_t pitch, uint16_t colorSet) {
    if (!sprite || !sprite->is_init || !sprite->pixels || !dest) return;
    if (colorSet >= MAX_COLORSET) return;
    
    int transPair, indexCount, colorCount;
    int colorGradation;
    uint16_t* pDestTemp;
    uint16_t* pPixels;
    
    int rectBottom = sprite->height;
    
    if (rectBottom > 0) {
        int i = rectBottom - 1;
        dest = (uint16_t*)((uint8_t*)dest + i * pitch);
        
        do {
            pPixels = sprite->pixels[i];
            pDestTemp = dest;
            
            transPair = *pPixels++;
            
            if (transPair > 0) {
                int j = transPair;
                do {
                    pDestTemp += *pPixels++;
                    indexCount = *pPixels++;
                    
                    if (indexCount > 0) {
                        int k = indexCount;
                        do {
                            colorGradation = (*pPixels & 0xFF);
                            pPixels++;
                            
                            /* 모든 인덱스 픽셀에 지정된 colorSet 사용 */
                            *pDestTemp = g_ColorSet[colorSet][colorGradation];
                            pDestTemp++;
                        } while (--k);
                    }
                    
                    colorCount = *pPixels++;
                    memcpy(pDestTemp, pPixels, colorCount * 2);
                    pDestTemp += colorCount;
                    pPixels += colorCount;
                } while (--j);
            }
            
            dest = (uint16_t*)((uint8_t*)dest - pitch);
        } while (i--);
    }
}

void index_sprite_blt_darkness(const IndexSprite* sprite, uint16_t* dest,
                               uint16_t pitch, uint8_t darkBits) {
    if (!sprite || !sprite->is_init || !sprite->pixels || !dest) return;
    if (darkBits >= MAX_DARKBIT) darkBits = MAX_DARKBIT - 1;
    
    int transPair, indexCount, colorCount;
    int colorSet, colorGradation;
    uint16_t* pDestTemp;
    uint16_t* pPixels;
    
    int rectBottom = sprite->height;
    
    if (rectBottom > 0) {
        int i = rectBottom - 1;
        dest = (uint16_t*)((uint8_t*)dest + i * pitch);
        
        do {
            pPixels = sprite->pixels[i];
            pDestTemp = dest;
            
            transPair = *pPixels++;
            
            if (transPair > 0) {
                int j = transPair;
                do {
                    pDestTemp += *pPixels++;
                    indexCount = *pPixels++;
                    
                    if (indexCount > 0) {
                        int k = indexCount;
                        do {
                            colorSet = (*pPixels >> 8) & 0xFF;
                            colorGradation = (*pPixels & 0xFF);
                            pPixels++;
                            
                            /* 어두움이 적용된 색상 사용 */
                            *pDestTemp = g_ColorSetDarkness[darkBits][g_IndexValue[colorSet]][colorGradation];
                            pDestTemp++;
                        } while (--k);
                    }
                    
                    colorCount = *pPixels++;
                    
                    /* 직접 색상에 어두움 적용 */
                    for (int l = 0; l < colorCount; l++) {
                        uint16_t color = pPixels[l];
                        int r = (colorset_get_red(color) >> darkBits);
                        int g = (colorset_get_green(color) >> darkBits);
                        int b = (colorset_get_blue(color) >> darkBits);
                        *pDestTemp++ = colorset_make_color(r, g, b);
                    }
                    pPixels += colorCount;
                } while (--j);
            }
            
            dest = (uint16_t*)((uint8_t*)dest - pitch);
        } while (i--);
    }
}

void index_sprite_blt_alpha(const IndexSprite* sprite, uint16_t* dest,
                            uint16_t pitch, uint8_t alpha) {
    if (!sprite || !sprite->is_init || !sprite->pixels || !dest) return;
    
    int transPair, indexCount, colorCount;
    int colorSet, colorGradation;
    uint16_t* pDestTemp;
    uint16_t* pPixels;
    
    int rectBottom = sprite->height;
    int invAlpha = 255 - alpha;
    
    if (rectBottom > 0) {
        int i = rectBottom - 1;
        dest = (uint16_t*)((uint8_t*)dest + i * pitch);
        
        do {
            pPixels = sprite->pixels[i];
            pDestTemp = dest;
            
            transPair = *pPixels++;
            
            if (transPair > 0) {
                int j = transPair;
                do {
                    pDestTemp += *pPixels++;
                    indexCount = *pPixels++;
                    
                    if (indexCount > 0) {
                        int k = indexCount;
                        do {
                            colorSet = (*pPixels >> 8) & 0xFF;
                            colorGradation = (*pPixels & 0xFF);
                            pPixels++;
                            
                            uint16_t srcColor = g_ColorSet[g_IndexValue[colorSet]][colorGradation];
                            uint16_t dstColor = *pDestTemp;
                            
                            /* 알파 블렌딩 */
                            int sr = colorset_get_red(srcColor);
                            int sg = colorset_get_green(srcColor);
                            int sb = colorset_get_blue(srcColor);
                            int dr = colorset_get_red(dstColor);
                            int dg = colorset_get_green(dstColor);
                            int db = colorset_get_blue(dstColor);
                            
                            int r = (sr * alpha + dr * invAlpha) / 255;
                            int g = (sg * alpha + dg * invAlpha) / 255;
                            int b = (sb * alpha + db * invAlpha) / 255;
                            
                            *pDestTemp = colorset_make_color(r, g, b);
                            pDestTemp++;
                        } while (--k);
                    }
                    
                    colorCount = *pPixels++;
                    
                    for (int l = 0; l < colorCount; l++) {
                        uint16_t srcColor = pPixels[l];
                        uint16_t dstColor = *pDestTemp;
                        
                        int sr = colorset_get_red(srcColor);
                        int sg = colorset_get_green(srcColor);
                        int sb = colorset_get_blue(srcColor);
                        int dr = colorset_get_red(dstColor);
                        int dg = colorset_get_green(dstColor);
                        int db = colorset_get_blue(dstColor);
                        
                        int r = (sr * alpha + dr * invAlpha) / 255;
                        int g = (sg * alpha + dg * invAlpha) / 255;
                        int b = (sb * alpha + db * invAlpha) / 255;
                        
                        *pDestTemp++ = colorset_make_color(r, g, b);
                    }
                    pPixels += colorCount;
                } while (--j);
            }
            
            dest = (uint16_t*)((uint8_t*)dest - pitch);
        } while (i--);
    }
}

void index_sprite_blt_effect(const IndexSprite* sprite, uint16_t* dest,
                             uint16_t pitch) {
    if (!sprite || !sprite->is_init || !sprite->pixels || !dest) return;
    
    int transPair, indexCount, colorCount;
    int colorSet, colorGradation;
    uint16_t* pDestTemp;
    uint16_t* pPixels;
    
    int rectBottom = sprite->height;
    
    if (rectBottom > 0) {
        int i = rectBottom - 1;
        dest = (uint16_t*)((uint8_t*)dest + i * pitch);
        
        do {
            pPixels = sprite->pixels[i];
            pDestTemp = dest;
            
            transPair = *pPixels++;
            
            if (transPair > 0) {
                int j = transPair;
                do {
                    pDestTemp += *pPixels++;
                    indexCount = *pPixels++;
                    
                    if (indexCount > 0) {
                        int k = indexCount;
                        do {
                            colorSet = (*pPixels >> 8) & 0xFF;
                            colorGradation = (*pPixels & 0xFF);
                            pPixels++;
                            
                            uint16_t srcColor = g_ColorSet[g_IndexValue[colorSet]][colorGradation];
                            uint16_t dstColor = *pDestTemp;
                            
                            /* 가산 블렌딩 */
                            int r = colorset_get_red(srcColor) + colorset_get_red(dstColor);
                            int g = colorset_get_green(srcColor) + colorset_get_green(dstColor);
                            int b = colorset_get_blue(srcColor) + colorset_get_blue(dstColor);
                            
                            if (r > 31) r = 31;
                            if (g > 31) g = 31;
                            if (b > 31) b = 31;
                            
                            *pDestTemp = colorset_make_color(r, g, b);
                            pDestTemp++;
                        } while (--k);
                    }
                    
                    colorCount = *pPixels++;
                    
                    for (int l = 0; l < colorCount; l++) {
                        uint16_t srcColor = pPixels[l];
                        uint16_t dstColor = *pDestTemp;
                        
                        int r = colorset_get_red(srcColor) + colorset_get_red(dstColor);
                        int g = colorset_get_green(srcColor) + colorset_get_green(dstColor);
                        int b = colorset_get_blue(srcColor) + colorset_get_blue(dstColor);
                        
                        if (r > 31) r = 31;
                        if (g > 31) g = 31;
                        if (b > 31) b = 31;
                        
                        *pDestTemp++ = colorset_make_color(r, g, b);
                    }
                    pPixels += colorCount;
                } while (--j);
            }
            
            dest = (uint16_t*)((uint8_t*)dest - pitch);
        } while (i--);
    }
}

/* ============================================================================
 * SDL 렌더링 함수
 * ============================================================================ */

/**
 * IndexSprite를 RGBA32 픽셀 버퍼로 디코딩
 */
static uint32_t* index_sprite_decode_rgba(const IndexSprite* sprite, uint16_t colorSetOverride) {
    if (!sprite || !sprite->is_init || !sprite->pixels) return NULL;
    if (sprite->width == 0 || sprite->height == 0) return NULL;
    
    uint32_t* pixels = (uint32_t*)malloc(sprite->width * sprite->height * sizeof(uint32_t));
    if (!pixels) return NULL;
    
    /* 투명으로 초기화 */
    memset(pixels, 0, sprite->width * sprite->height * sizeof(uint32_t));
    
    int useOverride = (colorSetOverride < MAX_COLORSET);
    
    for (int y = 0; y < sprite->height; y++) {
        uint16_t* pPixels = sprite->pixels[y];
        uint32_t* pDest = pixels + y * sprite->width;
        int x = 0;
        
        int transPair = *pPixels++;
        
        for (int j = 0; j < transPair && x < sprite->width; j++) {
            /* 투명 픽셀 건너뛰기 */
            int transCount = *pPixels++;
            x += transCount;
            pDest += transCount;
            
            /* 인덱스 픽셀 */
            int indexCount = *pPixels++;
            for (int k = 0; k < indexCount && x < sprite->width; k++) {
                int colorSet = (*pPixels >> 8) & 0xFF;
                int colorGradation = (*pPixels & 0xFF);
                pPixels++;
                
                uint16_t rgb565;
                if (useOverride) {
                    rgb565 = g_ColorSet[colorSetOverride][colorGradation];
                } else {
                    rgb565 = g_ColorSet[g_IndexValue[colorSet]][colorGradation];
                }
                
                /* RGB565를 RGBA32로 변환 */
                RGBA32 rgba = rgb565_to_rgba32(rgb565, 0);
                *pDest++ = ((uint32_t)rgba.r) |
                          ((uint32_t)rgba.g << 8) |
                          ((uint32_t)rgba.b << 16) |
                          ((uint32_t)rgba.a << 24);
                x++;
            }
            
            /* 일반(직접 색상) 픽셀 */
            int colorCount = *pPixels++;
            for (int l = 0; l < colorCount && x < sprite->width; l++) {
                uint16_t rgb565 = pPixels[l];
                
                RGBA32 rgba = rgb565_to_rgba32(rgb565, 0);
                *pDest++ = ((uint32_t)rgba.r) |
                          ((uint32_t)rgba.g << 8) |
                          ((uint32_t)rgba.b << 16) |
                          ((uint32_t)rgba.a << 24);
                x++;
            }
            pPixels += colorCount;
        }
    }
    
    return pixels;
}

SDL_Texture* index_sprite_create_texture(const IndexSprite* sprite,
                                         SDL_Renderer* renderer) {
    return index_sprite_create_texture_colorset(sprite, renderer, 0xFFFF);
}

SDL_Texture* index_sprite_create_texture_colorset(const IndexSprite* sprite,
                                                  SDL_Renderer* renderer,
                                                  uint16_t colorSet) {
    if (!sprite || !sprite->is_init || !renderer) return NULL;
    if (sprite->width == 0 || sprite->height == 0) return NULL;
    
    uint32_t* pixels = index_sprite_decode_rgba(sprite, colorSet);
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

int index_sprite_render(const IndexSprite* sprite, SDL_Renderer* renderer,
                        int x, int y) {
    return index_sprite_render_colorset(sprite, renderer, x, y, 0xFFFF);
}

int index_sprite_render_colorset(const IndexSprite* sprite, SDL_Renderer* renderer,
                                 int x, int y, uint16_t colorSet) {
    if (!sprite || !sprite->is_init || !renderer) return -1;
    if (sprite->width == 0 || sprite->height == 0) return 0;
    
    SDL_Texture* texture = index_sprite_create_texture_colorset(sprite, renderer, colorSet);
    if (!texture) return -1;
    
    SDL_Rect dstRect = { x, y, sprite->width, sprite->height };
    int result = SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    
    SDL_DestroyTexture(texture);
    return result;
}
