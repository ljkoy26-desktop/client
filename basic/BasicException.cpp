#include "BasicException.h"
#include <stdio.h>
#include <stdlib.h>

void g_SetNewHandler()
{
	// 메모리 할당 실패에 대한 new 핸들러를 설정한다
	// SDL 백엔드에서는 스텁으로 동작한다
}

void g_BasicException(EXCEPTION_CODE code, char* sz_error, char* file, unsigned long line)
{
	// 기본 예외 처리기
	if (sz_error == NULL)
	{
		switch (code)
		{
			case MEM_ALLOC:
				fprintf(stderr, "Memory allocation failed at %s:%lu\n", file, line);
				break;
			case FILE_OPEN:
				fprintf(stderr, "File open failed at %s:%lu\n", file, line);
				break;
			case FILE_LOAD:
				fprintf(stderr, "File load failed at %s:%lu\n", file, line);
				break;
			case FAILED_JOB:
				fprintf(stderr, "Job failed at %s:%lu\n", file, line);
				break;
			case NULL_REF:
				fprintf(stderr, "Null reference at %s:%lu\n", file, line);
				break;
			default:
				fprintf(stderr, "Unknown error at %s:%lu\n", file, line);
				break;
		}
	}
	else
	{
		fprintf(stderr, "Error: %s at %s:%lu\n", sz_error, file, line);
	}

	// 디버그 모드에서는 assert 처리한다
	#ifdef _DEBUG
	assert(false);
	#else
	// 릴리즈 모드에서는 종료한다
	exit(1);
	#endif
}
