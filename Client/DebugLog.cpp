//-----------------------------------------------------------------------------
// DebugLog.cpp
//
// Dark Eden 클라이언트용 경량 크로스플랫폼 로깅 시스템
// 구현부
//-----------------------------------------------------------------------------

#include "DebugLog.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "../../basic/Platform.h"

#ifdef PLATFORM_WINDOWS
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif

// 플랫폼별 포함 파일
#ifdef PLATFORM_WINDOWS
	#define PLATFORM_LOCK_INITIALIZED 1
#endif

//-----------------------------------------------------------------------------
// 설정
//-----------------------------------------------------------------------------
typedef struct {
	LogLevel level;
	bool output_to_console;
	bool output_to_file;
	bool output_to_array;
	char log_file[256];
	FILE *log_fp;
} LogConfig;

static LogConfig g_config = {
	LOG_LEVEL_INFO,		// 기본 레벨
	true,				// 콘솔 출력 활성화
	false,				// 파일 출력 비활성화
	false,				// 배열 출력 비활성화
	"",					// 로그 파일 경로
	NULL				// 파일 핸들
};

static bool g_initialized = false;

//-----------------------------------------------------------------------------
// 스레드 안전성
//-----------------------------------------------------------------------------
static CRITICAL_SECTION g_log_lock;

//-----------------------------------------------------------------------------
// 헬퍼 함수
//-----------------------------------------------------------------------------

// 전체 경로에서 파일명만 추출 (예: "/path/to/GameInit.cpp" -> "GameInit.cpp")
static const char* get_filename(const char *path) {
	if (path == NULL) return "unknown";

	const char *filename = strrchr(path, '/');
	if (filename == NULL) {
		filename = strrchr(path, '\\');	// Windows 경로 구분자
	}

	return (filename != NULL) ? (filename + 1) : path;
}

// 레벨 문자열 반환
static const char* get_level_string(LogLevel level) {
	switch (level) {
		case LOG_LEVEL_DEBUG: return "DEBUG";
		case LOG_LEVEL_INFO:  return "INFO ";
		case LOG_LEVEL_WARN:  return "WARN ";
		case LOG_LEVEL_ERROR: return "ERROR";
		default:             return "UNKNOWN";
	}
}

// 밀리초 포함 타임스탬프 반환
// 형식: "2024-01-20 23:45:12.123"
static void get_timestamp(char *buffer, size_t size) {
#ifdef PLATFORM_WINDOWS
	struct _timeb timebuf;
	_ftime(&timebuf);
	struct tm *tm_info = localtime(&timebuf.time);
	snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
			 tm_info->tm_year + 1900,
			 tm_info->tm_mon + 1,
			 tm_info->tm_mday,
			 tm_info->tm_hour,
			 tm_info->tm_min,
			 tm_info->tm_sec,
			 timebuf.millitm);
#else
	struct timeval tv;
	struct tm *tm_info;
	gettimeofday(&tv, NULL);
	tm_info = localtime(&tv.tv_sec);
	snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
			 tm_info->tm_year + 1900,
			 tm_info->tm_mon + 1,
			 tm_info->tm_mday,
			 tm_info->tm_hour,
			 tm_info->tm_min,
			 tm_info->tm_sec,
			 tv.tv_usec / 1000);
#endif
}

//-----------------------------------------------------------------------------
// 공개 인터페이스 구현
//-----------------------------------------------------------------------------

void log_init(void) {
	if (g_initialized) {
		return;	// 이미 초기화됨
	}

	// 잠금 먼저 초기화
	InitializeCriticalSection(&g_log_lock);

	// 빌드 타입에 따른 기본 레벨 설정
#ifdef _DEBUG
	g_config.level = LOG_LEVEL_INFO;
#else
	g_config.level = LOG_LEVEL_ERROR;	// 프로덕션: 오류만 출력
#endif

	g_initialized = true;

	// 초기화 메시지를 stderr에 직접 출력 (잠금 우회)
	fprintf(stderr, "[DEBUG LOG] Logging system initialized (level: %s)\n",
			g_config.level == LOG_LEVEL_DEBUG ? "DEBUG" :
			g_config.level == LOG_LEVEL_INFO ? "INFO" :
			g_config.level == LOG_LEVEL_WARN ? "WARN" :
			g_config.level == LOG_LEVEL_ERROR ? "ERROR" : "NONE");
	fflush(stderr);
}

void log_cleanup(void) {
	if (!g_initialized) {
		return;
	}

	// 종료 메시지를 stderr에 직접 출력 (잠금 문제 우회)
	fprintf(stderr, "[DEBUG LOG] Logging system shutting down\n");
	fflush(stderr);

	// 로그 파일이 열려있으면 닫는다
	if (g_config.log_fp != NULL) {
		fclose(g_config.log_fp);
		g_config.log_fp = NULL;
	}

	// 잠금 정리
	DeleteCriticalSection(&g_log_lock);

	g_initialized = false;
}

void log_set_level(LogLevel level) {
	EnterCriticalSection(&g_log_lock);
	g_config.level = level;
	LeaveCriticalSection(&g_log_lock);
}

void log_set_console_output(bool enable) {
	EnterCriticalSection(&g_log_lock);
	g_config.output_to_console = enable;
	LeaveCriticalSection(&g_log_lock);
}

void log_set_file_output(const char *path) {
	EnterCriticalSection(&g_log_lock);

	// 기존 파일이 열려있으면 닫는다
	if (g_config.log_fp != NULL) {
		fclose(g_config.log_fp);
		g_config.log_fp = NULL;
	}

	if (path != NULL && path[0] != '\0') {
		// 경로 저장
		strncpy(g_config.log_file, path, sizeof(g_config.log_file) - 1);
		g_config.log_file[sizeof(g_config.log_file) - 1] = '\0';

		// 파일 열기
		g_config.log_fp = fopen(g_config.log_file, "w");
		if (g_config.log_fp != NULL) {
			g_config.output_to_file = true;
		} else {
			// 파일 열기 실패, 콘솔로 대체
			g_config.output_to_file = false;
		}
	} else {
		g_config.output_to_file = false;
		g_config.log_file[0] = '\0';
	}

	LeaveCriticalSection(&g_log_lock);
}

void log_set_array_output(bool enable) {
	EnterCriticalSection(&g_log_lock);
	g_config.output_to_array = enable;
	LeaveCriticalSection(&g_log_lock);
}

//-----------------------------------------------------------------------------
// 핵심 로깅 함수
//-----------------------------------------------------------------------------

void log_write(LogLevel level,
			   const char *file,
			   int line,
			   const char *fmt,
			   ...)
{
	// 빠른 경로: 레벨 필터링 (잠금 불필요)
	if (level < g_config.level || !g_initialized) {
		return;
	}

	// 파일명 추출
	const char *filename = get_filename(file);
	const char *level_str = get_level_string(level);

	// 메시지 형식화
	char message[2048];
	va_list args;
	va_start(args, fmt);
	vsnprintf(message, sizeof(message), fmt, args);
	va_end(args);

	// 전체 로그 라인 생성
	char log_line[2048];
	char timestamp[64];
	get_timestamp(timestamp, sizeof(timestamp));

	snprintf(log_line, sizeof(log_line),
			 "[%s] [%s] [%s:%d] %s",
			 timestamp, level_str, filename, line, message);

	// 출력용 임계 구역
	EnterCriticalSection(&g_log_lock);

	// 콘솔에 출력 (stderr)
	if (g_config.output_to_console) {
		fprintf(stderr, "%s\n", log_line);
		fflush(stderr);
	}

	// 파일에 출력
	if (g_config.output_to_file && g_config.log_fp != NULL) {
		fprintf(g_config.log_fp, "%s\n", log_line);
		fflush(g_config.log_fp);
	}

	// 메모리 배열에 출력 (g_pDebugMessage)
	// 주의: CMessageArray와의 순환 의존성 방지를 위해 비활성화됨
	// 필요 시 이 파일 앞에 CMessageArray.h를 포함하여 활성화
#if 0
	if (g_config.output_to_array && g_pDebugMessage != NULL) {
		g_pDebugMessage->Add(log_line);
	}
#endif

	LeaveCriticalSection(&g_log_lock);
}
