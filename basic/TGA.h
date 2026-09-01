/*-----------------------------------------------------------------------------

	TGA.h

	1999.5.	  KJTINC
	2000.8.29. 

-----------------------------------------------------------------------------*/

#ifndef __TARGA__
#define __TARGA__

#include "Basics.h"
#include "2D.h"
#include "DLL.h"

//----------------------------------------------------------------------------
// Tga 헤더
//----------------------------------------------------------------------------
struct S_TGAHEAD
{
	BYTE idsize;
	BYTE colormaptype;  // 팔레트 사용 여부
	BYTE imagetype;

	//
	// WORD colormapstart;
	// WORD colormaplength
	//
	// *VC의 word align을 피하기 위함!!!
	//
	//WORD colormapstart;
	BYTE colormapstart1;
	BYTE colormapstart2;
	//WORD colormaplength; // *VC의 word align을 피하기 위함!!!
	BYTE colormaplength1;
	BYTE colormaplength2;

	BYTE colormapbits;
	WORD xstart;
	WORD ystart;
	WORD width;
	WORD height;
	BYTE bpp;   // 픽셀당 비트 수
	BYTE descriptor;
};

//-----------------------------------------------------------------------------
// TGA 클래스
//
//-----------------------------------------------------------------------------
class DllClass Tga
{
private:
	S_TGAHEAD					m_head;

	char *						m_sz_filename;
	BYTE *						m_p_data;

	int							m_colorkey;

	void	CopyLine(BYTE *dest, BYTE *src, int buf_length, int pixel_byte, bool reverse);
	void	TransformBit(int pixel_byte, WORD **p_data, int w, int h);
	void	ClearData();

public:

	Tga();
	~Tga();

	//
	// 이미지 정보 조회
	//
	// 이전 버전: bool	Get_TGA_ImageInfo(char *sz_filename, S_PICINFO *picinfo);
	//
	int	Width() const { return m_head.width; }
	int	Height() const { return m_head.height; }
	int	BPP() const { return m_head.bpp; }
	const char * Filename() const { return m_sz_filename; }

	//
	// TGA 파일 로드
	//
	bool	Load(const char * sz_filename);
	bool	LoadButNoConvertTo565(const char * sz_filename);

	//
	// 블리팅(Blitting)
	//
	void	Bltz(const S_SURFACEINFO * p_dest_surface, int x, int y);
	void	CkBltz(const S_SURFACEINFO * p_dest_surface, int x, int y);
	void	SetColorkey(int colorkey);

	//
	// 값 조회...
	//
	bool	GetSurfaceInfo(S_SURFACEINFO &surfaceinfo) const;
};

#endif