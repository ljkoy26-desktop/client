/* infutil.h -- types and macros common to blocks and codes
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* 경고: 이 파일은 응용 프로그램에서 직접 사용해서는 *안 됩니다*. 이 파일은
   압축 라이브러리 구현의 일부이며 변경될 수 있습니다.
   응용 프로그램은 zlib.h만 사용해야 합니다.
 */

#ifndef _INFUTIL_H
#define _INFUTIL_H

typedef enum {
      TYPE,     /* 타입 비트 읽기 (3개, 끝 비트 포함) */
      LENS,     /* 저장용 길이 읽기 */
      STORED,   /* 저장 블록 처리 중 */
      TABLE,    /* 테이블 길이 읽기 */
      BTREE,    /* 동적 블록의 비트 길이 트리 읽기 */
      DTREE,    /* 동적 블록의 길이/거리 트리 읽기 */
      CODES,    /* 고정 또는 동적 블록 처리 중 */
      DRY,      /* 윈도우에 남은 바이트 출력 */
      DONE,     /* 마지막 블록 완료 */
      BAD}      /* 데이터 오류 발생--여기서 중단 */
inflate_block_mode;

/* inflate 블록 반공개 상태 */
struct inflate_blocks_state {

  /* 모드 */
  inflate_block_mode  mode;     /* 현재 inflate_block 모드 */

  /* 모드 의존적 정보 */
  union {
    uInt left;          /* STORED 상태일 때, 복사할 바이트 수 */
    struct {
      uInt table;               /* 테이블 길이 (14비트) */
      uInt index;               /* blens (또는 border) 내 인덱스 */
      uIntf *blens;             /* 코드의 비트 길이 */
      uInt bb;                  /* 비트 길이 트리 깊이 */
      inflate_huft *tb;         /* 비트 길이 디코딩 트리 */
    } trees;            /* DTREE 상태일 때, 트리 디코딩 정보 */
    struct {
      inflate_codes_statef
         *codes;
    } decode;           /* CODES 상태일 때, 현재 상태 */
  } sub;                /* 서브모드 */
  uInt last;            /* 이 블록이 마지막 블록이면 true */

  /* 모드 독립적 정보 */
  uInt bitk;            /* 비트 버퍼의 비트 수 */
  uLong bitb;           /* 비트 버퍼 */
  inflate_huft *hufts;  /* 트리 공간을 위한 단일 malloc */
  Bytef *window;        /* 슬라이딩 윈도우 */
  Bytef *end;           /* 슬라이딩 윈도우 다음 바이트 */
  Bytef *read;          /* 윈도우 읽기 포인터 */
  Bytef *write;         /* 윈도우 쓰기 포인터 */
  check_func checkfn;   /* 검사 함수 */
  uLong check;          /* 출력 검사값 */

};


/* inflate 입출력 매크로 정의 */
/*   포인터 업데이트 및 반환 */
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush(s,z,r);}
/*   바이트 및 비트 읽기 */
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}
/*   바이트 출력 */
#define WAVAIL (uInt)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(uInt)WAVAIL;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)WAVAIL;}}
#define FLUSH {UPDOUT r=inflate_flush(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=Z_OK;}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}
/*   로컬 포인터 로드 */
#define LOAD {LOADIN LOADOUT}

/* 하위 비트 마스크 (Visual C++ 경고 방지를 위해 크기 명시) */
extern uInt inflate_mask[17];

/* 슬라이딩 윈도우에서 출력 영역으로 최대한 복사 */
extern int inflate_flush OF((
    inflate_blocks_statef *,
    z_streamp ,
    int));

struct internal_state      {int dummy;}; /* for buggy compilers */

#endif
