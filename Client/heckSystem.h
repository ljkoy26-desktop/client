// heckSystem.h: CheckSystem 클래스의 인터페이스.
//
//////////////////////////////////////////////////////////////////////
#ifndef __HECKSYSTEM_H__
#define __HECKSYSTEM_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif


class CheckSystem
{
public:
	BOOL GetSystem();
	CheckSystem();
	virtual ~CheckSystem();

};

#endif // !defined(AFX_HECKSYSTEM_H__6B8B3F6F_FEBE_481A_8C37_32FE368CC667__INCLUDED_)
