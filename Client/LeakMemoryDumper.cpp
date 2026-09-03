//////////////////////////////////////////////////////////////////////////////
/// \file LeakMemoryDumper.cpp
/// \author sonee
/// \date 2004.12.30
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "LeakMemoryDumper.h"

#ifdef _DEBUG

typedef struct {
	uintptr_t	address;
	size_t	size;
	char	file[64];
	DWORD	line;
} ALLOC_INFO;

typedef std::list<ALLOC_INFO*> AllocList;
AllocList allocList;

void AddTrack(uintptr_t addr, size_t asize, const char *fname, DWORD lnum)
{
	ALLOC_INFO *info;
	// 여기서 추적된 new(__FILE__, __LINE__)를 우회해야 한다 (참조:
	// LeakMemoryDumper.h의 #define new DEBUG_NEW) - `new (ALLOC_INFO)`는
	// 동일한 추적 배치 new로 매크로 확장되며, 이는 *이* 할당을 추적하기 위해
	// AddTrack()을 재귀적으로 호출하여 무한 재귀가 발생한다
	// (어딘가에서 처음 `new`를 호출하는 순간 무조건 스택 오버플로우 발생).
	// 대신 일반 malloc()으로 부기 노드를 할당한다.
	info = (ALLOC_INFO*)malloc(sizeof(ALLOC_INFO));
	info->address = addr;
	strncpy(info->file, fname, 63);
	info->line = lnum;
	info->size = asize;
	allocList.insert(allocList.begin(), info);
}

void RemoveTrack(uintptr_t addr)
{
	AllocList::iterator i;

	for(i = allocList.begin(); i != allocList.end(); i++)
	{
		if((*i)->address == addr)
		{
			allocList.remove((*i));
			break;
		}
	}
}

void DumpUnfreed()
{
	AllocList::iterator i;
	size_t totalSize = 0;
	char buf[1024];

	for(i = allocList.begin(); i != allocList.end(); i++)
	{
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS 0x%p\t%zu unfreed\n",
			(*i)->file, (*i)->line, (void*)(*i)->address, (*i)->size);
		OutputDebugString(buf);
		totalSize += (*i)->size;
	}
	sprintf(buf, "-----------------------------------------------------------\n");
	OutputDebugString(buf);
	sprintf(buf, "Total Unfreed: %zu bytes\n", totalSize);
	OutputDebugString(buf);
}

#endif