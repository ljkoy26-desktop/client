/* infblock.h -- header to use infblock.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* 경고: 이 파일은 응용 프로그램에서 직접 사용해서는 *안 됩니다*. 이 파일은
   압축 라이브러리 구현의 일부이며 변경될 수 있습니다.
   응용 프로그램은 zlib.h만 사용해야 합니다.
 */

struct inflate_blocks_state;
typedef struct inflate_blocks_state FAR inflate_blocks_statef;

extern inflate_blocks_statef * inflate_blocks_new OF((
    z_streamp z,
    check_func c,               /* 검사 함수 */
    uInt w));                   /* 윈도우 크기 */

extern int inflate_blocks OF((
    inflate_blocks_statef *,
    z_streamp ,
    int));                      /* 초기 반환 코드 */

extern void inflate_blocks_reset OF((
    inflate_blocks_statef *,
    z_streamp ,
    uLongf *));                  /* 출력 검사값 */

extern int inflate_blocks_free OF((
    inflate_blocks_statef *,
    z_streamp));

extern void inflate_set_dictionary OF((
    inflate_blocks_statef *s,
    const Bytef *d,  /* 딕셔너리 */
    uInt  n));       /* 딕셔너리 길이 */

extern int inflate_blocks_sync_point OF((
    inflate_blocks_statef *s));
