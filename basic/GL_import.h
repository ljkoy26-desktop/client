#ifndef __GL_IMPORT__
#define __GL_IMPORT__

#include "2D.h"
#include "picinfo.h"
#include "TGA.h"
#include "IMG.h"
#include "BasicData.h"

DllImport void FillRect(S_SURFACEINFO *info, Rect * rect, int color);
DllImport void (*filledRect)(S_SURFACEINFO *, S_RECT *, int);
DllImport void (*cls)(S_SURFACEINFO *, int);
DllImport void (*putPixel)(S_SURFACEINFO *, int, int, int); // putPixel 함수 포인터
DllImport int (*getPixel)(S_SURFACEINFO *, int, int); // getPixel 함수 포인터
DllImport void (*vertline)(S_SURFACEINFO *, int, int, int, int, int); // vertline 함수 포인터
DllImport void (*horzline)(S_SURFACEINFO *, int, int, int, int, int); // horzline 함수 포인터
DllImport void (*Bltz)(const S_SURFACEINFO *,
                  const S_SURFACEINFO *,
                  const S_RECT *,
                  const S_RECT *); // copy_2D_Space 함수 포인터
DllImport void (*CkBltz)(const S_SURFACEINFO *,
                  const S_SURFACEINFO *,
                  const S_RECT *,
                  const S_RECT *,
						const int); // copy_2D_Space_colorkey 함수 포인터

#ifdef PLATFORM_WINDOWS
/* DDSURFACEDESC("2"가 붙지 않은 원본 DirectDraw 서페이스 디스크립터)는
   이 프로젝트의 SDL 대체 타입으로 포팅된 적이 없고 더 이상 호출하는
   곳도 없다 - 실제로 사용되는 것은 아래의 DDSURFACEDESC2 오버로드뿐이다. */
DllImport void	SetSurfaceInfo(S_SURFACEINFO *surface_info, const DDSURFACEDESC2 *p_ddsd);
#endif
DllImport void	SetSurfaceInfo(S_SURFACEINFO &surface_info, void *p_surface, int w, int h, int pitch=0);
DllImport void	SetRect(S_RECT &rect, int x, int y, int w, int h);
DllImport int	Get_ColorkeyColor(S_PICINFO *picinfo);
DllImport int	Get_ColorkeyColor(S_SURFACEINFO *surfaceinfo);
DllImport DWORD GL_RGB(DWORD r, DWORD g, DWORD b);
DllImport void	InitializeGL(int bpp, int r_bit, int g_bit, int b_bit);
DllImport int	Convert24RGBto16(int R, int G, int B);
DllImport void	rectangle(S_SURFACEINFO *, int lx, int ly, int rx, int ry, int color);
DllImport void rectangle(S_SURFACEINFO * info, Rect * p_rect, int color);
DllImport void	line(S_SURFACEINFO *, int x1, int y1, int x2, int y2, int color);
DllImport void	TransparentBlt16(S_SURFACEINFO *dest_surface_info, 
								S_SURFACEINFO *src_surface_info, 
								S_RECT *dest_rect,
								S_RECT *src_rect);
DllImport int	getPixel16(S_SURFACEINFO *info, int x, int y); // utility에서 사용!

DllImport bool	TestTga(const char * filename);
DllImport bool GetTgaPicInfo(const char * sz_filename, S_PICINFO &picinfo);

#endif