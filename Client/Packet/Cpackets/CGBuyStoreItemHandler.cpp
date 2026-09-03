//////////////////////////////////////////////////////////////////////////////
// 파일명: CGBuyStoreItemHandler.cpp
// 작성자: 김성민
// 설명: 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGBuyStoreItem.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "PacketAssert.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// 플레이어가 원하는 물건을 파는 NPC와 그 아이템이 있는지
// 확인하고, 일반 아이템과 모터 사이클 처리부분으로 분기한다.
//////////////////////////////////////////////////////////////////////////////
void CGBuyStoreItemHandler::execute (CGBuyStoreItem* pPacket , Player* pPlayer)

throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

#endif

	__END_CATCH
}


  