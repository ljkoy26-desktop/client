//-----------------------------------------------------------------------------
// MEffectManager.h
//-----------------------------------------------------------------------------

#ifndef __MEFFECTMANAGER_H__
#define __MEFFECTMANAGER_H__

#pragma warning(disable:4786)

#include <list>
class MEffect;

class MEffectManager {
	public :
		// 이펙트 목록
		typedef std::list<MEffect*>						EFFECT_LIST;

	public :
		MEffectManager();
		virtual ~MEffectManager();

		//------------------------------------------------------
		// 해제
		//------------------------------------------------------
		virtual void		Release();

		//------------------------------------------------------
		// 추가
		//------------------------------------------------------
		virtual void		AddEffect(MEffect* pEffect);

		//------------------------------------------------------
		// 업데이트
		//------------------------------------------------------
		virtual void		Update() = 0;

		//------------------------------------------------------
		// 목록
		//------------------------------------------------------
		int					GetSize() const			{ return m_listEffect.size(); }
		EFFECT_LIST::const_iterator GetEffects()	{ return m_listEffect.begin(); }

	protected :
		EFFECT_LIST			m_listEffect;
};

#endif

