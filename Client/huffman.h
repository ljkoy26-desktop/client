#ifndef HUFFMAN_H
#define HUFFMAN_H

#ifdef PLATFORM_WINDOWS
#include <wtypes.h>
#else
#include "../basic/Platform.h"  // For UINT and other basic types
#endif
#include "bit_res.h"


#define HUFFBITS UINT
#define HTN	34
#define MXOFF	250

struct huffcodetab
{
  char tablename[3];			/* 테이블 설명을 담는 문자열 */
  UINT xlen; 				/* x 인덱스 최대값 */
  UINT ylen;	            /* y 인덱스 최대값 */
  UINT linbits; 			/* linbits의 개수 */
  UINT linmax;				/* linbits에 저장할 최대 수 */
  int ref;					/* 양수이면 참조를 나타냄 */
  HUFFBITS *table;			/* array[xlen][ylen] 포인터 */
  unsigned char *hlen;		/* array[xlen][ylen] 포인터 */
  unsigned char(*val)[2];	/* 디코더 트리 */
  UINT treelen;				/* 디코더 트리의 길이 */
};

extern struct huffcodetab ht[HTN];

int huffman_decoder(struct huffcodetab *h, int *x, int *y, int *v,
                      int *w, Bit_Reserve *br);

#endif
