/*----------------------------------------------------------------------------

	GlobalVariables.cpp

	Dark Eden 클라이언트의 전역 변수 정의.
	크로스 플랫폼 SDL 빌드를 위해 Client.cpp 에서 분리하였다.

	2025.01.16

-----------------------------------------------------------------------------*/

#include "Client_PCH.h"

// 전방 선언 - 타입 정의를 위한 헤더 포함
#include "ClientDef.h"
#include "Packet/RequestClientPlayerManager.h"
#include "Packet/RequestServerPlayerManager.h"
#include "Packet/PacketFactoryManager.h"
#include "Packet/PacketValidator.h"
#include "PacketFunction.h"
#include "Packet/Properties.h"
#include "ServerInfo.h"
#include "ProfileManager.h"
#include "WavePackFileManager.h"
#include "CSoundPartManager.h"
#include "MZone.h"
#include "CMessageArray.h"
// #include "IVolume.h"  // VolumeLib 는 SDL 빌드에서 사용 불가
#include "MScreenEffectManager.h"
#include "SpriteLib/CSpriteSurface.h"
#include "SpriteLib/CStorageSurface.h"
#include "CDirectSoundStream.h"
#include "CMP3.h"
#include "CAvi.h"
#include "ClientCommunicationManager.h"
#include "MWorkThread.h"
#include "../VS_UI/src/widget/u_window.h"  // WindowManager 를 위해 포함
#include "../VS_UI/src/hangul/Ci.h"
#include "../basic/timer2.h"

// 필요한 것만 포함한다.
#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include <sys/time.h>
#endif

//-----------------------------------------------------------------------------
// 플랫폼 종속 전역 변수
//-----------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS
	HWND				g_hWnd = NULL;
	HINSTANCE			g_hInstance = NULL;
#else
	void*				g_hWnd = NULL;
	void*				g_hInstance = NULL;
#endif

int					g_x = 400;
int					g_y = 300;

// macOS 용 Windows 시간 함수 스텁
#ifdef PLATFORM_MACOS
DWORD timeGetTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (DWORD)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
#endif

//-----------------------------------------------------------------------------
// 메시지 배열
//-----------------------------------------------------------------------------
CMessageArray*		g_pSystemMessage = NULL;
CMessageArray*		g_pPlayerMessage = NULL;
CMessageArray*		g_pGameMessage = NULL;
CMessageArray*		g_pHelpMessage = NULL;
CMessageArray*		g_pNoticeMessage = NULL;

#ifdef	OUTPUT_DEBUG
CMessageArray*		g_pDebugMessage = NULL;
#endif

//-----------------------------------------------------------------------------
// 네트워크 & 소켓
//-----------------------------------------------------------------------------
ClientPlayer*						g_pSocket = NULL;
RequestClientPlayerManager*		g_pRequestClientPlayerManager = NULL;
RequestServerPlayerManager*		g_pRequestServerPlayerManager = NULL;

//-----------------------------------------------------------------------------
// 사운드
//-----------------------------------------------------------------------------
CSoundPartManager*		g_pSoundManager = NULL;
WavePackFileManager*	g_pWavePackFileManager = NULL;

//-----------------------------------------------------------------------------
// 존(Zone)
//-----------------------------------------------------------------------------
int					g_nZoneLarge = 0;
int					g_nZoneSmall = 0;
bool				g_bZoneLargeLoadImage = false;
bool				g_bZoneSmallLoadImage = false;
MZone*				g_pZoneLarge = NULL;
MZone*				g_pZoneSmall = NULL;
bool				g_bZonePlayerInLarge = false;
DWORD				g_ZoneRandomSoundTime = 0;

//-----------------------------------------------------------------------------
// 매니저
//-----------------------------------------------------------------------------
ProfileManager*			g_pProfileManager = NULL;
MScreenEffectManager*	g_pInventoryEffectManager = NULL;

//-----------------------------------------------------------------------------
// 그래픽
//-----------------------------------------------------------------------------
CSpriteSurface*		g_pBack = NULL;
CSpriteSurface*		g_pLast = NULL;
CStorageSurface*	g_pCursorSurface = NULL;
int					g_ScreenShotNumber = 0;

//-----------------------------------------------------------------------------
// 입력 상태
//-----------------------------------------------------------------------------
POINT				g_SelectSector = { 0, 0 };
BOOL				g_bLButtonDown = FALSE;
BOOL				g_bRButtonDown = FALSE;
BOOL				g_bCButtonDown = FALSE;
BOOL				g_bUIInput = FALSE;

//-----------------------------------------------------------------------------
// 게임 상태
//-----------------------------------------------------------------------------
BOOL				g_bActiveApp = TRUE;
BOOL				g_bActiveGame = FALSE;
BOOL				g_bNeedUpdate = FALSE;
int					g_UpdateDelay = 0;

//-----------------------------------------------------------------------------
// FPS(초당 프레임)
//-----------------------------------------------------------------------------
DWORD				g_CurrentTime = 0;
DWORD				g_CurrentFrame = 0;
int					g_FrameCount = 0;
int					g_StartFrameCount = 0;
DWORD				g_StartTime = 0;
DWORD				g_EndTime = 0;
int					g_FrameRate = 0;

//-----------------------------------------------------------------------------
// 렌더링 플래그
//-----------------------------------------------------------------------------
bool				g_bPutMessage = true;
bool				g_bNewDraw = false;
bool				g_bSmoothCursor = false;
bool				g_bNetStatusGood = true;

//-----------------------------------------------------------------------------
// 기타 전역 변수
//-----------------------------------------------------------------------------
CSDLStream*	g_pSDLStream = NULL;
MWorkThread*		g_pLoadingThread = NULL;
bool				g_bFullScreen = false;
int					g_MaxNPC = 100;
bool				g_bHAL = false;
bool				g_bMusicSW = true;

//-----------------------------------------------------------------------------
// 추가로 누락된 전역 변수 (basic/ 에서)
//-----------------------------------------------------------------------------
bool gbl_info_show = true;
bool gbl_show_item = true;

//-----------------------------------------------------------------------------
// 패킷 시스템 전역 변수
//-----------------------------------------------------------------------------
PacketFactoryManager*	g_pPacketFactoryManager = NULL;
PacketValidator*			g_pPacketValidator = NULL;
// g_pPacketItemOustersCoat 는 PacketFunction.cpp 에 정의되어 있다.
WindowManager*			gpC_window_manager = NULL;

//-----------------------------------------------------------------------------
// 설정 & 파일
//-----------------------------------------------------------------------------
Properties*				g_pConfigKorean = NULL;
Properties*				g_pFileDef = NULL;

//-----------------------------------------------------------------------------
// MP3 플레이어
//-----------------------------------------------------------------------------
CMP3*					g_pMP3 = NULL;

//-----------------------------------------------------------------------------
// 비디오 & 음악
//-----------------------------------------------------------------------------
CAVI*					g_pAvi = NULL;
bool					g_SDLAudio = false;
bool					g_Music = true;

//-----------------------------------------------------------------------------
// 게임 모드
//-----------------------------------------------------------------------------
enum CLIENT_MODE			g_Mode = MODE_OPENING;
enum CLIENT_MODE			g_ModeNext = MODE_OPENING;

//-----------------------------------------------------------------------------
// 존 설정
//-----------------------------------------------------------------------------
int						g_ZoneCreatureColorSet = 0;
WORD					g_MyBatColorSet = 0xFFFF;
bool					g_bHolyLand = false;
bool					g_bZoneSafe = false;

//-----------------------------------------------------------------------------
// 관전 모드
//-----------------------------------------------------------------------------
bool					g_bWatchMode = false;

//-----------------------------------------------------------------------------
// 마우스 위치 (VS_UI 용)
//-----------------------------------------------------------------------------
int						g_mouse_x = 0;
int						g_mouse_y = 0;

//-----------------------------------------------------------------------------
// 크리처 변신 타입
//-----------------------------------------------------------------------------
int						g_MorphCreatureType = 0;

//-----------------------------------------------------------------------------
// 사운드 설정
//-----------------------------------------------------------------------------
int						g_SoundPerSecond = 10;

//-----------------------------------------------------------------------------
// 클라이언트 통신 매니저
//-----------------------------------------------------------------------------
class ClientCommunicationManager*	g_pClientCommunicationManager = NULL;

//-----------------------------------------------------------------------------
// VS_UI & Basic 라이브러리 전역 변수
//-----------------------------------------------------------------------------
CI*				gC_ci = NULL;
C_TIMER2		gC_timer2;
