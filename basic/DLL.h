/*-----------------------------------------------------------------------------

	DLL.h

	동적 링크 라이브러리(DLL) 공통 헤더 파일.

	2000.8.8. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __DLL_H__
#define __DLL_H__

#define _CPP

#ifdef PLATFORM_WINDOWS
// 이전에는 여기서 `export`/`import`를 단순 매크로로 정의한 뒤 아래에서
// DllExport/DllImport로 별칭 처리했다. `export`는 예약된 C++ 키워드이며
// (역사적으로 exported 템플릿용) C++ 표준 라이브러리는 이것이 매크로화되는
// 것을 명시적으로 막는다 - <xkeycheck.h>가 `export`가 매크로인 상태에서
// 표준 헤더가 include되는 순간 #error(C1189)를 발생시킨다. 순수한
// `export`/`import` 매크로를 아예 두지 않고 DllExport/DllImport를 직접
// 정의하면 이 충돌을 완전히 피할 수 있다; 이 파일 밖에서는 순수 이름을
// 사용하는 곳이 없었다.
#ifndef _CPP
#define DllExport extern "C" __declspec (dllexport)
#define DllImport extern "C" __declspec (dllimport)
#else
#define DllExport __declspec (dllexport)
#define DllImport __declspec (dllimport)
#endif

#ifdef _DLL_EXPORT
#define DllClass		DllExport
#else
#define DllClass		DllImport
#endif
#else
// Windows가 아닌 플랫폼: 빈 매크로
#define DllExport
#define DllImport extern
#define DllClass
#endif

#endif