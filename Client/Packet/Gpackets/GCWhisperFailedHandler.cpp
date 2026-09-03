//////////////////////////////////////////////////////////////////////
//
// Filename    : GCWhisperFailedHandler.cc
// 작성자: elca@ewestsoft.com
// 설명:
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCWhisperFailed.h"
#include "MGameStringTable.h"
#include "ClientDef.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCWhisperFailedHandler::execute ( GCWhisperFailed * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY


	// info = 5
	UI_AddChatToHistory( (*g_pGameStringTable)[STRING_MESSAGE_WHISPER_FAILED].GetString(), NULL, 5, RGB(255, 255, 255) );

	__END_CATCH
}
