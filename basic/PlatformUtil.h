/*-----------------------------------------------------------------------------

	PlatformUtil.h

	플랫폼 관련 유틸리티.
	플랫폼 추상화 계층을 사용하도록 갱신됨.

	Original: 2000.10.11. KJTINC
	Updated: 2025.01.14

-----------------------------------------------------------------------------*/

#ifndef __PLATFORMUTIL_H__
#define __PLATFORMUTIL_H__

#include "Platform.h"

// 키보드 스캔 코드 매크로 (이제 플랫폼을 인식함)
#ifdef PLATFORM_WINDOWS
	#define SCAN_CODE(x)			LOBYTE(HIWORD(x))
#else
	#define SCAN_CODE(x)			platform_get_scan_code(x)
#endif

//-----------------------------------------------------------------------------
// 공개 인터페이스
//-----------------------------------------------------------------------------
inline bool g_GetCtrlPushState() {
	return platform_is_ctrl_pressed() ? true : false;
}

#endif