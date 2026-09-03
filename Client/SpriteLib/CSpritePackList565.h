#include <fstream>
//----------------------------------------------------------------------
// CSpritePackList565.h
//----------------------------------------------------------------------
// 5:6:5를 위한 클래스
//----------------------------------------------------------------------

#ifndef	__CSPRITEPACKLIST565_H__
#define	__CSPRITEPACKLIST565_H__

#pragma warning(disable:4786)

#include <list>
#include "DrawTypeDef.h"
#include "CSprite.h"
#include "CSpritePackList.h"

typedef	std::list<CSprite*>		SPRITE_LIST;


class CSpritePackList565 : public CSpritePackList {
	public :
		CSpritePackList565() {}
		~CSpritePackList565() {}

		//---------------------------------------------------
		// 파일 입출력
		//---------------------------------------------------
		void		LoadFromFile(std::ifstream& file);
};

#endif



