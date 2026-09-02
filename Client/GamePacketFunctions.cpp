//-----------------------------------------------------------------------------
// GamePacketFunctions.cpp
//-----------------------------------------------------------------------------
// PacketFunction.cpp 에서 분리한 크로스 플랫폼 게임 함수
// 클라이언트에 필요하지만 Windows 전용 API 에 의존하지 않는 함수들
//-----------------------------------------------------------------------------

#include "Client_PCH.h"
#include "MGameDef.h"
#include "MTopView.h"
#include "ClientDef.h"
#include "MCreatureWear.h"
#include "MItem.h"
#include "MEffectGeneratorTable.h"
#include "ClientConfig.h"
#include "MitemOptionTable.h"
#include "AddonDef.h"
#include "MItem.h"
#include "MInventory.h"
#include "MSlayerGear.h"
#include "MVampireGear.h"
#include "MOustersGear.h"
#include "MMoneyManager.h"
#include "MGameStringTable.h"
#include "VS_UI_Mouse_pointer.h"
#include "VS_UI.h"
#include "UIDialog.h"
#include "CServerInformation.h"
#include "PacketFunction.h"
#include "UserInformation.h"
#include "MFakeCreature.h"
#include "MSoundTable.h"
#include "MGuildMarkManager.h"
#include "MGuildInfoMapper.h"
#include "UIFunction.h"
#include "ServerInfo.h"
// packet
#include "packet/PetInfo.h"
#include "Packet/PCSlayerInfo3.h"
#include "Packet/PCOustersInfo3.h"
#include "Packet/GPackets/GCAddItemToZone.h"
#include "Packet/GPackets/GCAddMonster.h"
#include "Packet/PCSlayerInfo2.h"
#include "Packet/PCVampireInfo2.h"
#include "Packet/PCOustersInfo2.h"
#include "Packet/InventoryInfo.h"
#include "Packet/GearInfo.h"
#include "Packet/ExtraInfo.h"
#include "Packet/ModifyInfo.h"
#include "ExperienceTable.h"
#include "EffectInfo.h"
#include "MScreenEffectManager.h"
#include "TempInformation.h"
#include "DebugInfo.h"
#include "MEventManager.h"
#include "MCreature.h"
#include "MZone.h"
#include "MPlayer.h"
#include "VS_UI_GameCommon2.h"
#include "MEffectSpriteTypeTable.h"
#include "EffectSpriteTypeDef.h"
#include "BloodBibleSignInfo.h"

#define SAFE_DELETE(x)		{ if(x!=NULL) delete x; x=NULL; }

extern MScreenEffectManager*	g_pInventoryEffectManager;
extern DWORD				g_PreviousCreatureType;

// PacketFunction.cpp 에 정의된 헤어스타일 ID
extern int	g_PacketHairMaleID[3];
extern int	g_PacketHairFemaleID[3];

//-----------------------------------------------------------------------------
// NewFakeCreature - 크리처 타입으로 페이크 크리처 생성 (int 파라미터 4개)
//-----------------------------------------------------------------------------
MFakeCreature*
NewFakeCreature(int creatureType, int x, int y, int dir)
{
	//------------------------------------------------------
	// Fake Creature 생성
	//------------------------------------------------------
	MFakeCreature* pFakeCreature = new MFakeCreature;

	pFakeCreature->SetZone( g_pZone );

#ifdef OUTPUT_DEBUG
	char name[20];
	sprintf(name, "Fake%d", pFakeCreature->GetID());
	pFakeCreature->SetName( name );
#endif

	//pFakeCreature->SetID( 0 );  자동 발급 된다.
//	pFakeCreature->SetSameBody( pCreature );

//	if (pCreature->IsWear())
//	{
//		pFakeCreature->SetSameWear( (MCreatureWear*)pCreature );
//	}

	pFakeCreature->SetCreatureType(creatureType);
	pFakeCreature->SetPosition( x, y );
	pFakeCreature->SetServerPosition( x, y );

	pFakeCreature->SetDirection( dir );
	pFakeCreature->SetCurrentDirection( dir );
	pFakeCreature->SetAction( ACTION_STAND );

	return pFakeCreature;
}

//-----------------------------------------------------------------------------
// NewFakeCreature - 기존 크리처로부터 페이크 크리처 생성 (파라미터 3개)
//-----------------------------------------------------------------------------
MFakeCreature*
NewFakeCreature(MCreature* pCreature, int x, int y)
{
	//------------------------------------------------------
	// Fake Creature 생성
	//------------------------------------------------------
	MFakeCreature* pFakeCreature = new MFakeCreature;

	pFakeCreature->SetZone( g_pZone );

	#ifdef OUTPUT_DEBUG
		char name[20];
		sprintf(name, "Fake%d", pFakeCreature->GetID());
		pFakeCreature->SetName( name );
	#endif

	//pFakeCreature->SetID( 0 );  자동 발급 된다.
	pFakeCreature->SetSameBody( pCreature );

	if (pCreature->IsWear())
	{
		pFakeCreature->SetSameWear( (MCreatureWear*)pCreature );
	}

	pFakeCreature->SetPosition( x, y );
	pFakeCreature->SetServerPosition( x, y );

	int dir = pCreature->GetDirection();

	pFakeCreature->SetDirection( dir );
	pFakeCreature->SetCurrentDirection( dir );
	pFakeCreature->SetAction( ACTION_STAND );
	pFakeCreature->SetWeaponSpeed( pCreature->GetWeaponSpeed() );
	pFakeCreature->SetBasicActionInfo( pCreature->GetBasicActionInfo() );
	pFakeCreature->SetSpecialActionInfo( pCreature->GetSpecialActionInfo() );

	// add by Coffee
	//pFakeCreature->SetCreatureType(pFakeCreature->GetCreatureType());

	return pFakeCreature;
}

//-----------------------------------------------------------------------------
// SetFadeStart - 페이드 효과 시작
//-----------------------------------------------------------------------------
void SetFadeStart(char start, char end, char step, BYTE r, BYTE g, BYTE b, WORD delay)
{
	if (g_pTopView != NULL)
		g_pTopView->SetFadeStart(start, end, step, r, g, b, delay);
}

//-----------------------------------------------------------------------------
// SetFadeEnd - 페이드 효과 종료
//-----------------------------------------------------------------------------
void SetFadeEnd()
{
	if (g_pTopView != NULL)
		g_pTopView->SetFadeEnd();
}

//-----------------------------------------------------------------------------
// SetPetInfo - 펫 정보 설정
//-----------------------------------------------------------------------------
void SetPetInfo(PetInfo* pPetInfo, unsigned int flag)
{
	(void)pPetInfo; (void)flag;
	// 스텁 구현 - 펫 정보 처리
}

//-----------------------------------------------------------------------------
// SetGearInfo - 장비 정보 설정
//-----------------------------------------------------------------------------
void SetGearInfo(GearInfo* pGearInfo, BloodBibleSignInfo* pBloodBibleInfo)
{
	(void)pGearInfo; (void)pBloodBibleInfo;
	// 스텁 구현 - 장비 정보 처리
}

//-----------------------------------------------------------------------------
// SetExtraInfo - 추가 정보 설정
//-----------------------------------------------------------------------------
void SetExtraInfo(ExtraInfo* pExtraInfo)
{
	(void)pExtraInfo;
	// 스텁 구현 - 추가 정보 처리
}

//-----------------------------------------------------------------------------
// SetEffectInfo - 이펙트 정보 설정
//-----------------------------------------------------------------------------
void SetEffectInfo(MCreature* pCreature, EffectInfo* pEffectInfo, int delayedFrame)
{
	(void)pCreature; (void)pEffectInfo; (void)delayedFrame;
	// 스텁 구현 - 이펙트 정보 처리
}

//-----------------------------------------------------------------------------
// ComeFromPortal - 포탈에서 나오는 크리처
//-----------------------------------------------------------------------------
void ComeFromPortal(MCreature* pCreature)
{
	if (pCreature == NULL)
		return;

	//------------------------------------------------------
	// Fake Creature 생성
	//------------------------------------------------------
	int x = pCreature->GetX();
	int y = pCreature->GetY();
	MFakeCreature* pFakeCreature = NewFakeCreature( pCreature, x, y );

	pFakeCreature->SetFakeCreatureType( MFakeCreature::FAKE_CREATURE_TO_PORTAL );

	//------------------------------------------------------
	// Fake Creature를 Zone에 추가
	//------------------------------------------------------
	if (!g_pZone->AddFakeCreature( pFakeCreature ))
	{
		delete pFakeCreature;
	}

	//------------------------------------------------------
	// 빙빙 돌아서 캐릭을 휘감는 이펙트 표현..
	//------------------------------------------------------
	ExecuteActionInfoFromMainNode(
			RESULT_MAGIC_BLOODY_TUNNEL_INTO,
			x, y, 0,
			DIRECTION_DOWN,
			pFakeCreature->GetID(),
			x, y, 0,
			0,
			NULL,
			false);
}

//-----------------------------------------------------------------------------
// MoveIntoPortal - 포탈로 들어가는 크리처
//-----------------------------------------------------------------------------
void MoveIntoPortal(MCreature* pCreature)
{
	int x = pCreature->GetX();
	int y = pCreature->GetY();
	MFakeCreature* pFakeCreature = NewFakeCreature( pCreature, x, y );

	pFakeCreature->SetFakeCreatureType( MFakeCreature::FAKE_CREATURE_TO_PORTAL );

	//------------------------------------------------------
	// Fake Creature를 Zone에 추가
	//------------------------------------------------------
	if (!g_pZone->AddFakeCreature( pFakeCreature ))
	{
		delete pFakeCreature;
	}

	//------------------------------------------------------
	// 빙빙 돌아서 캐릭을 휘감는 이펙트 표현..
	//------------------------------------------------------
	ExecuteActionInfoFromMainNode(
			RESULT_MAGIC_BLOODY_TUNNEL_INTO,
			x, y, 0,
			DIRECTION_DOWN,
			pFakeCreature->GetID(),
			x, y, 0,
			0,
			NULL,
			false);
}

//-----------------------------------------------------------------------------
// SkillIllendue - 일렌듀 스킬 이펙트
//-----------------------------------------------------------------------------
void SkillIllendue(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID)
{
	if (pUserCreature==NULL || pTargetCreature==NULL)
	{
		DEBUG_ADD("[Error] ShadowDancing. Creature NULL or Not WearCreature");
		return;
	}

	int x = pTargetCreature->GetX();
	int y = pTargetCreature->GetY();

	int sx = pUserCreature->GetX() - x;
	int sy = pUserCreature->GetY() - y;

	if (sx>0) sx=1; else if (sx<0) sx=-1; else sx=0;
	if (sy>0) sy=1; else if (sy<0) sy=-1; else sy=0;

	POINT Straight[4] = {
		{ -2, -2 },
		{ -2,  2 },
		{ 2,  -2 },
		{ 2,   2 },
	};

	POINT Diagonal[4] = {
		{ 0,  2 },
		{ 2,  0 },
		{ -2, 0 },
		{ 0, -2 },
	};

	POINT *Position = (pUserCreature->GetDirection() & 0x1) ? Diagonal : Straight;
	if (skillID == TEMP_SKILL_ILLUSION_INVERSION)
	{
		Position = Straight;
	}

	for (int i=0; i<4; i++)
	{
		MFakeCreature* pFakeCreature = NewFakeCreature( pUserCreature, pUserCreature->GetX() , pUserCreature->GetY() );

		int dir = pFakeCreature->GetDirectionToPosition( pTargetCreature->GetX(), pTargetCreature->GetY() );
		pFakeCreature->SetDirection( dir );
		pFakeCreature->SetCurrentDirection( dir );

		pFakeCreature->SetAction( ACTION_MAGIC );

		pFakeCreature->SetFakeCreatureType( MFakeCreature::FAKE_CREATURE_FAST_MOVE_ACTION );

		if (!g_pZone->AddFakeCreature( pFakeCreature ))
		{
			delete pFakeCreature;
		}

		if (skillID==SKILL_ATTACK_MELEE)
		{
			skillID = pUserCreature->GetBasicActionInfo();
		}

		if (skillID >= g_pActionInfoTable->GetMinResultActionInfo())
		{
			DEBUG_ADD_FORMAT("[Error] SkillType Error = %d", skillID);
			return;
		}

		DWORD delayFrame = 0;

		pFakeCreature->SetFakeCreatureFastMoveAction(
			pTargetCreature->GetX() + Position[i].x, pTargetCreature->GetY() + Position[i].y,
			skillID,
			pTargetCreature->GetID() );
	}
}

//-----------------------------------------------------------------------------
// SkillBlazeWalk - 블레이즈 워크 스킬 이펙트
//-----------------------------------------------------------------------------
void SkillBlazeWalk(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID, BYTE grade)
{
	if (pUserCreature==NULL || pTargetCreature==NULL)
	{
		DEBUG_ADD("[Error] ShadowDancing. Creature NULL or Not WearCreature");
		return;
	}

	POINT TargetList[3] =
	{
		{ 1, 0 },
		{ -1, 0 },
		{ 0, 1 }
	};

	for (int i=0; i<=grade; i++)
	{
		MFakeCreature* pFakeCreature = NewFakeCreature( pUserCreature, pUserCreature->GetX() , pUserCreature->GetY() );

		int dir;
		switch( i )
		{
		case 0 :
			dir = 0;
			break;
		case 1 :
			dir = 4;
			break;
		case 2 :
			dir = 6;
			break;
		}

		pFakeCreature->SetDirection( dir );
		pFakeCreature->SetCurrentDirection( dir );

		pFakeCreature->SetAction( ACTION_MAGIC );

		pFakeCreature->SetFakeCreatureType( MFakeCreature::FAKE_CREATURE_FAST_MOVE_ACTION );

		if (!g_pZone->AddFakeCreature( pFakeCreature ))
		{
			delete pFakeCreature;
		}

		if (skillID==SKILL_ATTACK_MELEE)
		{
			skillID = pUserCreature->GetBasicActionInfo();
		}

		DWORD delayFrame = 0;

		pFakeCreature->SetFakeCreatureFastMoveAction(
			pUserCreature->GetX() + TargetList[i].x, pUserCreature->GetY() + TargetList[i].y,
			skillID,
			pTargetCreature->GetID() );
	}
}

//-----------------------------------------------------------------------------
// SkillToSector - 섹터 대상 스킬 이펙트
//-----------------------------------------------------------------------------
void SkillToSector(TYPE_ACTIONINFO nActionInfo, TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, int dir, DWORD delayFrame, MActionResult* pResult)
{
	(void)nActionInfo; (void)sX; (void)sY; (void)dir; (void)delayFrame; (void)pResult;
	// 스텁 구현 - 섹터 대상 스킬
}

//-----------------------------------------------------------------------------
// SetPCSlayerInfo - PC 슬레이어 정보 설정
//-----------------------------------------------------------------------------
void SetPCSlayerInfo(PCSlayerInfo2* pInfo)
{
	if (pInfo == NULL || g_pPlayer == NULL) return;

	DEBUG_ADD( "Set Slayer Info" );

	g_pPlayer->SetID( pInfo->getObjectID() );
	g_pPlayer->SetSight( pInfo->getSight() );

	//--------------------------------------------------
	// Player 몸 설정 - 중요: 크리처 타입을 설정해야 한다!
	//--------------------------------------------------
	if (pInfo->getCompetence()==0)
	{
		g_pPlayer->SetCompetence( 0 );
		g_pPlayer->SetCreatureType( CREATURETYPE_SLAYER_OPERATOR );
		g_pPlayer->SetMale( pInfo->getSex()==MALE );
	}
	else
	{
		g_pPlayer->SetCreatureType( (pInfo->getSex()==MALE)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE );
	}

	g_pPlayer->SetBodyColor1( pInfo->getSkinColor() );
	g_pPlayer->SetMasterEffectType( pInfo->getMasterEffectColor() );
	SetAddonToSlayer( g_pPlayer, pInfo );

	// TODO: PacketFunction.cpp:2691-2953 의 나머지 구현 추가 필요
	// 길드 정보, 랭크, 스탯, 도메인 레벨 등 포함
}

//-----------------------------------------------------------------------------
// AddItemToZone - 존에 아이템 추가
//-----------------------------------------------------------------------------
MItem* AddItemToZone(GCAddItemToZone* pPacket, BOOL bDropping)
{
	(void)pPacket; (void)bDropping;
	// 스텁 구현 - 존에 아이템 추가
	return NULL;
}

//-----------------------------------------------------------------------------
// g_PrintColorStr - 컬러 문자열 출력 (UI 설명용)
//-----------------------------------------------------------------------------
int g_PrintColorStr(int x, int y, const char* str, PrintInfo& info, DWORD color)
{
	(void)x; (void)y; (void)str; (void)info; (void)color;
	// 스텁 구현 - 컬러 문자열 출력
	return 0;
}

//-----------------------------------------------------------------------------
// SetAddonToOusters - 아우스터즈 애드온 정보 설정 (PCOustersInfo2)
//-----------------------------------------------------------------------------
void SetAddonToOusters(MCreatureWear* pCreature, const PCOustersInfo2* pInfo)
{
	(void)pCreature; (void)pInfo;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// SetAddonToOusters - 아우스터즈 애드온 정보 설정 (PCOustersInfo3)
//-----------------------------------------------------------------------------
void SetAddonToOusters(MCreatureWear* pCreature, const PCOustersInfo3* pInfo)
{
	if (pCreature == NULL || pInfo == NULL)
		return;

	int coatType = pInfo->getCoatType();
	int armType = pInfo->getArmType();

	// g_pPacketItemOustersCoat 배열이 제대로 초기화되었는지 확인
	if (coatType < 0 || coatType >= OUSTERS_COAT_MAX) {
		fprintf(stderr, "ERROR: SetAddonToOusters: coatType=%d is out of range [0, %d)\n", coatType, OUSTERS_COAT_MAX);
		return;
	}
	if (g_pPacketItemOustersCoat[coatType] == NULL) {
		fprintf(stderr, "ERROR: SetAddonToOusters: g_pPacketItemOustersCoat[%d] is NULL! InitPacketItemTable() may not have been called.\n", coatType);
		return;
	}

	MItem* pCoat = g_pPacketItemOustersCoat[coatType];
	MItem* pArm = g_pPacketItemOustersArm[armType];

	pCreature->SetAddonItem( pCoat );
	pCreature->SetAddonItem( pArm );

	pCreature->SetAddonColorSet1( ADDON_COAT, pInfo->getHairColor() );
	pCreature->SetAddonColorSet2( ADDON_COAT, pInfo->getCoatColor() );
	pCreature->SetAddonColorSet1( ADDON_TROUSER, pInfo->getBootsColor() );
	pCreature->SetAddonColorSet2( ADDON_TROUSER, pInfo->getBootsColor() );
	pCreature->SetAddonColorSet1( ADDON_RIGHTHAND, pInfo->getArmColor() );
	pCreature->SetAddonColorSet2( ADDON_RIGHTHAND, pInfo->getArmColor() );

	pCreature->SetWeaponSpeed( pInfo->getAttackSpeed() );
}

//-----------------------------------------------------------------------------
// 패킷 핸들러에 필요한 추가 함수들
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AffectModifyInfo - 상태에 변경 정보 적용
//-----------------------------------------------------------------------------
void AffectModifyInfo(MStatus* pStatus, ModifyInfo* pInfo)
{
	(void)pStatus; (void)pInfo;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// SetAddonToSlayer - 슬레이어 애드온 설정 (PCSlayerInfo2)
//-----------------------------------------------------------------------------
void SetAddonToSlayer(MCreatureWear* pCreature, const PCSlayerInfo2* pInfo)
{
	if (pCreature == NULL || pInfo == NULL) return;

	//--------------------------------------------------
	// 머리 설정
	//--------------------------------------------------
	if (pInfo->getSex()==MALE)
	{
		pCreature->SetAddonHair(g_PacketHairMaleID[(int)pInfo->getHairStyle()], pInfo->getHairColor());
	}
	else
	{
		pCreature->SetAddonHair(g_PacketHairFemaleID[(int)pInfo->getHairStyle()], pInfo->getHairColor());
	}

	if(pInfo->getAdvancementLevel()>0)
		pCreature->SetAddonColorSet1( ADDON_COAT, pInfo->getHairColor() );
	else
		pCreature->SetAddonColorSet1( ADDON_COAT, pInfo->getSkinColor() );

	pCreature->SetAddonColorSet1( ADDON_TROUSER, pInfo->getSkinColor() );
}

//-----------------------------------------------------------------------------
// SetAddonToSlayer - 슬레이어 애드온 설정 (PCSlayerInfo3)
//-----------------------------------------------------------------------------
void SetAddonToSlayer(MCreatureWear* pCreature, const PCSlayerInfo3* pInfo)
{
	(void)pCreature; (void)pInfo;
	// 스텁 구현 - TODO: PacketFunction.cpp:856-1150 의 전체 구현 추가 필요
}

//-----------------------------------------------------------------------------
// SetAddonToSlayer - 슬레이어 애드온 설정 (int npcID)
//-----------------------------------------------------------------------------
void SetAddonToSlayer(MCreatureWear* pCreature, int npcID)
{
	(void)pCreature; (void)npcID;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// SetInventoryInfo - 인벤토리 정보 설정
//-----------------------------------------------------------------------------
void SetInventoryInfo(InventoryInfo* pInventoryInfo)
{
	(void)pInventoryInfo;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// SetPCOustersInfo - PC 아우스터즈 정보 설정
//-----------------------------------------------------------------------------
void SetPCOustersInfo(PCOustersInfo2* pInfo)
{
	if (pInfo == NULL || g_pPlayer == NULL) return;

	DEBUG_ADD( "Set Ousters Info" );

	g_pPlayer->SetID( pInfo->getObjectID() );
	g_pPlayer->SetSight( pInfo->getSight() );

	//--------------------------------------------------
	// Player 몸 설정 - 중요: 크리처 타입을 설정해야 한다!
	//--------------------------------------------------
	if (pInfo->getCompetence()==0)
	{
		g_pPlayer->SetCompetence( 0 );
		g_pPlayer->SetCreatureType( CREATURETYPE_OUSTERS_OPERATOR );
		g_pPlayer->SetMale( pInfo->getSex()==MALE );
	}
	else
	{
		g_pPlayer->SetCreatureType( CREATURETYPE_OUSTERS );
	}

	g_pPlayer->SetGroundCreature();
	g_pPlayer->SetBodyColor1( pInfo->getHairColor() );
	g_pPlayer->SetMasterEffectType( pInfo->getMasterEffectColor() );

	// TODO: PacketFunction.cpp:3253-3370 의 나머지 구현 추가 필요
}

//-----------------------------------------------------------------------------
// SetPCVampireInfo - PC 뱀파이어 정보 설정
//-----------------------------------------------------------------------------
void SetPCVampireInfo(PCVampireInfo2* pInfo)
{
	if (pInfo == NULL || g_pPlayer == NULL) return;

	DEBUG_ADD( "Set Vampire Info" );

	g_pPlayer->SetID( pInfo->getObjectID() );
	g_pPlayer->SetSight( pInfo->getSight() );

	//--------------------------------------------------
	// Player 몸 설정 - 중요: 크리처 타입을 설정해야 한다!
	//--------------------------------------------------
	// 박쥐나 늑대로 변신 중인 경우..
	if (g_PreviousCreatureType==CREATURETYPE_WOLF
		|| g_PreviousCreatureType==CREATURETYPE_BAT
		|| g_PreviousCreatureType==CREATURETYPE_WER_WOLF)
	{
		g_pPlayer->SetCreatureType( g_PreviousCreatureType );
		if( pInfo->getCompetence() == 0 )
			g_pPlayer->SetCompetence( 0 );
	}
	else
	{
		if (pInfo->getCompetence()==0)
		{
			g_pPlayer->SetCompetence( 0 );
			g_pPlayer->SetCreatureType( CREATURETYPE_VAMPIRE_OPERATOR );
			g_pPlayer->SetMale( pInfo->getSex()==MALE );
		}
		else
		{
			int creatureType = (pInfo->getSex()==MALE)? CREATURETYPE_VAMPIRE_MALE1 : CREATURETYPE_VAMPIRE_FEMALE1;
			g_pPlayer->SetCreatureType( creatureType );
		}
	}

	g_pPlayer->SetMale( pInfo->getSex()==MALE );

	// 지상, 공중 이동 결정
	if (g_pPlayer->GetCreatureType()==CREATURETYPE_BAT)
	{
		g_pPlayer->SetFlyingCreature();
	}
	else
	{
		g_pPlayer->SetGroundCreature();
	}

	g_pPlayer->SetBodyColor1( pInfo->getSkinColor() );
	g_pPlayer->SetMasterEffectType( pInfo->getMasterEffectColor() );

	if( g_pPlayer->GetCreatureType() == CREATURETYPE_WER_WOLF )
		g_pPlayer->SetBodyColor1( 377 );

	if( pInfo->getBatColor() != 0 )
		g_pPlayer->SetBatColor( pInfo->getBatColor() );
	else
		g_pPlayer->SetBatColor( 0xFFFF );

	// TODO: PacketFunction.cpp:2954-3252 의 나머지 구현 추가 필요
}

//-----------------------------------------------------------------------------
// SkillCrossCounter - 크로스 카운터 스킬 이펙트
//-----------------------------------------------------------------------------
void SkillCrossCounter(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID)
{
	(void)pUserCreature; (void)pTargetCreature; (void)skillID;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// SkillShadowDancing - 섀도우 댄싱 스킬 이펙트
//-----------------------------------------------------------------------------
void SkillShadowDancing(MCreature* pUserCreature, MCreature* pTargetCreature, int type)
{
	(void)pUserCreature; (void)pTargetCreature; (void)type;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// g_PrintColorStrOut - 테두리가 있는 컬러 문자열 출력
//-----------------------------------------------------------------------------
int g_PrintColorStrOut(int x, int y, const char* str, PrintInfo& info, DWORD color1, DWORD color2)
{
	(void)x; (void)y; (void)str; (void)info; (void)color1; (void)color2;
	// 스텁 구현
	return 0;
}

//-----------------------------------------------------------------------------
// AddOustersElemental - 아우스터즈 원소 이펙트 추가
//-----------------------------------------------------------------------------
void AddOustersElemental(MCreature* pCreature, int elementalType, int count)
{
	(void)pCreature; (void)elementalType; (void)count;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// CheckItemForSkillIcon - 아이템이 스킬 아이콘을 표시해야 하는지 확인
//-----------------------------------------------------------------------------
bool CheckItemForSkillIcon(const MItem* pItem)
{
	(void)pItem;
	// 스텁 구현
	return false;
}

//-----------------------------------------------------------------------------
// 추가로 필요한 함수들
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// g_PrintColorStrShadow - 그림자가 있는 컬러 문자열 출력
//-----------------------------------------------------------------------------
int g_PrintColorStrShadow(int x, int y, const char* str, PrintInfo& info, DWORD color1, DWORD color2)
{
	(void)x; (void)y; (void)str; (void)info; (void)color1; (void)color2;
	// 스텁 구현
	return 0;
}

//-----------------------------------------------------------------------------
// CreateActionResultNode - 액션 결과 노드 생성
//-----------------------------------------------------------------------------
void CreateActionResultNode(MCreature* pCreature, int type, BYTE count)
{
	(void)pCreature; (void)type; (void)count;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// 게임에 필요한 전역 변수
//-----------------------------------------------------------------------------

// 마우스 위치 전역 변수 (VS_UI 선언과 일치하도록 언더스코어 접두어 없음)
extern "C" {
	int g_mouse_x = 0;
	int g_mouse_y = 0;
}

// 아우스터즈 패킷 아이템 전역 변수는 PacketFunction.cpp 에 정의되어 있다.
// MItem* g_pPacketItemOustersArm[OUSTERS_ARM_MAX] = {NULL};
// MItem* g_pPacketItemOustersCoat[OUSTERS_COAT_MAX] = {NULL};

//-----------------------------------------------------------------------------
// 패킷 아이템 테이블 전역 변수 (PacketFunction.cpp 에서)
// 참고: PacketFunction.cpp 는 Windows가 아닌 플랫폼에서는 제외된다.
//-----------------------------------------------------------------------------
MItem*		g_pPacketItemHelm[HELMET_MAX] = { NULL };
MItem*		g_pPacketItemJacket[JACKET_MAX] = { NULL };
MItem*		g_pPacketItemPants[PANTS_MAX] = { NULL };
MItem*		g_pPacketItemWeapon[WEAPON_MAX] = { NULL };
MItem*		g_pPacketItemShield[SHIELD_MAX] = { NULL };
MItem*		g_pPacketItemMotorcycle[MOTORCYCLE_MAX] = { NULL };
MItem*		g_pPacketItemShoulder[SHOULDER_MAX] = { NULL };

MItem*		g_pPacketItemOustersCoat[OUSTERS_COAT_MAX] = { NULL };
MItem*		g_pPacketItemOustersArm[OUSTERS_ARM_MAX] = { NULL };

// 웨이브 팩 파일 매니저 (언더스코어 접두어 없음)
class CWavePackFileManager;
CWavePackFileManager* g_pWavePackFileManager = NULL;

// 정보 표시 전역 변수 (언더스코어 접두어 없음)
extern "C" {
	bool gbl_info_show = true;
	bool gbl_show_item = true;
}

//-----------------------------------------------------------------------------
// GameFunctions.cpp 에서 여전히 필요한 함수들
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SetGoreLevel - 고어 레벨 설정
//-----------------------------------------------------------------------------
void SetGoreLevel(bool bGoreLevel)
{
	(void)bGoreLevel;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// GetNMClipData - 넷마블 클립보드 데이터 가져오기
//-----------------------------------------------------------------------------
bool GetNMClipData(char* pBuffer, unsigned int bufferSize, const char* pURL, bool bUseHTML)
{
	(void)pBuffer; (void)bufferSize; (void)pURL; (void)bUseHTML;
	// 스텁 구현
	return false;
}

//-----------------------------------------------------------------------------
// SendBugReport - 버그 리포트 전송
//-----------------------------------------------------------------------------
void SendBugReport(const char* pFormat, ...)
{
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// SendCrashReport - 크래시 리포트 전송
//-----------------------------------------------------------------------------
void SendCrashReport()
{
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// AddVampirePortal - 뱀파이어 포탈 추가
//-----------------------------------------------------------------------------
void AddVampirePortal(int serverID, const char* ownerName, TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, DWORD delayFrame, int zoneID, TYPE_SECTORPOSITION zoneX, TYPE_SECTORPOSITION zoneY, bool bStartFromMainNode)
{
	(void)serverID; (void)ownerName; (void)sX; (void)sY; (void)delayFrame; (void)zoneID; (void)zoneX; (void)zoneY; (void)bStartFromMainNode;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// ConvertAlignment - 얼라인먼트 값 변환
//-----------------------------------------------------------------------------
int ConvertAlignment(int alignment)
{
	// 스텁 구현
	return alignment;
}

//-----------------------------------------------------------------------------
// 추가로 필요한 함수들
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SetDragonTorando - 드래곤 토네이도 이펙트 설정
//-----------------------------------------------------------------------------
void SetDragonTorando(int type, DWORD color, int count, int delay)
{
	(void)type; (void)color; (void)count; (void)delay;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// GetBombActionInfo - 폭탄 액션 정보 가져오기
//-----------------------------------------------------------------------------
int GetBombActionInfo(int itemType)
{
	(void)itemType;
	// 스텁 구현
	return 0;
}

//-----------------------------------------------------------------------------
// GetMineActionInfo - 지뢰 액션 정보 가져오기
//-----------------------------------------------------------------------------
int GetMineActionInfo(int itemType)
{
	(void)itemType;
	// 스텁 구현
	return 0;
}

//-----------------------------------------------------------------------------
// PopupErrorMessage - 오류 메시지 팝업
//-----------------------------------------------------------------------------
void PopupErrorMessage(ErrorID errorID)
{
	(void)errorID;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// CheckMacScreenMode - Mac 화면 모드 확인
//-----------------------------------------------------------------------------
bool CheckMacScreenMode()
{
	// 스텁 구현
	return false;
}

//-----------------------------------------------------------------------------
// Send_nProtect_Auth - nProtect 인증 전송
//-----------------------------------------------------------------------------
void Send_nProtect_Auth(DWORD authCode)
{
	(void)authCode;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// SetServerGroupName - 서버 그룹 이름 설정
//-----------------------------------------------------------------------------
void SetServerGroupName(const char* pName)
{
	(void)pName;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// 그 밖에 필요한 함수들
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// InitPacketItemTable - 패킷 아이템 테이블 초기화
// 참고: Windows가 아닌 플랫폼에서 제외되는 PacketFunction.cpp 에서 복사됨
//-----------------------------------------------------------------------------
void InitPacketItemTable()
{
	//------------------------------------------------------------
	// MOustersCoat
	//------------------------------------------------------------
	MOustersCoat* pOustersCoatBasic = new MOustersCoat;	 pOustersCoatBasic->SetItemType( 0 ); pOustersCoatBasic->ClearItemOption();
	MOustersCoat* pOustersCoat1 = new MOustersCoat;	 pOustersCoat1->SetItemType( 0 ); pOustersCoat1->ClearItemOption();
	MOustersCoat* pOustersCoat2 = new MOustersCoat;	 pOustersCoat2->SetItemType( 3 ); pOustersCoat2->ClearItemOption();
	MOustersCoat* pOustersCoat3 = new MOustersCoat;	 pOustersCoat3->SetItemType( 6 ); pOustersCoat3->ClearItemOption();
	MOustersCoat* pOustersCoat4 = new MOustersCoat;	 pOustersCoat4->SetItemType( 6 ); pOustersCoat4->ClearItemOption();

	g_pPacketItemOustersCoat[OUSTERS_COAT_BASIC] = pOustersCoatBasic;
	g_pPacketItemOustersCoat[OUSTERS_COAT1] = pOustersCoat1;
	g_pPacketItemOustersCoat[OUSTERS_COAT2] = pOustersCoat2;
	g_pPacketItemOustersCoat[OUSTERS_COAT3] = pOustersCoat3;
	g_pPacketItemOustersCoat[OUSTERS_COAT4] = pOustersCoat4;

	//------------------------------------------------------------
	// MOustersArm
	//------------------------------------------------------------
	MOustersWristlet* pOustersArm1 = new MOustersWristlet;	 pOustersArm1->SetItemType( 0 ); pOustersArm1->ClearItemOption();
	MOustersChakram* pOustersArm2 = new MOustersChakram;	 pOustersArm2->SetItemType( 0 ); pOustersArm2->ClearItemOption();

	g_pPacketItemOustersArm[OUSTERS_ARM_GAUNTLET] = pOustersArm1;
	g_pPacketItemOustersArm[OUSTERS_ARM_CHAKRAM] = pOustersArm2;

	//------------------------------------------------------------
	// HELM
	//------------------------------------------------------------
	MHelm* pHelm1 = new MHelm;	 pHelm1->SetItemType( 0 ); pHelm1->ClearItemOption();
	MHelm* pHelm2 = new MHelm;	 pHelm2->SetItemType( 3 ); pHelm2->ClearItemOption();
	MHelm* pHelm3 = new MHelm;	 pHelm3->SetItemType( 15 ); pHelm3->ClearItemOption();
	MHelm* pHelm4 = new MHelm;	 pHelm4->SetItemType( 16 ); pHelm4->ClearItemOption();
	MHelm* pHelm5 = new MHelm;	 pHelm5->SetItemType( 17 ); pHelm5->ClearItemOption();

	g_pPacketItemHelm[HELMET_NONE]	= NULL;
	g_pPacketItemHelm[HELMET1]		= pHelm1;
	g_pPacketItemHelm[HELMET2]		= pHelm2;
	g_pPacketItemHelm[HELMET3]		= pHelm3;
	g_pPacketItemHelm[HELMET4]		= pHelm4;
	g_pPacketItemHelm[HELMET5]		= pHelm5;

	//------------------------------------------------------------
	// COAT
	//------------------------------------------------------------
	MCoat* pCoat1 = new MCoat;	pCoat1->SetItemType( 0 );	pCoat1->ClearItemOption();
	MCoat* pCoat2 = new MCoat;	pCoat2->SetItemType( 4 );	pCoat2->ClearItemOption();
	MCoat* pCoat3 = new MCoat;	pCoat3->SetItemType( 8 );	pCoat3->ClearItemOption();
	MCoat* pCoat4 = new MCoat;	pCoat4->SetItemType( 28 );	pCoat4->ClearItemOption();
	MCoat* pCoat5 = new MCoat;	pCoat5->SetItemType( 30 );	pCoat5->ClearItemOption();
	MCoat* pCoat6 = new MCoat;	pCoat6->SetItemType( 32 );	pCoat6->ClearItemOption();

	g_pPacketItemJacket[JACKET_BASIC]	= NULL;
	g_pPacketItemJacket[JACKET1]		= pCoat1;
	g_pPacketItemJacket[JACKET2]		= pCoat2;
	g_pPacketItemJacket[JACKET3]		= pCoat3;
	g_pPacketItemJacket[JACKET4]		= pCoat4;
	g_pPacketItemJacket[JACKET5]		= pCoat5;
	g_pPacketItemJacket[JACKET6]		= pCoat6;

	//------------------------------------------------------------
	// TROUSER
	//------------------------------------------------------------
	MTrouser* pTrouser1 = new MTrouser;		pTrouser1->SetItemType( 0 ); pTrouser1->ClearItemOption();
	MTrouser* pTrouser2 = new MTrouser;		pTrouser2->SetItemType( 4 ); pTrouser2->ClearItemOption();
	MTrouser* pTrouser3 = new MTrouser;		pTrouser3->SetItemType( 8 ); pTrouser3->ClearItemOption();

	g_pPacketItemPants[PANTS_BASIC]		= NULL;
	g_pPacketItemPants[PANTS1]			= pTrouser1;
	g_pPacketItemPants[PANTS2]			= pTrouser2;
	g_pPacketItemPants[PANTS3]			= pTrouser3;

	//------------------------------------------------------------
	// WEAPON
	//------------------------------------------------------------
	MSword*	pSword = new MSword;	pSword->SetItemType( 0 );	pSword->ClearItemOption();
	MSword*	pSword1 = new MSword;	pSword1->SetItemType( 16 );	pSword1->ClearItemOption();

	MBlade*	pBlade = new MBlade;	pBlade->SetItemType( 0 );	pBlade->ClearItemOption();
	MBlade*	pBlade1 = new MBlade;	pBlade1->SetItemType( 16 );	pBlade1->ClearItemOption();

	MGunAR*	pGunAR = new MGunAR;	pGunAR->SetItemType( 0 );	pGunAR->ClearItemOption();
	MGunAR*	pGunAR1 = new MGunAR;	pGunAR1->SetItemType( 14 );	pGunAR1->ClearItemOption();
	MGunAR*	pGunAR2 = new MGunAR;	pGunAR2->SetItemType( 15 );	pGunAR2->ClearItemOption();
	MGunAR*	pGunAR3 = new MGunAR;	pGunAR3->SetItemType( 16 );	pGunAR3->ClearItemOption();

	MGunTR*	pGunSR = new MGunTR;	pGunSR->SetItemType( 0 );	pGunSR->ClearItemOption();
	MGunTR*	pGunSR1 = new MGunTR;	pGunSR1->SetItemType( 14 );	pGunSR1->ClearItemOption();
	MGunTR*	pGunSR2 = new MGunTR;	pGunSR2->SetItemType( 15 );	pGunSR2->ClearItemOption();
	MGunTR*	pGunSR3 = new MGunTR;	pGunSR3->SetItemType( 16 );	pGunSR3->ClearItemOption();

	MGunSG*	pGunSG = new MGunSG;	pGunSG->SetItemType( 0 );	pGunSG->ClearItemOption();

	MGunSMG*pGunSMG = new MGunSMG;	pGunSMG->SetItemType( 0 );	pGunSMG->ClearItemOption();

	MCross*	pCross = new MCross;	pCross->SetItemType( 0 );	pCross->ClearItemOption();
	MCross*	pCross1 = new MCross;	pCross1->SetItemType( 14 );	pCross1->ClearItemOption();

	MMace*  pMace  = new MMace;     pMace->SetItemType( 0 );	pMace->ClearItemOption();
	MMace*  pMace1  = new MMace;     pMace1->SetItemType( 14 );	pMace1->ClearItemOption();

	g_pPacketItemWeapon[WEAPON_NONE]			= NULL;
	g_pPacketItemWeapon[WEAPON_SWORD]			= pSword;
	g_pPacketItemWeapon[WEAPON_SWORD1]			= pSword1;

	g_pPacketItemWeapon[WEAPON_BLADE]			= pBlade;
	g_pPacketItemWeapon[WEAPON_BLADE1]			= pBlade1;

	g_pPacketItemWeapon[WEAPON_AR]				= pGunAR;
	g_pPacketItemWeapon[WEAPON_AR1]				= pGunAR1;
	g_pPacketItemWeapon[WEAPON_AR2]				= pGunAR2;
	g_pPacketItemWeapon[WEAPON_AR3]				= pGunAR3;

	g_pPacketItemWeapon[WEAPON_SR]				= pGunSR;
	g_pPacketItemWeapon[WEAPON_SR1]				= pGunSR1;
	g_pPacketItemWeapon[WEAPON_SR2]				= pGunSR2;
	g_pPacketItemWeapon[WEAPON_SR3]				= pGunSR3;

	g_pPacketItemWeapon[WEAPON_SG]				= pGunSG;
	g_pPacketItemWeapon[WEAPON_SMG]				= pGunSMG;

	g_pPacketItemWeapon[WEAPON_CROSS]			= pCross;
	g_pPacketItemWeapon[WEAPON_CROSS1]			= pCross1;

	g_pPacketItemWeapon[WEAPON_MACE]			= pMace;
	g_pPacketItemWeapon[WEAPON_MACE1]			= pMace1;

	//------------------------------------------------------------
	// SHIELD
	//------------------------------------------------------------
	MShield* pShield1 = new MShield; pShield1->SetItemType( 0 ); pShield1->ClearItemOption();
	MShield* pShield2 = new MShield; pShield2->SetItemType( 5 ); pShield2->ClearItemOption();
	MShield* pShield3 = new MShield; pShield3->SetItemType( 15 ); pShield3->ClearItemOption();
	MShield* pShield4 = new MShield; pShield4->SetItemType( 17 ); pShield4->ClearItemOption();

	g_pPacketItemShield[SHIELD_NONE] = NULL;
	g_pPacketItemShield[SHIELD1] = pShield1;
	g_pPacketItemShield[SHIELD2] = pShield2;
	g_pPacketItemShield[SHIELD3] = pShield3;
	g_pPacketItemShield[SHIELD4] = pShield4;

	//------------------------------------------------------------
	// motorcycle
	//------------------------------------------------------------
	MMotorcycle* pMotor1 = new MMotorcycle;	pMotor1->SetItemType( 0 ); pMotor1->ClearItemOption();
	MMotorcycle* pMotor2 = new MMotorcycle;	pMotor2->SetItemType( 6 ); pMotor2->ClearItemOption();
	MMotorcycle* pMotor3 = new MMotorcycle;	pMotor3->SetItemType( 7 ); pMotor3->ClearItemOption();

	g_pPacketItemMotorcycle[MOTORCYCLE_NONE]	= NULL;
	g_pPacketItemMotorcycle[MOTORCYCLE1]		= pMotor1;
	g_pPacketItemMotorcycle[MOTORCYCLE2]		= pMotor2;
	g_pPacketItemMotorcycle[MOTORCYCLE3]		= pMotor3;

	//------------------------------------------------------------
	// shoulder
	//------------------------------------------------------------
	MShoulderArmor* pShoulder1 = new MShoulderArmor;	pShoulder1->SetItemType( 0 ); pShoulder1->ClearItemOption();

	g_pPacketItemShoulder[SHOULDER_NONE]	= NULL;
	g_pPacketItemShoulder[SHOULDER1]		= pShoulder1;
}

//-----------------------------------------------------------------------------
// SendUserIDToGameMonA - GameMon 에 유저 ID 전송
//-----------------------------------------------------------------------------
void SendUserIDToGameMonA(const char* pUserID)
{
	(void)pUserID;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// ConvertDurationToFrame - 지속시간을 프레임으로 변환
//-----------------------------------------------------------------------------
DWORD ConvertDurationToFrame(int duration)
{
	// 스텁 구현
	return duration / 30;  // 30 FPS 로 가정
}

//-----------------------------------------------------------------------------
// GetDirectionToPosition - 위치까지의 방향 가져오기
//-----------------------------------------------------------------------------
BYTE GetDirectionToPosition(int originX, int originY, int destX, int destY)
{
	// 스텁 구현
	return 0;
}

//-----------------------------------------------------------------------------
// 마지막으로 필요한 함수들
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GetOustersCreatureType - 아우스터즈 크리처 타입 가져오기
//-----------------------------------------------------------------------------
int GetOustersCreatureType(int coatType)
{
	// 스텁 구현
	return coatType;
}

//-----------------------------------------------------------------------------
// GetVampireCreatureType - 뱀파이어 크리처 타입 가져오기
//-----------------------------------------------------------------------------
int GetVampireCreatureType(int level, bool bFemale, int rank)
{
	// 스텁 구현
	(void)level; (void)bFemale; (void)rank;
	return 0;
}

//-----------------------------------------------------------------------------
// ReleasePacketItemTable - 패킷 아이템 테이블 해제
//-----------------------------------------------------------------------------
void ReleasePacketItemTable()
{
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// UseItemOK - 아이템 사용 가능 여부
//-----------------------------------------------------------------------------
bool UseItemOK()
{
	// 스텁 구현
	return true;
}

//-----------------------------------------------------------------------------
// SetFlagTo - 플래그 설정
//-----------------------------------------------------------------------------
void SetFlagTo(bool bFlag)
{
	(void)bFlag;
	// 스텁 구현
}

//-----------------------------------------------------------------------------
// 변환 유틸리티 함수
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ConvertMillisecondToFrame - 밀리초를 프레임으로 변환
//-----------------------------------------------------------------------------
DWORD ConvertMillisecondToFrame(unsigned long ms)
{
	// 스텁 구현 - 30 FPS 로 가정
	return ms / 33;
}

//-----------------------------------------------------------------------------
// ConvertDurationToMillisecond - 지속시간을 밀리초로 변환
//-----------------------------------------------------------------------------
DWORD ConvertDurationToMillisecond(int duration)
{
	// 스텁 구현 - duration 은 프레임 단위로 가정
	return duration * 33;  // 밀리초로 변환 (30 FPS 가정)
}

//-----------------------------------------------------------------------------
// Windows 전용 함수 (macOS 용 스텁)
//-----------------------------------------------------------------------------
