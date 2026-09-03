//-----------------------------------------------------------------------------
// DebugLog.h
//
// Dark Eden 클라이언트를 위한 경량 크로스플랫폼 로깅 시스템
// 예전 DEBUG_ADD 계열 매크로를 대체한다
//
// 특징:
// - __FILE__, __LINE__ 자동 포함
// - 로그 레벨: DEBUG/INFO/WARN/ERROR/NONE
// - 출력 설정 가능: 콘솔/파일/메모리 배열
// - 스레드 안전 (CRITICAL_SECTION 사용)
// - 조건부 컴파일: Release 빌드에서는 DEBUG 로그 제거
//-----------------------------------------------------------------------------

#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// 로그 레벨 열거형
//-----------------------------------------------------------------------------
typedef enum {
	LOG_LEVEL_DEBUG = 0,	// 상세 진단 정보
	LOG_LEVEL_INFO,		// 일반 정보 메시지
	LOG_LEVEL_WARN,		// 경고 메시지
	LOG_LEVEL_ERROR,	// 오류 메시지
	LOG_LEVEL_NONE		// 모든 로깅 비활성화
} LogLevel;

//-----------------------------------------------------------------------------
// 공개 인터페이스
//-----------------------------------------------------------------------------

// 초기화 및 정리
void log_init(void);
void log_cleanup(void);

// 설정
void log_set_level(LogLevel level);
void log_set_console_output(bool enable);
void log_set_file_output(const char *path);
void log_set_array_output(bool enable);

// 핵심 로깅 함수 (매크로에서 호출됨)
void log_write(LogLevel level,
			   const char *file,
			   int line,
			   const char *fmt,
			   ...);

//-----------------------------------------------------------------------------
// 로깅 매크로
//-----------------------------------------------------------------------------


#define LOG_DEBUG(fmt, ...) \
	log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
	log_write(LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
	log_write(LOG_LEVEL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
	log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

//-----------------------------------------------------------------------------
// 하위 호환성 - 예전 DEBUG_ADD 매크로를 새 시스템에 매핑
//-----------------------------------------------------------------------------

#define DEBUG_ADD(msg)			LOG_INFO("%s", msg)
#define DEBUG_ADD_ERR(msg)		LOG_ERROR("%s", msg)
#define DEBUG_ADD_WAR(msg)		LOG_WARN("%s", msg)
#define DEBUG_ADD_FORMAT(fmt, ...)	LOG_INFO(fmt, ##__VA_ARGS__)
#define DEBUG_ADD_FORMAT_ERR(fmt, ...)	LOG_ERROR(fmt, ##__VA_ARGS__)
#define DEBUG_ADD_FORMAT_WAR(fmt, ...)	LOG_WARN(fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // __DEBUG_LOG_H__
