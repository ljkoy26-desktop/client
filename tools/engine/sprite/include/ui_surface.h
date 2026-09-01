/**
 * @file ui_surface.h
 * @brief UI 렌더링을 위한 SDL2 서피스 래퍼
 * 
 * 스프라이트 블릿 및 기본 도형 그리기를 위한 잠금 가능한(lockable) 텍스처 서피스를 제공합니다.
 * CSpriteSurfaceSDL의 C 구현체입니다.
 * 
 * 요구사항: 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8
 */

#ifndef UI_SURFACE_H
#define UI_SURFACE_H

#include <stdint.h>
#include "sprite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UI_Surface UI_Surface;

typedef struct {
    int x, y;
    int w, h;
} UI_Rect;

/* ============================================================================
 * 라이프사이클 (생성/소멸)
 * ============================================================================ */

/**
 * 스트리밍 텍스처를 사용하여 UI 서피스 초기화
 * @param surface UI_Surface 구조체 포인터
 * @param renderer 사용할 SDL 렌더러
 * @param width 서피스 너비 (픽셀 단위)
 * @param height 서피스 높이 (픽셀 단위)
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int ui_surface_init(UI_Surface* surface, SDL_Renderer* renderer, int width, int height);

/**
 * UI 서피스를 동적 할당 및 초기화
 * @param renderer 사용할 SDL 렌더러
 * @param width 서피스 너비 (픽셀 단위)
 * @param height 서피스 높이 (픽셀 단위)
 * @return 할당된 UI_Surface 포인터, 실패 시 NULL
 */
UI_Surface* ui_surface_create(SDL_Renderer* renderer, int width, int height);

SDL_Renderer* ui_surface_get_render(UI_Surface *surface);

/**
 * 서피스 리소스 해제
 * @param surface UI_Surface 구조체 포인터
 */
void ui_surface_free(UI_Surface* surface);

/* ============================================================================
 * 픽셀 접근을 위한 Lock/Unlock
 * ============================================================================ */

/**
 * 직접 픽셀 접근을 위해 서피스 잠금(Lock)
 * @param surface UI_Surface 구조체 포인터
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int ui_surface_lock(UI_Surface* surface);

/**
 * 픽셀 접근 후 서피스 잠금 해제(Unlock)
 * @param surface UI_Surface 구조체 포인터
 * @return 성공 시 0, 실패 시 음수 에러 코드
 */
int ui_surface_unlock(UI_Surface* surface);

/**
 * 서피스가 잠겨 있는지 확인
 * @param surface UI_Surface 구조체 포인터
 * @return 잠겨 있으면 1, 그렇지 않으면 0
 */
int ui_surface_is_locked(UI_Surface* surface);

/* ============================================================================
 * 스프라이트 블릿 (Blitting)
 * ============================================================================ */

/**
 * 디코딩된 스프라이트를 서피스에 블릿
 * @param surface 대상 서피스
 * @param x X 좌표
 * @param y Y 좌표
 * @param sprite 블릿할 디코딩된 스프라이트
 */
void ui_surface_blit_sprite(UI_Surface* surface, int x, int y, DecodedSprite* sprite);

/**
 * 알파 블렌딩을 적용하여 스프라이트 블릿
 * @param surface 대상 서피스
 * @param x X 좌표
 * @param y Y 좌표
 * @param sprite 블릿할 디코딩된 스프라이트
 * @param alpha 알파 값 (0-255, 0=투명, 255=불투명)
 */
void ui_surface_blit_sprite_alpha(UI_Surface* surface, int x, int y, DecodedSprite* sprite, uint8_t alpha);

/**
 * 색상 틴트(Color Tinting)를 적용하여 스프라이트 블릿
 * @param surface 대상 서피스
 * @param x X 좌표
 * @param y Y 좌표
 * @param sprite 블릿할 디코딩된 스프라이트
 * @param color 틴트할 RGB 색상 (0x00RRGGBB 포맷)
 */
void ui_surface_blit_sprite_color(UI_Surface* surface, int x, int y, DecodedSprite* sprite, uint32_t color);

/**
 * 어두움(Darkness) 효과를 적용하여 스프라이트 블릿
 * @param surface 대상 서피스
 * @param x X 좌표
 * @param y Y 좌표
 * @param sprite 블릿할 디코딩된 스프라이트
 * @param darkness 어두움 레벨 (0=보통, 255=검정)
 */
void ui_surface_blit_sprite_darkness(UI_Surface* surface, int x, int y, DecodedSprite* sprite, uint8_t darkness);

/* ============================================================================
 * 기본 도형 그리기 (Primitives)
 * ============================================================================ */

/**
 * 단색으로 사각형 채우기
 * @param surface 대상 서피스
 * @param rect 채울 사각형 영역 (전체 서피스는 NULL)
 * @param color RGBA 색상 (0xRRGGBBAA 포맷)
 */
void ui_surface_fill_rect(UI_Surface* surface, UI_Rect* rect, uint32_t color);

/**
 * 수평선 그리기
 * @param surface 대상 서피스
 * @param x 시작 X 좌표
 * @param y Y 좌표
 * @param length 선 길이 (픽셀 단위)
 * @param color RGBA 색상 (0xRRGGBBAA 포맷)
 */
void ui_surface_hline(UI_Surface* surface, int x, int y, int length, uint32_t color);

/**
 * 수직선 그리기
 * @param surface 대상 서피스
 * @param x X 좌표
 * @param y 시작 Y 좌표
 * @param length 선 길이 (픽셀 단위)
 * @param color RGBA 색상 (0xRRGGBBAA 포맷)
 */
void ui_surface_vline(UI_Surface* surface, int x, int y, int length, uint32_t color);

/**
 * 사각형 테두리 그리기
 * @param surface 대상 서피스
 * @param rect 그릴 사각형 영역
 * @param color RGBA 색상 (0xRRGGBBAA 포맷)
 */
void ui_surface_draw_rect(UI_Surface* surface, UI_Rect* rect, uint32_t color);

/**
 * 서피스 전체를 특정 색상으로 지우기
 * @param surface 대상 서피스
 * @param color RGBA 색상 (0xRRGGBBAA 포맷)
 */
void ui_surface_clear(UI_Surface* surface, uint32_t color);

/**
 * 한 UI_Surface에서 다른 UI_Surface로 블릿
 * @param dst 대상 서피스
 * @param dst_rect 대상 사각형 영역 (전체 dst는 NULL)
 * @param src 원본 서피스
 * @param src_rect 원본 사각형 영역 (전체 src는 NULL)
 */
void ui_surface_blit_surface(UI_Surface* dst, const UI_Rect* dst_rect,
                             UI_Surface* src, const UI_Rect* src_rect);

struct SDL_Texture;

/* ============================================================================
 * 접근자 (Accessors)
 * ============================================================================ */

/**
 * 렌더링용 SDL 텍스처 반환
 * @param surface UI_Surface 구조체 포인터
 * @return SDL_Texture 포인터
 */
SDL_Texture* ui_surface_get_texture(UI_Surface* surface);

/**
 * 서피스 크기 반환
 * @param surface UI_Surface 구조체 포인터
 * @param width 너비 출력 포인터 (NULL 가능)
 * @param height 높이 출력 포인터 (NULL 가능)
 */
void ui_surface_get_size(UI_Surface* surface, int* width, int* height);

#ifdef __cplusplus
}
#endif

#endif /* UI_SURFACE_H */
