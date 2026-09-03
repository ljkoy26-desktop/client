//----------------------------------------------------------------------
// 
// 파일명: LCCreatePCOK.cpp
// 작성자: Reiot
// 설명: 
// 
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCCreatePCOK.h"

//----------------------------------------------------------------------
// execute packet handler
//----------------------------------------------------------------------
void LCCreatePCOK::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	LCCreatePCOKHandler::execute( this , pPlayer );
		
	__END_CATCH
}


