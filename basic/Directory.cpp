/*-----------------------------------------------------------------------------

	Directory.cpp

	디렉토리 유틸리티의 크로스플랫폼 구현.
	플랫폼 추상화 계층을 사용한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#include "Directory.h"
#include "Platform.h"
#include <string.h>
#include <stdlib.h>

/* 전역 인스턴스 */
C_DIRECTORY gC_directory;

/* ============================================================================
 * 생성자/소멸자
 * ============================================================================ */

C_DIRECTORY::C_DIRECTORY()
{
	m_sz_program_directory = NULL;
	m_sz_mixed_directory = NULL;

	/* 실행 파일 디렉토리를 가져온다 */
	char dir[512];
	if (platform_get_executable_dir(dir, sizeof(dir)) == 0) {
		m_sz_program_directory = new char[strlen(dir) + 1];
		strcpy(m_sz_program_directory, dir);
	}
}

C_DIRECTORY::~C_DIRECTORY()
{
	if (m_sz_program_directory != NULL) {
		delete[] m_sz_program_directory;
		m_sz_program_directory = NULL;
	}

	if (m_sz_mixed_directory != NULL) {
		delete[] m_sz_mixed_directory;
		m_sz_mixed_directory = NULL;
	}
}

/* ============================================================================
 * 조회 함수
 * ============================================================================ */

char* C_DIRECTORY::GetMixedPathWidthProgramDir(const char* name) const
{
	if (m_sz_program_directory == NULL) return NULL;

	/* 전체 경로를 위한 버퍼를 할당한다 */
	size_t len = strlen(m_sz_program_directory) + strlen(name) + 1;
	char* result = new char[len];
	strcpy(result, m_sz_program_directory);
	strcat(result, name);

	return result;
}

char* C_DIRECTORY::GetProgramDirectory() const
{
	return m_sz_program_directory;
}

char* C_DIRECTORY::GetMixedDirectory(const char* path, const char* filename) const
{
	/* 결합된 경로를 위한 버퍼를 할당한다 */
	size_t len = strlen(path) + strlen(filename) + 2;
	char* result = new char[len];
	strcpy(result, path);

	/* 필요하면 경로 구분자를 추가한다 */
	size_t pathLen = strlen(path);
	if (pathLen > 0 && path[pathLen - 1] != platform_get_path_separator()) {
		strcat(result, platform_get_path_separator() == '\\' ? "\\" : "/");
	}

	strcat(result, filename);

	return result;
}

/* ============================================================================
 * 검사 함수
 * ============================================================================ */

bool C_DIRECTORY::ExistFile(const char* fullpath) const
{
	return platform_file_exists(fullpath) ? true : false;
}
