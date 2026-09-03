//-----------------------------------------------------------------------------
// MEffectManager.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MEffectManager.h"
#include "MEffect.h"

//-----------------------------------------------------------------------------
//
// 생성자/소멸자
//
//-----------------------------------------------------------------------------
MEffectManager::MEffectManager()
{
}

MEffectManager::~MEffectManager()
{
	Release();
}

//-----------------------------------------------------------------------------
//
// 멤버 함수
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 해제
//-----------------------------------------------------------------------------
void		
MEffectManager::Release()
{
	EFFECT_LIST::iterator iEffect = m_listEffect.begin();

	// 다 지운당..
	while (iEffect != m_listEffect.end())
	{
		MEffect* pEffect = *iEffect;

		delete pEffect;

		iEffect ++;
	}

	m_listEffect.clear();
}

//-----------------------------------------------------------------------------
// 이펙트 추가
//-----------------------------------------------------------------------------
void		
MEffectManager::AddEffect(MEffect* pEffect)
{
	if (pEffect != NULL)
	{
		// list에 추가
		m_listEffect.push_front( pEffect );
	}
}

//-----------------------------------------------------------------------------
// 업데이트
//-----------------------------------------------------------------------------
/*
void		
MEffectManager::Update()
{
	EFFECT_LIST::iterator iEffect = m_listEffect.begin();

	while (iEffect != m_listEffect.end())
	{
		MEffect* pEffect = *iEffect;

		if (pEffect->Update())
		{
			// 보통?의 경우
		}
		else
		{
			// Effect가 끝난 경우(더 이상 update할 필요가 없는 경우)
		}

		iEffect ++;
	}
}
*/
