//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTakeOutOKHandler.cc
// 작성자: elca@ewestsoft.com
// 설명:
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTakeOutOK.h"
#include "UIDialog.h"
#include "MGameStringTable.h"

extern void UI_RemoveItemFromItemShop( DWORD id );

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCTakeOutOKHandler::execute ( GCTakeOutOK * pGCTakeOutOK , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_TAKE_OUT_OK].GetString() );

	UI_RemoveItemFromItemShop( pGCTakeOutOK->getObjectID() );
		
	__END_CATCH
}
