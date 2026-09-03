//----------------------------------------------------------------------
// MEffectSpriteTypeTable.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MEffectSpriteTypeTable.h"


//----------------------------------------------------------------------
// 전역 변수
//----------------------------------------------------------------------
EFFECTSPRITETYPE_TABLE*			g_pEffectSpriteTypeTable = NULL;
MActionEffectSpriteTypeTable*	g_pActionEffectSpriteTypeTable = NULL;

// 메모리 보호: 손상 감지를 위해 포인터 복사본을 저장한다
// 포인터가 손상된 경우, 섀도우 복사본과 비교하여 감지할 수 있다
#ifdef __SANITIZE_ADDRESS__
EFFECTSPRITETYPE_TABLE* g_pEffectSpriteTypeTable_shadow = NULL;
EFFECTSPRITETYPE_TABLE::TYPE* g_pEffectSpriteTypeTable_m_pTypeInfo_shadow = NULL;
static uint64_t g_pEffectSpriteTypeTable_canary = 0xDEADBEEFCAFEBABEULL;

void validate_effect_sprite_table_pointer(const char* location) {
	if (g_pEffectSpriteTypeTable != g_pEffectSpriteTypeTable_shadow) {
		fprintf(stderr, "[CORRUPTION] g_pEffectSpriteTypeTable corrupted at %s!\n", location);
		fprintf(stderr, "[CORRUPTION] Expected: %p, Got: %p\n",
		        g_pEffectSpriteTypeTable_shadow, g_pEffectSpriteTypeTable);
		// 중단하지 않음 - ASAN이 더 나은 진단으로 크래시를 처리하도록 한다
	}

	// m_pTypeInfo 내부 포인터 손상 여부 확인
	if (g_pEffectSpriteTypeTable != NULL && g_pEffectSpriteTypeTable_shadow != NULL) {
		EFFECTSPRITETYPE_TABLE::TYPE* current_m_pTypeInfo = g_pEffectSpriteTypeTable->GetInternalPointer();
		if (current_m_pTypeInfo != g_pEffectSpriteTypeTable_m_pTypeInfo_shadow) {
			fprintf(stderr, "[CORRUPTION] g_pEffectSpriteTypeTable->m_pTypeInfo corrupted at %s!\n", location);
			fprintf(stderr, "[CORRUPTION] Expected m_pTypeInfo: %p, Got: %p\n",
			        g_pEffectSpriteTypeTable_m_pTypeInfo_shadow, current_m_pTypeInfo);
		}

		// m_pTypeInfo가 해제된 SDL 서피스 메모리 영역을 가리키는지 확인
		// SDL 서피스는 일반적으로 특정 메모리 범위에 할당된다
		uintptr_t ptr_addr = (uintptr_t)current_m_pTypeInfo;
		// 해제된 영역에 있을 수 있는 힙 포인터처럼 보이는지 확인
		if (ptr_addr > 0x1000 && ptr_addr < 0x100000000ULL) {
			// ASAN으로 메모리가 오염(poisoned)되었는지 확인
			if (__asan_address_is_poisoned(current_m_pTypeInfo, sizeof(EFFECTSPRITETYPE_TABLE::TYPE))) {
				fprintf(stderr, "[CORRUPTION] m_pTypeInfo points to poisoned/freed memory at %s!\n", location);
				fprintf(stderr, "[CORRUPTION] m_pTypeInfo=%p\n", current_m_pTypeInfo);
			}
		}
	}

	if (g_pEffectSpriteTypeTable_canary != 0xDEADBEEFCAFEBABEULL) {
		fprintf(stderr, "[CORRUPTION] Canary corrupted at %s!\n", location);
		fprintf(stderr, "[CORRUPTION] Canary value: 0x%llx\n", g_pEffectSpriteTypeTable_canary);
	}
}

#define VALIDATE_TABLE() validate_effect_sprite_table_pointer(__FUNCTION__)
#else
#define VALIDATE_TABLE() do {} while(0)
#endif

//----------------------------------------------------------------------
//
//		FRAME_TYPE
//
//----------------------------------------------------------------------
void		
FRAME_TYPE::SaveToFile(std::ofstream& file)
{
	file.write((const char*)&FrameID, SIZE_FRAMEID);
}

void		
FRAME_TYPE::LoadFromFile(std::ifstream& file)
{
	file.read((char*)&FrameID, SIZE_FRAMEID);
}

//----------------------------------------------------------------------
//
//		ACTION_FRAMEID_ARRAY
//
//----------------------------------------------------------------------
ACTION_FRAMEID_ARRAY::ACTION_FRAMEID_ARRAY()
{
	Init( ACTION_MAX_SLAYER );
}

ACTION_FRAMEID_ARRAY::~ACTION_FRAMEID_ARRAY()
{
}

//----------------------------------------------------------------------
//
//		MActionEffectSpriteTypeTable
//
//----------------------------------------------------------------------
MActionEffectSpriteTypeTable::MActionEffectSpriteTypeTable()
{
}

MActionEffectSpriteTypeTable::~MActionEffectSpriteTypeTable()
{
}

//----------------------------------------------------------------------
//
//		EFFECTSPRITETYPETABLE_INFO
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// 파일에 저장
//----------------------------------------------------------------------
EFFECTSPRITETYPETABLE_INFO::EFFECTSPRITETYPETABLE_INFO()
{
	bPairFrameBack = false;
}

void			
EFFECTSPRITETYPETABLE_INFO::SaveToFile(std::ofstream& file)
{
	BYTE bt = (BYTE)BltType;
	file.write((const char*)&bt, 1);
	file.write((const char*)&FrameID, SIZE_FRAMEID);			

	BYTE flag=0;
	
	flag = ( (RepeatFrame==true)<<1 ) | ( bPairFrameBack == true );

	file.write((const char*)&flag, 1);

	file.write((const char*)&ActionEffectFrameID, SIZE_FRAMEID);
	//file.write((const char*)&PairFrameID, SIZE_FRAMEID);	
	file.write((const char*)&FemaleEffectSpriteType, SIZE_EFFECTSPRITETYPE);		

	//----------------------------------------------------------
	// Pair FrameID 목록
	//----------------------------------------------------------
	BYTE numPair = PairFrameIDList.size();
	file.write((const char*)&numPair, 1);

	FRAMEID_LIST::const_iterator iFrame = PairFrameIDList.begin();

	while (iFrame != PairFrameIDList.end())
	{
		TYPE_FRAMEID frameID = *iFrame;

		file.write((const char*)&frameID, SIZE_FRAMEID);

		iFrame++;
	}
}

//----------------------------------------------------------------------
// 파일에서 불러오기
//----------------------------------------------------------------------
void			
EFFECTSPRITETYPETABLE_INFO::LoadFromFile(std::ifstream& file)
{
	BYTE bt;
	file.read((char*)&bt, 1);
	BltType = (BLT_TYPE)bt;
	file.read((char*)&FrameID, SIZE_FRAMEID);	
	BYTE flag;
	
	file.read((char*)&flag, 1);
	
	RepeatFrame = (flag>>1)&0x1;
	bPairFrameBack = (flag)&0x1;

	file.read((char*)&ActionEffectFrameID, SIZE_FRAMEID);
	//file.read((char*)&PairFrameID, SIZE_FRAMEID);	
	file.read((char*)&FemaleEffectSpriteType, SIZE_EFFECTSPRITETYPE);	

	//----------------------------------------------------------
	// Pair FrameID 목록
	//----------------------------------------------------------
	PairFrameIDList.clear();

	BYTE numPair;	
	file.read((char*)&numPair, 1);

	TYPE_FRAMEID frameID;
	int intNumPair = numPair;
	for (int i=0; i<intNumPair; i++)
	{	
		file.read((char*)&frameID, SIZE_FRAMEID);

		PairFrameIDList.push_back( frameID );
	}
}