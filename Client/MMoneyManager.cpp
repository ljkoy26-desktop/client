//-----------------------------------------------------------------------------
// MMoneyManager.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MMoneyManager.h"

#include "MHelpManager.h"
#include "MHelpDef.h"
//-----------------------------------------------------------------------------
// 전역 변수
//-----------------------------------------------------------------------------
MMoneyManager*		g_pMoneyManager = NULL;

//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
MMoneyManager::MMoneyManager()
{
	m_MoneyLimit	= 2000000000;		// 20억
	m_Money			= 0;
}

MMoneyManager::MMoneyManager(const MMoneyManager& mm)
{
	m_MoneyLimit = mm.m_MoneyLimit;
	m_Money = mm.m_Money;
}

MMoneyManager::~MMoneyManager()
{
}

//-----------------------------------------------------------------------------
//
// member functions
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 돈 설정
//-----------------------------------------------------------------------------
bool
MMoneyManager::SetMoney(int money)
{
	// 0이하거나 한계를 넘어가면 안된다.
	if (money<0 || money > m_MoneyLimit)
	{
		return false; 
	}

	m_Money = money;

	// 2004, 5, 6, sobeit add start - 돈이 10 만이 넘으면 보관함 도움말
#ifdef __GAME_CLIENT__
	static bool CanBuyStorage = false;
	if( false == CanBuyStorage && m_Money>100000 )
	{
		ExecuteHelpEvent(HELP_EVENT_STORAGE_BUY);
		CanBuyStorage = true;
	}
#endif
	//2004, 5, 6 sobeit add end
	return true;
}

//-----------------------------------------------------------------------------
// 돈 추가
//-----------------------------------------------------------------------------
bool
MMoneyManager::AddMoney(int money)
{
	return SetMoney( m_Money + money );
}

//-----------------------------------------------------------------------------
// 돈 제거
//-----------------------------------------------------------------------------
bool
MMoneyManager::UseMoney(int money)
{
	return SetMoney( m_Money - money );
}

//-----------------------------------------------------------------------------
// 돈 추가 가능 여부 확인
//-----------------------------------------------------------------------------
bool		
MMoneyManager::CanAddMoney(int money)
{
	int left = m_Money + money;

	if (money > m_MoneyLimit)
	{
		return false; 
	}

	return true;
}

//-----------------------------------------------------------------------------
// 돈 사용 가능 여부 확인
//-----------------------------------------------------------------------------
bool		
MMoneyManager::CanUseMoney(int money)
{
	int left = m_Money - money;

	if (left<0)
	{
		return false; 
	}

	return true;
}