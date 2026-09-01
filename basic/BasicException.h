/*-----------------------------------------------------------------------------

	BasicException.h

	2000.8.25. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __BASICEXCEPTION_H__
#define __BASICEXCEPTION_H__

#include <assert.h>

#define _Error(code)						g_BasicException(code, NULL, __FILE__, __LINE__)
#define _ErrorStr(sz_error)			g_BasicException(INVALID_EXCEPTION_CODE, sz_error, __FILE__, __LINE__)
#define CheckMemAlloc(x)				if (!x) _Error(MEM_ALLOC)

enum EXCEPTION_CODE
{
	INVALID_EXCEPTION_CODE,

	MEM_ALLOC,		// 메모리 할당 실패.
	FILE_OPEN,		// 파일 열기 실패.
	FILE_LOAD,		// 파일 로드 실패.
	FAILED_JOB,		// 작업 실패.
	NULL_REF,		// null 참조.
};

void g_SetNewHandler();
void g_BasicException(EXCEPTION_CODE code, char * sz_error, char * file, unsigned long line);

#endif