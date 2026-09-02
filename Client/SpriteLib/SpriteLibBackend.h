#include <fstream>
/*-----------------------------------------------------------------------------

	SpriteLibBackend.h

	SpriteLib의 크로스플랫폼 지원을 위한 통합 백엔드 인터페이스.
	Windows(DirectDraw)와 SDL2 백엔드 양쪽을 위한 C 인터페이스를 제공한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#ifndef __SPRITELIBBACKEND_H__
#define __SPRITELIBBACKEND_H__

/* 자체적으로 관리하기 위해 Platform.h의 extern "C" 블록을 닫는다 */
#ifdef __cplusplus
extern "C" {
#endif

#include "../basic/Platform.h"

/* ============================================================================
 * 백엔드 선택
 * ============================================================================ */

/* 사용할 백엔드를 정의한다 */
#ifdef SPRITELIB_BACKEND_SDL
	#include <SDL.h>
#else
	/* Windows/DirectDraw 백엔드 (원본) */
#endif

/* ============================================================================
 * 타입 정의
 * ============================================================================ */

/* 백엔드 객체를 위한 불투명 핸들 */
typedef struct spritectl_surface_s* spritectl_surface_t;
typedef struct spritectl_sprite_s* spritectl_sprite_t;
typedef struct spritectl_pack_s* spritectl_pack_t;

/* 잘못된 핸들 */
#define SPRITECTL_INVALID_SURFACE	((spritectl_surface_t)NULL)
#define SPRITECTL_INVALID_SPRITE	((spritectl_sprite_t)NULL)
#define SPRITECTL_INVALID_PACK		((spritectl_pack_t)NULL)

/* 픽셀 포맷 상수 */
#define SPRITECTL_FORMAT_RGB565		0
#define SPRITECTL_FORMAT_RGB555		1
#define SPRITECTL_FORMAT_RGBA32		2

/* 서페이스 정보 구조체 */
typedef struct {
	int width;
	int height;
	int pitch;
	void* pixels;
	int format;  /* SPRITECTL_FORMAT_* */
} spritectl_surface_info_t;

/* 스프라이트 정보 구조체 */
typedef struct {
	int width;
	int height;
	int format;  /* SPRITECTL_FORMAT_* */
	size_t data_size;
} spritectl_sprite_info_t;

/* 블리팅 플래그 */
#define SPRITECTL_BLT_NONE			0x0000
#define SPRITECTL_BLT_FLIP_LR		0x0001  /* 좌우 반전 */
#define SPRITECTL_BLT_FLIP_UD		0x0002  /* 상하 반전 */
#define SPRITECTL_BLT_ALPHA			0x0004  /* 알파 블렌딩 */
#define SPRITECTL_BLT_COLORKEY		0x0008  /* 컬러키 사용 */

/* ============================================================================
 * 서페이스 함수
 * ============================================================================ */

/**
 * 렌더링용 서페이스를 생성한다
 * @param width 서페이스 너비(픽셀)
 * @param height 서페이스 높이(픽셀)
 * @param format 픽셀 포맷 (SPRITECTL_FORMAT_*)
 * @return 서페이스 핸들, 실패 시 SPRITECTL_INVALID_SURFACE
 */
spritectl_surface_t spritectl_create_surface(int width, int height, int format);

/**
 * 서페이스를 파괴한다
 * @param surface 서페이스 핸들
 */
void spritectl_destroy_surface(spritectl_surface_t surface);

/**
 * 픽셀에 직접 접근하기 위해 서페이스를 잠근다
 * @param surface 서페이스 핸들
 * @param info 서페이스 정보를 담을 출력 구조체
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_lock_surface(spritectl_surface_t surface, spritectl_surface_info_t* info);

/**
 * 픽셀 접근 후 서페이스 잠금을 해제한다
 * @param surface 서페이스 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_unlock_surface(spritectl_surface_t surface);

/**
 * 서페이스를 특정 색상으로 지운다
 * @param surface 서페이스 핸들
 * @param color 색상 값 (포맷에 따라 다름)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_clear_surface(spritectl_surface_t surface, uint32_t color);

/**
 * 서페이스 크기를 가져온다
 * @param surface 서페이스 핸들
 * @param width 출력 너비 (NULL 가능)
 * @param height 출력 높이 (NULL 가능)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_get_surface_size(spritectl_surface_t surface, int* width, int* height);

/**
 * 서페이스를 SDL 렌더러에 표시한다 (화면 출력용)
 * 이 함수는 서페이스를 텍스처로 변환한 뒤 렌더링한다.
 * @param surface 표시할 서페이스 핸들
 * @param renderer 표시 대상 SDL 렌더러
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_present_surface(spritectl_surface_t surface, void* renderer);

/* ============================================================================
 * 스프라이트 함수
 * ============================================================================ */

/**
 * 원본 픽셀 데이터로부터 스프라이트를 생성한다
 * @param width 스프라이트 너비(픽셀)
 * @param height 스프라이트 높이(픽셀)
 * @param format 픽셀 포맷 (SPRITECTL_FORMAT_*)
 * @param pixels 픽셀 데이터 포인터
 * @param data_size 픽셀 데이터 크기(바이트)
 * @return 스프라이트 핸들, 실패 시 SPRITECTL_INVALID_SPRITE
 */
spritectl_sprite_t spritectl_create_sprite(int width, int height, int format,
                                           const void* pixels, size_t data_size);

/**
 * 스프라이트를 파괴한다
 * @param sprite 스프라이트 핸들
 */
void spritectl_destroy_sprite(spritectl_sprite_t sprite);

/**
 * 스프라이트 정보를 가져온다
 * @param sprite 스프라이트 핸들
 * @param info 출력 스프라이트 정보
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_get_sprite_info(spritectl_sprite_t sprite, spritectl_sprite_info_t* info);

/**
 * 스프라이트 픽셀 데이터를 가져온다
 * @param sprite 스프라이트 핸들
 * @param buffer 픽셀 데이터를 복사할 버퍼 (크기만 조회하려면 NULL 가능)
 * @param buffer_size 버퍼 크기(바이트)
 * @return 성공 시 필요한 버퍼 크기, 실패 시 0
 */
size_t spritectl_get_sprite_data(spritectl_sprite_t sprite, void* buffer, size_t buffer_size);

/* ============================================================================
 * 블리팅 함수
 * ============================================================================ */

/**
 * 스프라이트를 서페이스에 블릿한다
 * @param dest 대상 서페이스 핸들
 * @param x 대상 X 위치
 * @param y 대상 Y 위치
 * @param sprite 원본 스프라이트 핸들
 * @param flags 블리팅 플래그 (SPRITECTL_BLT_*)
 * @param alpha 알파 값 (0-255, SPRITECTL_BLT_ALPHA가 설정된 경우 사용)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_blt_sprite(spritectl_surface_t dest, int x, int y,
                         spritectl_sprite_t sprite, int flags, int alpha);

/**
 * 스프라이트를 크기 조정하여 서페이스에 블릿한다
 * @param dest 대상 서페이스 핸들
 * @param x 대상 X 위치
 * @param y 대상 Y 위치
 * @param sprite 원본 스프라이트 핸들
 * @param scale 배율 (256 = 1배, 128 = 0.5배, 512 = 2배)
 * @param flags 블리팅 플래그 (SPRITECTL_BLT_*)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_blt_sprite_scaled(spritectl_surface_t dest, int x, int y,
                                spritectl_sprite_t sprite, int scale, int flags);

/**
 * 서페이스를 서페이스에 블릿한다
 * @param dest 대상 서페이스 핸들
 * @param dest_rect 대상 사각형 (전체 서페이스는 NULL)
 * @param src 원본 서페이스 핸들
 * @param src_rect 원본 사각형 (전체 서페이스는 NULL)
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_blt_surface(spritectl_surface_t dest,
                          const RECT* dest_rect,
                          spritectl_surface_t src,
                          const RECT* src_rect);

/* ============================================================================
 * 스프라이트 팩 함수
 * ============================================================================ */

/**
 * SPK 파일에서 스프라이트 팩을 로드한다
 * @param filename SPK 파일 경로
 * @param pack 출력 팩 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_load_pack(const char* filename, spritectl_pack_t* pack);

/**
 * 스프라이트 팩 리소스를 해제한다
 * @param pack 팩 핸들
 */
void spritectl_free_pack(spritectl_pack_t pack);

/**
 * 팩에서 스프라이트를 가져온다
 * @param pack 팩 핸들
 * @param index 스프라이트 인덱스
 * @param sprite 출력 스프라이트 핸들
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_get_sprite_from_pack(spritectl_pack_t pack, int index,
                                   spritectl_sprite_t* sprite);

/**
 * 팩의 스프라이트 개수를 가져온다
 * @param pack 팩 핸들
 * @return 팩 안의 스프라이트 개수, 실패 시 0
 */
int spritectl_get_pack_count(spritectl_pack_t pack);

/* ============================================================================
 * 픽셀 변환 함수
 * ============================================================================ */

/**
 * RGB565를 RGB555로 변환한다
 * @param src 원본 픽셀 (RGB565)
 * @param dest 대상 버퍼 (RGB555)
 * @param pixel_count 변환할 픽셀 수
 */
void spritectl_convert_565_to_555(const uint16_t* src, uint16_t* dest, int pixel_count);

/**
 * RGB555를 RGB565로 변환한다
 * @param src 원본 픽셀 (RGB555)
 * @param dest 대상 버퍼 (RGB565)
 * @param pixel_count 변환할 픽셀 수
 */
void spritectl_convert_555_to_565(const uint16_t* src, uint16_t* dest, int pixel_count);

/**
 * RGB565를 RGBA32로 변환한다
 * @param src 원본 픽셀 (RGB565)
 * @param dest 대상 버퍼 (RGBA32)
 * @param pixel_count 변환할 픽셀 수
 * @param colorkey 투명 처리를 위한 컬러키 값 (0xFFFF = 없음)
 */
void spritectl_convert_565_to_rgba(const uint16_t* src, uint32_t* dest,
                                   int pixel_count, uint16_t colorkey);

/**
 * RGB555를 RGBA32로 변환한다
 * @param src 원본 픽셀 (RGB555)
 * @param dest 대상 버퍼 (RGBA32)
 * @param pixel_count 변환할 픽셀 수
 * @param colorkey 투명 처리를 위한 컬러키 값 (0xFFFF = 없음)
 */
void spritectl_convert_555_to_rgba(const uint16_t* src, uint32_t* dest,
                                   int pixel_count, uint16_t colorkey);

/* ============================================================================
 * 초기화
 * ============================================================================ */

/**
 * SpriteLib 백엔드를 초기화한다
 * 다른 함수를 사용하기 전에 프로그램 시작 시 호출한다
 * @return 성공 시 0, 실패 시 0이 아닌 값
 */
int spritectl_init(void);

/**
 * SpriteLib 백엔드를 종료한다
 * 프로그램 종료 시 호출한다
 */
void spritectl_shutdown(void);

/* ============================================================================
 * RLE 스프라이트 지원
 * ============================================================================ */

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

/**
 * RLE 기반 스프라이트 렌더링 (원본 DirectX BltClipWidth를 모방)
 * 이 함수는 RLE 데이터를 사용해 대상 서페이스에 직접 기록하며,
 * 알파 블렌딩 대신 투명 픽셀을 건너뛴다.
 */
int spritectl_blt_sprite_rle(spritectl_surface_t dest, int x, int y,
                              spritectl_sprite_t sprite, int flags, int alpha);

#ifdef __cplusplus
}
#endif

#endif /* __SPRITELIBBACKEND_H__ */
