#include <fstream>
#ifndef __MPALETTE_H__
#define __MPALETTE_H__

#ifdef PLATFORM_WINDOWS
	#include <windows.h>
#else
	#include "../basic/Platform.h"
#endif

class MPalette
{
public:
	MPalette();
	~MPalette();
	
	//--------------------------------------------------------
	// 초기화/해제
	//--------------------------------------------------------
	void	Release();
	void	Init(BYTE size);
	
	//--------------------------------------------------------
	// 크기
	//--------------------------------------------------------
	BYTE		GetSize() const { return m_Size; }
	
	//--------------------------------------------------------
	// 연산자
	//--------------------------------------------------------
	WORD&		operator [] (BYTE n)		{ return m_pColor[n]; }
	WORD&		operator [] (BYTE n) const { return m_pColor[n]; }
	void		operator = (const MPalette& pal);
	
	//--------------------------------------------------------
	// 파일 입출력
	//--------------------------------------------------------
	virtual bool	SaveToFile(std::ofstream& file) { return false; };
	virtual bool	LoadFromFile(std::ifstream& file) { return false; };		

	bool IsInit() const { return (m_Size == 0)?false:true; }
	
protected:
	WORD *		m_pColor;
	BYTE		m_Size;
};

class MPalette555 : public MPalette
{
public:
	//--------------------------------------------------------
	// 파일 입출력
	//--------------------------------------------------------
	bool LoadFromFile(std::ifstream &file);
	bool SaveToFile(std::ofstream &file);
};

class MPalette565 : public MPalette
{
public:
	//--------------------------------------------------------
	// 파일 입출력
	//--------------------------------------------------------
	bool LoadFromFile(std::ifstream &file);
	bool SaveToFile(std::ofstream &file);
};

#endif