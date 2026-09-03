/*-----------------------------------------------------------------------------

	CGHandlersStub.cpp

	CG(클라이언트→게임) 패킷 핸들러의 스텁 구현.
	실제 핸들러 구현은 서버 측에 있으므로 이 스텁이 필요하다.

-----------------------------------------------------------------------------*/

#include "Client_PCH.h"

// 핸들러 클래스 선언을 위한 패킷 헤더 포함
#include "Packet/Cpackets/CGBuyStoreItem.h"
#include "Packet/Cpackets/CGConnectSetKey.h"
#include "Packet/Cpackets/CGDisplayItem.h"
#include "Packet/Cpackets/CGDonationMoney.h"
#include "Packet/Cpackets/CGLotterySelect.h"
#include "Packet/Cpackets/CGMixItem.h"
#include "Packet/Cpackets/CGRequestStoreInfo.h"
#include "Packet/Cpackets/CGSelectQuest.h"
#include "Packet/Cpackets/CGSelectRegenZone.h"
#include "Packet/Cpackets/CGStoreClose.h"
#include "Packet/Cpackets/CGStoreOpen.h"
#include "Packet/Cpackets/CGStoreSign.h"
#include "Packet/Cpackets/CGUndisplayItem.h"
#include "Packet/Cpackets/CGWithdrawPet.h"

// 상점 관련 핸들러 스텁 구현
void CGBuyStoreItemHandler::execute(CGBuyStoreItem* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGConnectSetKeyHandler::execute(CGConnectSetKey* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGDisplayItemHandler::execute(CGDisplayItem* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGDonationMoneyHandler::execute(CGDonationMoney* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGLotterySelectHandler::execute(CGLotterySelect* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGMixItemHandler::execute(CGMixItem* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGRequestStoreInfoHandler::execute(CGRequestStoreInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGSelectQuestHandler::execute(CGSelectQuest* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGSelectRegenZoneHandler::execute(CGSelectRegenZone* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGStoreCloseHandler::execute(CGStoreClose* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGStoreOpenHandler::execute(CGStoreOpen* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGStoreSignHandler::execute(CGStoreSign* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGUndisplayItemHandler::execute(CGUndisplayItem* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
void CGWithdrawPetHandler::execute(CGWithdrawPet* pPacket, Player* pPlayer) throw(ProtocolException, Error) {}
