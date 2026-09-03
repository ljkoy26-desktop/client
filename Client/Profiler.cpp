//----------------------------------------------------------------------
// Profiler.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "Profiler.h"

// <MMSystem.h> not included: this file only uses timeGetTime(), which
// basic/Platform.h already routes through platform_get_ticks(); including
// the real header here conflicts with that macro (see basic/Platform.h).

using namespace std;


//----------------------------------------------------------------------
// 전역 변수
//----------------------------------------------------------------------
Profiler*	g_pProfiler = NULL;

//----------------------------------------------------------------------
//
//							ProfilerInfo
//
//----------------------------------------------------------------------
ProfilerInfo::ProfilerInfo()
{
	Clear();
}

ProfilerInfo::~ProfilerInfo()
{
}

//----------------------------------------------------------------------
// 초기화
//----------------------------------------------------------------------
void		
ProfilerInfo::Clear()
{
	m_StartTime = 0;
	m_TotalTime = 0;
	m_Times = 0;
}

//----------------------------------------------------------------------
// 시작
//----------------------------------------------------------------------
void		
ProfilerInfo::Begin()
{
	//if (m_StartTime!=0)
	{
		//End();

	}

	m_StartTime = timeGetTime();
}

//----------------------------------------------------------------------
// 종료
//----------------------------------------------------------------------
void		
ProfilerInfo::End()
{
	if (m_StartTime!=0)
	{
		DWORD endTime = timeGetTime();

		m_TotalTime += endTime - m_StartTime;
		m_Times ++;

		m_StartTime = 0;
	}
}

//----------------------------------------------------------------------
// 파일에 저장
//----------------------------------------------------------------------
void
ProfilerInfo::WriteToFile(ofstream& file) const
{	
	char str[256];
	sprintf(str, "%10d  %10d  %7.3f", m_Times, m_TotalTime, GetAverageTime());

	file << str;
}

//----------------------------------------------------------------------
//
//							Profiler
//
//----------------------------------------------------------------------
Profiler::Profiler()
{
}

Profiler::~Profiler()
{
	Release();
}

//----------------------------------------------------------------------
// 해제
//----------------------------------------------------------------------
void			
Profiler::Release()
{
	PROFILE_MAP::iterator iInfo = m_mapProfile.begin();

	while (iInfo != m_mapProfile.end())
	{
		delete iInfo->second;

		iInfo ++;
	}

	m_mapProfile.clear();
}

//----------------------------------------------------------------------
// 시작
//----------------------------------------------------------------------
void			
Profiler::Begin(const char* pName)
{
	std::string name = pName;

	PROFILE_MAP::iterator iInfo = m_mapProfile.find( name );

	if (iInfo != m_mapProfile.end())
	{
		iInfo->second->Begin();
	}
	else
	{
		// 없으면 생성해서 추가해준다.
		ProfilerInfo* pInfo = new ProfilerInfo;

		m_mapProfile[name] = pInfo;

		pInfo->Begin();
	}
}

//----------------------------------------------------------------------
// 종료
//----------------------------------------------------------------------
void			
Profiler::End(const char* pName)
{
	PROFILE_MAP::iterator iInfo = m_mapProfile.find( std::string(pName) );

	if (iInfo != m_mapProfile.end())
	{
		iInfo->second->End();
	}
}

//----------------------------------------------------------------------
// 프로파일 정보 보유 여부 확인
//----------------------------------------------------------------------
bool			
Profiler::HasProfileInfo(const char* pName) const
{
	PROFILE_MAP::const_iterator iInfo = m_mapProfile.find( std::string(pName) );

	if (iInfo != m_mapProfile.end())
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// 실행 횟수 반환
//----------------------------------------------------------------------
DWORD			
Profiler::GetTimes(const char* pName) const
{
	PROFILE_MAP::const_iterator iInfo = m_mapProfile.find( std::string(pName) );

	if (iInfo != m_mapProfile.end())
	{
		return iInfo->second->GetTimes();
	}

	return 0;
}

//----------------------------------------------------------------------
// 총 시간 반환
//----------------------------------------------------------------------
DWORD			
Profiler::GetTotalTime(const char* pName) const
{
	PROFILE_MAP::const_iterator iInfo = m_mapProfile.find( std::string(pName) );

	if (iInfo != m_mapProfile.end())
	{
		return iInfo->second->GetTotalTime();
	}

	return 0;
}

//----------------------------------------------------------------------
// 평균 시간 반환
//----------------------------------------------------------------------
float
Profiler::GetAverageTime(const char* pName) const
{
	PROFILE_MAP::const_iterator iInfo = m_mapProfile.find( std::string(pName) );

	if (iInfo != m_mapProfile.end())
	{
		return iInfo->second->GetAverageTime();
	}

	return 0;
}

//----------------------------------------------------------------------
// 파일에 저장
//----------------------------------------------------------------------
void		
Profiler::WriteToFile(const char* pFilename, bool bAppend) const
{
	DWORD flag = (bAppend? ios::app : 0);

	ofstream file(pFilename, flag);

	PROFILE_MAP::const_iterator iInfo = m_mapProfile.begin();

	char str[256];

	sprintf(str, "%-20s ", "ProfilerName");
	file << "\n\n"
		<< str
		<< " LoopTimes   TotalTime   AverageTime"
		<< "\n\n";

	while (iInfo != m_mapProfile.end())
	{
		sprintf(str, "%-20s ", (const char*)iInfo->first.c_str());

		file << str;

		iInfo->second->WriteToFile( file );

		file << "\n";

		iInfo ++;
	}

	file.close();
}