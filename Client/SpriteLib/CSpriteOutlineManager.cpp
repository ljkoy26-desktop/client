#include "CSpriteOutlineManager.h"
#include "CSprite.h"
#include "CIndexSprite.h"

CSpriteOutlineManager::CSpriteOutlineManager()
{
	m_Outlines.clear();
}

CSpriteOutlineManager::~CSpriteOutlineManager()
{
	Clear();
}

void CSpriteOutlineManager::Clear()
{
	m_Outlines.clear();
}

void CSpriteOutlineManager::Add(int x, int y, CSprite* pSprite)
{
	OutlineEntry entry;
	entry.x = x;
	entry.y = y;
	entry.pSprite = pSprite;
	entry.pIndexSprite = NULL;
	entry.color = 0;
	m_Outlines.push_back(entry);
}

void CSpriteOutlineManager::Add(int x, int y, CIndexSprite* pSprite, unsigned short color)
{
	OutlineEntry entry;
	entry.x = x;
	entry.y = y;
	entry.pSprite = NULL;
	entry.pIndexSprite = pSprite;
	entry.color = color;
	m_Outlines.push_back(entry);
}

void CSpriteOutlineManager::Generate(int type)
{
	// TODO: 스프라이트 윤곽선을 생성한다
	// 지금은 스텁이다
	(void)type;
}
