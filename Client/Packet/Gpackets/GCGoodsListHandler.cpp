////////////////////////////////////////////////////////////////////////////////
// Filename    : GCGoodsListHandler.cpp
// 작성자: 김성민
// 설명:
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCGoodsList.h"
#include "ClientDef.h"
#include "UIFunction.h"

void GCGoodsListHandler::execute ( GCGoodsList * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	UI_RunItemShop( pPacket );			

	__END_CATCH
}
