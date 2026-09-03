//-----------------------------------------------------------------------------
// MShopTemplateTable.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MShopTemplateTable.h"

//-----------------------------------------------------------------------------
// global
//-----------------------------------------------------------------------------
MShopTemplateTable*		g_pShopTemplateTable = NULL;

//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
MShopTemplate::MShopTemplate()
{
}

MShopTemplate::~MShopTemplate()
{
}


//-----------------------------------------------------------------------------
// 파일에 저장
//-----------------------------------------------------------------------------
void		
MShopTemplate::SaveToFile(std::ofstream& file)
{
	//file.write((const char*)&m_ID,        szShopTemplateID);
	file.write((const char*)&Type,      1);
	file.write((const char*)&Class,     4);
	file.write((const char*)&MinType,   2);
	file.write((const char*)&MaxType,   2);
	file.write((const char*)&MinOption, 1);
	file.write((const char*)&MaxOption, 1);
}

//-----------------------------------------------------------------------------
// 파일에서 불러오기
//-----------------------------------------------------------------------------
void		
MShopTemplate::LoadFromFile(std::ifstream& file)
{
	//file.read((char*)&m_ID,        szShopTemplateID);
	file.read((char*)&Type,      1);
	file.read((char*)&Class,     4);
	file.read((char*)&MinType,   2);
	file.read((char*)&MaxType,   2);
	file.read((char*)&MinOption, 1);
	file.read((char*)&MaxOption, 1);
}


//-----------------------------------------------------------------------------
//
// MShopTemplateTable
//
//-----------------------------------------------------------------------------
MShopTemplateTable::MShopTemplateTable()
{
}

MShopTemplateTable::~MShopTemplateTable()
{
}

