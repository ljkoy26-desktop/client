/********************************************************************
	created:	2003/10/17
	created:	17:10:2003   13:48
	filename: 	E:\designed\project\client\CSystemInfo.cpp
	file path:	E:\designed\project\client
	file base:	CSystemInfo
	file ext:	cpp
	author:		sonee

	purpose:	시스템 정보를 알아낸다.
				2003-10-17		CPU Clock 얻어오기
								MMX,SSE2 테크놀러지 Enable 여부
								Hyper Thread Enable 여부
*********************************************************************/
#include "Client_PCH.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <intrin.h>
#else
#include "../basic/Platform.h"
#endif
#include "CSystemInfo.h"

#ifdef PLATFORM_WINDOWS

#pragma warning( disable : 4035 )		// disable 시켜버리자-_-;
#pragma warning( disable: 4800 ) //'int' : forcing value to bool 'true' or 'false' (performance warning)

// 아래의 원래 VC6/x86 구현은 인라인 __asm(생 RDTSC/CPUID 명령코드,
// Pentium 이전 CPU에서의 불법 명령 오류를 잡기 위한 __try/__except 포함)을 사용했다.
// MSVC의 x64 컴파일러는 인라인 어셈블러를 전혀 지원하지 않으며(오류 C4235),
// CPUID/RDTSC는 x64 아키텍처가 시작된 이후 x64 코드를 실행 가능한 모든 CPU에서
// 무조건 존재하므로, 이를 동등한 __cpuid/__rdtsc 컴파일러 내장 함수(<intrin.h>)로
// 재작성하고 x64에서는 불필요한 불법 명령 처리를 제거했다.
inline unsigned __int64 theCycleCount(void)
{
	return __rdtsc();
}

static bool cpuid(unsigned long function, unsigned long& out_eax, unsigned long& out_ebx, unsigned long& out_ecx, unsigned long& out_edx)
{
#ifdef _LINUX
	asm("cpuid": "=a" (out_eax), "=b" (out_ebx), "=c" (out_ecx), "=d" (out_edx) : "a" (function));
	return true;
#else
	int info[4];
	__cpuid(info, (int)function);
	out_eax = (unsigned long)info[0];
	out_ebx = (unsigned long)info[1];
	out_ecx = (unsigned long)info[2];
	out_edx = (unsigned long)info[3];
	return true;
#endif
}

long CSystemInfo::GetCpuClock()
{
	unsigned __int64			start;
	unsigned __int64			overhead;

	start = theCycleCount();
	overhead = theCycleCount()-start;
	start = theCycleCount();
	Sleep(100);

	unsigned cpuspeed100 = (unsigned)( (theCycleCount()-start-overhead) / 1000 );
	return cpuspeed100 /100;
}

// --------------------------------------------------------------------------
bool CSystemInfo::CheckMMXTechnology()
{
	// 위의 cpuid()/theCycleCount() 주석 참조: CPUID와 MMX 레지스터 상태(EMMS)는
	// x86-64에서 무조건 존재하므로, 원래의 __try/__except 탐색 코드는 제거했다.
	int info[4];
	__cpuid(info, 1);      // 0 = 벤더 문자열, 1 = 버전 정보, 2 = 캐시 정보
	DWORD RegEDX = (DWORD)info[3];

	return (RegEDX & 0x800000) != 0;   // 비트 23이 설정되면 MMX 기술 지원
}


// --------------------------------------------------------------------------
// x86-64는 하드웨어 SSE/SSE2 지원을 기본 ABI의 일부로 요구하므로,
// 이 x64 빌드를 실행 가능한 모든 CPU에서 항상 true를 반환한다.
// 원래의 __asm/CPUID/__try 탐색 코드(VC6, 32비트 전용)가
// 더 이상 적용되지 않는 이유는 위의 cpuid()/theCycleCount() 주석을 참조.
bool CSystemInfo::CheckSSETechnology(void)
{
	return true;
}

bool CSystemInfo::CheckSSE2Technology()
{
	return true;
}

// --------------------------------------------------------------------------
bool CSystemInfo::Check3DNowTechnology()
{
	int info[4];
	__cpuid(info, (int)0x80000000);        // AMD 확장 함수 최대 지원 번호
	unsigned long RegEAX = (unsigned long)info[0];

	if (RegEAX <= 0x80000000UL)
	{
		return false;                       // AMD 확장 CPUID 함수 없음
	}

	__cpuid(info, (int)0x80000001);
	return ((unsigned long)info[3] >> 31) != 0;    // edx의 비트 31: 3DNow 지원
}

// 프로세서가 하이퍼-스레딩 기술을 지원하면 true, 그렇지 않으면 false 반환.
// 이 함수가 true를 반환해도 하이퍼-스레딩이 실제로 활성화되어 있다는 의미는 아니다.
bool CSystemInfo::CheckHyperThreadTechnology()
{
	const unsigned int HT_BIT		 = 0x10000000;  // EDX[28] - 비트 28이 설정되면 하이퍼-스레딩 하드웨어 지원
	const unsigned int FAMILY_ID     = 0x0f00;      // EAX[11:8] - 비트 11~8: 프로세서 패밀리 ID
	const unsigned int EXT_FAMILY_ID = 0x0f00000;	// EAX[23:20] - 비트 23~20: 확장 패밀리 프로세서 ID
	const unsigned int PENTIUM4_ID   = 0x0f00;		// 펜티엄 4 패밀리 프로세서 ID

	unsigned long unused,
				  reg_eax = 0,
				  reg_edx = 0,
				  vendor_id[3] = {0, 0, 0};

	// cpuid 명령 지원 여부 확인
	if( !cpuid(0,unused, vendor_id[0],vendor_id[2],vendor_id[1])
	 || !cpuid(1,reg_eax,unused,unused,reg_edx) )
	 return false;

	// 펜티엄 4 이상 프로세서인지 확인
	if (((reg_eax & FAMILY_ID) ==  PENTIUM4_ID) || (reg_eax & EXT_FAMILY_ID))
		if (vendor_id[0] == 'uneG' && vendor_id[1] == 'Ieni' && vendor_id[2] == 'letn')
			return (reg_edx & HT_BIT) != 0;	// 하이퍼-스레딩을 지원하는 정품 인텔 프로세서

	return false;  // 정품 인텔 프로세서가 아님
}
#else
// 비-Windows 플랫폼(macOS/Linux) - 스텁 구현

inline uint64_t theCycleCount(void)
{
    // 스텁 구현 - 0 반환
    return 0;
}

static bool cpuid(unsigned long function, unsigned long& out_eax, unsigned long& out_ebx, unsigned long& out_ecx, unsigned long& out_edx)
{
    // 스텁 구현 - 특별한 CPU 기능 없다고 가정
    out_eax = out_ebx = out_ecx = out_edx = 0;
    return false;
}

long CSystemInfo::GetCpuClock()
{
    // 스텁 구현 - 기본값 반환
    return 2000; // 2GHz로 가정
}

bool CSystemInfo::CheckMMXTechnology()
{
    // 스텁 구현 - 현대 시스템에서 MMX 사용 가능으로 가정
    return true;
}

bool CSystemInfo::CheckSSETechnology()
{
    // 스텁 구현 - 현대 시스템에서 SSE 사용 가능으로 가정
    return true;
}

bool CSystemInfo::CheckSSE2Technology()
{
    // 스텁 구현 - 현대 시스템에서 SSE2 사용 가능으로 가정
    return true;
}

bool CSystemInfo::CheckHyperThreadTechnology()
{
    // 스텁 구현 - 하이퍼-스레딩 없다고 가정
    return false;
}

#endif // PLATFORM_WINDOWS
