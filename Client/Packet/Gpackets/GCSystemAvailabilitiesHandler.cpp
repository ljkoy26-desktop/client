//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSystemAvailabilitiesHandler.cc
// 작성자: elca@ewestsoft.com
// 설명:
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSystemAvailabilities.h"
#include <bitset>
#include "SystemAvailabilities.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCSystemAvailabilitiesHandler::execute ( GCSystemAvailabilities * pGCSystemAvailabilities , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
	g_pSystemAvailableManager->SetFlag( pGCSystemAvailabilities->getFlag() );
	g_pSystemAvailableManager->SetLimitSkillLevel( pGCSystemAvailabilities->getSkillLimit() );
	g_pSystemAvailableManager->SetOpenDegree( pGCSystemAvailabilities->getDegree() - 1 );

	__END_CATCH
}
