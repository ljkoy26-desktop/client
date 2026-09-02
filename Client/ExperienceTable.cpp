//---------------------------------------------------------------------------------
// ExperienceTable.cpp
//---------------------------------------------------------------------------------
#include "Client_PCH.h"
#include "ExperienceTable.h"

//---------------------------------------------------------------------------------
// 전역 변수
//---------------------------------------------------------------------------------
ExperienceTable* g_pExperienceTable = NULL;

//---------------------------------------------------------------------------------
//
//					ExperienceTable
//
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// 생성자/소멸자
//---------------------------------------------------------------------------------
ExperienceTable::ExperienceTable()
{
	// 냠냠..
}

ExperienceTable::~ExperienceTable()
{
	// 음냐.. 안해도 되는데.. 걍..
	Release();
}

//---------------------------------------------------------------------------------
// Release
//---------------------------------------------------------------------------------
void
ExperienceTable::Release()
{
	m_STRExp.Release();
	m_DEXExp.Release();
	m_INTExp.Release();
	m_VampireExp.Release();
	m_OustersExp.Release();
	m_SlayerRankExp.Release();
	m_VampireRankExp.Release();
	m_OustersRankExp.Release();
	m_PetExp.Release();
}

//---------------------------------------------------------------------------------
// LoadFromFileSTR
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFileSTR(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);

	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_STRExp.Init( num + 1 );

	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_STRExp[level].LoadFromFile( file );		
	}
}

//---------------------------------------------------------------------------------
// DEX 파일에서 로드
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFileDEX(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);

	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_DEXExp.Init( num + 1 );

	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_DEXExp[level].LoadFromFile( file );		
	}
}

//---------------------------------------------------------------------------------
// INT 파일에서 로드
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFileINT(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);

	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_INTExp.Init( num + 1 );

	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_INTExp[level].LoadFromFile( file );		
	}
}

//---------------------------------------------------------------------------------
// 뱀파이어 파일에서 로드
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFileVampire(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);

	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_VampireExp.Init( num + 1 );

	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_VampireExp[level].LoadFromFile( file );		
	}
}

//---------------------------------------------------------------------------------
// 아우스터스 파일에서 로드
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFileOusters(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);
	
	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_OustersExp.Init( num + 1 );
	
	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_OustersExp[level].LoadFromFile( file );		
	}
}

//---------------------------------------------------------------------------------
// 슬레이어 랭크 파일에서 로드
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFileSlayerRank(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);

	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_SlayerRankExp.Init( num + 1 );

	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_SlayerRankExp[level].LoadFromFile( file );		
	}
}

//---------------------------------------------------------------------------------
// 뱀파이어 랭크 파일에서 로드
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFileVampireRank(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);

	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_VampireRankExp.Init( num + 1 );

	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_VampireRankExp[level].LoadFromFile( file );		
	}
}

//---------------------------------------------------------------------------------
// 아우스터스 랭크 파일에서 로드
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFileOustersRank(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);
	
	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_OustersRankExp.Init( num + 1 );
	
	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_OustersRankExp[level].LoadFromFile( file );		
	}
}

//---------------------------------------------------------------------------------
// 아우스터스 랭크 파일에서 로드
//---------------------------------------------------------------------------------
void		
ExperienceTable::LoadFromFilePetExp(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);
	
	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_PetExp.Init( num + 1 );
	
	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_PetExp[level].LoadFromFile( file );		
	}
}

void
ExperienceTable::LoadFromFileAdvanceMent(std::ifstream& file)
{
	int num, level;
	file.read((char*)&num, 4);
	
	// file에는 1 level부터 들어가있는걸로 가정하기 때문에...
	m_advanceSkillExp.Init( num + 1 );
	
	for (int i=0; i<num; i++)
	{
		file.read((char*)&level, 4);
		
		// level에 맞게 loading한다.
		m_advanceSkillExp[level].LoadFromFile( file );		
	}
}



//---------------------------------------------------------------------------------
// STR 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetSTRInfo(int level) const
{
	return m_STRExp[level];
}

//---------------------------------------------------------------------------------
// DEX 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetDEXInfo(int level) const
{
	return m_DEXExp[level];
}

//---------------------------------------------------------------------------------
// INT 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetINTInfo(int level) const
{
	return m_INTExp[level];
}

//---------------------------------------------------------------------------------
// 뱀파이어 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetVampireInfo(int level) const
{
	return m_VampireExp[level];
}

//---------------------------------------------------------------------------------
// 아우스터스 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetOustersInfo(int level) const
{
	return m_OustersExp[level];
}

//---------------------------------------------------------------------------------
// 슬레이어 랭크 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetRankInfo(int level, Race_t race) const
{
	switch(race)
	{
	case RACE_SLAYER:
		return m_SlayerRankExp[level];
		break;

	case RACE_VAMPIRE:
		return m_VampireRankExp[level];
		break;

	case RACE_OUSTERS:
		return m_VampireRankExp[level];
		break;
	}

	return m_SlayerRankExp[level];
}

//---------------------------------------------------------------------------------
// 슬레이어 랭크 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetSlayerRankInfo(int level) const
{
	return m_SlayerRankExp[level];
}

//---------------------------------------------------------------------------------
// 뱀파이어 랭크 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetVampireRankInfo(int level) const
{
	return m_VampireRankExp[level];
}

//---------------------------------------------------------------------------------
// 아우스터스 랭크 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetOustersRankInfo(int level) const
{
	return m_OustersRankExp[level];
}

//---------------------------------------------------------------------------------
// 아우스터스 랭크 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetPetExp(int level) const
{
	return m_PetExp[level];
}


//---------------------------------------------------------------------------------
// 어드밴스먼트 정보 반환
//---------------------------------------------------------------------------------
const ExpInfo&		
ExperienceTable::GetAdvanceMent(int level) const
{
	return m_advanceSkillExp[level];
}
