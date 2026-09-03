//-----------------------------------------------------------------------------
// GameObject.cpp
//-----------------------------------------------------------------------------

#ifndef	__GAMEOBJECT_H__
#define	__GAMEOBJECT_H__

//----------------------------------------------------------------------
// 프로그램 정보
//----------------------------------------------------------------------
#include "ClientConfig.h"
#include "UserOption.h"

//----------------------------------------------------------------------
// 게임 오브젝트
//----------------------------------------------------------------------
#include "MGameTime.h"
#include "MWeather.h"

#include "MObject.h"
#include "MCreature.h"
#include "MCreatureWear.h"
#include "MPlayer.h"
#include "MItem.h"
#include "MImageObject.h"
#include "MAnimationObject.h"
#include "MInteractionObject.h"
#include "MLinearEffect.h"
#include "MParabolaEffect.h"

//----------------------------------------------------------------------
// 게임 정보 테이블
//----------------------------------------------------------------------
#include "MItemTable.h"
#include "MItemOptionTable.h"
#include "MCreatureTable.h"
#include "MInteractionObjectTable.h"
#include "MEffectGeneratorTable.h"
#include "MEffectSpriteTypeTable.h"
#include "MActionInfoTable.h"
#include "MEffectStatusTable.h"
#include "MEffectTarget.h"
#include "MZoneTable.h"
#include "MMusicTable.h"
#include "MSoundTable.h"
#include "CUsingColorSetTable.h"
#include "MShopTemplateTable.h"
#include "MNPCTable.h"

//----------------------------------------------------------------------
// 인터페이스
//----------------------------------------------------------------------
#include "MSlayerGear.h"
#include "MVampireGear.h"
#include "MOustersGear.h"
#include "MInventory.h"
#include "MSkillManager.h"
#include "TalkBox.h"

#endif
