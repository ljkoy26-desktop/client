/*-----------------------------------------------------------------------------

	client_PCH.h

	CMake 빌드를 위한 최소한의 미리 컴파일된 헤더.
	원본 VC6 미리 컴파일된 헤더를 대체한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#ifndef __CLIENT_PCH_H__
#define __CLIENT_PCH_H__

/* 게임 클라이언트 빌드임을 정의한다 */
#define __GAME_CLIENT__

/* 플랫폼 타입 */
#include "../basic/Platform.h"

/* 표준 C++ 라이브러리 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>

/* 편의를 위해 표준 네임스페이스를 사용한다 */
using namespace std;

/* Windows가 아닌 플랫폼을 위한 RECT 정의 */
#ifndef PLATFORM_WINDOWS
#ifndef RECT_DEFINED
#define RECT_DEFINED
typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT, *PRECT, *LPRECT;
#endif

/* POINT 정의 */
#ifndef POINT_DEFINED
#define POINT_DEFINED
typedef struct tagPOINT {
	LONG x;
	LONG y;
} POINT, *PPOINT, *LPPOINT;
#endif

/* SIZE 정의 */
#ifndef SIZE_DEFINED
#define SIZE_DEFINED
typedef struct tagSIZE {
	LONG cx;
	LONG cy;
} SIZE, *PSIZE, *LPSIZE;
#endif

#endif /* PLATFORM_WINDOWS */

/* Packet 시스템을 위한 예외 처리 */
#include "Packet/Exception.h"

#endif /* __CLIENT_PCH_H__ */
