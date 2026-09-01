/**
 * @file sprite.c
 * @brief 스프라이트 파일 포맷 파싱 및 렌더링 구현부
 * 
 * 스프라이트 파일 포맷:
 *   - 너비 (2 바이트, uint16_t, 리틀 엔디언)
 *   - 높이 (2 바이트, uint16_t, 리틀 엔디언)
 *   - 각 스캔라인별:
 *     - 길이 (2 바이트, uint16_t) - RLE 데이터의 WORD 개수
 *     - RLE 데이터 (길이 * 2 바이트)
 * 
 * RLE 라인 포맷:
 *   - 세그먼트 수 (1 WORD) - 세그먼트 개수
 *   - 각 세그먼트별:
 *     - 투명 픽셀 수 (1 WORD) - 투명 픽셀 개수
 *     - 색상 픽셀 수 (1 WORD) - 색상 픽셀 개수
 *     - 픽셀 데이터 (색상 픽셀 수 WORDs) - RGB565 픽셀 데이터
 */

#include <SDL.h>
#include "sprite.h"
#include "color.h"
#include <stdlib.h>
#include <string.h>

/**
 * 파일 스트림에서 스프라이트 로드
 * 요구사항: 3.1, 3.2, 3.3, 3.4, 3.5
 */
int sprite_load(Sprite* sprite, FILE* file) {
    if (!sprite || !file) {
        return -1;
    }
    
    /* 스프라이트 구조체 초기화 */
    memset(sprite, 0, sizeof(Sprite));
    sprite->is_valid = 0;
    
    /* 너비 읽기 (2 바이트, 리틀 엔디언) - 요구사항 3.1 */
    if (fread(&sprite->width, 2, 1, file) != 1) {
        return -1;
    }
    
    /* 높이 읽기 (2 바이트, 리틀 엔디언) - 요구사항 3.2 */
    if (fread(&sprite->height, 2, 1, file) != 1) {
        return -1;
    }
    
    /* 빈 스프라이트 처리 - 요구사항 3.3 */
    if (sprite->width == 0 || sprite->height == 0) {
        sprite->is_valid = 1;  /* 비어있지만 유효함 */
        sprite->scanlines = NULL;
        sprite->scanline_lens = NULL;
        return 0;
    }
    
    /* 스캔라인 배열 할당 */
    sprite->scanlines = (uint16_t**)calloc(sprite->height, sizeof(uint16_t*));
    if (!sprite->scanlines) {
        return -3;  /* 메모리 할당 실패 */
    }
    
    sprite->scanline_lens = (uint16_t*)calloc(sprite->height, sizeof(uint16_t));
    if (!sprite->scanline_lens) {
        free(sprite->scanlines);
        sprite->scanlines = NULL;
        return -3;
    }
    
    /* 각 스캔라인 읽기 - 요구사항 3.4, 3.5 */
    for (int i = 0; i < sprite->height; i++) {
        uint16_t len;
        
        /* 라인 데이터 길이 읽기 (2 바이트) - 요구사항 3.4 */
        if (fread(&len, 2, 1, file) != 1) {
            sprite_free(sprite);
            return -2;  /* 파일 포맷 에러 */
        }
        
        sprite->scanline_lens[i] = len;
        
        if (len > 0) {
            /* RLE 데이터 할당 및 읽기 - 요구사항 3.5 */
            sprite->scanlines[i] = (uint16_t*)malloc(len * sizeof(uint16_t));
            if (!sprite->scanlines[i]) {
                sprite_free(sprite);
                return -3;
            }
            
            if (fread(sprite->scanlines[i], 2, len, file) != len) {
                sprite_free(sprite);
                return -2;
            }
        } else {
            sprite->scanlines[i] = NULL;
        }
    }
    
    sprite->is_valid = 1;
    return 0;
}

/**
 * 스프라이트 리소스 해제
 */
void sprite_free(Sprite* sprite) {
    if (!sprite) {
        return;
    }
    
    if (sprite->scanlines) {
        for (int i = 0; i < sprite->height; i++) {
            if (sprite->scanlines[i]) {
                free(sprite->scanlines[i]);
            }
        }
        free(sprite->scanlines);
        sprite->scanlines = NULL;
    }
    
    if (sprite->scanline_lens) {
        free(sprite->scanline_lens);
        sprite->scanline_lens = NULL;
    }
    
    sprite->width = 0;
    sprite->height = 0;
    sprite->is_valid = 0;
}

/**
 * 스프라이트 RLE 데이터를 RGBA32 픽셀로 디코딩
 * 요구사항: 3.6, 3.7, 6.3, 6.4, 6.5
 */
int sprite_decode(const Sprite* sprite, DecodedSprite* decoded, 
                  uint16_t colorkey) {
    if (!sprite || !decoded) {
        return -1;
    }
    
    /* 디코딩된 스프라이트 초기화 */
    memset(decoded, 0, sizeof(DecodedSprite));
    decoded->width = sprite->width;
    decoded->height = sprite->height;
    decoded->texture = NULL;
    
    /* 빈 스프라이트 처리 */
    if (sprite->width == 0 || sprite->height == 0) {
        decoded->pixels = NULL;
        return 0;
    }
    
    /* 픽셀 버퍼 할당 */
    size_t pixel_count = (size_t)sprite->width * sprite->height;
    decoded->pixels = (uint32_t*)calloc(pixel_count, sizeof(uint32_t));
    if (!decoded->pixels) {
        return -3;
    }
    
    /* 각 스캔라인 디코딩 */
    for (int y = 0; y < sprite->height; y++) {
        uint32_t* row = decoded->pixels + (y * sprite->width);
        
        /* 행을 투명으로 초기화 */
        for (int x = 0; x < sprite->width; x++) {
            row[x] = 0;  /* RGBA(0,0,0,0) - 요구사항 6.4 */
        }
        
        if (!sprite->scanlines[y] || sprite->scanline_lens[y] == 0) {
            continue;  /* 빈 스캔라인 */
        }
        
        uint16_t* rle_data = sprite->scanlines[y];
        
        /* 세그먼트 개수 읽기 - 요구사항 3.6 */
        int count = rle_data[0];
        int rle_index = 1;
        int x = 0;
        
        /* 각 세그먼트 처리 - 요구사항 3.7 */
        for (int seg = 0; seg < count && x < sprite->width; seg++) {
            int trans_count = rle_data[rle_index++];  /* 투명 픽셀 수 */
            int color_count = rle_data[rle_index++];  /* 색상 픽셀 수 */
            
            /* 투명 픽셀 건너뛰기 (이미 0으로 초기화됨) */
            x += trans_count;
            
            /* 색상 픽셀 복사 - 요구사항 6.5 */
            for (int c = 0; c < color_count && x < sprite->width; c++) {
                uint16_t pixel = rle_data[rle_index++];
                RGBA32 rgba = rgb565_to_rgba32(pixel, colorkey);
                
                /* RGBA32를 uint32_t로 패킹 (SDL용 ABGR/RGBA 순서) */
                row[x] = ((uint32_t)rgba.r) |
                         ((uint32_t)rgba.g << 8) |
                         ((uint32_t)rgba.b << 16) |
                         ((uint32_t)rgba.a << 24);
                x++;
            }
        }
    }
    
    return 0;
}

/**
 * 디코딩된 스프라이트 리소스 해제
 */
void decoded_sprite_free(DecodedSprite* decoded) {
    if (!decoded) {
        return;
    }
    
    if (decoded->pixels) {
        free(decoded->pixels);
        decoded->pixels = NULL;
    }
    
    if (decoded->texture) {
        SDL_DestroyTexture(decoded->texture);
        decoded->texture = NULL;
    }
    
    decoded->width = 0;
    decoded->height = 0;
}

/**
 * 디코딩된 스프라이트로부터 SDL 텍스처 생성
 * 요구사항: 6.1, 6.2, 6.6
 */
int decoded_sprite_create_texture(DecodedSprite* decoded, 
                                  SDL_Renderer* renderer) {
    if (!decoded || !renderer) {
        return -1;
    }
    
    /* 빈 스프라이트 처리 */
    if (decoded->width == 0 || decoded->height == 0 || !decoded->pixels) {
        decoded->texture = NULL;
        return 0;
    }
    
    /* 기존 텍스처가 있으면 해제 */
    if (decoded->texture) {
        SDL_DestroyTexture(decoded->texture);
        decoded->texture = NULL;
    }
    
    /* RGBA32 포맷으로 텍스처 생성 - 요구사항 6.1 */
    decoded->texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        decoded->width,
        decoded->height
    );
    
    if (!decoded->texture) {
        return -1;
    }
    
    /* 투명 처리를 위한 블렌드 모드 설정 - 요구사항 6.2 */
    if (SDL_SetTextureBlendMode(decoded->texture, SDL_BLENDMODE_BLEND) != 0) {
        SDL_DestroyTexture(decoded->texture);
        decoded->texture = NULL;
        return -1;
    }
    
    /* 픽셀 데이터를 텍스처에 업로드 - 요구사항 6.3 */
    int pitch = decoded->width * sizeof(uint32_t);
    if (SDL_UpdateTexture(decoded->texture, NULL, decoded->pixels, pitch) != 0) {
        SDL_DestroyTexture(decoded->texture);
        decoded->texture = NULL;
        return -1;
    }
    
    return 0;
}
