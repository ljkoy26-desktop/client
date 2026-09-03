//////////////////////////////////////////////////////////////////////
//
// Filename    : GCHPRecoveryToOhtersHandler.cc
// 작성자: elca@ewestsoft.com
// 설명:
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCHPRecoveryStartToSelf.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCHPRecoveryStartToSelfHandler::execute ( GCHPRecoveryStartToSelf * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		

	// item 사용 검증
	//UseItemOK();

	if (g_pPlayer!=NULL)
	{
		DWORD delay = ConvertDurationToMillisecond( pPacket->getDelay() );
	
		g_pPlayer->SetRecoveryHP( pPacket->getQuantity(),
									pPacket->getPeriod(),
									delay );
	}	


	__END_CATCH
}
