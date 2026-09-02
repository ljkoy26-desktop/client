#include <fstream>
//----------------------------------------------------------------------
// CSpritePackList555.h
//----------------------------------------------------------------------
// 5:5:5를 위한 클래스
//----------------------------------------------------------------------

#ifndef	__CSPRITEPACKLIST555_H__
#define	__CSPRITEPACKLIST555_H__

#pragma warning(disable:4786)

#include <list>
#include "DrawTypeDef.h"
#include "CSprite.h"
#include "CSpritePackList.h"

typedef	std::list<CSprite*>		SPRITE_LIST;


class CSpritePackList555 : public CSpritePackList {
	public :
		CSpritePackList555() {}
		~CSpritePackList555() {}

		//---------------------------------------------------
		// 파일 입출력
		//---------------------------------------------------
		void		LoadFromFile(std::ifstream& file);
};

#endif

