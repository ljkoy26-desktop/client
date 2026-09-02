//----------------------------------------------------------------------
// GameHelpers.h
//
// 게임 헬퍼 함수 전방 선언
//----------------------------------------------------------------------

#ifndef __GAMEHELPERS_H__
#define __GAMEHELPERS_H__

#include "Types.h"
#include "PacketInfo.h"
#include "ModifyInfo.h"
#include "EffectInfo.h"
#include "PetInfo.h"
#include "GearInfo.h"
#include "ExtraInfo.h"
#include "InventoryInfo.h"
#include "PCSlayerInfo2.h"
#include "PCOustersInfo2.h"
#include "PCVampireInfo2.h"
#include "PCSlayerInfo3.h"
#include "PCOustersInfo3.h"
#include "BloodBibleSignInfo.h"
#include "MStatus.h"
#include "MActionResult.h"
#include "GCAddMonster.h"
#include "GCAddItemToZone.h"
#include "VS_UI/src/widget/LineEditor.h"

// 전방 선언
class MCreature;
class MItem;
template<typename T> class COrderedList;

// 클라이언트 모드 열거형
enum CLIENT_MODE {
	CLIENT_MODE_WINDOW,
	CLIENT_MODE_FULLSCREEN,
	CLIENT_MODE_WINDOW_1024,
	CLIENT_MODE_FULLSCREEN_1024
};

// 전역 변수
extern int _NPGAMEMON_SUCCESS;

// 펫 및 정보 함수
void SetPetInfo(PetInfo* pInfo, unsigned int flag);
void SetGearInfo(GearInfo* pGear, BloodBibleSignInfo* pBible);
void SetExtraInfo(ExtraInfo* pInfo);
void SetInventoryInfo(InventoryInfo* pInfo);
void SetBloodBibleSlot(BloodBibleSignInfo* pInfo);
void SetEffectInfo(MCreature* pCreature, EffectInfo* pInfo, int count);

// 플레이어 정보 함수
void SetPCSlayerInfo(PCSlayerInfo2* pInfo);
void SetPCOustersInfo(PCOustersInfo2* pInfo);
void SetPCVampireInfo(PCVampireInfo2* pInfo);

// 어드온 함수
void SetAddonToSlayer(MCreatureWear* pWear, int value);
void SetAddonToSlayer(MCreatureWear* pWear, const PCSlayerInfo3* pInfo);
void SetAddonToOusters(MCreatureWear* pWear, const PCOustersInfo3* pInfo);
void AddOustersElemental(MCreature* pCreature, int type, int value);

// 크리처 함수
void NewFakeCreature(MCreature* pCreature, int type, int flag);
void MoveIntoPortal(MCreature* pCreature);
void ComeFromPortal(MCreature* pCreature);
void Add_Wild_Wolf(MCreature* pCreature, MCreature* pMaster, bool flag);
void Add_RocketRuncher(MCreature* pCreature, MCreature* pTarget);
void Add_Race_SlayerMonster(GCAddMonster* pPacket);
void Add_Race_OustersMonster(GCAddMonster* pPacket);
void AddItemToZone(GCAddItemToZone* pPacket, int layer);

// 스킬 함수
void SkillToSector(WORD type, WORD x, WORD y, int dir, DWORD flag, MActionResult* pResult);
void SkillShadowDancing(MCreature* pCreature, MCreature* pTarget, int type);
void SkillIllendue(MCreature* pCreature, MCreature* pTarget, int type);
void SkillCrossCounter(MCreature* pCreature, MCreature* pTarget, int type);
void SkillBlazeWalk(MCreature* pCreature, MCreature* pTarget, int type, BYTE flag);

// 액션 및 결과 함수
void ExecuteActionInfoFromMainNode(WORD type1, WORD type2, WORD type3, int val1, int val2,
	UINT flag, WORD x1, WORD y1, int dir, DWORD flag2, MActionResult* pResult,
	bool bFlag, int param1, int param2);
void CreateActionResultNode(MCreature* pCreature, int type, BYTE flag);

// 상태 함수
void AffectModifyInfo(MStatus* pStatus, ModifyInfo* pInfo);
void ReleaseUselessCreatureSPKExcept(const COrderedList<int>& list);

// 유틸리티 함수
void CheckItemForSkillIcon(const MItem* pItem);
bool GetMakeItemFitPosition(MItem* pItem, ITEM_CLASS cls, int type, tagPOINT& point);

// 모드 및 표시 함수
void SetMode(CLIENT_MODE mode);
void SetFadeStart(char r, char g, char b, BYTE r2, BYTE g2, BYTE b2, WORD speed);

// 출력 함수
void g_Print(int x, int y, const char* str, PrintInfo* pInfo);
void g_PrintColorStr(int x, int y, const char* str, PrintInfo& info, DWORD color);
void g_PrintColorStrOut(int x, int y, const char* str, PrintInfo& info, DWORD color1, DWORD color2);
void g_PrintColorStrShadow(int x, int y, const char* str, PrintInfo& info, DWORD color1, DWORD color2);

// 예외 핸들러
void g_BasicException(EXCEPTION_CODE code, char* file, char* func, DWORD line);

// 그리기 함수
void FillRect(S_SURFACEINFO* pInfo, Rect* pRect, int color);
void rectangle(S_SURFACEINFO* pInfo, Rect* pRect, int color);

#endif // __GAMEHELPERS_H__
