#include <fstream>
//----------------------------------------------------------------------
// CIndexSprite555.h
//----------------------------------------------------------------------
// 5:5:5 비디오 카드에 대한 스프라이트 클래스
//----------------------------------------------------------------------

#ifndef	__CINDEXSPRITE555_H__
#define	__CINDEXSPRITE555_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif
std::ofstream;
std::ifstream;


#include "CIndexSprite.h"


class CIndexSprite555 : public CIndexSprite {
	public :
		//---------------------------------------------------------
		// fstream에서 저장/불러오기를 한다.
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);		

};


#endif



