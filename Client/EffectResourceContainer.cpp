//----------------------------------------------------------------------
// EffectResourceContainer.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "EffectResourceContainer.h"
#include <iostream>
#include <stdio.h>

//----------------------------------------------------------------------
// 생성자/소멸자
//----------------------------------------------------------------------
EffectResourceContainer::EffectResourceContainer()
	: m_loaded(false)
{
}

EffectResourceContainer::~EffectResourceContainer()
{
	// 리소스는 자동으로 해제됨 (RAII)
}

//----------------------------------------------------------------------
// Load - 이펙트 리소스 전체 로드
//----------------------------------------------------------------------
bool EffectResourceContainer::Load(const char* basePath)
{
	if (m_loaded) {
		std::cerr << "[EffectResourceContainer] Already loaded" << std::endl;
		return true;
	}

	std::cout << "[EffectResourceContainer] Loading resources from: " << basePath << std::endl;

	// 1. 프레임 팩 로드
	if (!LoadFramePacks(basePath)) {
		std::cerr << "[EffectResourceContainer] Failed to load frame packs" << std::endl;
		return false;
	}

	// 2. 스프라이트 팩 로드
	if (!LoadSpritePacks(basePath)) {
		std::cerr << "[EffectResourceContainer] Failed to load sprite packs" << std::endl;
		return false;
	}

	// 3. 팔레트 팩 로드
	if (!LoadPalettePacks(basePath)) {
		std::cerr << "[EffectResourceContainer] Failed to load palette packs" << std::endl;
		return false;
	}

	m_loaded = true;
	std::cout << "[EffectResourceContainer] All resources loaded successfully" << std::endl;
	return true;
}

//----------------------------------------------------------------------
// LoadFramePacks - 프레임 애니메이션 팩 로드
//----------------------------------------------------------------------
bool EffectResourceContainer::LoadFramePacks(const char* basePath)
{
	char path[256];
	std::ifstream file;

	// 일반 이펙트 프레임 팩
	snprintf(path, sizeof(path), "%sNormalEffect.efpk", basePath);
	file.open(path, std::ios::binary);
	if (file.is_open()) {
		if (m_EffectNormalFPK.LoadFromFile(file)) {
			std::cout << "  Loaded: " << path << std::endl;
		}
		file.close();
	} else {
		std::cerr << "  Warning: Failed to open " << path << std::endl;
	}

	// 알파 이펙트 프레임 팩
	// 여러 파일명 변형 시도
	bool alphaLoaded = false;
	const char* alphaFilenames[] = {"Effect.efpk", "effect.efpk", "EffectAlpha.efpk"};
	for (int i = 0; i < 3; i++) {
		snprintf(path, sizeof(path), "%s%s", basePath, alphaFilenames[i]);
		file.open(path, std::ios::binary);
		if (file.is_open()) {
			if (m_EffectAlphaFPK.LoadFromFile(file)) {
				std::cout << "  Loaded: " << path << std::endl;
				alphaLoaded = true;
			}
			file.close();
			break;
		}
	}
	if (!alphaLoaded) {
		std::cerr << "  Warning: Failed to open any Alpha Effect FPK" << std::endl;
	}

	// 그림자 이펙트 프레임 팩
	snprintf(path, sizeof(path), "%sShadowEffect.efpk", basePath);
	file.open(path, std::ios::binary);
	if (file.is_open()) {
		if (m_EffectShadowFPK.LoadFromFile(file)) {
			std::cout << "  Loaded: " << path << std::endl;
		}
		file.close();
	} else {
		std::cerr << "  Warning: Failed to open " << path << std::endl;
	}

	// 화면 이펙트 프레임 팩
	snprintf(path, sizeof(path), "%sEffectScreen.efpk", basePath);
	file.open(path, std::ios::binary);
	if (file.is_open()) {
		if (m_EffectScreenFPK.LoadFromFile(file)) {
			std::cout << "  Loaded: " << path << std::endl;
		}
		file.close();
	} else {
		std::cerr << "  Warning: Failed to open " << path << std::endl;
	}

	return true;
}

//----------------------------------------------------------------------
// LoadSpritePacks - 스프라이트 팩 로드
//----------------------------------------------------------------------
bool EffectResourceContainer::LoadSpritePacks(const char* basePath)
{
	char path[256];
	std::ifstream file;

	// 일반 이펙트 스프라이트 팩
	snprintf(path, sizeof(path), "%sEffect.spk", basePath);
	file.open(path, std::ios::binary);
	if (file.is_open()) {
		if (m_EffectNormalSPK.LoadFromFile(file)) {
			std::cout << "  Loaded: " << path << " (" << m_EffectNormalSPK.GetSize() << " sprites)" << std::endl;
		}
		file.close();
	} else {
		std::cerr << "  Warning: Failed to open " << path << std::endl;
	}

	// 알파 이펙트 스프라이트 팩
	bool alphaSpkLoaded = false;
	const char* alphaSpkFilenames[] = {"Effect.aspk", "effect.aspk"};
	for (int i = 0; i < 2; i++) {
		snprintf(path, sizeof(path), "%s%s", basePath, alphaSpkFilenames[i]);
		file.open(path, std::ios::binary);
		if (file.is_open()) {
			if (m_EffectAlphaSPK.LoadFromFile(file)) {
				std::cout << "  Loaded: " << path << " (" << m_EffectAlphaSPK.GetSize() << " sprites)" << std::endl;
				alphaSpkLoaded = true;
			}
			file.close();
			break;
		}
	}
	if (!alphaSpkLoaded) {
		std::cerr << "  Warning: Failed to open any Alpha Effect SPK" << std::endl;
	}

	// 그림자 이펙트 스프라이트 팩
	snprintf(path, sizeof(path), "%sEffect.sspk", basePath);
	file.open(path, std::ios::binary);
	if (file.is_open()) {
		if (m_EffectShadowSPK.LoadFromFile(file)) {
			std::cout << "  Loaded: " << path << " (" << m_EffectShadowSPK.GetSize() << " sprites)" << std::endl;
		}
		file.close();
	} else {
		std::cerr << "  Warning: Failed to open " << path << std::endl;
	}

	// 화면 이펙트 스프라이트 팩
	snprintf(path, sizeof(path), "%sEffectScreen.spk", basePath);
	file.open(path, std::ios::binary);
	if (file.is_open()) {
		if (m_EffectScreenSPK.LoadFromFile(file)) {
			std::cout << "  Loaded: " << path << " (" << m_EffectScreenSPK.GetSize() << " sprites)" << std::endl;
		}
		file.close();
	} else {
		std::cerr << "  Warning: Failed to open " << path << std::endl;
	}

	return true;
}

//----------------------------------------------------------------------
// LoadPalettePacks - 팔레트 팩 로드
//----------------------------------------------------------------------
bool EffectResourceContainer::LoadPalettePacks(const char* basePath)
{
	char path[256];
	std::ifstream file;

	// 알파 이펙트 팔레트 팩
	bool alphaPpkLoaded = false;
	const char* alphaPpkFilenames[] = {"Effect.ppk", "effect.ppk", "EffectAlpha.ppk"};
	for (int i = 0; i < 3; i++) {
		snprintf(path, sizeof(path), "%s%s", basePath, alphaPpkFilenames[i]);
		file.open(path, std::ios::binary);
		if (file.is_open()) {
			if (m_EffectAlphaPPK.LoadFromFile(file)) {
				std::cout << "  Loaded: " << path << std::endl;
				alphaPpkLoaded = true;
			}
			file.close();
			break;
		}
	}
	if (!alphaPpkLoaded) {
		std::cerr << "  Warning: Failed to open any Alpha Effect PPK" << std::endl;
	}

	// 화면 이펙트 팔레트 팩
	snprintf(path, sizeof(path), "%sEffectScreen.ppk", basePath);
	file.open(path, std::ios::binary);
	if (file.is_open()) {
		if (m_EffectScreenPPK.LoadFromFile(file)) {
			std::cout << "  Loaded: " << path << std::endl;
		}
		file.close();
	} else {
		std::cerr << "  Warning: Failed to open " << path << std::endl;
	}

	return true;
}

//----------------------------------------------------------------------
// GetFramePack - 해당하는 프레임 팩 반환
//----------------------------------------------------------------------
CEffectFramePack* EffectResourceContainer::GetFramePack(BLT_TYPE bltType)
{
	switch (bltType) {
		case BLT_NORMAL:
			return &m_EffectNormalFPK;
		case BLT_EFFECT:
			return &m_EffectAlphaFPK;
		case BLT_SHADOW:
			return &m_EffectShadowFPK;
		case BLT_SCREEN:
			return &m_EffectScreenFPK;
		default:
			std::cerr << "[EffectResourceContainer] Invalid BltType: " << bltType << std::endl;
			return nullptr;
	}
}

//----------------------------------------------------------------------
// GetEffectFrame - 이펙트 프레임 안전 반환 (경계 검사 포함)
//----------------------------------------------------------------------
CEffectFrame* EffectResourceContainer::GetEffectFrame(BLT_TYPE bltType, TYPE_FRAMEID type,
													   BYTE direction, BYTE frame)
{
	CEffectFramePack* pack = GetFramePack(bltType);
	if (!pack) {
		return nullptr;
	}

	// 경계 검사
	if (type >= pack->GetSize()) {
		// 로그 넘침 방지를 위해 무음 실패
		return nullptr;
	}

	// 방향 배열 가져오기
	auto& directionArray = (*pack)[type];
	if (direction >= directionArray.GetSize()) {
		return nullptr;
	}

	// 프레임 배열 가져오기
	auto& frameArray = directionArray[direction];
	if (frame >= frameArray.GetSize()) {
		return nullptr;
	}

	return &frameArray[frame];
}

//----------------------------------------------------------------------
// GetSprite - 스프라이트 안전 반환 (경계 검사 포함)
// 참고: BLT_SCREEN은 CSpritePal*를 반환하고 나머지는 CSprite*를 반환한다
//----------------------------------------------------------------------
void* EffectResourceContainer::GetSprite(BLT_TYPE bltType, int spriteID)
{
	switch (bltType) {
		case BLT_NORMAL:
			if (spriteID >= 0 && spriteID < m_EffectNormalSPK.GetSize()) {
				return (void*)&m_EffectNormalSPK[spriteID];
			}
			break;

		case BLT_SHADOW:
			if (spriteID >= 0 && spriteID < m_EffectShadowSPK.GetSize()) {
				return (void*)GetShadowSprite(spriteID);
			}
			break;

		case BLT_SCREEN:
			if (spriteID >= 0 && spriteID < m_EffectScreenSPK.GetSize()) {
				return (void*)&m_EffectScreenSPK[spriteID];
			}
			break;

		case BLT_EFFECT:
			// BLT_EFFECT는 알파 스프라이트를 사용하므로 GetAlphaSprite를 사용해야 한다
			std::cerr << "[EffectResourceContainer] BLT_EFFECT should use GetAlphaSprite" << std::endl;
			return nullptr;

		default:
			break;
	}

	return nullptr;
}

//----------------------------------------------------------------------
// GetAlphaSprite - 알파 스프라이트 반환 (BLT_EFFECT용)
//----------------------------------------------------------------------
CAlphaSpritePal* EffectResourceContainer::GetAlphaSprite(int spriteID)
{
	if (spriteID >= 0 && spriteID < m_EffectAlphaSPK.GetSize()) {
		return &m_EffectAlphaSPK[spriteID];
	}
	return nullptr;
}

//----------------------------------------------------------------------
// GetShadowSprite - 그림자 스프라이트 반환 (BLT_SHADOW용)
//----------------------------------------------------------------------
CShadowSprite* EffectResourceContainer::GetShadowSprite(int spriteID)
{
	if (spriteID >= 0 && spriteID < m_EffectShadowSPK.GetSize()) {
		return &m_EffectShadowSPK[spriteID];
	}
	return nullptr;
}

//----------------------------------------------------------------------
// GetPalette - 팔레트 안전 반환 (경계 검사 포함)
// 단일 MPalette 포인터를 반환한다
//----------------------------------------------------------------------
MPalette* EffectResourceContainer::GetPalette(BLT_TYPE bltType, TYPE_FRAMEID type)
{
	switch (bltType) {
		case BLT_EFFECT:
			if (type >= 0 && type < m_EffectAlphaPPK.GetSize()) {
				return &m_EffectAlphaPPK[type];
			}
			break;

		case BLT_SCREEN:
			if (type >= 0 && type < m_EffectScreenPPK.GetSize()) {
				return &m_EffectScreenPPK[type];
			}
			break;

		default:
			// BLT_NORMAL과 BLT_SHADOW는 팔레트가 필요 없다
			return nullptr;
	}

	return nullptr;
}
