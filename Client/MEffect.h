//----------------------------------------------------------------------
// MEffect.h
//-------------------------------------------------------------------------
// 화면에 표시되는 이펙트
//----------------------------------------------------------------------------------
//
// < 이펙트란 무엇인가?
// - 화면에 표시되는 이펙트.
// - 캐릭터, 아이템, 건물, 장애물 등을 제외한 (그 외?) 애니메이션.
// - 모든 이펙트는 CAlphaSprite와 알파 채널 블렌딩을 사용하여 표현된다.
// - 캐릭터가 검을 휘두르거나 성직자의 오라 같은 것은
//   여기서 다루는 이펙트에 포함되지 않는다.
// - 이펙트는 독립 오브젝트로 볼 수 있다.
///
// < 이펙트 타입 >
// = 미사일(Missile)
// - 픽셀 기반으로 이동한다.
// - 발사 위치에서 목표 위치까지 애니메이션된다.
// - 일정한 속도(픽셀)로 이동한다.
// - 유도 미사일일 수도 있고 아닐 수도 있다.
// - 장애물, 캐릭터, 건물 등과 충돌 시 제거될 수 있다.
// - 목표 위치에 도달하면 사라진다.
//
// = 타일 마법(Tile Magic)
// - 특정 타일 위에서 애니메이션된다.
// - 타일보다 크기가 클 수 있다.
// - 타일에서 마지막으로 출력된다.
// - 일정 프레임 후 사라진다.
//
//----------------------------------------------------------------------
// - 모든 이펙트는 Zone 내의 Sector에 속해야 한다.
//   출력 순서를 y좌표로 정렬해야 하므로,
//   Sector의 오브젝트 출력 순서에 따라 출력하기 위해 Sector에 배치하기로 결정했다.
//
// - 이펙트는 일정 시간 동안 지속된다.
//   즉, 카운터를 설정하여 0이 되면 종료된다.
//----------------------------------------------------------------------
//
// 이펙트는 파일에 저장되지 않는다.
//----------------------------------------------------------------------
/* 클래스 계층

MEffect --+-- MMovingEffect ---- ....

MEffect: 타일에 고정된 이펙트
*/
//----------------------------------------------------------------------

#ifndef	__MEFFECT_H__
#define	__MEFFECT_H__

#pragma warning(disable:4786)

#include "framelib/CAnimationFrame.h"
#include "MTypeDef.h"
#include "MObject.h"
#include "MEffectTarget.h"
#include "EffectResourceContainer.h"

#include <fstream>
using namespace std;



extern DWORD	g_CurrentFrame;


class MEffect : public MObject, public CAnimationFrame {
	
	public :
		enum EFFECT_TYPE 
		{
			EFFECT_SECTOR = 0,		// Sector에 고정 
			EFFECT_MOVING,			// 움직이는 Effect
			EFFECT_LINEAR,			// 직선 이동
			EFFECT_GUIDANCE,		// 추적(유도)하고 끝남
			EFFECT_HOMING,			// 곡선형의 유도탄
			EFFECT_PARABOLA,		// 포물선
			EFFECT_ATTACH,			// (캐릭터에) 붙음
			EFFECT_SCREEN,			// 화면좌표로 표현
			EFFECT_CHASE,			// 추적하면서 계속 붙어 다님
			EFFECT_ATTACH_ORBIT,	// 캐릭터에 붙어서 빙빙~ 돌면서 붙어다님
		};

	public :
		// 신규 생성자: 의존성 주입 지원 (명시적 리소스 컨테이너 필요)
		MEffect(BYTE bltType, EffectResourceContainer* resources);

		// 구 생성자: 이전 버전 호환성 유지 (리소스 컨테이너 없음)
		MEffect(BYTE bltType);

		~MEffect();

		//--------------------------------------------------------
		// 리소스 컨테이너 관리 (신규 추가)
		//--------------------------------------------------------
		// 리소스 컨테이너 설정 (의존성 주입용)
		void SetResourceContainer(EffectResourceContainer* resources);

		// 리소스 컨테이너 반환
		EffectResourceContainer* GetResourceContainer() const { return m_pResources; }

		//--------------------------------------------------------
		// FrameID 설정
		//--------------------------------------------------------
		void			SetFrameID(TYPE_FRAMEID FrameID, BYTE max);

		//--------------------------------------------------------
		// 이펙트 타입 반환
		//--------------------------------------------------------
		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_SECTOR; }

		void SetEst( int est ) { m_est = est; }
		int GetEst() const { return m_est; }
		//--------------------------------------------------------
		// 선택 가능 여부
		//--------------------------------------------------------
		virtual bool		IsSelectable() const		{ return false; }

		//--------------------------------------------------------
		// 끝나는 시간...
		//--------------------------------------------------------		
		// 현재 시간 + last까지 작동		
		void			SetCount(DWORD last, DWORD linkCount=0xFFFF);
		DWORD			GetEndFrame() const			{ return m_EndFrame; }
		DWORD			GetEndLinkFrame() const		{ return m_EndLinkFrame; }
		bool			IsEnd() const				{ return g_CurrentFrame >= m_EndFrame; }

		
		//--------------------------------------------------------
		// 한 번의 Update에 호출될 함수..
		//--------------------------------------------------------
		virtual bool	Update();
		
		//--------------------------------------------------------
		// Sector에서의 좌표
		//--------------------------------------------------------	
		void			SetPosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);
		void			SetX(TYPE_SECTORPOSITION x);
		void			SetY(TYPE_SECTORPOSITION y);
		void			SetZ(int z)						{ m_PixelZ = (float)z; }
		void			SetDirection(BYTE d)			{ m_Direction = d; }
		BYTE			GetDirection() const			{ return m_Direction; }
		void			SetLight(char light)			{ m_Light = light; }

	
		//--------------------------------------------------------
		// Pixel 좌표 설정
		//--------------------------------------------------------
		void			SetPixelPosition(int x, int y, int z);

		//--------------------------------------------------------
		// Get Functions
		//--------------------------------------------------------
		char			GetLight() const	{ return m_Light; }
		virtual int		GetPixelX() const	{ return (int)m_PixelX; }
		virtual int		GetPixelY() const	{ return (int)m_PixelY; }
		virtual int		GetPixelZ() const	{ return (int)m_PixelZ; }

		//--------------------------------------------------------
		// 위력
		//--------------------------------------------------------
		void			SetPower(BYTE power)	{ m_Power = power; }
		BYTE			GetPower() const		{ return m_Power; }

		//--------------------------------------------------------
		// 속도
		//--------------------------------------------------------
		void			SetStepPixel(WORD step)	{ m_StepPixel=step; }
		WORD			GetStepPixel() const	{ return m_StepPixel; }

		//--------------------------------------------------------
		// Effect 연결을 위한 정보
		//--------------------------------------------------------
		void			SetLink(TYPE_ACTIONINFO nActionInfo, MEffectTarget* pEffectTarget);		

		void			SetEffectTargetNULL();

		// 몇 번째 ActionInfo인가?
		TYPE_ACTIONINFO	GetActionInfo()	const { return m_nActionInfo; }

		// 남은 Effect개수를 return한다.
		MEffectTarget*	GetEffectTarget()		{ return m_pEffectTarget; }
		int				GetLinkSize()			{ return (m_pEffectTarget==NULL || m_pEffectTarget->IsEnd())? 0 : m_pEffectTarget->GetCurrentPhase(); }	

		void			SetMulti(bool bMulti)	{ m_bMulti = bMulti; }
		const bool		IsMulti()				{ return m_bMulti; }

		void			SetDelayFrame(DWORD frame);
		bool			IsDelayFrame() const;

		// 2004, 10, 15, sobeit add start - 이펙트 생성을 기다린다.
		void			SetWaitFrame(DWORD frame);
		bool			IsWaitFrame() const;
		// 2004, 10, 15, sobeit add end

		// 2004, 9, 30, sobeit add start - 보여줄지 말지..
		bool			IsSkipDraw() const			{ return m_bDrawSkip; }
		void			SetDrawSkip(bool bSkip)		{ m_bDrawSkip = bSkip;}
		// 2004, 9, 30, sobeit add end
	protected :
		// PixelPositon으로서 Sector좌표를 설정한다.
		void			AffectPosition();



	protected :
		DWORD					m_DelayFrame;
		
		int	m_est;

		// 끝나는 시간
		DWORD					m_EndFrame;
		DWORD					m_EndLinkFrame;	// 다음 link로 넘어가는 frame
		
		BYTE					m_Direction;	// 방향

		// Effect의 밝기 
		char					m_Light;

		// Effect가 존재하는 Zone에서의 Pixel좌표(현재 위치)
		float			m_PixelX;
		float			m_PixelY;
		float			m_PixelZ;
		WORD			m_StepPixel;

		BYTE			m_Power;

		// 다음 Effect로의 연결을 위한 정보
		TYPE_ACTIONINFO			m_nActionInfo;
		MEffectTarget*			m_pEffectTarget;

		static TYPE_OBJECTID	s_ID;			// ID발급을 위한...

		// 중복 가능한가
		bool			m_bMulti;

		bool			m_bDrawSkip;
		DWORD			m_dwWaitFrame;

		// 신규 추가: 리소스 컨테이너 (의존성 주입)
		EffectResourceContainer*	m_pResources;
};

//----------------------------------------------------------------------
// MSelectableEffect - 마우스로 선택되는 effect
//----------------------------------------------------------------------
class MSelectableEffect : public MEffect {
	public :
		MSelectableEffect(BYTE bltType) : MEffect(bltType) {}
		~MSelectableEffect() {}

		//--------------------------------------------------------
		// 선택 가능 여부
		//--------------------------------------------------------
		bool		IsSelectable() const		{ return true; }
};

#endif



