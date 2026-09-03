//---------------------------------------------------------------------------
// MHelpStringTable.h
//---------------------------------------------------------------------------

#ifndef __MHELPSTRINGTABLE_H__
#define __MHELPSTRINGTABLE_H__

#include "MStringArray.h"
#include "MHelpDef.h"

class MHelpStringTable : public MStringArray {
	public :
		MHelpStringTable();
		~MHelpStringTable();

		//------------------------------------------------------
		// 초기화
		//------------------------------------------------------
		void		Init( int size );

		//------------------------------------------------------
		// Displayed
		//------------------------------------------------------
		void		ClearDisplayed();
		bool		IsDisplayed(HELP_OUTPUT ho)		{ return m_Displayed[ho]; }

		//-------------------------------------------------------
		// Reference
		//-------------------------------------------------------		
		MString&	operator [] (int type);
		MString&	Get(int type);

		//-------------------------------------------------------
		// 파일 입출력
		//-------------------------------------------------------
		//void			SaveToFile(std::ofstream& file);
		void			LoadFromFile(std::ifstream& file);

	protected :
		CTypeTable<bool>		m_Displayed;
};

extern MHelpStringTable*	g_pHelpStringTable;

#endif
