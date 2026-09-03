#include <fstream>
#ifndef __MPALETTE_PACK_H__
#define __MPALETTE_PACK_H__

#include "CTypePack.h"
#include "MPalette.h"

typedef CTypePack2<MPalette, MPalette555, MPalette565> MPalettePack;

//class MPalettePack: public CTypePack2<MPalette>
//{
//public:
//	//--------------------------------------------------------
//	// 초기화/해제
//	//--------------------------------------------------------
//	void	Init(WORD size, bool b565 = true);
//
//	//--------------------------------------------------------
//	// 파일 입출력
//	//--------------------------------------------------------
//	bool LoadFromFile(std::ifstream &file, bool b565 = true);
//	bool LoadFromFile(LPCTSTR lpszFilename, bool b565 = true);
//	
//protected:
//	bool			m_b565;
//};

#endif