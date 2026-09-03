#include <fstream>
//----------------------------------------------------------------------
// CSprite555.h
//----------------------------------------------------------------------
// 5:5:5 비디오 카드에 대한 스프라이트 클래스
//----------------------------------------------------------------------

#ifndef	__CSPRITE555_H__
#define	__CSPRITE555_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif
std::ofstream;
std::ifstream;


#include "CSprite.h"


class CSprite555 : public CSprite {
	public :
		CSprite555() = default;
		virtual ~CSprite555() = default;

		//---------------------------------------------------------
		// fstream에서 저장/불러오기를 한다.
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);
		//bool		LoadFromFileToBuffer(std::ifstream& file);

};


#endif



