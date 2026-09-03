//----------------------------------------------------------------------
//
// 파일명: LCVersionCheckOKHandler.cpp
// 작성자: Reiot
// 설명: 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCVersionCheckOK.h"
#include "ClientDef.h"


//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void LCVersionCheckOKHandler::execute ( LCVersionCheckOK * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__


	// 정상적인 경우.. 아무것도 할게 없다.

#endif
		
	__END_CATCH
}
