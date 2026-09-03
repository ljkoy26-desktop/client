/* inffast.h -- header to use inffast.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* 경고: 이 파일은 응용 프로그램에서 직접 사용해서는 *안 됩니다*. 이 파일은
   압축 라이브러리 구현의 일부이며 변경될 수 있습니다.
   응용 프로그램은 zlib.h만 사용해야 합니다.
 */

extern int inflate_fast OF((
    uInt,
    uInt,
    inflate_huft *,
    inflate_huft *,
    inflate_blocks_statef *,
    z_streamp ));
