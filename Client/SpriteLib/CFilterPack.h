#include <fstream>
//----------------------------------------------------------------------
// CFilterPack.h
//----------------------------------------------------------------------
//
// FilterPack의 Filter에 대한 File Pointer Index가 필요하다.
// FilterSet에서 읽기 위해서.
//
//----------------------------------------------------------------------

#ifndef	__CFILTERPACK_H__
#define	__CFILTERPACK_H__

#include "DrawTypeDef.h"
#include "CFilter.h"

class CFilterPack {
	public :
		CFilterPack();
		~CFilterPack();

		//--------------------------------------------------------
		// 초기화/해제
		//--------------------------------------------------------
		void		Init(TYPE_FILTERID count);		
		void		Release();		
		
		//--------------------------------------------------------
		// 파일 입출력
		//--------------------------------------------------------
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);		
		
		//--------------------------------------------------------
		// 크기
		//--------------------------------------------------------
		TYPE_FILTERID	GetSize() const		{ return m_nFilters; }

		//--------------------------------------------------------
		// 연산자
		//--------------------------------------------------------
		CFilter&	operator [] (TYPE_FILTERID n) { return m_pFilters[n]; }

	protected :
		TYPE_FILTERID		m_nFilters;		// CFilter의 개수
		CFilter*			m_pFilters;		// CFilter들을 저장해둔다.
};

#endif
	