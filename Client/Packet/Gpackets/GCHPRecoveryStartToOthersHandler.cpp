//////////////////////////////////////////////////////////////////////
//
// Filename    : GCHPRecoveryStartToOthersHandler.cc
// 작성자: elca@ewestsoft.com
// 설명:
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCHPRecoveryStartToOthers.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCHPRecoveryStartToOthersHandler::execute ( GCHPRecoveryStartToOthers * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY


	if (g_pZone!=NULL)
	{
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

		if (pCreature!=NULL)
		{
			DWORD delay = ConvertDurationToMillisecond( pPacket->getDelay() );

			pCreature->SetRecoveryHP( pPacket->getQuantity(),
										pPacket->getPeriod(),
										delay );
		}
	}

	__END_CATCH
}
