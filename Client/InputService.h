/*-----------------------------------------------------------------------------

	InputService.h

	전역 입력 서비스에 대한 전방 선언 및 접근자.
	게임 코드를 위한 더 깔끔한 include 경로를 제공한다.

	2025.02.04 - DXLib 헤더 의존성 감소를 위해 생성
	            2025.02.04 - CDirectInput 대신 InputCodes를 사용하도록 수정

-----------------------------------------------------------------------------*/

#ifndef __INPUTSERVICE_H__
#define __INPUTSERVICE_H__

// 키 코드 정의 포함 (플랫폼 독립적인 SDL 기반)
#include "basic/InputCodes.h"

// 입력 서비스 클래스 전방 선언
class CSDLInput;

// 전역 입력 서비스 인스턴스 (DXLib/CDirectInput.cpp에 정의됨)
extern CSDLInput* g_pSDLInput;

#endif // __INPUTSERVICE_H__
