// RarFile.cpp: CRarFile 클래스의 구현.
// RAR 의존성 없이 크로스 플랫폼 지원을 위해 수정됨
//////////////////////////////////////////////////////////////////////

#include "RarFile.h"
#pragma warning(disable:4786)
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// 오류 보고 매크로 (크로스 플랫폼)
//////////////////////////////////////////////////////////////////////
#ifdef PLATFORM_WINDOWS
	#define RARFILE_ERROR(msg) { \
		OutputDebugStringA("[RARFile ERROR] "); \
		OutputDebugStringA(msg); \
		OutputDebugStringA("\n"); \
	}
#else
	#define RARFILE_ERROR(msg) { \
		fprintf(stderr, "[RARFile ERROR] %s\n", msg); \
		fflush(stderr); \
	}
#endif

//////////////////////////////////////////////////////////////////////
// 생성자
//////////////////////////////////////////////////////////////////////
CRarFile::CRarFile()
{
	m_file_pointer = NULL;
	m_data = NULL;
	m_size = 0;
}

//////////////////////////////////////////////////////////////////////
// 생성자
//////////////////////////////////////////////////////////////////////
CRarFile::CRarFile(const char *rar_filename, const char *pass)
{
	m_file_pointer = NULL;
	m_data = NULL;
	m_size = 0;
	SetRAR(rar_filename, pass);
}

//////////////////////////////////////////////////////////////////////
// 소멸자
//////////////////////////////////////////////////////////////////////
CRarFile::~CRarFile()
{
	Release();
}

//////////////////////////////////////////////////////////////////////
// 해제
//////////////////////////////////////////////////////////////////////
void CRarFile::Release()
{
	if(m_data != NULL){
		free(m_data);
		m_data = NULL;
		m_file_pointer = NULL;
	}
	m_size = 0;
}

//////////////////////////////////////////////////////////////////////
// SetRAR
// RAR 파일 경로를 디렉터리 경로로 변환
//////////////////////////////////////////////////////////////////////
void CRarFile::SetRAR(const char *rar_filename, const char *pass)
{
	if (rar_filename == NULL || rar_filename[0] == '\0')
	{
		m_rar_filename = "";
		m_base_dir = "";
		m_password = "";
		return;
	}

	m_rar_filename = rar_filename;
	m_password = pass;  // 비밀번호를 저장하지만 사용하지는 않음(압축 해제된 파일에는 필요 없음)

	// .rpk/.rar 파일이 들어있는 디렉터리를 기준 디렉터리로 사용한다.
	// 게임 데이터는 아카이브 이름을 딴 하위 폴더가 아니라 아카이브 자체
	// 옆에 압축 해제된 내용물을 그대로 배치하기 때문이다.
	// 예: "Data/Info/infodata.rpk" -> "Data/Info/"
	std::string path = rar_filename;
	size_t lastSlash = path.find_last_of("/\\");
	if (lastSlash != std::string::npos) {
		m_base_dir = path.substr(0, lastSlash + 1);
	} else {
		m_base_dir = "";
	}
}

//////////////////////////////////////////////////////////////////////
// Open
// 압축 해제된 디렉터리에서 파일 열기
//////////////////////////////////////////////////////////////////////
bool CRarFile::Open(const char *in_filename)
{
	if (in_filename == NULL || in_filename[0] == '\0')
	{
		RARFILE_ERROR("Open called with NULL or empty filename");
		return false;
	}

	Release();

	// 기준 디렉터리와 파일명을 합쳐서 전체 경로를 만든다
	std::string fullPath = m_base_dir + in_filename;

	// 파일 열기
	FILE* file = fopen(fullPath.c_str(), "rb");
	if (file == NULL)
	{
		// 상세 오류 정보 기록
		char errorMsg[512];
		snprintf(errorMsg, sizeof(errorMsg),
				"Failed to open file: %s (base_dir=%s, filename=%s)",
				fullPath.c_str(), m_base_dir.c_str(), in_filename);
		RARFILE_ERROR(errorMsg);
		return false;
	}

	// 파일 크기 얻기
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (fileSize <= 0)
	{
		char errorMsg[256];
		snprintf(errorMsg, sizeof(errorMsg),
				"File has invalid size: %s (size=%ld)", fullPath.c_str(), fileSize);
		RARFILE_ERROR(errorMsg);
		fclose(file);
		return false;
	}

	// 버퍼를 할당하고 파일 전체를 읽는다
	m_data = (char*)malloc(fileSize + 1);
	if (m_data == NULL)
	{
		RARFILE_ERROR("Memory allocation failed for file data");
		fclose(file);
		return false;
	}

	size_t bytesRead = fread(m_data, 1, fileSize, file);
	fclose(file);

	if (bytesRead != (size_t)fileSize) {
		char errorMsg[256];
		snprintf(errorMsg, sizeof(errorMsg),
				"Read size mismatch: %s (expected=%ld, actual=%zu)",
				fullPath.c_str(), fileSize, bytesRead);
		RARFILE_ERROR(errorMsg);
		free(m_data);
		m_data = NULL;
		return false;
	}

	m_size = (int)bytesRead;
	m_data[m_size] = '\0';  // 문자열 연산을 위해 null 종료 처리
	m_file_pointer = m_data;

	return true;
}

//////////////////////////////////////////////////////////////////////
// Read
// 버퍼로 데이터 복사
//////////////////////////////////////////////////////////////////////
char* CRarFile::Read(char *buf, int size)
{
	if(m_file_pointer == NULL || IsEOF())
		return NULL;

	memcpy(buf, m_file_pointer, size);
	char* re = m_data;
	m_file_pointer += size;
	return re;
}

//////////////////////////////////////////////////////////////////////
// Read
// size만큼 파일 포인터 이동
//////////////////////////////////////////////////////////////////////
char* CRarFile::Read(int size)
{
	if(m_file_pointer == NULL || IsEOF())
		return NULL;

	char* re = (char*)m_file_pointer;
	m_file_pointer += size;
	return re;
}

//////////////////////////////////////////////////////////////////////
// GetString
// 현재 파일 포인터 위치에서 한 줄을 읽는다
//////////////////////////////////////////////////////////////////////
bool CRarFile::GetString(char* buf, int size)
{
	if (buf == NULL || size <= 0)
		return false;

	if (m_file_pointer == NULL || IsEOF())
	{
		buf[0] = '\0';
		return false;
	}

	// 데이터 내 현재 위치 확인
	long currentPos = m_file_pointer - m_data;
	if (currentPos >= m_size)
	{
		buf[0] = '\0';
		return false;
	}

	// 줄바꿈 문자 찾기
	char* lineStart = m_file_pointer;
	char* newline = (char*)memchr(lineStart, '\n', m_size - currentPos);

	int lineLength;
	if (newline != NULL)
	{
		// 줄바꿈을 찾음 - 줄 길이 계산
		lineLength = (int)(newline - lineStart);

		// 다음 호출을 위해 줄바꿈 문자는 건너뜀
		m_file_pointer = newline + 1;
	}
	else
	{
		// 줄바꿈을 찾지 못함 - 끝까지 읽음
		lineLength = (int)(m_size - currentPos);
		m_file_pointer = m_data + m_size;
	}

	// 줄을 버퍼로 복사(버퍼 크기 준수)
	int copyLength = lineLength;
	if (copyLength >= size)
		copyLength = size - 1;

	memcpy(buf, lineStart, copyLength);
	buf[copyLength] = '\0';

	// 뒤에 붙은 \r 제거(Windows CRLF 파일인 경우)
	if (copyLength > 0 && buf[copyLength - 1] == '\r')
	{
		buf[copyLength - 1] = '\0';
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// IsEOF
//////////////////////////////////////////////////////////////////////
bool CRarFile::IsEOF(int plus)
{
	if (m_file_pointer == NULL)
		return true;

	long currentPos = m_file_pointer - m_data;
	return (currentPos + plus >= m_size);
}

//////////////////////////////////////////////////////////////////////
// GetList
// 호환성을 위한 스텁 구현
//////////////////////////////////////////////////////////////////////
std::vector<std::string> *CRarFile::GetList(char *filter)
{
	// 스텁: 빈 목록 반환
	static std::vector<std::string> emptyList;
	return &emptyList;
}
