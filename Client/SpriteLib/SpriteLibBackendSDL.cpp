/*-----------------------------------------------------------------------------

	SpriteLibBackendSDL.cpp

	SpriteLib을 위한 SDL2 백엔드 구현부.
	SDL2를 사용하여 통합 백엔드 인터페이스를 구현한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#include "SpriteLibBackendSDL.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * 전역 상태
 * ============================================================================ */

static int g_spritectl_initialized = 0;
static SDL_Renderer* g_spritectl_default_renderer = NULL;

/* spritectl_blt_sprite()의 non-RLE 폴백 경로(아래 참조)에서 재사용하는 스크래치
 * 서피스 - 매 호출마다 할당/해제하는 대신, 지금까지 블릿된 스프라이트 중 가장
 * 큰 크기로 늘어난다. */
static SDL_Surface* g_spritectl_blt_scratch = NULL;
static int g_spritectl_blt_scratch_w = 0;
static int g_spritectl_blt_scratch_h = 0;

/* ============================================================================
 * 초기화
 * ============================================================================ */

int spritectl_init(void) {
	if (g_spritectl_initialized) {
		return 0;  /* 이미 초기화됨 */
	}

	/* SDL 비디오 서브시스템을 초기화한다 */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SpriteLib Backend: SDL_Init failed: %s\n", SDL_GetError());
		return -1;
	}

	g_spritectl_initialized = 1;
	return 0;
}

void spritectl_shutdown(void) {
	if (!g_spritectl_initialized) {
		return;
	}

	/* 기본 렌더러를 정리한다 */
	if (g_spritectl_default_renderer) {
		SDL_DestroyRenderer(g_spritectl_default_renderer);
		g_spritectl_default_renderer = NULL;
	}

	if (g_spritectl_blt_scratch) {
		SDL_FreeSurface(g_spritectl_blt_scratch);
		g_spritectl_blt_scratch = NULL;
		g_spritectl_blt_scratch_w = 0;
		g_spritectl_blt_scratch_h = 0;
	}

	SDL_Quit();
	g_spritectl_initialized = 0;
}

/* ============================================================================
 * 서피스 함수
 * ============================================================================ */

SDL_Surface* spritectl_sdl_create_surface(int width, int height, int format) {
	SDL_Surface* surf = NULL;

	/* 비트 깊이와 마스크를 계산한다 */
	switch (format) {
		case SPRITECTL_FORMAT_RGB565:
			/* RGB565: R은 11~15비트, G는 5~10비트, B는 0~4비트 */
			surf = SDL_CreateRGBSurface(0, width, height, 16,
			                            0xF800,  /* R 마스크 */
			                            0x07E0,  /* G 마스크 */
			                            0x001F,  /* B 마스크 */
			                            0x0000); /* A 마스크 */
			if (surf) {
				static int debug_565_count = 0;
				if (debug_565_count < 3) {
					fprintf(stderr, "Created RGB565 surface: requested format=RGB565, actual format=%s\n",
						SDL_GetPixelFormatName(surf->format->format));
					debug_565_count++;
				}
			}
			return surf;
		case SPRITECTL_FORMAT_RGB555:
			/* RGB555: R은 10~14비트, G는 5~9비트, B는 0~4비트 */
			surf = SDL_CreateRGBSurface(0, width, height, 16,
			                            0x7C00,  /* R 마스크 */
			                            0x03E0,  /* G 마스크 */
			                            0x001F,  /* B 마스크 */
			                            0x0000); /* A 마스크 */
			if (surf) {
				static int debug_555_count = 0;
				if (debug_555_count < 3) {
					fprintf(stderr, "Created RGB555 surface: requested format=RGB555, actual format=%s\n",
						SDL_GetPixelFormatName(surf->format->format));
					debug_555_count++;
				}
			}
			return surf;
		case SPRITECTL_FORMAT_RGBA32:
			/* RGBA32: 표준 바이트 순서 */
			return SDL_CreateRGBSurface(0, width, height, 32,
			                            0x000000FF,  /* R 마스크 */
			                            0x0000FF00,  /* G 마스크 */
			                            0x00FF0000,  /* B 마스크 */
			                            0xFF000000); /* A 마스크 */
		default:
			/* 기본값은 RGB565 */
			surf = SDL_CreateRGBSurface(0, width, height, 16,
			                            0xF800,  /* R 마스크 */
			                            0x07E0,  /* G 마스크 */
			                            0x001F,  /* B 마스크 */
			                            0x0000); /* A 마스크 */
			if (surf) {
				fprintf(stderr, "Created DEFAULT surface: actual format=%s\n",
					SDL_GetPixelFormatName(surf->format->format));
			}
			return surf;
	}
}

uint32_t spritectl_sdl_get_pixelformat(int format) {
	switch (format) {
		case SPRITECTL_FORMAT_RGB565:
			return SDL_PIXELFORMAT_RGB565;
		case SPRITECTL_FORMAT_RGB555:
			return SDL_PIXELFORMAT_RGB555;
		case SPRITECTL_FORMAT_RGBA32:
			return SDL_PIXELFORMAT_RGBA32;
		default:
			return SDL_PIXELFORMAT_UNKNOWN;
	}
}

spritectl_surface_t spritectl_create_surface(int width, int height, int format) {
	spritectl_surface_t surface;

	if (!g_spritectl_initialized) {
		fprintf(stderr, "SpriteLib Backend: Not initialized\n");
		return SPRITECTL_INVALID_SURFACE;
	}

	/* 서피스 구조체를 할당한다 */
	surface = (spritectl_surface_t)malloc(sizeof(struct spritectl_surface_s));
	if (!surface) {
		fprintf(stderr, "SpriteLib Backend: Failed to allocate surface\n");
		return SPRITECTL_INVALID_SURFACE;
	}

	/* SDL 서피스를 생성한다 */
	surface->surface = spritectl_sdl_create_surface(width, height, format);
	if (!surface->surface) {
		free(surface);
		return SPRITECTL_INVALID_SURFACE;
	}

	/* 필드를 초기화한다 */
	surface->texture = NULL;
	surface->renderer = NULL;
	surface->width = width;
	surface->height = height;
	surface->format = format;
	surface->locked = 0;
	surface->ref_count = 1;

	return surface;
}

void spritectl_destroy_surface(spritectl_surface_t surface) {
	if (!surface) {
		return;
	}

	surface->ref_count--;
	if (surface->ref_count > 0) {
		return;  /* 아직 참조되고 있음 */
	}

	/* SDL 리소스를 해제한다 */
	if (surface->texture) {
		SDL_DestroyTexture(surface->texture);
	}
	if (surface->surface) {
		SDL_FreeSurface(surface->surface);
	}

	free(surface);
}

int spritectl_lock_surface(spritectl_surface_t surface, spritectl_surface_info_t* info) {
	if (!surface || !info) {
		return -1;
	}

	/* SDL 서피스를 잠근다 */
	if (SDL_LockSurface(surface->surface) != 0) {
		fprintf(stderr, "SpriteLib Backend: Failed to lock surface: %s\n", SDL_GetError());
		return -1;
	}

	surface->locked++;

	/* info 구조체를 채운다 */
	info->width = surface->width;
	info->height = surface->height;
	info->pitch = surface->surface->pitch;
	info->pixels = surface->surface->pixels;
	info->format = surface->format;

	return 0;
}

int spritectl_unlock_surface(spritectl_surface_t surface) {
	if (!surface || surface->locked == 0) {
		return -1;
	}

	SDL_UnlockSurface(surface->surface);
	surface->locked--;

	return 0;
}

int spritectl_clear_surface(spritectl_surface_t surface, uint32_t color) {
	SDL_Rect rect;

	if (!surface) {
		return -1;
	}

	/* 서피스 전체를 색상으로 채운다 */
	rect.x = 0;
	rect.y = 0;
	rect.w = surface->width;
	rect.h = surface->height;

	SDL_FillRect(surface->surface, &rect, (Uint32)color);

	return 0;
}

int spritectl_get_surface_size(spritectl_surface_t surface, int* width, int* height) {
	if (!surface) {
		return -1;
	}

	if (width) *width = surface->width;
	if (height) *height = surface->height;

	return 0;
}

/* ============================================================================
 * 스프라이트 함수
 * ============================================================================ */

spritectl_sprite_t spritectl_create_sprite(int width, int height, int format,
                                           const void* pixels, size_t data_size) {
	spritectl_sprite_t sprite;

	if (!pixels || data_size == 0) {
		return SPRITECTL_INVALID_SPRITE;
	}

	/* 스프라이트 구조체를 할당한다 */
	sprite = (spritectl_sprite_t)malloc(sizeof(struct spritectl_sprite_s));
	if (!sprite) {
		fprintf(stderr, "SpriteLib Backend: Failed to allocate sprite\n");
		return SPRITECTL_INVALID_SPRITE;
	}

	/* 필드를 초기화한다 */
	sprite->width = width;
	sprite->height = height;
	sprite->format = format;
	sprite->data_size = data_size;
	sprite->rgba_pixels = NULL;
	sprite->scanline_rle = NULL;
	sprite->scanline_lens = NULL;
	sprite->has_rle = 0;
	sprite->ref_count = 1;

	/* 픽셀 데이터를 할당하고 복사한다 */
	sprite->pixels = (uint16_t*)malloc(data_size);
	if (!sprite->pixels) {
		free(sprite);
		return SPRITECTL_INVALID_SPRITE;
	}
	memcpy(sprite->pixels, pixels, data_size);

	return sprite;
}

void spritectl_destroy_sprite(spritectl_sprite_t sprite) {
	if (!sprite) {
		return;
	}

	sprite->ref_count--;
	if (sprite->ref_count > 0) {
		return;  /* 아직 참조되고 있음 */
	}

	/* 픽셀 데이터를 해제한다 */
	if (sprite->pixels) {
		free(sprite->pixels);
	}
	if (sprite->rgba_pixels) {
		free(sprite->rgba_pixels);
	}

	/* RLE 데이터를 해제한다 */
	if (sprite->scanline_rle) {
		for (int y = 0; y < sprite->height; y++) {
			if (sprite->scanline_rle[y]) {
				free(sprite->scanline_rle[y]);
			}
		}
		free(sprite->scanline_rle);
	}
	if (sprite->scanline_lens) {
		free(sprite->scanline_lens);
	}

	free(sprite);
}

int spritectl_get_sprite_info(spritectl_sprite_t sprite, spritectl_sprite_info_t* info) {
	if (!sprite || !info) {
		return -1;
	}

	info->width = sprite->width;
	info->height = sprite->height;
	info->format = sprite->format;
	info->data_size = sprite->data_size;

	return 0;
}

size_t spritectl_get_sprite_data(spritectl_sprite_t sprite, void* buffer, size_t buffer_size) {
	if (!sprite) {
		return 0;
	}

	if (!buffer) {
		return sprite->data_size;  /* 필요한 크기를 반환한다 */
	}

	if (buffer_size < sprite->data_size) {
		return 0;  /* 버퍼가 너무 작음 */
	}

	memcpy(buffer, sprite->pixels, sprite->data_size);
	return sprite->data_size;
}

/* ================================================================================
* 블리팅 함수
* ================================================================================= */

/**
* RLE 기반 스프라이트 렌더링 - 원래 DirectX의 BltClipWidth 함수를 모방합니다.
* 이 함수는 RLE 데이터를 직접 처리하고 투명하지 않은 픽셀만 대상 표면에 씁니다.

이는 원래 DirectX 구현과 정확히 동일합니다.
*
* SDL 블리팅과의 주요 차이점:
* - 투명 픽셀은 블렌딩되지 않고 건너뛰어집니다.
* - 즉, 대상 표면의 원래 콘텐츠가 그대로 나타납니다.
*/
int spritectl_blt_sprite_rle(spritectl_surface_t dest, int x, int y,
	spritectl_sprite_t sprite, int flags, int alpha) {
	if (!dest || !sprite || !sprite->has_rle || !sprite->scanline_rle) {
		return -1;
	}

	SDL_Surface* sdl_surface = dest->surface;
	if (!sdl_surface) {
		return -1;
	}

	/* 픽셀에 직접 접근하기 위해 대상 서피스를 잠근다 */
	if (SDL_MUSTLOCK(sdl_surface)) {
		SDL_LockSurface(sdl_surface);
	}

	/* 대상 서피스 정보를 가져온다 */
	int dest_width = sdl_surface->w;
	int dest_height = sdl_surface->h;
	int dest_pitch = sdl_surface->pitch;
	int dest_bytes_per_pixel = sdl_surface->format->BytesPerPixel;
	int dest_stride = dest_pitch / dest_bytes_per_pixel;  /* 한 행당 픽셀 수 */

	/* 클리핑 사각형을 계산한다 */
	int clip_left = (x < 0) ? -x : 0;
	int clip_top = (y < 0) ? -y : 0;
	int clip_right = (x + sprite->width > dest_width) ? dest_width - x : sprite->width;
	int clip_bottom = (y + sprite->height > dest_height) ? dest_height - y : sprite->height;

	if (clip_left >= clip_right || clip_top >= clip_bottom) 
	{
		if (SDL_MUSTLOCK(sdl_surface)) 
		{
			SDL_UnlockSurface(sdl_surface);
		}
		return 0;  /* 완전히 클리핑되었지만 에러는 아니다 */
	}

	/* 디버그: 첫 호출 시 서피스 정보를 출력한다 */
	static int debug_printed = 0;
	if (debug_printed < 3) 
	{
		fprintf(stderr, "[RLE BLT] sprite=%dx%d, pos=(%d,%d), surface=%dx%d, bpp=%d, pitch=%d, stride=%d\n",
			sprite->width, sprite->height, x, y, dest_width, dest_height, dest_bytes_per_pixel, dest_pitch, dest_stride);
		debug_printed++;
	}

	/* 각 스캔라인을 처리한다 */
	for (int sy = clip_top; sy < clip_bottom; sy++)
	{
		if (!sprite->scanline_rle[sy] || sprite->scanline_lens[sy] == 0)
		{
			continue;  /* 비어있는 스캔라인 */
		}

		/* 대상 행 포인터를 가져온다 - 포맷 유연성을 위해 uint8_t*를 사용한다 */
		uint8_t* dest_row_bytes = (uint8_t*)sdl_surface->pixels + (y + sy) * dest_pitch;

		/* RLE 세그먼트를 처리한다 */
		uint16_t* rle_data = sprite->scanline_rle[sy];
		uint16_t rle_data_size = sprite->scanline_lens[sy];
		int rle_index = 0;

		/* RLE 데이터를 검증한다 */
		if (rle_data_size < 1)
		{
			if (debug_printed < 3)
			{
				fprintf(stderr, "[RLE BLT] Invalid RLE data size: %d at scanline %d\n", rle_data_size, sy);
			}
			continue;
		}

		int seg_count = rle_data[rle_index++];
		if (rle_index + seg_count * 2 > rle_data_size)
		{
			if (debug_printed < 3)
			{
				fprintf(stderr, "[RLE BLT] RLE data corruption: seg_count=%d, size=%d at scanline %d\n",
					seg_count, rle_data_size, sy);
			}
			continue;
		}

		int sx = 0;

		for (int seg = 0; seg < seg_count; seg++)
		{
			int trans_count = rle_data[rle_index++];
			int color_count = rle_data[rle_index++];

			/* 투명 픽셀은 건너뛴다 */
			sx += trans_count;

			/* 안전 처리: 스프라이트 너비를 넘어섰다면 이 세그먼트 전체를 건너뛴다 */
			if (sx >= sprite->width)
			{
				rle_index += color_count;  /* 이 세그먼트의 색상 픽셀을 모두 건너뛴다 */
				continue;  /* 다음 세그먼트로 이동한다 */
			}

			/* 색상 픽셀을 복사한다 */
			for (int c = 0; c < color_count && sx < sprite->width; c++)
			{
				/* RLE 데이터 범위를 넘어 읽으려는지 확인한다 */
				if (rle_index >= rle_data_size)
				{
					if (debug_printed < 3)
					{
						fprintf(stderr, "[RLE BLT] RLE data overrun: rle_index=%d, size=%d, sx=%d\n",
							rle_index, rle_data_size, sx);
					}
					break;
				}

				/* 이 픽셀이 클리핑 범위 안에 있는지 확인한다 */
				if (sx >= clip_left && sx < clip_right) {
					uint16_t pixel = rle_data[rle_index];

					/* 실제 대상 위치를 계산한다 */
					int dest_x = x + sx;
					if (dest_x >= 0 && dest_x < dest_stride)
					{
						/* 대상 포맷에 따라 픽셀을 쓴다 */

						if (dest_bytes_per_pixel == 2)
						{
							/* RGB565 대상 - 직접 쓴다 */
							uint16_t* dest_row_16 = (uint16_t*)dest_row_bytes;
							dest_row_16[dest_x] = pixel;
						}
						else if (dest_bytes_per_pixel == 4)
						{
							/* RGBA32 대상 - RGB565를 RGBA32로 변환한다 */
							uint8_t r, g, b;
							spritectl_565_to_rgb(pixel, &r, &g, &b);
							uint32_t* dest_row_32 = (uint32_t*)dest_row_bytes;

							/* 필요하다면 알파 블렌딩을 적용한다 */
							if (flags & SPRITECTL_BLT_ALPHA) {
								/* 대상과 알파 블렌딩한다 */
								uint32_t dest_pixel = dest_row_32[dest_x];
								uint8_t dest_r = dest_pixel & 0xFF;
								uint8_t dest_g = (dest_pixel >> 8) & 0xFF;
								uint8_t dest_b = (dest_pixel >> 16) & 0xFF;

								/* 블렌드: src * alpha/255 + dst * (1 - alpha/255) */
								uint8_t blend_r = (r * alpha + dest_r * (255 - alpha)) / 255;
								uint8_t blend_g = (g * alpha + dest_g * (255 - alpha)) / 255;
								uint8_t blend_b = (b * alpha + dest_b * (255 - alpha)) / 255;

								dest_row_32[dest_x] = (255 << 24) | (blend_b << 16) | (blend_g << 8) | blend_r;
							}
							else
							{
								/* 불투명하게 쓴다 */
								dest_row_32[dest_x] = (255 << 24) | (b << 16) | (g << 8) | r;
							}
						}
						else if (debug_printed < 3)
						{
							fprintf(stderr, "[RLE BLT] Unsupported dest BPP: %d\n", dest_bytes_per_pixel);
						}
					}
					else if (debug_printed < 3)
					{
						fprintf(stderr, "[RLE BLT] OUT OF BOUNDS: dest_x=%d, stride=%d, sx=%d, x=%d\n",
							dest_x, dest_stride, sx, x);
					}
				}

				rle_index++;  /* 클리핑되더라도 픽셀은 소비한다 */
				sx++;
			}
		}
	}

	/* 대상 서피스 잠금을 해제한다 */
	if (SDL_MUSTLOCK(sdl_surface))
	{
		SDL_UnlockSurface(sdl_surface);
	}

	return 0;
}

spritectl_sprite_t spritectl_create_sprite_rle(int width, int height) {
	spritectl_sprite_t sprite;

	if (width <= 0 || height <= 0) {
		return SPRITECTL_INVALID_SPRITE;
	}

	/* 스프라이트 구조체를 할당한다 */
	sprite = (spritectl_sprite_t)malloc(sizeof(struct spritectl_sprite_s));
	if (!sprite) {
		fprintf(stderr, "SpriteLib Backend: Failed to allocate RLE sprite\n");
		return SPRITECTL_INVALID_SPRITE;
	}

	/* 필드를 초기화한다 */
	sprite->width = width;
	sprite->height = height;
	sprite->format = SPRITECTL_FORMAT_RGB565;  /* RLE 스프라이트는 RGB565로 가정한다 */
	sprite->data_size = 0;
	sprite->rgba_pixels = NULL;
	sprite->pixels = NULL;  /* RLE 스프라이트는 디코딩된 픽셀을 갖지 않는다 */
	sprite->ref_count = 1;
	sprite->has_rle = 1;  /* RLE 스프라이트로 표시한다 */

	/* RLE 배열을 할당한다 */
	sprite->scanline_lens = (uint16_t*)calloc(height, sizeof(uint16_t));
	if (!sprite->scanline_lens) {
		free(sprite);
		return SPRITECTL_INVALID_SPRITE;
	}

	sprite->scanline_rle = (uint16_t**)calloc(height, sizeof(uint16_t*));
	if (!sprite->scanline_rle) {
		free(sprite->scanline_lens);
		free(sprite);
		return SPRITECTL_INVALID_SPRITE;
	}

	return sprite;
}

int spritectl_sprite_set_scanline_rle(spritectl_sprite_t sprite, int y,
                                       const uint16_t* rle_data, int rle_size) {
	if (!sprite || !sprite->has_rle) {
		return -1;
	}

	if (y < 0 || y >= sprite->height) {
		return -1;
	}

	if (rle_size <= 0) {
		return -1;
	}

	/* 이 스캔라인의 기존 RLE 데이터를 해제한다 */
	if (sprite->scanline_rle[y]) {
		free(sprite->scanline_rle[y]);
	}

	/* RLE 데이터를 할당하고 복사한다 */
	sprite->scanline_rle[y] = (uint16_t*)malloc(rle_size * sizeof(uint16_t));
	if (!sprite->scanline_rle[y]) {
		return -1;
	}

	memcpy(sprite->scanline_rle[y], rle_data, rle_size * sizeof(uint16_t));
	sprite->scanline_lens[y] = rle_size;

	return 0;
}

int spritectl_blt_sprite(spritectl_surface_t dest, int x, int y,
                         spritectl_sprite_t sprite, int flags, int alpha) {
	if (!dest || !sprite) {
		return -1;
	}

	/* 스프라이트에 RLE 데이터가 있으면 (원래 DirectX처럼) RLE 기반 렌더링을 사용한다 */
	if (sprite->has_rle && sprite->scanline_rle) {
		return spritectl_blt_sprite_rle(dest, x, y, sprite, flags, alpha);
	}

	/* RLE 데이터가 없는 스프라이트는 예전 방식으로 폴백한다 */
	static int fallback_count = 0;
	if (fallback_count < 3) {
		fprintf(stderr, "[SpriteLib] WARNING: Using fallback rendering for sprite without RLE data (sprite=%p, has_rle=%d)\n",
		        (void*)sprite, sprite->has_rle);
		fallback_count++;
	}

	/* RLE 데이터가 없는 스프라이트는 예전 방식으로 폴백한다 */
	SDL_Rect dest_rect;
	SDL_Surface* src_surface = NULL;
	int result = -1;

	// 중요: 블릿할 때 대상 서피스는 잠겨 있으면 안 된다
	// 블릿 전에 잠금을 해제하고, 블릿 후 예상 상태를 유지하기 위해 다시 잠근다
	bool was_locked = (dest->locked > 0);
	int saved_lock_count = dest->locked;

	if (was_locked) {
		// 블릿을 위해 서피스 잠금을 해제한다
		while (dest->locked > 0) {
			SDL_UnlockSurface(dest->surface);
			dest->locked--;
		}
	}

	/* NOTE: 여기서는 대상 서피스를 잠그지 않는다.
	 * SDL_BlitSurface가 필요한 잠금을 내부적으로 알아서 처리한다.
	 * 블릿 전에 대상 서피스를 잠그면 잘못된 것이며 에러를 일으킨다.
	 */

	/* 매 호출마다 새 SDL_Surface를 생성/파괴하는 대신, 지금까지 블릿된 스프라이트 중
	 * 가장 큰 크기에 맞춘 영구 스크래치 서피스를 재사용한다. 이 폴백 경로는 텍스트
	 * 글리프를 포함한 모든 non-RLE 스프라이트에 대해 실행되므로(spritectl_create_sprite()는
	 * has_rle = 0을 항상 설정한다) 텍스트가 많은 화면(예: 옵션 다이얼로그의 Hotkey 탭)에서는
	 * 매 프레임 키바인드 목록을 그리는 것만으로 초당 수천 번의 surface alloc/free가
	 * 발생하고 있었다. */
	if (!g_spritectl_blt_scratch ||
	    g_spritectl_blt_scratch_w < sprite->width ||
	    g_spritectl_blt_scratch_h < sprite->height) {
		if (g_spritectl_blt_scratch) {
			SDL_FreeSurface(g_spritectl_blt_scratch);
		}
		if (g_spritectl_blt_scratch_w < sprite->width) {
			g_spritectl_blt_scratch_w = sprite->width;
		}
		if (g_spritectl_blt_scratch_h < sprite->height) {
			g_spritectl_blt_scratch_h = sprite->height;
		}
		g_spritectl_blt_scratch = SDL_CreateRGBSurface(0, g_spritectl_blt_scratch_w, g_spritectl_blt_scratch_h, 32,
		                                              0xFF, 0xFF00, 0xFF0000, 0xFF000000);
		if (!g_spritectl_blt_scratch) {
			g_spritectl_blt_scratch_w = 0;
			g_spritectl_blt_scratch_h = 0;
			return -1;
		}
	}
	src_surface = g_spritectl_blt_scratch;

	/* 캐시된 RGBA 픽셀이 있으면 사용하고, 없으면 변환 후 캐시한다 */
	if (sprite->rgba_pixels == NULL && sprite->format != SPRITECTL_FORMAT_RGBA32) {
		/* RGBA32가 아닌 포맷은 RGBA 픽셀로 변환하여 캐시한다 */
		sprite->rgba_pixels = (uint32_t*)malloc(sprite->width * sprite->height * sizeof(uint32_t));
		if (sprite->rgba_pixels) {
			if (sprite->format == SPRITECTL_FORMAT_RGB565) {
				spritectl_convert_565_to_rgba(sprite->pixels, sprite->rgba_pixels,
				                              sprite->width * sprite->height, 0x0000);
			} else if (sprite->format == SPRITECTL_FORMAT_RGB555) {
				spritectl_convert_555_to_rgba(sprite->pixels, sprite->rgba_pixels,
				                              sprite->width * sprite->height, 0x0000);
			} else {
				/* 알 수 없는 포맷이면 불투명한 흰색으로 채운다 */
				for (int i = 0; i < sprite->width * sprite->height; i++) {
					sprite->rgba_pixels[i] = 0xFFFFFFFF;
				}
			}
		}
	}

	/* 픽셀 소스를 결정한다 */
	const uint32_t* pixel_src;
	if (sprite->format == SPRITECTL_FORMAT_RGBA32) {
		pixel_src = (const uint32_t*)sprite->pixels;
	} else if (sprite->rgba_pixels) {
		pixel_src = sprite->rgba_pixels;
	} else {
		/* 폴백: 불투명한 흰색으로 채운다 */
		static uint32_t* fallback_pixels = NULL;
		static int fallback_size = 0;
		if (!fallback_pixels || fallback_size < sprite->width * sprite->height) {
			free(fallback_pixels);
			fallback_pixels = (uint32_t*)malloc(sprite->width * sprite->height * sizeof(uint32_t));
			fallback_size = sprite->width * sprite->height;
			if (fallback_pixels) {
				for (int i = 0; i < sprite->width * sprite->height; i++) {
					fallback_pixels[i] = 0xFFFFFFFF;
				}
			}
		}
		pixel_src = fallback_pixels;
	}

	/* (더 클 수도 있는) 스크래치 서피스의 왼쪽 위 모서리로 복사한다.
	 * pitch가 sprite->width * 4보다 클 수 있으므로 행 단위로 복사한다. */
	SDL_LockSurface(src_surface);
	uint8_t* dst_row = (uint8_t*)src_surface->pixels;
	const uint8_t* src_row = (const uint8_t*)pixel_src;
	size_t row_bytes = (size_t)sprite->width * sizeof(uint32_t);
	for (int row = 0; row < sprite->height; row++) {
		memcpy(dst_row, src_row, row_bytes);
		dst_row += src_surface->pitch;
		src_row += row_bytes;
	}
	SDL_UnlockSurface(src_surface);

	/* 알파 블렌딩을 처리한다. src_surface는 이제 재사용되는 스크래치 버퍼이므로(위 참조)
	 * alpha mod는 매 호출마다 무조건 리셋해야 한다 - 그렇지 않으면 non-alpha 블릿이
	 * 같은 버퍼를 공유했던 이전 alpha 블릿의 낡은 mod 값을 그대로 이어받을 수 있다. */
	SDL_SetSurfaceAlphaMod(src_surface, (flags & SPRITECTL_BLT_ALPHA) ? (Uint8)alpha : 255);

	/* 关键: 始终启用混合模式以支持 alpha 通道 */
	/* 即使不是 alpha 模式，也需要 BLEND 模式来处理 colorkey 产生的透明像素 */
	if (SDL_SetSurfaceBlendMode(src_surface, SDL_BLENDMODE_BLEND) != 0) {
		fprintf(stderr, "[SpriteLib] SDL_SetSurfaceBlendMode failed: %s\n",
		        SDL_GetError());
	}

	/* 대상 사각형을 설정한다 */
	dest_rect.x = x;
	dest_rect.y = y;
	dest_rect.w = sprite->width;
	dest_rect.h = sprite->height;

	/* 스크래치 서피스에서 스프라이트 크기만큼의 영역만 블릿한다 */
	SDL_Rect src_rect = {0, 0, sprite->width, sprite->height};
	if (SDL_BlitSurface(src_surface, &src_rect, dest->surface, &dest_rect) != 0) {
		fprintf(stderr, "SpriteLib Backend: SDL_BlitSurface failed: %s\n", SDL_GetError());
		result = -1;
	} else {
		result = 0;
	}

	/* NOTE: src_surface는 영구 스크래치 버퍼(g_spritectl_blt_scratch)이므로 여기서 해제하지 않는다. */

	// 이전에 잠겨 있었다면 예상 상태를 유지하기 위해 서피스를 다시 잠근다
	if (was_locked) {
		for (int i = 0; i < saved_lock_count; i++) {
			SDL_LockSurface(dest->surface);
			dest->locked++;
		}
	}

	return result;
}

int spritectl_blt_sprite_scaled(spritectl_surface_t dest, int x, int y,
                                spritectl_sprite_t sprite, int scale, int flags) {
	SDL_Surface* src_surface = NULL;
	SDL_Surface* scaled_surface = NULL;
	int result = -1;
	int scaled_width, scaled_height;

	if (!dest || !sprite) {
		return -1;
	}

	/* 스케일된 크기를 계산한다 (scale 단위는 1/256) */
	scaled_width = (sprite->width * scale) / 256;
	scaled_height = (sprite->height * scale) / 256;

	if (scaled_width <= 0 || scaled_height <= 0) {
		return 0;  /* 너무 작아서 보이지 않는다 */
	}

	/* 스프라이트 픽셀로부터 임시 서피스를 생성한다 */
	src_surface = SDL_CreateRGBSurface(0, sprite->width, sprite->height, 32,
	                                  0xFF, 0xFF00, 0xFF0000, 0xFF000000);
	if (!src_surface) {
		return -1;
	}

	/* 캐시된 RGBA 픽셀이 있으면 사용하고, 없으면 변환 후 캐시한다 */
	if (sprite->rgba_pixels == NULL && sprite->format != SPRITECTL_FORMAT_RGBA32) {
		/* RGBA32가 아닌 포맷은 RGBA 픽셀로 변환하여 캐시한다 */
		sprite->rgba_pixels = (uint32_t*)malloc(sprite->width * sprite->height * sizeof(uint32_t));
		if (sprite->rgba_pixels) {
			if (sprite->format == SPRITECTL_FORMAT_RGB565) {
				spritectl_convert_565_to_rgba(sprite->pixels, sprite->rgba_pixels,
				                              sprite->width * sprite->height, 0x0000);
			} else if (sprite->format == SPRITECTL_FORMAT_RGB555) {
				spritectl_convert_555_to_rgba(sprite->pixels, sprite->rgba_pixels,
				                              sprite->width * sprite->height, 0x0000);
			} else {
				/* 알 수 없는 포맷이면 불투명한 흰색으로 채운다 */
				for (int i = 0; i < sprite->width * sprite->height; i++) {
					sprite->rgba_pixels[i] = 0xFFFFFFFF;
				}
			}
		}
	}

	/* 픽셀 소스를 결정한다 */
	const uint32_t* pixel_src;
	if (sprite->format == SPRITECTL_FORMAT_RGBA32) {
		pixel_src = (const uint32_t*)sprite->pixels;
	} else if (sprite->rgba_pixels) {
		pixel_src = sprite->rgba_pixels;
	} else {
		/* 폴백: 불투명한 흰색으로 채운다 */
		static uint32_t* fallback_pixels = NULL;
		static int fallback_size = 0;
		if (!fallback_pixels || fallback_size < sprite->width * sprite->height) {
			free(fallback_pixels);
			fallback_pixels = (uint32_t*)malloc(sprite->width * sprite->height * sizeof(uint32_t));
			fallback_size = sprite->width * sprite->height;
			if (fallback_pixels) {
				for (int i = 0; i < sprite->width * sprite->height; i++) {
					fallback_pixels[i] = 0xFFFFFFFF;
				}
			}
		}
		pixel_src = fallback_pixels;
	}

	/* 임시 서피스로 복사한다 */
	SDL_LockSurface(src_surface);
	uint32_t* src_pixels = (uint32_t*)src_surface->pixels;
	memcpy(src_pixels, pixel_src, sprite->width * sprite->height * sizeof(uint32_t));
	SDL_UnlockSurface(src_surface);

	/* 스케일된 서피스를 생성한다 */
	scaled_surface = SDL_CreateRGBSurface(0, scaled_width, scaled_height, 32,
	                                    0xFF, 0xFF00, 0xFF0000, 0xFF000000);
	if (!scaled_surface) {
		SDL_FreeSurface(src_surface);
		return -1;
	}

	/* 서피스를 스케일링한다 */
	SDL_SoftStretch(src_surface, NULL, scaled_surface, NULL);

	/* 스케일된 스프라이트를 대상에 블릿한다 */
	SDL_Rect dest_rect;
	dest_rect.x = x;
	dest_rect.y = y;
	dest_rect.w = scaled_width;
	dest_rect.h = scaled_height;

	if (SDL_BlitSurface(scaled_surface, NULL, dest->surface, &dest_rect) != 0) {
		fprintf(stderr, "SpriteLib Backend: SDL_BlitSurface failed: %s\n", SDL_GetError());
		result = -1;
	} else {
		result = 0;
	}

	/* 정리 */
	SDL_FreeSurface(scaled_surface);
	SDL_FreeSurface(src_surface);

	return result;
}

int spritectl_blt_surface(spritectl_surface_t dest,
                          const RECT* dest_rect,
                          spritectl_surface_t src,
                          const RECT* src_rect) {
	SDL_Rect sdl_dest_rect, sdl_src_rect;

	if (!dest || !src) {
		return -1;
	}

	/* RECT를 SDL_Rect로 변환한다 */
	if (dest_rect) {
		sdl_dest_rect.x = dest_rect->left;
		sdl_dest_rect.y = dest_rect->top;
		sdl_dest_rect.w = dest_rect->right - dest_rect->left;
		sdl_dest_rect.h = dest_rect->bottom - dest_rect->top;
	} else {
		sdl_dest_rect.x = 0;
		sdl_dest_rect.y = 0;
		sdl_dest_rect.w = dest->width;
		sdl_dest_rect.h = dest->height;
	}

	if (src_rect) {
		sdl_src_rect.x = src_rect->left;
		sdl_src_rect.y = src_rect->top;
		sdl_src_rect.w = src_rect->right - src_rect->left;
		sdl_src_rect.h = src_rect->bottom - src_rect->top;
	} else {
		sdl_src_rect.x = 0;
		sdl_src_rect.y = 0;
		sdl_src_rect.w = src->width;
		sdl_src_rect.h = src->height;
	}

	/* 서피스를 서피스로 블릿한다 */
	if (SDL_BlitSurface(src->surface, &sdl_src_rect, dest->surface, &sdl_dest_rect) != 0) {
		fprintf(stderr, "SpriteLib Backend: SDL_BlitSurface failed: %s\n", SDL_GetError());
		return -1;
	}

	return 0;
}

/* ============================================================================
 * 스프라이트 팩 함수
 * ============================================================================ */

int spritectl_load_pack(const char* filename, spritectl_pack_t* pack) {
	FILE* file;
	uint16_t count;
	spritectl_pack_t pack_ptr = NULL;
	int result = -1;

	if (!filename || !pack) {
		return -1;
	}

	/* 팩 파일을 연다 */
	file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "SpriteLib Backend: Failed to open pack file: %s\n", filename);
		return -2;
	}

	/* 스프라이트 개수를 읽는다 (2바이트, 리틀 엔디안) */
	if (fread(&count, 2, 1, file) != 1) {
		fclose(file);
		return -4;
	}

	/* 팩 구조체를 할당한다 */
	pack_ptr = (spritectl_pack_t)malloc(sizeof(struct spritectl_pack_s));
	if (!pack_ptr) {
		fclose(file);
		return -3;
	}

	/* 팩 구조체를 초기화한다 */
	pack_ptr->filename = strdup(filename);
	pack_ptr->file = file;
	pack_ptr->sprite_count = count;
	pack_ptr->lazy_loading = 0;  /* 전체 로드 */
	pack_ptr->ref_count = 1;

	/* 스프라이트 배열을 할당한다 */
	if (count > 0) {
		pack_ptr->sprites = (spritectl_sprite_t*)calloc(count, sizeof(spritectl_sprite_t));
		if (!pack_ptr->sprites) {
			free(pack_ptr->filename);
			free(pack_ptr);
			fclose(file);
			return -3;
		}

		pack_ptr->loaded_flags = (int*)calloc(count, sizeof(int));
		if (!pack_ptr->loaded_flags) {
			free(pack_ptr->sprites);
			free(pack_ptr->filename);
			free(pack_ptr);
			fclose(file);
			return -3;
		}

		pack_ptr->file_offsets = NULL;  /* 전체 로드에는 필요 없다 */
	} else {
		pack_ptr->sprites = NULL;
		pack_ptr->loaded_flags = NULL;
		pack_ptr->file_offsets = NULL;
	}

	/* 각 스프라이트를 로드한다 */
	uint16_t colorkey = 0xFFFF;  /* 기본 컬러 키 */
	for (uint16_t i = 0; i < count; i++) {
		int load_result = spritectl_load_sprite_from_file(file, &pack_ptr->sprites[i], colorkey);
		if (load_result != 0) {
			pack_ptr->sprites[i] = SPRITECTL_INVALID_SPRITE;
			pack_ptr->loaded_flags[i] = 0;
		} else {
			pack_ptr->loaded_flags[i] = 1;
		}
	}

	fclose(file);
	pack_ptr->file = NULL;

	*pack = pack_ptr;
	return 0;
}

void spritectl_free_pack(spritectl_pack_t pack) {
	if (!pack) {
		return;
	}

	/* 모든 스프라이트를 해제한다 */
	if (pack->sprites) {
		for (int i = 0; i < pack->sprite_count; i++) {
			if (pack->sprites[i] != SPRITECTL_INVALID_SPRITE) {
				spritectl_destroy_sprite(pack->sprites[i]);
			}
		}
		free(pack->sprites);
		pack->sprites = NULL;
	}

	/* 나머지 리소스를 해제한다 */
	if (pack->filename) {
		free(pack->filename);
		pack->filename = NULL;
	}
	if (pack->loaded_flags) {
		free(pack->loaded_flags);
		pack->loaded_flags = NULL;
	}
	if (pack->file_offsets) {
		free(pack->file_offsets);
		pack->file_offsets = NULL;
	}
	if (pack->file) {
		fclose(pack->file);
		pack->file = NULL;
	}

	free(pack);
}

int spritectl_get_sprite_from_pack(spritectl_pack_t pack, int index,
                                   spritectl_sprite_t* sprite) {
	if (!pack || !sprite) {
		return -1;
	}

	/* 인덱스 범위를 확인한다 */
	if (index < 0 || index >= pack->sprite_count) {
		return -2;
	}

	/* 스프라이트를 반환한다 (로드되지 않았으면 invalid 핸들) */
	*sprite = pack->sprites[index];
	return 0;
}

int spritectl_get_pack_count(spritectl_pack_t pack) {
	if (!pack) {
		return 0;
	}
	return pack->sprite_count;
}

/* ============================================================================
 * 픽셀 변환 함수
 * ============================================================================ */

void spritectl_convert_565_to_555(const uint16_t* src, uint16_t* dest, int pixel_count) {
	int i;
	for (i = 0; i < pixel_count; i++) {
		uint16_t pixel = src[i];
		/* 565를 555로 변환한다: green의 하위 비트를 버린다 */
		dest[i] = ((pixel & 0xFFC0) >> 1) | (pixel & 0x001F);
	}
}

void spritectl_convert_555_to_565(const uint16_t* src, uint16_t* dest, int pixel_count) {
	int i;
	for (i = 0; i < pixel_count; i++) {
		uint16_t pixel = src[i];
		/* 555를 565로 변환한다: green의 하위 비트 자리에 0을 채워 넣는다 */
		dest[i] = ((pixel & 0x7FE0) << 1) | (pixel & 0x001F);
	}
}

void spritectl_convert_565_to_rgba(const uint16_t* src, uint32_t* dest,
                                   int pixel_count, uint16_t colorkey) {
	int i;
	for (i = 0; i < pixel_count; i++) {
		uint16_t pixel = src[i];
		uint8_t r, g, b, a;

		if (pixel == colorkey) {
			/* 투명 픽셀 */
			a = 0;
		} else {
			a = 255;
		}

		spritectl_565_to_rgb(pixel, &r, &g, &b);
		dest[i] = spritectl_rgb_to_rgba(r, g, b, a);
	}
}

void spritectl_convert_555_to_rgba(const uint16_t* src, uint32_t* dest,
                                   int pixel_count, uint16_t colorkey) {
	int i;
	for (i = 0; i < pixel_count; i++) {
		uint16_t pixel = src[i];
		uint8_t r, g, b, a;

		if (pixel == colorkey) {
			/* 투명 픽셀 */
			a = 0;
		} else {
			a = 255;
		}

		spritectl_555_to_rgb(pixel, &r, &g, &b);
		dest[i] = spritectl_rgb_to_rgba(r, g, b, a);
	}
}

/* ============================================================================
 * RLE 스프라이트 디코딩
 * ============================================================================ */

/**
 * RLE로 압축된 스프라이트 데이터를 RGB565 픽셀로 디코딩한다
 *
 * RLE 포맷 (스캔라인별):
 *   - 길이 (2바이트, uint16_t) - RLE 데이터의 WORD 개수
 *   - RLE 데이터 (길이 * 2바이트):
 *     - Count (1 WORD) - 세그먼트 개수
 *     - 각 세그먼트마다:
 *       - Trans Count (1 WORD) - 투명 픽셀 개수
 *       - Color Count (1 WORD) - 색상 픽셀 개수
 *       - Pixels (Color Count WORD) - RGB565 픽셀 데이터
 */
int spritectl_decode_rle_data(const uint16_t* rle_data, int rle_len,
                               uint16_t* pixels_out, int width) {
	int rle_index = 0;
	int x = 0;

	/* 세그먼트 개수를 읽는다 */
	if (rle_len == 0) {
		return 0;  /* 비어있는 스캔라인 */
	}

	int count = rle_data[rle_index++];
	rle_index = 1;

	/* 각 세그먼트를 처리한다 */
	for (int seg = 0; seg < count && x < width; seg++) {
		if (rle_index >= rle_len) break;

		/* 투명/색상 개수를 읽는다 */
		int trans_count = rle_data[rle_index++];
		int color_count = rle_data[rle_index++];

		/* 투명 픽셀은 건너뛴다 (0으로 남겨둔다) */
		x += trans_count;

		/* 색상 픽셀을 복사한다 */
		for (int c = 0; c < color_count && x < width; c++) {
			if (rle_index >= rle_len) break;
			pixels_out[x] = rle_data[rle_index++];
			x++;
		}
	}

	return 0;
}

int spritectl_decode_rle_sprite(const uint8_t* compressed, size_t compressed_size,
                                int width, int height,
                                uint16_t* pixels_out, size_t pixels_size) {
	FILE* file;
	uint16_t* scanline_lengths = NULL;
	uint8_t** scanlines = NULL;
	int result = -1;

	/* 지금은 원본 버퍼 대신 FILE 포인터로부터 로드한다 */
	/* 이는 원본과 동작이 같은 더 단순한 구현이다 */
	fprintf(stderr, "SpriteLib Backend: Use spritectl_load_sprite_from_file instead\n");
	return -1;
}

/**
 * 파일로부터 스프라이트를 로드한다 (RLE 인코딩됨)
 * 파일 포맷:
 *   - 너비 (2바이트, uint16_t)
 *   - 높이 (2바이트, uint16_t)
 *   - 각 스캔라인마다:
 *     - 길이 (2바이트, uint16_t)
 *     - RLE 데이터 (길이 * 2바이트)
 */
int spritectl_load_sprite_from_file(FILE* file, spritectl_sprite_t* sprite_out,
                                    uint16_t colorkey) {
	uint16_t width = 0, height = 0;
	uint16_t* scanline_lengths = NULL;
	uint16_t** scanline_rle = NULL;
	spritectl_sprite_t sprite = NULL;
	size_t pixel_count = 0;
	uint16_t* pixels = NULL;
	int result = -1;

	if (!file || !sprite_out) {
		return -1;
	}

	/* 너비와 높이를 읽는다 */
	if (fread(&width, 2, 1, file) != 1) goto cleanup;
	if (fread(&height, 2, 1, file) != 1) goto cleanup;

	/* 빈 스프라이트를 처리한다 */
	if (width == 0 || height == 0) {
		*sprite_out = SPRITECTL_INVALID_SPRITE;
		return 0;
	}

	pixel_count = width * height;

	/* 스캔라인 길이 배열을 할당한다 */
	scanline_lengths = (uint16_t*)calloc(height, sizeof(uint16_t));
	if (!scanline_lengths) goto cleanup;

	/* RLE 데이터 배열을 할당한다 */
	scanline_rle = (uint16_t**)calloc(height, sizeof(uint16_t*));
	if (!scanline_rle) goto cleanup;

	/* 각 스캔라인을 읽는다 */
	for (int y = 0; y < height; y++) {
		/* 스캔라인 데이터 길이를 읽는다 */
		if (fread(&scanline_lengths[y], 2, 1, file) != 1) goto cleanup;

		if (scanline_lengths[y] > 0) {
			/* RLE 데이터를 할당하고 읽는다 */
			scanline_rle[y] = (uint16_t*)malloc(scanline_lengths[y] * sizeof(uint16_t));
			if (!scanline_rle[y]) goto cleanup;

			if (fread(scanline_rle[y], 2, scanline_lengths[y], file) != scanline_lengths[y]) {
				free(scanline_rle[y]);
				scanline_rle[y] = NULL;
				goto cleanup;
			}
		}
	}

	/* RLE를 원본 픽셀로 디코딩한다 */
	pixels = (uint16_t*)calloc(pixel_count, sizeof(uint16_t));
	if (!pixels) goto cleanup;

	/* 각 스캔라인을 디코딩한다 */
	for (int y = 0; y < height; y++) {
		uint16_t* row = pixels + (y * width);

		if (scanline_rle[y] && scanline_lengths[y] > 0) {
			/* RLE 데이터를 디코딩한다 */
			int rle_index = 0;
			int x = 0;

			/* 세그먼트 개수를 읽는다 */
			int count = scanline_rle[y][rle_index++];

			/* 각 세그먼트를 처리한다 */
			for (int seg = 0; seg < count && x < width; seg++) {
				int trans_count = scanline_rle[y][rle_index++];
				int color_count = scanline_rle[y][rle_index++];

				/* 투명 픽셀은 건너뛴다 (0으로 유지한다) */
				x += trans_count;

				/* 색상 픽셀을 복사한다 */
				for (int c = 0; c < color_count && x < width; c++) {
					row[x] = scanline_rle[y][rle_index++];
					x++;
				}
			}
		}
	}

	/* 스프라이트 구조체를 생성한다 */
	sprite = spritectl_create_sprite(width, height, SPRITECTL_FORMAT_RGB565,
	                                  pixels, pixel_count * sizeof(uint16_t));
	if (!sprite) {
		free(pixels);
		goto cleanup;
	}

	free(pixels);  /* 스프라이트가 데이터를 복사했다 */

	/* 올바른 투명 렌더링을 위해 RLE 데이터를 보존한다 */
	sprite->scanline_rle = scanline_rle;
	sprite->scanline_lens = scanline_lengths;
	sprite->has_rle = 1;
	scanline_rle = NULL;  /* cleanup에서 해제하지 않도록 한다 */
	scanline_lengths = NULL;

	*sprite_out = sprite;
	result = 0;

cleanup:
	if (scanline_rle) {
		for (int y = 0; y < height; y++) {
			if (scanline_rle[y]) {
				free(scanline_rle[y]);
			}
		}
		free(scanline_rle);
	}
	if (scanline_lengths) {
		free(scanline_lengths);
	}

	return result;
}

/* ============================================================================
 * 서피스를 렌더러에 표시(present)한다
 * ============================================================================ */

int spritectl_present_surface(spritectl_surface_t surface, void* renderer_ptr) {
	if (!surface || !renderer_ptr) {
		return -1;
	}

	SDL_Renderer* renderer = (SDL_Renderer*)renderer_ptr;

	/* 백엔드 서피스로부터 SDL 서피스를 가져온다 */
	SDL_Surface* sdl_surface = surface->surface;
	if (!sdl_surface) {
		fprintf(stderr, "SpriteLib Backend: Surface has no SDL surface\n");
		return -1;
	}

	SDL_Rect dest_rect;
	dest_rect.x = 0;
	dest_rect.y = 0;
	dest_rect.w = sdl_surface->w;
	dest_rect.h = sdl_surface->h;

	/* 이 함수는 (고정 크기인 g_pBack 백버퍼를 표시하는 등의 이유로) 매 프레임 호출된다.
	 * 매 호출마다 전체 화면 크기의 SDL_Texture를 새로 만들고 파괴하면 완전히 유휴
	 * 상태일 때조차 GPU/드라이버 쪽 메모리 churn이 계속 발생했다. 대신 영구적인
	 * streaming 텍스처를 재사용하고, 렌더러가 바뀌었거나 아직 없을 때만 (다시) 생성한다. */
	if (surface->texture != NULL && surface->renderer != renderer) {
		SDL_DestroyTexture(surface->texture);
		surface->texture = NULL;
	}

	if (surface->texture == NULL) {
		surface->texture = SDL_CreateTexture(renderer, sdl_surface->format->format,
			SDL_TEXTUREACCESS_STREAMING, sdl_surface->w, sdl_surface->h);
		if (surface->texture != NULL) {
			surface->renderer = renderer;

			// DEBUG: 텍스처 포맷을 확인한다
			Uint32 format;
			if (SDL_QueryTexture(surface->texture, &format, NULL, NULL, NULL) == 0) {
				fprintf(stderr, "Persistent texture created: surface_format=%s, texture_format=%s\n",
					SDL_GetPixelFormatName(sdl_surface->format->format), SDL_GetPixelFormatName(format));
			}
		}
	}

	if (surface->texture != NULL &&
		SDL_UpdateTexture(surface->texture, NULL, sdl_surface->pixels, sdl_surface->pitch) == 0 &&
		SDL_RenderCopy(renderer, surface->texture, NULL, &dest_rect) == 0) {
		return 0;
	}

	/* 폴백: 렌더러가 이 픽셀 포맷을 네이티브 텍스처 포맷으로 지원하지 않을 수 있다
	 * (또는 위의 빠른 경로가 다른 이유로 실패했을 수 있다).
	 * SDL_CreateTextureFromSurface는 필요한 변환을 알아서 수행하지만, 매 호출마다
	 * 텍스처를 할당/해제하는 비용이 든다 - 예전 동작과 동일하며, 안전망 용도로만
	 * 남겨둔다. */
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sdl_surface);
	if (!texture) {
		fprintf(stderr, "SpriteLib Backend: Failed to create texture: %s\n", SDL_GetError());
		return -1;
	}

	int ok = (SDL_RenderCopy(renderer, texture, NULL, &dest_rect) == 0);
	if (!ok) {
		fprintf(stderr, "SpriteLib Backend: Failed to render texture: %s\n", SDL_GetError());
	}
	SDL_DestroyTexture(texture);

	return ok ? 0 : -1;
}
