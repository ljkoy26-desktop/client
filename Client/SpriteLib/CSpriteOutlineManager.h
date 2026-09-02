#ifndef __CSPRITEOUTLINEMANAGER_H__
#define __CSPRITEOUTLINEMANAGER_H__

class CSprite;
class CIndexSprite;

#include <vector>

struct OutlineEntry {
	int x;
	int y;
	class CSprite* pSprite;
	class CIndexSprite* pIndexSprite;
	unsigned short color;
};

class CSpriteOutlineManager
{
public:
	CSpriteOutlineManager();
	~CSpriteOutlineManager();

	void Clear();
	bool IsInit() const { return !m_Outlines.empty(); } // 초기화 여부에 대한 단순 확인
	void Add(int x, int y, class CSprite* pSprite);
	void Add(int x, int y, class CIndexSprite* pSprite, unsigned short color = 0); // color 파라미터에 이제 기본값이 있음
	void Generate(int type = 0); // type 파라미터에 이제 기본값이 있음

	// 스프라이트를 그리기 위한 윤곽선 항목에 접근한다
	const std::vector<OutlineEntry>& GetOutlines() const { return m_Outlines; }

private:
	std::vector<OutlineEntry> m_Outlines;
};

#endif /* __CSPRITEOUTLINEMANAGER_H__ */
