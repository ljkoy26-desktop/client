# basic.vcxproj 영어→한글 주석 번역 완료 목록

## 번역 기준
- `basic.vcxproj` 기준 폴더 전체 CPP/H 파일 대상 (27개)
- .cpp, .h 파일만 번역 (`basic/CMakeLists.txt`, `basic/README.md`는 영어 본문이 남아있으나
  CPP/H가 아니라서 이번 범위에서는 제외)
- 영어 주석을 한글로 번역, 기존 로직/한글 주석 보존
- UTF-8 with BOM 인코딩 유지

---

## 번역 완료 (커밋 `cb1c248`)

| 파일명 | 번역 내용 |
|--------|-----------|
| `basic/2d.h` | DDraw.h를 include하지 않는 이유 설명 블록 |
| `basic/AudioTypes.h` | 파일 헤더, 볼륨/버퍼 기능/멀티미디어 구조체 섹션 헤더, DirectSound 상수 설명, MMCKINFO/DSBPOSITIONNOTIFY 헤더 가드 충돌 방지 설명 |
| `basic/BasicData.cpp` | 파일 헤더(macOS 포팅 설명) |
| `basic/BasicData.h` | Point/Rect 클래스 헤더 |
| `basic/BasicException.cpp` | new 핸들러, 기본 예외 처리기, 디버그/릴리즈 모드 분기 주석 |
| `basic/BasicException.h` | EXCEPTION_CODE 각 항목 설명(메모리 할당/파일 열기/로드/작업/null 참조 실패) |
| `basic/BasicMemory.h` | 파일 헤더, 메모리 해제 매크로 설명 |
| `basic/Basics.h` | 파일 헤더 |
| `basic/ColorDraw.cpp` | 정적 멤버 정의, ARGB 4:4:4:4 포맷, InitMask 함수 설명, RGB 5:6:5 필수값 경고 주석 |
| `basic/ColorDraw.h` | 파일 헤더, 색상 성분 추출/픽셀 포맷 감지/색상 생성/변환/시프트값/마스크 배열/초기화 섹션 헤더 |
| `basic/DLL.h` | 파일 헤더, `export`/`import` 매크로 충돌 회피 설명 블록, 비Windows 빈 매크로 주석 |
| `basic/Directory.cpp` | 파일 헤더, 생성자/소멸자·조회 함수·검사 함수 섹션 헤더, 실행 파일 디렉토리/경로 버퍼 할당 주석 |
| `basic/Directory.h` | 조회/설정·검사/외부 참조 섹션 헤더 |
| `basic/GL_import.cpp` | 파일 헤더, 비Windows 함수 포인터 정의 주석 |
| `basic/GL_import.h` | putPixel/getPixel/vertline/horzline/Bltz 등 함수 포인터 주석, DDSURFACEDESC 미사용 설명 블록 |
| `basic/IMG.h` | 파일 헤더, IMG 클래스/헤더 섹션, 값 조회 주석 |
| `basic/InputCodes.h` | 파일 헤더, 키 코드 섹션 헤더(문자/숫자/기능키/특수키/보조키/탐색키/숫자패드/기타), DIK_* 호환 별칭 설명 블록, 각 키 인라인 주석(백스페이스, Enter, 방향키 등) |
| `basic/PS.h` | 파일 헤더 |
| `basic/Platform.h` (2038줄) | Windows API 비Windows 스텁 함수/상수 설명 주석 다수(CRITICAL_SECTION, GDI, DirectDraw/DirectSound 상수, WNDCLASS, 메시지 매크로, WideCharToMultiByte 등), 플랫폼 감지/선택, 호출 규약, 기본 타입 정의 섹션 헤더 |
| `basic/PlatformSDL.cpp` | 시간/스레드·뮤텍스·이벤트·동적라이브러리/파일·경로/키보드/레지스트리·설정/오류 보고/초기화 섹션 헤더, SDL 스레드 정리, 설정 파일 읽기/쓰기 주석 |
| `basic/PlatformUtil.h` | 파일 헤더, 키보드 스캔 코드 매크로, 공개 인터페이스 섹션 헤더 |
| `basic/TGA.h` | Tga 헤더 구조체, colormaptype/bpp 인라인 주석, VC word align 회피 설명, TGA 클래스/이미지 정보 조회/파일 로드/블리팅 섹션 헤더 |
| `basic/Timer2.cpp` | 파일 헤더, 전역 인스턴스/생성자/소멸자 및 Add/Delete/Execute/Refresh/Pause/Continue/ResetSpeed 각 함수 설명, 큐 확장/일시정지 상태 주석 |
| `basic/Typedef.h` | 파일 헤더, Platform.h include 설명 |
| `basic/i_signal.h` | 파일 헤더, 키보드 스캔 코드/키보드·마우스 시그널 정의 섹션 헤더, 억음부호/메인 키보드 인라인 주석 |
| `basic/picinfo.h` | 그림 파일 정보 섹션, 이미지 데이터 포인터/bpp/파일명 인라인 주석 |
| `basic/timer2.h` | 파일 헤더, Timer 클래스 설명, 일시정지 상태/실행할 메서드/타이머 큐 주석 |

코드 로직은 변경하지 않았으며, 문자열 리터럴(오류 메시지 등)은 번역 대상에서 제외했습니다.
