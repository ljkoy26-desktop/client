//----------------------------------------------------------------------
// MGuildInfoMapper.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MGuildInfoMapper.h"

//----------------------------------------------------------------------
// 전역 변수
//----------------------------------------------------------------------
MGuildInfoMapper* g_pGuildInfoMapper = NULL;


//----------------------------------------------------------------------
//
//						GUILD_INFO
//
//----------------------------------------------------------------------
GUILD_INFO::GUILD_INFO()
{
	m_SpriteID = SPRITEID_NULL;	
}

GUILD_INFO::GUILD_INFO(const char* pGuildName, const char* pLeaderName)
{
	m_SpriteID = SPRITEID_NULL;
//	m_GuildName = pGuildName;
//	m_LeaderName = pLeaderName;
}

GUILD_INFO::~GUILD_INFO()
{
}

//----------------------------------------------------------------------
// 파일에 저장
//----------------------------------------------------------------------
void
GUILD_INFO::SaveToFile(std::ofstream& file)
{
	file.write((const char*)&m_SpriteID, SIZE_SPRITEID);
//	m_GuildName.SaveToFile( file );
//	m_LeaderName.SaveToFile( file );
}

//----------------------------------------------------------------------
// 파일에서 불러오기
//----------------------------------------------------------------------
void
GUILD_INFO::LoadFromFile(std::ifstream& file)
{
	file.read((char*)&m_SpriteID, SIZE_SPRITEID);
//	MString tempString;
//	tempString.LoadFromFile(file);
//	tempString.LoadFromFile(file);
//	m_GuildName.LoadFromFile( file );
//	m_LeaderName.LoadFromFile( file );
}

//----------------------------------------------------------------------
// 대입 연산자
//----------------------------------------------------------------------
void			
GUILD_INFO::operator = (const GUILD_INFO& info)
{
	m_SpriteID = info.m_SpriteID;
//	m_GuildName = info.m_GuildName;
//	m_LeaderName = info.m_GuildName;
}

//----------------------------------------------------------------------
//
//						MGuildInfoMapper
//
//----------------------------------------------------------------------
MGuildInfoMapper::MGuildInfoMapper()
{
}

MGuildInfoMapper::~MGuildInfoMapper()
{
	Release();
}

//----------------------------------------------------------------------
// 초기화
//----------------------------------------------------------------------
/*
void
MGuildInfoMapper::Init(int n)
{
	Release();

	//resize( n );	
}
*/

//----------------------------------------------------------------------
// 해제
//----------------------------------------------------------------------
void	
MGuildInfoMapper::Release()
{
	GUILD_INFO_MAP::iterator iInfo = begin();

	while (iInfo != end())
	{
		GUILD_INFO* pInfo = iInfo->second;

		if (pInfo!=NULL)
		{
			delete pInfo;
		}

		iInfo ++;
	}

	clear();
}

//----------------------------------------------------------------------
// 연산자 []
//----------------------------------------------------------------------
/*
TYPE_SPRITEID&	
MGuildInfoMapper::operator [] (int n)
{
	//return at(n);
	GUILD_INFO_MAP::iterator iID = find( n );

	if (iID != end())
	{
		return iID->
	}
}

const TYPE_SPRITEID&	
MGuildInfoMapper::operator [] (int n) const
{
	//return at(n);
}
*/

//----------------------------------------------------------------------
// 반환
//----------------------------------------------------------------------
GUILD_INFO*	
MGuildInfoMapper::Get(WORD guildID) const
{
	GUILD_INFO_MAP::const_iterator iID = find( guildID );

	if (iID == end())
	{
		return NULL;
	}

	return iID->second;
}

//----------------------------------------------------------------------
// 설정
//----------------------------------------------------------------------
void			
MGuildInfoMapper::Set(WORD guildID, GUILD_INFO* pInfo)
{
	// 이미 있었다면 지운다.
	GUILD_INFO* pOldInfo = Get(guildID);

	if (pOldInfo!=NULL)
	{
		delete pOldInfo;
	}

	// 재 설정..
	(*this)[guildID] = pInfo;
}

//----------------------------------------------------------------------
// 파일에 저장
//----------------------------------------------------------------------
void
MGuildInfoMapper::SaveToFile(std::ofstream& file)
{
	int num = size();

	file.write((const char*)&num, 4);

	GUILD_INFO_MAP::iterator iID = begin();

	while (iID != end())
	{
		WORD		guildID	= iID->first;
		GUILD_INFO*	pInfo	= iID->second;

		file.write((const char*)&guildID, 2);
		pInfo->SaveToFile( file );

		iID ++;
	}
}

//----------------------------------------------------------------------
// 파일에서 불러오기
//----------------------------------------------------------------------
void
MGuildInfoMapper::LoadFromFile(std::ifstream& file)
{
	int num;

	file.read((char*)&num, 4);

	Release();

	WORD guildID;
	
	for (int i=0; i<num; i++)
	{
		file.read((char*)&guildID, 2);
		
		GUILD_INFO*	pInfo	= new GUILD_INFO;
		pInfo->LoadFromFile( file );

		(*this)[guildID] = pInfo;
	}
}

//----------------------------------------------------------------------
// 파일에서 값 변경
//----------------------------------------------------------------------
// 화일에서 사바사바.. -_-;
//----------------------------------------------------------------------
/*
void	
MGuildInfoMapper::ChangeValueToFile(const char* pFilename, WORD guildID) const
{
	std::ofstream file(pFilename, ios::binary | ios::ate);

	file.seekp( 4 + guildID*(sizeof(WORD)+SIZE_SPRITEID), ios::beg );

	TYPE_SPRITEID spriteID = at(guildID);
	file.write((const char*)&spriteID, SIZE_SPRITEID);
}
*/

//----------------------------------------------------------------------
// 정보를 파일에 저장
//----------------------------------------------------------------------
//void	
//MGuildInfoMapper::SaveInfoToFile(const char* pFilename)
//{
//	std::ofstream file( pFilename );
//
//	GUILD_INFO_MAP::iterator iID = begin();
//
//	char str[256];
//	int emptyGuildMarkCount = 0;
//
//	file << "|| '''길드ID''' || '''길드 이름''' || '''마스터 이름''' || '''길드마크 SpriteID''' ||" << endl;
//
//	while (iID != end())
//	{
//		WORD		guildID	= iID->first;
//		GUILD_INFO*	pInfo	= iID->second;
//
//		sprintf(str, "|| %3d || %20s || %20s ||", guildID, pInfo->GetGuildName(), pInfo->GetLeaderName());
//		if(pInfo->GetSpriteID() == 0xffff)
//		{
//			sprintf(str, "%s 길드마크 없음 ||", str);
//			emptyGuildMarkCount++;
//		}
//		else
//			sprintf(str, "%s %13d ||", str, pInfo->GetSpriteID());
//
//		file << str << endl;
//
//		iID ++;
//	}
//
//	sprintf(str, "|| %5d 개의 길드중 %5d개의 길드 마크 없음 ||", size(), emptyGuildMarkCount);
//	file << str << endl;
//
//	file.close();
// }