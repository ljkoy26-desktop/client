//-----------------------------------------------------------------------------
// Globals.cpp
//-----------------------------------------------------------------------------
// 게임에 필요한 전역 변수 및 유틸리티 함수
// 심볼 중복을 방지하기 위해 GamePacketFunctions.cpp 에서 분리하였다.
//-----------------------------------------------------------------------------

#include "Client_PCH.h"
#include "VS_UI/src/hangul/Fl2.h"  // For PrintInfo definition

//-----------------------------------------------------------------------------
// 전역 변수
//-----------------------------------------------------------------------------

// 웨이브 팩 파일 매니저
class CWavePackFileManager;
CWavePackFileManager* g_pWavePackFileManager = NULL;

// 정보 표시 관련 전역 변수
extern "C" {
	bool gbl_info_show = true;
	bool gbl_show_item = true;
}

//-----------------------------------------------------------------------------
// 출력 함수
//-----------------------------------------------------------------------------
// SDL 빌드에서는 RenderingFunctions.cpp 에 구현되어 있다.

//-----------------------------------------------------------------------------
// 플랫폼 종속 함수 (Windows 전용)
//-----------------------------------------------------------------------------

// CheckMacScreenMode - PacketFunction.cpp 에 #ifdef PLATFORM_WINDOWS 블록으로
// 이미 실제 구현이 존재한다. 이 스텁은 해당 구현이 제외되는 플랫폼에서만
// 필요한 대체 구현이다. Windows 에서 둘 다 정의하면 심볼 중복(LNK2005)이 발생한다.
#ifndef PLATFORM_WINDOWS
void CheckMacScreenMode()
{
	// 스텁 구현 - Windows 전용 함수
}
#endif

// GetNMClipData - 넷마블 클립보드 데이터 가져오기 (Windows 전용)
bool GetNMClipData(char* pBuffer, unsigned int bufferSize, const char* pURL, bool bUseHTML)
{
	(void)pBuffer; (void)bufferSize; (void)pURL; (void)bUseHTML;
	// 스텁 구현 - Windows 전용 함수
	return false;
}

// SendUserIDToGameMonA - GameMon 안티치트에 유저 ID 전송 (Windows 전용)
void SendUserIDToGameMonA(const char* pUserID)
{
	(void)pUserID;
	// 스텁 구현 - Windows GameMon 안티치트는 macOS 에서 사용 불가
}
