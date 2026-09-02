//----------------------------------------------------------------------
// CFileIndexTable.h
//----------------------------------------------------------------------
//
// Index File을 Load한다.
//
//----------------------------------------------------------------------

#ifndef	__CFILEINDEXTABLE_H__
#define	__CFILEINDEXTABLE_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif

#include <fstream>

class CFileIndexTable {
	public :
		CFileIndexTable();
		~CFileIndexTable();		

		//--------------------------------------------------------
		// 파일 입출력
		//--------------------------------------------------------
		bool			LoadFromFile(std::ifstream& indexFile);		

		WORD				GetSize()					{ return m_Size; }

		//--------------------------------------------------------
		// 연산자
		//--------------------------------------------------------
		const long&		operator [] (WORD n) { return m_pIndex[n]; }

		//--------------------------------------------------------
		// 해제
		//--------------------------------------------------------
		void			Release();


	protected :
		//--------------------------------------------------------
		// 초기화/해제
		//--------------------------------------------------------
		void			Init(WORD count);		
		

	protected :
		WORD			m_Size;				// 개수
		long*			m_pIndex;			// 파일 위치
};

#endif
