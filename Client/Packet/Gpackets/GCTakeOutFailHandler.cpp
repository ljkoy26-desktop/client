//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTakeOutFailHandler.cc
// 작성자: elca@ewestsoft.com
// 설명:
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTakeOutFail.h"
#include "UIDialog.h"
#include "MGameStringTable.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCTakeOutFailHandler::execute ( GCTakeOutFail * pGCTakeOutFail , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY 

	g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_TAKE_OUT_FAIL].GetString() );

	__END_CATCH
}
