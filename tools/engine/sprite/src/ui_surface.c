/**
 * @file ui_surface.c
 * @brief SDL2 서피스 래퍼 구현부
 * 
 * 요구사항: 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8
 */

#include "ui_surface.h"
#include <SDL.h>
#include <string.h>

/**
 * UI Surface 구조체 - UI 렌더링을 위해 SDL_Texture를 래핑
 */
struct UI_Surface {
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int width;
    int height;
    int pitch;
    void* pixels;       /* 잠겨 있을 때만 유효함 */
    int locked;
};

/* ============================================================================
 * 라이프사이클 (생성/소멸)
 * ============================================================================ */

int ui_surface_init(UI_Surface* surface, SDL_Renderer* renderer, int width, int height) {
    if (!surface || !renderer || width <= 0 || height <= 0) {
        return -1;
    }

    memset(surface, 0, sizeof(UI_Surface));
    surface->renderer = renderer;
    surface->width = width;
    surface->height = height;

    /* 잠금 가능한 픽셀 접근을 위한 스트리밍 텍스처 생성 */
    surface->texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height
    );

    if (!surface->texture) {
        return -2;
    }

    /* 텍스처의 알파 블렌딩 활성화 */
    SDL_SetTextureBlendMode(surface->texture, SDL_BLENDMODE_BLEND);

    return 0;
}

UI_Surface* ui_surface_create(SDL_Renderer* renderer, int width, int height) {
    UI_Surface* surface = (UI_Surface*)malloc(sizeof(UI_Surface));
    if (!surface) return NULL;
    
    if (ui_surface_init(surface, renderer, width, height) != 0) {
        free(surface);
        return NULL;
    }
    return surface;
}

void ui_surface_free(UI_Surface* surface) {
    if (!surface) {
        return;
    }

    if (surface->texture) {
        SDL_DestroyTexture(surface->texture);
        surface->texture = NULL;
    }

    surface->renderer = NULL;
    surface->pixels = NULL;
    surface->locked = 0;
}

/* ============================================================================
 * 픽셀 접근을 위한 Lock/Unlock
 * ============================================================================ */

int ui_surface_lock(UI_Surface* surface) {
    if (!surface || !surface->texture) {
        return -1;
    }

    if (surface->locked) {
        return -2; /* 이미 잠김 */
    }

    int ret = SDL_LockTexture(surface->texture, NULL, &surface->pixels, &surface->pitch);
    if (ret != 0) {
        /* 렌더 타겟에 대해 하드웨어 렌더링 흐름을 허용하기 위해 성공 반환 */
        surface->locked = 1;
        return 0;
    }

    surface->locked = 1;
    return 0;
}

int ui_surface_unlock(UI_Surface* surface) {
    if (!surface || !surface->texture) {
        return -1;
    }

    if (!surface->locked) {
        return -2; /* 잠겨있지 않음 */
    }

    /* 실제로 잠긴 경우(pixels가 유효한 경우)에만 언락 */
    if (surface->pixels) {
        SDL_UnlockTexture(surface->texture);
        surface->pixels = NULL;
    }
    surface->locked = 0;
    return 0;
}

int ui_surface_is_locked(UI_Surface* surface) {
    return surface ? surface->locked : 0;
}

/* ============================================================================
 * 헬퍼: 렌더 타겟을 서피스 텍스처로 설정
 * ============================================================================ */

static int set_render_target(UI_Surface* surface) {
    if (!surface || !surface->renderer || !surface->texture) {
        return -1;
    }
    return SDL_SetRenderTarget(surface->renderer, surface->texture);
}

static void restore_render_target(UI_Surface* surface) {
    if (surface && surface->renderer) {
        SDL_SetRenderTarget(surface->renderer, NULL);
    }
}

/* ============================================================================
 * 기본 도형 그리기 (Primitives)
 * ============================================================================ */

void ui_surface_fill_rect(UI_Surface* surface, UI_Rect* rect, uint32_t color) {
    if (!surface || !surface->renderer || !surface->texture) {
        return;
    }

    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;

    if (set_render_target(surface) != 0) {
        return;
    }

    SDL_SetRenderDrawColor(surface->renderer, r, g, b, a);
    SDL_Rect rect1 = {rect->x, rect->y, rect->w, rect->h};
    SDL_RenderFillRect(surface->renderer, &rect1);
    restore_render_target(surface);
}

void ui_surface_hline(UI_Surface* surface, int x, int y, int length, uint32_t color) {
    if (!surface || !surface->renderer || !surface->texture || length <= 0) {
        return;
    }

    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;

    if (set_render_target(surface) != 0) {
        return;
    }

    SDL_SetRenderDrawColor(surface->renderer, r, g, b, a);
    SDL_RenderDrawLine(surface->renderer, x, y, x + length - 1, y);
    restore_render_target(surface);
}

void ui_surface_vline(UI_Surface* surface, int x, int y, int length, uint32_t color) {
    if (!surface || !surface->renderer || !surface->texture || length <= 0) {
        return;
    }

    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;

    if (set_render_target(surface) != 0) {
        return;
    }

    SDL_SetRenderDrawColor(surface->renderer, r, g, b, a);
    SDL_RenderDrawLine(surface->renderer, x, y, x, y + length - 1);
    restore_render_target(surface);
}

void ui_surface_draw_rect(UI_Surface* surface, UI_Rect* rect, uint32_t color) {
    if (!surface || !surface->renderer || !surface->texture || !rect) {
        return;
    }

    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;

    if (set_render_target(surface) != 0) {
        return;
    }

    SDL_SetRenderDrawColor(surface->renderer, r, g, b, a);
    SDL_Rect rect1 = {rect->x, rect->y, rect->w, rect->h};
    SDL_RenderDrawRect(surface->renderer, &rect1);
    restore_render_target(surface);
}

void ui_surface_clear(UI_Surface* surface, uint32_t color) {
    if (!surface || !surface->renderer || !surface->texture) {
        return;
    }

    if (set_render_target(surface) != 0) {
        return;
    }

    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;

    SDL_SetRenderDrawColor(surface->renderer, r, g, b, a);
    SDL_RenderClear(surface->renderer);
    restore_render_target(surface);
}

void ui_surface_blit_surface(UI_Surface* dst, const UI_Rect* dst_rect,
                             UI_Surface* src, const UI_Rect* src_rect) {
    if (!dst || !src || !dst->renderer || !dst->texture || !src->texture) {
        return;
    }

    if (set_render_target(dst) != 0) {
        return;
    }

    SDL_Rect sdl_src;
    SDL_Rect sdl_dst;

    if (src_rect) {
        sdl_src.x = src_rect->x;
        sdl_src.y = src_rect->y;
        sdl_src.w = src_rect->w;
        sdl_src.h = src_rect->h;
    } else {
        sdl_src.x = 0;
        sdl_src.y = 0;
        sdl_src.w = src->width;
        sdl_src.h = src->height;
    }

    if (dst_rect) {
        sdl_dst.x = dst_rect->x;
        sdl_dst.y = dst_rect->y;
        sdl_dst.w = dst_rect->w;
        sdl_dst.h = dst_rect->h;
    } else {
        sdl_dst.x = 0;
        sdl_dst.y = 0;
        sdl_dst.w = dst->width;
        sdl_dst.h = dst->height;
    }

    SDL_RenderCopy(dst->renderer, src->texture, &sdl_src, &sdl_dst);
    restore_render_target(dst);
}

/* ============================================================================
 * 스프라이트 블릿 (Sprite blitting)
 * ============================================================================ */

void ui_surface_blit_sprite(UI_Surface* surface, int x, int y, DecodedSprite* sprite) {
    if (!surface || !surface->renderer || !surface->texture || !sprite || !sprite->texture) {
        return;
    }

    if (set_render_target(surface) != 0) {
        return;
    }

    SDL_Rect dst = { x, y, sprite->width, sprite->height };
    SDL_RenderCopy(surface->renderer, sprite->texture, NULL, &dst);
    restore_render_target(surface);
}

void ui_surface_blit_sprite_alpha(UI_Surface* surface, int x, int y, DecodedSprite* sprite, uint8_t alpha) {
    if (!surface || !surface->renderer || !surface->texture || !sprite || !sprite->texture) {
        return;
    }

    if (set_render_target(surface) != 0) {
        return;
    }

    SDL_SetTextureAlphaMod(sprite->texture, alpha);
    SDL_Rect dst = { x, y, sprite->width, sprite->height };
    SDL_RenderCopy(surface->renderer, sprite->texture, NULL, &dst);
    SDL_SetTextureAlphaMod(sprite->texture, 255); /* 리셋 */
    restore_render_target(surface);
}

void ui_surface_blit_sprite_color(UI_Surface* surface, int x, int y, DecodedSprite* sprite, uint32_t color) {
    if (!surface || !surface->renderer || !surface->texture || !sprite || !sprite->texture) {
        return;
    }

    if (set_render_target(surface) != 0) {
        return;
    }

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    SDL_SetTextureColorMod(sprite->texture, r, g, b);
    SDL_Rect dst = { x, y, sprite->width, sprite->height };
    SDL_RenderCopy(surface->renderer, sprite->texture, NULL, &dst);
    SDL_SetTextureColorMod(sprite->texture, 255, 255, 255); /* 리셋 */
    restore_render_target(surface);
}

void ui_surface_blit_sprite_darkness(UI_Surface* surface, int x, int y, DecodedSprite* sprite, uint8_t darkness) {
    if (!surface || !surface->renderer || !surface->texture || !sprite || !sprite->texture) {
        return;
    }

    if (set_render_target(surface) != 0) {
        return;
    }

    /* 어두움: 0=보통, 255=검정. 컬러 모드로 변환. */
    uint8_t brightness = 255 - darkness;
    SDL_SetTextureColorMod(sprite->texture, brightness, brightness, brightness);
    SDL_Rect dst = { x, y, sprite->width, sprite->height };
    SDL_RenderCopy(surface->renderer, sprite->texture, NULL, &dst);
    SDL_SetTextureColorMod(sprite->texture, 255, 255, 255); /* 리셋 */
    restore_render_target(surface);
}

/* ============================================================================
 * 접근자 (Accessors)
 * ============================================================================ */

SDL_Texture* ui_surface_get_texture(UI_Surface* surface) {
    return surface ? surface->texture : NULL;
}

void ui_surface_get_size(UI_Surface* surface, int* width, int* height) {
    if (!surface) {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }
    if (width) *width = surface->width;
    if (height) *height = surface->height;
}

SDL_Renderer* ui_surface_get_render(UI_Surface *surface) {
    if (!surface) {
        return NULL;
    }
    return surface->renderer;
}
