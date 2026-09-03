//----------------------------------------------------------------------
// MInteractionObjectTable.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MInteractionObjectTable.h"

//----------------------------------------------------------------------
// 전역 변수
//----------------------------------------------------------------------
INTERACTIONOBJECT_TABLE* 	g_pInteractionObjectTable = NULL;


//----------------------------------------------------------------------
// 파일에 저장
//----------------------------------------------------------------------
void		
INTERACTIONOBJECTTABLE_INFO::SaveToFile(std::ofstream& file)
{
	file.write((const char*)&Type, 1);
	file.write((const char*)&FrameID, SIZE_FRAMEID);			
	file.write((const char*)&Property, 4);
	file.write((const char*)&SoundID, 4);
}

//----------------------------------------------------------------------
// 파일에서 불러오기
//----------------------------------------------------------------------
void			
INTERACTIONOBJECTTABLE_INFO::LoadFromFile(std::ifstream& file)
{
	file.read((char*)&Type, 1);
	file.read((char*)&FrameID, SIZE_FRAMEID);			
	file.read((char*)&Property, 4);
	file.read((char*)&SoundID, 4);
}
