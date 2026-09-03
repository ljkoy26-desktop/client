#include <fstream>
//----------------------------------------------------------------------
// CSprite565.h
//----------------------------------------------------------------------
// 5:6:5 비디오 카드에 대한 스프라이트 클래스
//----------------------------------------------------------------------

#ifndef	__CSPRITE565_H__
#define	__CSPRITE565_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif
std::ofstream;
std::ifstream;


#include "CSprite.h"


class CSprite565 : public CSprite {
	public :
		CSprite565() = default;
		virtual ~CSprite565() = default;

		//---------------------------------------------------------
		// fstream에서 저장/불러오기를 한다.
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);
		//bool	LoadFromFileToBuffer(std::ifstream& file);
};


#endif



