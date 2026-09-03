//////////////////////////////////////////////////////////////////////////////
// 파일명: CLGetWorldList.cpp 
// 작성자: reiot@ewestsoft.com
// 설명: 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CLGetWorldList.h"

void CLGetWorldList::read (SocketInputStream & iStream) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void CLGetWorldList::write (SocketOutputStream & oStream) const 
     throw (ProtocolException , Error)
{
	__BEGIN_TRY
	__END_CATCH
}

void CLGetWorldList::execute (Player* pPlayer) 
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY

	#ifndef __GAME_CLIENT__
		CLGetWorldListHandler::execute(this, pPlayer);
	#endif

	__END_CATCH
}

