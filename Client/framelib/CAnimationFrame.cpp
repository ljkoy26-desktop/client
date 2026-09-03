//----------------------------------------------------------------------
// CAnimationFrame.cpp
//----------------------------------------------------------------------
#include "Frame_PCH.h"
#include "CAnimationFrame.h"

//----------------------------------------------------------------------
//
// 생성자/소멸자
//
//----------------------------------------------------------------------

CAnimationFrame::CAnimationFrame(BYTE bltType)
{
	m_FrameID		= 0;

	m_MaxFrame		= 0;
	m_CurrentFrame	= 0;

	m_BltType		= bltType;
}

CAnimationFrame::~CAnimationFrame()
{
}


//----------------------------------------------------------------------
//
// 멤버 함수
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// 파일로 저장
//----------------------------------------------------------------------
void	
CAnimationFrame::SaveToFile(std::ofstream& file)
{
	file.write((const char*)&m_FrameID, SIZE_FRAMEID);
	file.write((const char*)&m_MaxFrame, 1);
	//file.write((const char*)&m_CurrentFrame, 1);
}

//----------------------------------------------------------------------
// 파일에서 로드
//----------------------------------------------------------------------
void	
CAnimationFrame::LoadFromFile(std::ifstream& file)
{
	file.read((char*)&m_FrameID, SIZE_FRAMEID);
	file.read((char*)&m_MaxFrame, 1);
	//file.write((const char*)&m_CurrentFrame, 1);
}