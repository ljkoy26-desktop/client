#include <fstream>
/*-----------------------------------------------------------------------------

	SpriteLibBackendSDL.h

	SpriteLib을 위한 내부 SDL2 백엔드 구현 구조체.
	이 구조체들은 공개 인터페이스에서는 불투명하게 다뤄진다.

	2025.01.14

-----------------------------------------------------------------------------*/

#ifndef __SPRITELIBBACKENDSDL_H__
#define __SPRITELIBBACKENDSDL_H__

#include "SpriteLibBackend.h"

#ifdef SPRITELIB_BACKEND_SDL

/* 타입이 선언된 곳과 맞추기 위해 C 링키지용 extern "C" 블록을 연다 */
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>

/* ============================================================================
 * 내부 서페이스 구조체
 * ============================================================================ */

struct spritectl_surface_s {
	SDL_Surface* surface;       /* 픽셀 접근용 SDL 서페이스 */
	SDL_Texture* texture;       /* 렌더링용 SDL 텍스처 (선택 사항) */
	SDL_Renderer* renderer;     /* SDL 렌더러 (텍스처 사용 시) */
	int width;
	int height;
	int format;                 /* SPRITECTL_FORMAT_* */
	int locked;                 /* 잠금 횟수 */
	int ref_count;              /* 참조 카운트 */
};

/* ============================================================================
 * 내부 스프라이트 구조체
 * ============================================================================ */

struct spritectl_sprite_s {
	int width;
	int height;
	int format;                 /* SPRITECTL_FORMAT_* */
	uint16_t* pixels;           /* 디코딩된 픽셀 데이터 (565 또는 555 포맷) - NULL일 수 있음 */
	uint16_t** scanline_rle;    /* 스캔라인별 RLE 데이터 - 올바른 투명 처리를 위해 보존 */
	uint16_t* scanline_lens;    /* 각 스캔라인 RLE 데이터의 길이 */
	uint32_t* rgba_pixels;      /* 디코딩된 RGBA32 픽셀 (캐시됨) */
	size_t data_size;           /* 픽셀 데이터의 크기 */
	int ref_count;              /* 참조 카운트 */
	int has_rle;                /* RLE 데이터 사용 가능 여부 */
};

/* ============================================================================
 * 내부 스프라이트 팩 구조체
 * ============================================================================ */

struct spritectl_pack_s {
	char* filename;             /* SPK 파일 경로 */
	FILE* file;                 /* 지연 로딩용 파일 핸들 */
	int sprite_count;           /* 팩 안의 스프라이트 개수 */
	spritectl_sprite_t* sprites; /* 스프라이트 핸들 배열 */
	int* loaded_flags;          /* 어떤 스프라이트가 로드됐는지 (지연 로딩용) */
	uint32_t* file_offsets;     /* 각 스프라이트의 파일 오프셋 */
	int lazy_loading;           /* 지연 로딩 사용 여부 */
	int ref_count;              /* 참조 카운트 */
};

/* ============================================================================
 * 내부 헬퍼 함수
 * ============================================================================ */

/**
 * 지정된 픽셀 포맷으로 SDL 서페이스를 생성한다
 * @param width 서페이스 너비
 * @param height 서페이스 높이
 * @param format 픽셀 포맷 (SPRITECTL_FORMAT_*)
 * @return SDL_Surface 포인터, 실패 시 NULL
 */
SDL_Surface* spritectl_sdl_create_surface(int width, int height, int format);

/**
 * SPRITECTL 포맷으로부터 SDL 픽셀 포맷 enum을 얻는다
 * @param format SPRITECTL_FORMAT_* 값
 * @return SDL_PixelFormatEnum 값
 */
uint32_t spritectl_sdl_get_pixelformat(int format);

/**
 * 컬러키 값을 SDL 컬러키로 변환한다
 * @param format SPRITECTL_FORMAT_* 값
 * @param colorkey 원본 포맷의 컬러키
 * @return SDL용 Uint32 컬러키
 */
Uint32 spritectl_sdl_get_colorkey(int format, Uint16 colorkey);

/**
 * SDL 서페이스를 잠그고 픽셀 포인터를 반환한다
 * @param surface spritectl 서페이스 핸들
 * @param info 출력 서페이스 정보
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_sdl_lock_surface(spritectl_surface_t surface,
                               spritectl_surface_info_t* info);

/**
 * SDL 서페이스 잠금을 해제한다
 * @param surface spritectl 서페이스 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_sdl_unlock_surface(spritectl_surface_t surface);

/* ============================================================================
 * 스프라이트 디코딩 (RLE)
 * ============================================================================ */

/**
 * 파일에서 스프라이트를 로드한다 (RLE 인코딩)
 * @param file 스프라이트 데이터 위치에 있는 파일 포인터
 * @param sprite_out 출력 스프라이트 핸들
 * @param colorkey 투명 처리를 위한 컬러키 값
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_load_sprite_from_file(FILE* file, spritectl_sprite_t* sprite_out,
                                    uint16_t colorkey);

/**
 * RLE 압축된 스프라이트 데이터를 디코딩한다
 * @param compressed 압축 데이터
 * @param compressed_size 압축 데이터의 크기
 * @param width 스프라이트 너비
 * @param height 스프라이트 높이
 * @param pixels_out 출력 픽셀 버퍼 (미리 할당되어 있어야 함)
 * @param pixels_size 출력 버퍼의 크기
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_decode_rle_sprite(const uint8_t* compressed, size_t compressed_size,
                                int width, int height,
                                uint16_t* pixels_out, size_t pixels_size);

/* ============================================================================
 * 픽셀 포맷 변환 (내부용)
 * ============================================================================ */

/**
 * RGB565 픽셀에서 RGB 성분을 추출한다
 * 정확한 5/6비트 -> 8비트 변환을 위해 최적화된 시프트 알고리즘을 사용한다
 */
static inline void spritectl_565_to_rgb(uint16_t pixel, uint8_t* r, uint8_t* g, uint8_t* b) {
	/* 성분 추출 */
	uint8_t r5 = (pixel >> 11) & 0x1F;  /* 5비트 */
	uint8_t g6 = (pixel >> 5) & 0x3F;   /* 6비트 */
	uint8_t b5 = pixel & 0x1F;          /* 5비트 */

	/* 최적화된 시프트 알고리즘으로 8비트로 변환
	 * 5비트 -> 8비트: (value << 3) | (value >> 2) ≈ (value * 255) / 31
	 * 6비트 -> 8비트: (value << 2) | (value >> 4) ≈ (value * 255) / 63
	 */
	*r = (r5 << 3) | (r5 >> 2);
	*g = (g6 << 2) | (g6 >> 4);
	*b = (b5 << 3) | (b5 >> 2);
}

/**
 * RGB555 픽셀에서 RGB 성분을 추출한다
 * 정확한 5비트 -> 8비트 변환을 위해 최적화된 시프트 알고리즘을 사용한다
 */
static inline void spritectl_555_to_rgb(uint16_t pixel, uint8_t* r, uint8_t* g, uint8_t* b) {
	/* 성분 추출 */
	uint8_t r5 = (pixel >> 10) & 0x1F;  /* 5비트 */
	uint8_t g5 = (pixel >> 5) & 0x1F;   /* 5비트 */
	uint8_t b5 = pixel & 0x1F;          /* 5비트 */

	/* 최적화된 시프트 알고리즘으로 8비트로 변환
	 * 5비트 -> 8비트: (value << 3) | (value >> 2) ≈ (value * 255) / 31
	 */
	*r = (r5 << 3) | (r5 >> 2);
	*g = (g5 << 3) | (g5 >> 2);
	*b = (b5 << 3) | (b5 >> 2);
}

/**
 * RGB 성분을 RGBA32로 묶는다
 */
static inline uint32_t spritectl_rgb_to_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (uint32_t)a << 24 | (uint32_t)b << 16 | (uint32_t)g << 8 | (uint32_t)r;
}

/**
 * RLE 기반 스프라이트 렌더링 (원본 DirectX BltClipWidth를 모방)
 * 이 함수는 RLE 데이터를 사용해 대상 서페이스에 직접 기록하며,
 * 알파 블렌딩 대신 투명 픽셀을 건너뛴다.
 */
int spritectl_blt_sprite_rle(spritectl_surface_t dest, int x, int y,
                              spritectl_sprite_t sprite, int flags, int alpha);

/**
 * RLE 데이터를 지원하는 스프라이트 구조체를 생성한다
 * RLE 배열을 할당하지만 채우지는 않는다 - 호출자가 scanline_rle과 scanline_lens를 채워야 한다
 * @param width 스프라이트 너비
 * @param height 스프라이트 높이
 * @return 새 스프라이트 핸들, 실패 시 SPRITECTL_INVALID_SPRITE
 */
spritectl_sprite_t spritectl_create_sprite_rle(int width, int height);

/**
 * 특정 스캔라인에 대한 RLE 데이터를 설정한다
 * @param sprite 스프라이트 핸들 (반드시 spritectl_create_sprite_rle로 생성해야 함)
 * @param y 스캔라인 인덱스
 * @param rle_data RLE 데이터 (복사됨)
 * @param rle_size RLE 데이터 크기 (uint16_t 단위)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_sprite_set_scanline_rle(spritectl_sprite_t sprite, int y,
                                       const uint16_t* rle_data, int rle_size);

#ifdef __cplusplus
}
#endif

#endif /* SPRITELIB_BACKEND_SDL */

#endif /* __SPRITELIBBACKENDSDL_H__ */
