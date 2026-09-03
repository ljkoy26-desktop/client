/* inftrees.h -- header to use inftrees.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* 경고: 이 파일은 응용 프로그램에서 직접 사용해서는 *안 됩니다*. 이 파일은
   압축 라이브러리 구현의 일부이며 변경될 수 있습니다.
   응용 프로그램은 zlib.h만 사용해야 합니다.
 */

/* 허프만 코드 조회 테이블 항목 -- 16비트 포인터를 사용하는 머신
   (예: 소형 또는 중형 모델의 PC)에서는 이 항목이 4바이트이다. */

typedef struct inflate_huft_s FAR inflate_huft;

struct inflate_huft_s {
  union {
    struct {
      Byte Exop;        /* 추가 비트 수 또는 연산 */
      Byte Bits;        /* 이 코드 또는 서브코드의 비트 수 */
    } what;
    uInt pad;           /* 구조체를 2의 제곱으로 패딩 (16비트의 경우 4바이트, */
  } word;               /*  32비트 int의 경우 8바이트) */
  uInt base;            /* 리터럴, 길이 기준값, 거리 기준값,
                           또는 테이블 오프셋 */
};

/* 동적 트리의 최대 크기. 장시간이지만 완전하지 않은 탐색에서 발견된 최대값은
   1004개의 huft 구조체였다 (길이/리터럴 850개, 거리 154개이며,
   후자는 완전 탐색의 결과이다). 실제 최대값은 알 수 없지만,
   아래 값은 충분히 안전하다. */
#define MANY 1440

extern int inflate_trees_bits OF((
    uIntf *,                    /* 19개 코드 길이 */
    uIntf *,                    /* 비트 트리 원하는/실제 깊이 */
    inflate_huft * FAR *,       /* 비트 트리 결과 */
    inflate_huft *,             /* 트리를 위한 공간 */
    z_streamp));                /* 메시지용 */

extern int inflate_trees_dynamic OF((
    uInt,                       /* 리터럴/길이 코드 수 */
    uInt,                       /* 거리 코드 수 */
    uIntf *,                    /* 총 코드 길이 */
    uIntf *,                    /* 리터럴 원하는/실제 비트 깊이 */
    uIntf *,                    /* 거리 원하는/실제 비트 깊이 */
    inflate_huft * FAR *,       /* 리터럴/길이 트리 결과 */
    inflate_huft * FAR *,       /* 거리 트리 결과 */
    inflate_huft *,             /* 트리를 위한 공간 */
    z_streamp));                /* 메시지용 */

extern int inflate_trees_fixed OF((
    uIntf *,                    /* 리터럴 원하는/실제 비트 깊이 */
    uIntf *,                    /* 거리 원하는/실제 비트 깊이 */
    inflate_huft * FAR *,       /* 리터럴/길이 트리 결과 */
    inflate_huft * FAR *,       /* 거리 트리 결과 */
    z_streamp));                /* 메모리 할당용 */
