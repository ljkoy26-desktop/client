////////////////////////////////////////////////////////////////////////////////
//	created:	2004/12/22
//	file base:	client_pch.h
//
//	크로스 플랫폼 지원(macOS/Linux)을 위해 수정됨
////////////////////////////////////////////////////////////////////////////////

#pragma once

/* 아래 #ifdef 검사가 실행되기 전에 PLATFORM_WINDOWS를 정의한다: 이 PCH가
   번역 단위에서 처음 포함되는 헤더일 때는 이 시점에 매크로가 아직
   정의되지 않은 상태이므로(basic/Platform.h만 이를 정의함), 네이티브
   Windows 빌드에서도 POSIX 분기(<unistd.h>, <SDL2/SDL.h> 등)를 타게 된다. */
#if defined(_WIN32) || defined(_WIN64)
	#ifndef PLATFORM_WINDOWS
		#define PLATFORM_WINDOWS
	#endif
#endif

#ifdef PLATFORM_WINDOWS
	#pragma warning(disable:4290)
	#pragma warning(disable:4018)
	#pragma warning(disable:4244)
	#pragma warning(disable:4018)
	#pragma warning(disable:4786)
	#pragma warning(push)
#endif

#include <string>
#include <cassert>
#include <vector>
#include <map>
#include <list>
#include <deque>
#include <bitset>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef PLATFORM_WINDOWS
	/* 여기서는 실제 <MMSystem.h>/<Digitalv.h>/<DDraw.h>를 포함하지 않는다:
	   이 프로젝트는 더 이상 실제 DirectX/DirectSound를 사용하지 않으며,
	   basic/AudioTypes.h와 Client/DXLib/CDirectDraw.h가 동일한 타입 이름
	   (HMMIO, DDPIXELFORMAT, LPDIRECTDRAW7 등)에 대해 SDL 기반 대체
	   정의를 제공한다. 실제 헤더를 포함하면 호환되지 않는 타입으로
	   재정의되어버린다. WIN32_LEAN_AND_MEAN은 <windows.h> 자체가 이
	   헤더들을 자동으로 끌어들이지(lzexpand.h -> mmsystem.h -> ... 경로로)
	   않도록 막아준다. */
	#ifndef _WINDOWS_
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
	#include <io.h>
	#include <fcntl.h>
	#pragma warning(pop)
#else
	/* 플랫폼 추상화 계층 사용 */
	#include "../basic/Platform.h"
	#include <SDL2/SDL.h>
	#include <unistd.h>
#endif

//#include "GAME1024.h"

using std::string;
using std::vector;
using std::map;
using std::list;
using std::deque;
using std::bitset;

extern BOOL g_MyFull;
extern RECT g_GameRect;
//extern GAME1024 g_NewMode;
extern	LONG g_SECTOR_WIDTH;
extern	LONG g_SECTOR_HEIGHT;
extern	LONG g_SECTOR_WIDTH_HALF;
extern	LONG g_SECTOR_HEIGHT_HALF;
extern	LONG g_SECTOR_SKIP_PLAYER_LEFT;
extern	LONG g_SECTOR_SKIP_PLAYER_UP;

extern	LONG g_TILESURFACE_SECTOR_WIDTH;
extern	LONG g_TILESURFACE_SECTOR_HEIGHT;
extern	LONG g_TILESURFACE_SECTOR_OUTLINE_RIGHT;
extern	LONG g_TILESURFACE_SECTOR_OUTLINE_DOWN;
extern	LONG g_TILESURFACE_WIDTH;
extern	LONG g_TILESURFACE_HEIGHT;
extern	LONG g_TILESURFACE_OUTLINE_RIGHT;
extern	LONG g_TILESURFACE_OUTLINE_DOWN;
extern	LONG g_TILE_X_HALF;
extern	LONG g_TILE_Y_HALF;
