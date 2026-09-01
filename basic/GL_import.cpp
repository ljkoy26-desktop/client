/*----------------------------------------------------------------------------

	GL_import.cpp

	Windows가 아닌 플랫폼을 위한 GL_import 함수 포인터 정의.
	GL_import.h에서 extern으로 선언되고 여기서 정의된다.

-----------------------------------------------------------------------------*/

#include "GL_import.h"

// Windows가 아닌 플랫폼을 위한 함수 포인터 정의
#ifndef PLATFORM_WINDOWS

void (*Bltz)(const S_SURFACEINFO *, const S_SURFACEINFO *, const S_RECT *, const S_RECT *) = NULL;
void (*CkBltz)(const S_SURFACEINFO *, const S_SURFACEINFO *, const S_RECT *, const S_RECT *, const int) = NULL;
void (*filledRect)(S_SURFACEINFO *, S_RECT *, int) = NULL;
void (*cls)(S_SURFACEINFO *, int) = NULL;
void (*putPixel)(S_SURFACEINFO *, int, int, int) = NULL;
int (*getPixel)(S_SURFACEINFO *, int, int) = NULL;
void (*vertline)(S_SURFACEINFO *, int, int, int, int, int) = NULL;
void (*horzline)(S_SURFACEINFO *, int, int, int, int, int) = NULL;

#endif
