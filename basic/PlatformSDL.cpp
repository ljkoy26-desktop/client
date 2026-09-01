/*-----------------------------------------------------------------------------

	PlatformSDL.cpp

	플랫폼 추상화 계층의 SDL/POSIX 구현.
	크로스플랫폼 지원을 위해 SDL2와 POSIX API를 사용한다.

	2025.01.14

-----------------------------------------------------------------------------*/

#include "Platform.h"

/* 이 파일의 대부분(시간/스레드/뮤텍스/이벤트/동적 라이브러리/키보드/
   오류 보고/초기화-종료)은 Windows에서도 동일하게 동작하는 순수 SDL2 호출이며,
   Platform.h는 이 함수들을 무조건적으로 선언한다
   (예: platform_get_ticks() - timeGetTime()/GetTickCount()가 PLATFORM_WINDOWS에서도
   이 함수를 거쳐 호출된다. Platform.h 참고). 이 파일은 원래 전부
   `#ifndef PLATFORM_WINDOWS`로만 감싸여 있었고 프로젝트 어디에도 Windows 네이티브
   구현이 없었기 때문에, Windows에서는 이 함수들을 호출하는 코드가 실제로 컴파일되는
   순간마다 링크 타임에 미해결 외부 참조(LNK2001/LNK2019)가 발생했다 -
   __WIN32__/__WINDOWS__ CMake 수정으로 나머지 코드베이스가 막힘 없이 빌드되면서
   이제는 그 코드들이 모두 컴파일되고 있다.
   아래 File/Path Functions 섹션(platform_get_executable_dir(),
   platform_create_directory())은 순수하게 POSIX 전용(dirname(),
   readlink(), 2인자 mkdir())이며 Windows에서는 여전히 막혀 있다;
   현재 Windows 빌드에서는 둘 다 호출되지 않는다. */

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#ifndef PLATFORM_WINDOWS
#include <unistd.h>
#endif

#ifdef PLATFORM_LINUX
	#include <limits.h>
	#include <stdlib.h>
#endif

#ifdef PLATFORM_MACOS
	#include <limits.h>  /* PATH_MAX를 위함 */
#endif

#ifdef __EMSCRIPTEN__
	/* Emscripten은 PATH_MAX를 정의하지 않으므로 적절한 값을 정의한다 */
	#ifndef PATH_MAX
		#define PATH_MAX 4096
	#endif
	#include <limits.h>
	#include <stdlib.h>
	#include <libgen.h>  /* dirname을 위함 */
#endif

#ifdef PLATFORM_MACOS
	#include <libgen.h>
	#include <mach-o/dyld.h>
#endif

/* 이벤트 구조체 정의 (헤더에서는 불투명 타입) */
struct platform_event_s {
	SDL_mutex* mutex;
	SDL_cond* cond;
	int signaled;
};

/*=============================================================================
 * 시간 함수
 *=============================================================================*/

/* ============================================================================
 * 시간 함수
 * ============================================================================ */

DWORD platform_get_ticks(void) {
	return SDL_GetTicks();
}

uint64_t platform_get_performance_counter(void) {
	return SDL_GetPerformanceCounter();
}

uint64_t platform_get_performance_frequency(void) {
	return SDL_GetPerformanceFrequency();
}

void platform_sleep(DWORD ms) {
	SDL_Delay(ms);
}

/* ============================================================================
 * 스레드/뮤텍스/이벤트/동적 라이브러리 함수
 * ============================================================================ */
/* platform_thread_t/platform_mutex_t/platform_event_t/platform_lib_t는
   PLATFORM_WINDOWS에서 실제 Win32 HANDLE/HMODULE이다 (Platform.h 참고) -
   MWorkThread.cpp 같은 호출자는 이를 전제로 동작한다(예: platform_thread_create()의
   반환값을 바로 HANDLE로 캐스팅하고, 이벤트 멤버를 실제 CreateEvent()로 생성한 뒤
   platform_event_close()로 닫는 방식). 아래의 SDL 버전은 SDL_Thread 포인터,
   SDL_mutex 포인터, platform_event_s 포인터를 반환하며 이들과 호환되지 않으므로,
   Windows에서는 SDL 구현을 공유하지 않고 진짜 네이티브 구현이 필요하다. */
#ifdef PLATFORM_WINDOWS

platform_thread_t platform_thread_create(platform_thread_func_t func, void* param) {
	return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, param, 0, NULL);
}

int platform_thread_wait(platform_thread_t thread) {
	if (thread == NULL) return 1;
	return (WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0) ? 0 : 1;
}

void platform_thread_close(platform_thread_t thread) {
	if (thread != NULL) {
		CloseHandle(thread);
	}
}

void platform_event_close(platform_event_t event) {
	if (event != NULL) {
		CloseHandle(event);
	}
}

/* platform_mutex_*()/platform_event_create()/platform_event_wait()/
   platform_event_signal()/platform_event_reset()/platform_lib_*()는
   Windows에서 구현되어 있지 않다 - 현재 Windows 빌드에서는 이들을 호출하는 곳이
   없다(Windows에서 실제 동기화 기본 요소/이벤트 생성/동적 로딩이 필요한 코드는
   MWorkThread.cpp처럼 이 추상화 계층 대신 네이티브
   CreateMutex/CreateEvent/SetEvent/LoadLibrary API를 직접 사용한다). */

#else /* !PLATFORM_WINDOWS */

struct ThreadWrapperData {
	platform_thread_func_t func;
	void* param;
};

int SDLCALL ThreadWrapper(void* data) {
	ThreadWrapperData* wrapper = (ThreadWrapperData*)data;
	wrapper->func(wrapper->param);
	delete wrapper;
	return 0;
}

platform_thread_t platform_thread_create(platform_thread_func_t func, void* param) {
	ThreadWrapperData* wrapper = new ThreadWrapperData;
	wrapper->func = func;
	wrapper->param = param;

	return SDL_CreateThread(ThreadWrapper, "thread", wrapper);
}

int platform_thread_wait(platform_thread_t thread) {
	if (thread == NULL) return 1;
	int status = 0;
	SDL_WaitThread(thread, &status);
	return 0;
}

void platform_thread_close(platform_thread_t thread) {
	/* SDL 스레드는 SDL_WaitThread에 의해 자동으로 정리된다 */
	/* 별도의 종료 처리가 필요 없다 */
}

platform_mutex_t platform_mutex_create(int initial_locked) {
	SDL_mutex* mutex = SDL_CreateMutex();
	if (mutex != NULL && initial_locked) {
		SDL_LockMutex(mutex);
	}
	return mutex;
}

int platform_mutex_lock(platform_mutex_t mutex) {
	return (SDL_LockMutex(mutex) == 0) ? 0 : 1;
}

int platform_mutex_unlock(platform_mutex_t mutex) {
	return (SDL_UnlockMutex(mutex) == 0) ? 0 : 1;
}

void platform_mutex_close(platform_mutex_t mutex) {
	if (mutex != NULL) {
		SDL_DestroyMutex(mutex);
	}
}

platform_event_t platform_event_create(int manual_reset, int initial_state) {
	platform_event_t event = new struct platform_event_s;
	if (event == NULL) return NULL;

	event->mutex = SDL_CreateMutex();
	event->cond = SDL_CreateCond();
	event->signaled = initial_state ? 1 : 0;

	if (event->mutex == NULL || event->cond == NULL) {
		if (event->mutex) SDL_DestroyMutex(event->mutex);
		if (event->cond) SDL_DestroyCond(event->cond);
		delete event;
		return NULL;
	}

	return event;
}

int platform_event_wait(platform_event_t event, DWORD timeout) {
	if (event == NULL) return 1;

	SDL_LockMutex(event->mutex);

	/* 이미 시그널 상태라면 즉시 반환한다 */
	if (event->signaled) {
		if (!event->signaled) {
			/* 자동 리셋: 시그널을 해제한다 */
			event->signaled = 0;
		}
		SDL_UnlockMutex(event->mutex);
		return 0;
	}

	/* 시그널을 기다린다 */
	int result = 0;
	if (timeout == PLATFORM_INFINITE) {
		SDL_CondWait(event->cond, event->mutex);
	} else {
		result = SDL_CondWaitTimeout(event->cond, event->mutex, timeout);
	}

	if (event->signaled) {
		result = 0;
		if (!0) { /* manual_reset == 0이면 자동 리셋한다 */
			event->signaled = 0;
		}
	}

	SDL_UnlockMutex(event->mutex);
	return result;
}

int platform_event_signal(platform_event_t event) {
	if (event == NULL) return 1;

	SDL_LockMutex(event->mutex);
	event->signaled = 1;
	SDL_CondSignal(event->cond);
	SDL_UnlockMutex(event->mutex);

	return 0;
}

int platform_event_reset(platform_event_t event) {
	if (event == NULL) return 1;

	SDL_LockMutex(event->mutex);
	event->signaled = 0;
	SDL_UnlockMutex(event->mutex);

	return 0;
}

void platform_event_close(platform_event_t event) {
	if (event != NULL) {
		if (event->mutex) SDL_DestroyMutex(event->mutex);
		if (event->cond) SDL_DestroyCond(event->cond);
		delete event;
	}
}

platform_lib_t platform_lib_load(const char* filename) {
	return SDL_LoadObject(filename);
}

void* platform_lib_get_symbol(platform_lib_t lib, const char* symbol) {
	if (lib == NULL) return NULL;
	return SDL_LoadFunction(lib, symbol);
}

void platform_lib_free(platform_lib_t lib) {
	if (lib != NULL) {
		SDL_UnloadObject(lib);
	}
}

#endif /* PLATFORM_WINDOWS */

/* ============================================================================
 * 파일/경로 함수
 * ============================================================================ */

char platform_get_path_separator(void) {
	return '/';
}

int platform_file_exists(const char* filename) {
	struct stat st;
	return (stat(filename, &st) == 0);
}

/* 아직 Windows에서는 필요하지 않고(현재 Windows 빌드에서는 둘 다 호출되지 않음),
   순수하게 POSIX 전용이다(PATH_MAX, dirname(), readlink(), 2인자 POSIX mkdir()
   시그니처 - Windows의 _mkdir()은 경로만 받는다). */
#ifndef PLATFORM_WINDOWS
int platform_get_executable_dir(char* buffer, size_t size) {
	if (buffer == NULL || size == 0) return 1;

	char path[PATH_MAX] = {0};

	#ifdef PLATFORM_MACOS
		uint32_t bufsize = sizeof(path);
		if (_NSGetExecutablePath(path, &bufsize) != 0) {
			return 1;
		}
	#elif defined(PLATFORM_LINUX)
		ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
		if (count < 0) return 1;
		path[count] = '\0';
	#else
		return 1;
	#endif

	/* 디렉토리를 추출한다 */
	char* dir = dirname(path);
	if (dir == NULL) return 1;

	size_t len = strlen(dir);
	if (len + 1 > size) return 1;

	strcpy(buffer, dir);
	strcat(buffer, "/");
	return 0;
}

int platform_create_directory(const char* path) {
	#ifdef PLATFORM_LINUX
		return mkdir(path, 0755) == 0 ? 0 : 1;
	#else
		return mkdir(path, 0755) == 0 ? 0 : 1;
	#endif
}
#endif /* !PLATFORM_WINDOWS */

/* ============================================================================
 * 키보드 함수
 * ============================================================================ */

int platform_is_ctrl_pressed(void) {
	/* SDL을 통해 키보드 상태를 확인한다 */
	const Uint8* state = SDL_GetKeyboardState(NULL);
	return (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) ? 1 : 0;
}

BYTE platform_get_scan_code(DWORD lParam) {
	/* SDL은 스캔 코드를 직접 사용한다 */
	return (BYTE)lParam;
}

/* ============================================================================
 * 레지스트리/설정 함수
 * ============================================================================ */

/* 아직 Windows에서는 필요하지 않고(현재 Windows 빌드에서는 이들 중 어느 것도
   호출되지 않음), 위의 platform_get_executable_dir()/PATH_MAX에 의존하는데
   이들 자체도 POSIX 전용이라 Windows에서는 막혀 있다. */
#ifndef PLATFORM_WINDOWS

/* 설정 파일 경로 (레지스트리 대체용) */
static char g_config_file_path[PATH_MAX] = {0};

static void get_config_file_path(void) {
	if (g_config_file_path[0] != '\0') return; /* 이미 계산됨 */

	/* 실행 파일 디렉토리를 가져온다 */
	char exeDir[PATH_MAX];
	if (platform_get_executable_dir(exeDir, sizeof(exeDir)) != 0) {
		strcpy(exeDir, "./");
	}

	/* 실행 파일 디렉토리의 설정 파일을 사용한다 */
	snprintf(g_config_file_path, sizeof(g_config_file_path),
	         "%sDarkEden.conf", exeDir);
}

int platform_config_get_string(const char* key, const char* value,
                               char* buffer, DWORD* size) {
	get_config_file_path();

	FILE* file = fopen(g_config_file_path, "r");
	if (file == NULL) return 1;

	char line[512];
	char searchKey[256];
	snprintf(searchKey, sizeof(searchKey), "%s.%s=", key, value);

	int found = 0;
	while (fgets(line, sizeof(line), file) != NULL) {
		if (strncmp(line, searchKey, strlen(searchKey)) == 0) {
			const char* val = line + strlen(searchKey);
			/* 개행 문자를 제거한다 */
			char* newline = strchr(const_cast<char*>(val), '\n');
			if (newline) *newline = '\0';

			size_t len = strlen(val) + 1;
			if (len <= *size) {
				strcpy(buffer, val);
				*size = (DWORD)len;
				found = 1;
			}
			break;
		}
	}

	fclose(file);
	return found ? 0 : 1;
}

int platform_config_set_string(const char* key, const char* value,
                               const char* data) {
	get_config_file_path();

	/* 기존 내용을 읽는다 */
	char* content = NULL;
	long fileSize = 0;

	FILE* file = fopen(g_config_file_path, "r");
	if (file != NULL) {
		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		if (fileSize > 0) {
			content = new char[fileSize + 1];
			fread(content, 1, fileSize, file);
			content[fileSize] = '\0';
		}
		fclose(file);
	}

	/* 쓰기 모드로 연다 */
	file = fopen(g_config_file_path, "w");
	if (file == NULL) {
		if (content) delete[] content;
		return 1;
	}

	/* 기존 내용이 있으면 기록한다 */
	if (content != NULL) {
		fputs(content, file);
		delete[] content;
	}

	/* 새 키-값을 추가한다 */
	fprintf(file, "%s.%s=%s\n", key, value, data);
	fclose(file);

	return 0;
}
#endif /* !PLATFORM_WINDOWS */

/* ============================================================================
 * 오류 보고
 * ============================================================================ */

void platform_show_error(const char* title, const char* message) {
	/* SDL 플랫폼에서는 SDL 메시지 박스로 오류를 표시한다 */
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, NULL) != 0) {
		/* stderr로 대체 출력한다 */
		fprintf(stderr, "ERROR [%s]: %s\n", title, message);
	}
}

/* ============================================================================
 * 초기화
 * ============================================================================ */

int platform_init(void) {
	/* 필요한 SDL 서브시스템을 초기화한다 */
	if (SDL_Init(0) < 0) {
		return 1;
	}
	return 0;
}

void platform_shutdown(void) {
	SDL_Quit();
}
