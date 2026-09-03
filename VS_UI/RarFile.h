// RarFile.h: CRarFile 클래스의 인터페이스.
// RAR 의존성 없이 크로스 플랫폼 지원을 위해 수정됨
//////////////////////////////////////////////////////////////////////

#ifndef _RAR_FILE_HEADER_
#define _RAR_FILE_HEADER_

#pragma warning(disable:4786)

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * CRarFile - 압축 해제된 RAR 콘텐츠를 다루는 크로스 플랫폼 파일 리더
 *
 * RAR 파일은 동일한 이름의 디렉터리로 매핑된다:
 * - Data/Info/infodata.rpk  → Data/Info/infodata/
 * - Data/Ui/txt/Item.rpk    → Data/Ui/txt/Item/
 *
 * 이를 통해 unrar 라이브러리 의존성을 없애고 크로스 플랫폼 호환성을 개선한다
 */

class CRarFile
{
private:
	std::string m_rar_filename;
	std::string m_password;
	std::string m_base_dir;      // 압축 해제된 디렉터리 경로

	char *m_data;
	char *m_file_pointer;
	int m_size;

public:
	// 생성자
	CRarFile();
	CRarFile(const char *rar_filename, const char *pass);

	// 소멸자
	~CRarFile();

	// 리소스 해제
	void Release();

	// RAR 파일 경로 설정 (디렉터리 경로로 변환)
	void SetRAR(const char *rar_filename, const char *pass);

	// 압축 해제된 디렉터리에서 파일 열기
	bool Open(const char *in_filename);

	// 데이터 읽기
	char*	Read(char *buf, int size);
	char*	Read(int size);
	bool	GetString(char* buf, int size);

	// 파일 준비 여부 확인
	bool	IsSet()	{ return (m_data != NULL); }

	// EOF 여부 확인
	bool	IsEOF(int plus = 0);

	// 파일 목록 반환 (호환성을 위한 스텁)
	std::vector<std::string> *GetList(char *filter = NULL);

	char* GetFilePointer(){return m_file_pointer;};
};

#endif
