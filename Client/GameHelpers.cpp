//----------------------------------------------------------------------
// GameHelpers.cpp
//
// 게임 헬퍼 함수 스텁 구현
// macOS/Linux 호환성을 위한 플레이스홀더 구현
//----------------------------------------------------------------------

#include "Client_PCH.h"

// 불완전 타입 전방 선언
class MCreature;
class MItem;
class MStatus;
class MActionResult;
template<typename T> class COrderedList;

// 패킷 정보 전방 선언
struct PetInfo;
struct GearInfo;
struct ExtraInfo;
struct InventoryInfo;
struct EffectInfo;
struct BloodBibleSignInfo;
struct ModifyInfo;
struct PCSlayerInfo2;
struct PCOustersInfo2;
struct PCVampireInfo2;
struct PCSlayerInfo3;
struct PCOustersInfo3;
struct GCAddMonster;
struct GCAddItemToZone;
struct PrintInfo;

// 클라이언트 모드 열거형
enum CLIENT_MODE {
	CLIENT_MODE_WINDOW,
	CLIENT_MODE_FULLSCREEN,
	CLIENT_MODE_WINDOW_1024,
	CLIENT_MODE_FULLSCREEN_1024
};

// 참고: DWORD, WORD, BYTE 등은 Platform.h에 정의되어 있다
// 여기서 재정의할 필요 없음

// 전방 선언만 - 타입은 Platform.h에 정의됨
struct Rect;

// MCreatureWear 전방 선언
class MCreatureWear;

// 아이템 클래스 열거형 플레이스홀더
enum ITEM_CLASS { ITEM_CLASS_MAX = 0 };

// _NPGAMEMON_SUCCESS는 nProtectStub.cpp에 정의되어 있다
extern int _NPGAMEMON_SUCCESS;

// 펫 및 정보 함수
void SetPetInfo(PetInfo* pInfo, unsigned int flag)
{
	(void)pInfo; (void)flag;
}

void SetGearInfo(GearInfo* pGear, BloodBibleSignInfo* pBible)
{
	(void)pGear; (void)pBible;
}

void SetExtraInfo(ExtraInfo* pInfo)
{
	(void)pInfo;
}

void SetInventoryInfo(InventoryInfo* pInfo)
{
	(void)pInfo;
}

void SetBloodBibleSlot(BloodBibleSignInfo* pInfo)
{
	(void)pInfo;
}

void SetEffectInfo(MCreature* pCreature, EffectInfo* pInfo, int count)
{
	(void)pCreature; (void)pInfo; (void)count;
}

// 플레이어 정보 함수
void SetPCSlayerInfo(PCSlayerInfo2* pInfo)
{
	(void)pInfo;
}

void SetPCOustersInfo(PCOustersInfo2* pInfo)
{
	(void)pInfo;
}

void SetPCVampireInfo(PCVampireInfo2* pInfo)
{
	(void)pInfo;
}

// 어드온 함수
void SetAddonToSlayer(MCreatureWear* pWear, int value)
{
	(void)pWear; (void)value;
}

void SetAddonToSlayer(MCreatureWear* pWear, const PCSlayerInfo3* pInfo)
{
	(void)pWear; (void)pInfo;
}

void SetAddonToOusters(MCreatureWear* pWear, const PCOustersInfo3* pInfo)
{
	(void)pWear; (void)pInfo;
}

void AddOustersElemental(MCreature* pCreature, int type, int value)
{
	(void)pCreature; (void)value; (void)type;
}

// 크리처 함수
void NewFakeCreature(MCreature* pCreature, int type, int flag)
{
	(void)pCreature; (void)type; (void)flag;
}

void MoveIntoPortal(MCreature* pCreature)
{
	(void)pCreature;
}

void ComeFromPortal(MCreature* pCreature)
{
	(void)pCreature;
}

void Add_Wild_Wolf(MCreature* pCreature, MCreature* pMaster, bool flag)
{
	(void)pCreature; (void)pMaster; (void)flag;
}

void Add_RocketRuncher(MCreature* pCreature, MCreature* pTarget)
{
	(void)pCreature; (void)pTarget;
}

void Add_Race_SlayerMonster(GCAddMonster* pPacket)
{
	(void)pPacket;
}

void Add_Race_OustersMonster(GCAddMonster* pPacket)
{
	(void)pPacket;
}

void AddItemToZone(GCAddItemToZone* pPacket, int layer)
{
	(void)pPacket; (void)layer;
}

// 스킬 함수
void SkillToSector(WORD type, WORD x, WORD y, int dir, DWORD flag, MActionResult* pResult)
{
	(void)type; (void)x; (void)y; (void)dir; (void)flag; (void)pResult;
}

void SkillShadowDancing(MCreature* pCreature, MCreature* pTarget, int type)
{
	(void)pCreature; (void)pTarget; (void)type;
}

void SkillIllendue(MCreature* pCreature, MCreature* pTarget, int type)
{
	(void)pCreature; (void)pTarget; (void)type;
}

void SkillCrossCounter(MCreature* pCreature, MCreature* pTarget, int type)
{
	(void)pCreature; (void)pTarget; (void)type;
}

void SkillBlazeWalk(MCreature* pCreature, MCreature* pTarget, int type, BYTE flag)
{
	(void)pCreature; (void)pTarget; (void)type; (void)flag;
}

// 액션 및 결과 함수
void ExecuteActionInfoFromMainNode(WORD type1, WORD type2, WORD type3, int val1, int val2,
	UINT flag, WORD x1, WORD y1, int dir, DWORD flag2, MActionResult* pResult,
	bool bFlag, int param1, int param2)
{
	(void)type1; (void)type2; (void)type3; (void)val1; (void)val2;
	(void)flag; (void)x1; (void)y1; (void)dir; (void)flag2;
	(void)pResult; (void)bFlag; (void)param1; (void)param2;
}

void CreateActionResultNode(MCreature* pCreature, int type, BYTE flag)
{
	(void)pCreature; (void)type; (void)flag;
}

// 상태 함수
void AffectModifyInfo(MStatus* pStatus, ModifyInfo* pInfo)
{
	(void)pStatus; (void)pInfo;
}

void ReleaseUselessCreatureSPKExcept(const COrderedList<int>& list)
{
	(void)list;
}

// 유틸리티 함수
void CheckItemForSkillIcon(const MItem* pItem)
{
	(void)pItem;
}

bool GetMakeItemFitPosition(MItem* pItem, ITEM_CLASS cls, int type, tagPOINT& point)
{
	(void)pItem; (void)cls; (void)type; (void)point;
	return false;
}

// 모드 및 표시 함수
// SetMode()는 GameMain.cpp에 구현됨 - 중복 심볼 방지를 위해 스텁 제거
// void SetMode(CLIENT_MODE mode)
// {
// 	(void)mode;
// }

void SetFadeStart(char r, char g, char b, BYTE r2, BYTE g2, BYTE b2, WORD speed)
{
	(void)r; (void)g; (void)b; (void)r2; (void)g2; (void)b2; (void)speed;
}

// 출력 함수
void g_Print(int x, int y, const char* str, PrintInfo* pInfo)
{
	(void)x; (void)y; (void)str; (void)pInfo;
}

void g_PrintColorStr(int x, int y, const char* str, PrintInfo& info, DWORD color)
{
	(void)x; (void)y; (void)str; (void)info; (void)color;
}

void g_PrintColorStrOut(int x, int y, const char* str, PrintInfo& info, DWORD color1, DWORD color2)
{
	(void)x; (void)y; (void)str; (void)info; (void)color1; (void)color2;
}

void g_PrintColorStrShadow(int x, int y, const char* str, PrintInfo& info, DWORD color1, DWORD color2)
{
	(void)x; (void)y; (void)str; (void)info; (void)color1; (void)color2;
}

// g_BasicException은 BasicException.cpp에 정의되어 있다
// g_SetNewHandler는 BasicException.cpp에 정의되어 있다

// 그리기 함수
void FillRect(S_SURFACEINFO* pInfo, Rect* pRect, int color)
{
	(void)pInfo; (void)pRect; (void)color;
}

void rectangle(S_SURFACEINFO* pInfo, Rect* pRect, int color)
{
	(void)pInfo; (void)pRect; (void)color;
}
