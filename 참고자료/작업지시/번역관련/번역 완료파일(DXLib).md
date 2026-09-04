# DXLib.vcxproj 영어→한글 주석 번역 완료 목록

## 번역 기준
- `dxlib.vcxproj` 기준 컴파일 대상 cpp 8개 + 그 cpp가 **직접** include하는 헤더 7개 대상 (총 15개)
- .cpp, .h 파일만 번역 (`Client/DXLib/CMakeLists.txt`, `Client/DXLib/README.md`는 영어 본문이
  남아있으나 CPP/H가 아니라서 이번 범위에서는 제외)
- 영어 주석을 한글로 번역, 기존 로직/한글 주석 보존
- UTF-8 with BOM 인코딩 유지, 문자열 리터럴(오류 메시지 등)은 번역 대상에서 제외

---

## 번역 완료 (커밋 `fd1c78e`)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/DXLib/CDirectDraw.cpp` | SDL2 구현 설명 헤더, Flip()이 별도 파일(CSDLGraphicsFlip.cpp)에 구현된 이유, 정적 멤버 초기화, 생성자/소멸자 스텁, hWnd를 SDL_CreateWindowFrom()으로 래핑하는 이유, 5:6:5 포맷 비트마스크 메서드 주석 |
| `Client/DXLib/CDirectDraw.h` | 크로스플랫폼 include, Windows 타입 호환성, DirectDraw 구조체 스텁(DDPIXELFORMAT/DDSCAPS2/DDSURFACEDESC2/DDGAMMARAMP) 설명, 색상 변환·비트마스크·속성·Init·Display·InitMask 메서드 섹션 헤더, DirectDraw Surface Description 필드 설명 |
| `Client/DXLib/CDirectDrawSurface.cpp` | SDL2 구현/클래스 폐기(deprecated) 설명, 서피스 초기화·조작·Lock/Unlock·블리팅·감마 보정·GDI 텍스트·BMP 저장/로드 섹션 헤더, 각 스텁 함수의 "Not implemented - use SDL___ 대신 사용" 주석 다수 |
| `Client/DXLib/CDirectDrawSurface.h` | 디버그용 텍스트 출력, 감마/Init/비트맵 로드/서피스 복구/투명색 설정/Blt/Get/Lock·Unlock/연산자 섹션 헤더, 서피스 디스크립터 필드 설명 |
| `Client/DXLib/CDirectSound.cpp` | SDL2(SDL_mixer) 구현/폐기 설명, 초기화·음소거·볼륨·주파수·팬·사운드 버퍼·재생 제어·오류 처리 섹션 헤더, "Not implemented - use Mix_* 대신 사용" 주석 다수 |
| `Client/DXLib/CDirectSound.h` | 플랫폼 독립 include, DirectSound 타입 전방 선언 설명, Init/Release·Load/Release/Duplicate·Play/Stop·Mute·Frequency·Volume 섹션 헤더 |
| `Client/DXLib/CDirectMusic.cpp` | SDL2_mixer 구현/폐기 설명, 초기화·재생 제어·템포 제어 섹션 헤더, "Not implemented - use Mix_* 대신 사용" 주석 다수 |
| `Client/DXLib/CDirectMusic.h` | DirectMusic 타입 전방 선언 설명, Init/Release·Play/Stop/Pause/Resume·Tempo·Volume·port type·Protected Functions 섹션 헤더 |
| `Client/DXLib/CDirectSoundStream.cpp` | SDL2_mixer 스트리밍 구현/폐기 설명, E_FAIL 호환 정의, 스트림 조작·알림 및 갱신·볼륨 제어 섹션 헤더, "Not implemented" 주석 다수 |
| `Client/DXLib/CDirectSoundStream.h` | 플랫폼 독립 include, DirectSound 타입 전방 선언, 타입 정의 주석 |
| `Client/DXLib/CDirectDraw_StaticMembers.cpp` | 정적 멤버 정의/추가 정적 멤버 섹션 헤더 |
| `Client/DXLib/CDirectInput_Adapter.cpp` | Platform.h/키 코드 include 설명, DirectInput 타입 전방 선언, 생성자/소멸자/입력 상태 초기화/SDL 백엔드 초기화·해제·갱신 함수 설명, 키보드/마우스 상태 갱신·이벤트 트리거 주석, 마우스 위치/속도/가속도 스텁 설명 |
| `Client/DXLib/CDirectInput.h` | 백엔드 선택, 입력 백엔드 인터페이스 전체(Init/Release/Update/키 상태/마우스 위치·휠·버튼/텍스트 입력·IME 콜백), 사운드/뮤직/스트림 백엔드 인터페이스 전체(핸들, Init/Load/Play/Stop/Volume/Pan/Frequency 등), 백엔드 정보 조회 함수의 Doxygen 스타일 파라미터 설명 전체 번역 |
| `Client/DXLib/DXLibBackend.h` | CDirectInput.h/입력 포커스 매니저/MP3·OGG include 설명, DIK 상수 폴백 정의, ALT 키 별칭, 내부 상태(입력/텍스트 콜백/레거시 마우스 좌표/DIK-SDL 매핑 테이블) 섹션 헤더 |
| `Client/DXLib/DXLibBackendSDL.cpp` | 입력 백엔드 구현부(키 매핑 초기화 - 알파벳/숫자/기능키/특수키/방향키/기호/키패드, SDL 이벤트 갱신, 텍스트 입력/IME 조합 처리, 레거시 g_x/g_y 갱신) 주석, 사운드 백엔드(SDL_mixer 초기화, 패닝/주파수 미지원 설명) 주석, 뮤직/스트림 백엔드 구현 주석 |

코드 로직은 변경하지 않았습니다.

## 이번 범위 제외 파일 (dxlib.vcxproj에서 컴파일되지 않는 미사용 파일)

어떤 vcxproj에서도 참조되지 않는 것으로 확인됨 (huffman/mp3/ogg 디코더, 구버전 어댑터 등):

- Client/DXLib/BIT_RES.CPP, Client/DXLib/BIT_RES.H
- Client/DXLib/CDirectInput.cpp (CDirectInput_Adapter.cpp만 컴파일됨)
- Client/DXLib/CDirectMusic_Adapter.cpp
- Client/DXLib/CDirectSetupGetVersion.cpp, Client/DXLib/CDirectSetup.h
- Client/DXLib/CDirectSound_Adapter.cpp, Client/DXLib/CDirectSoundStream_Adapter.cpp
- Client/DXLib/header.cpp, Client/DXLib/header.h
- Client/DXLib/huffman.cpp, Client/DXLib/huffman.h
- Client/DXLib/mp3.cpp, Client/DXLib/mp3.h
- Client/DXLib/reader.cpp, Client/DXLib/reader.h
- Client/DXLib/soundbuf.cpp, Client/DXLib/soundbuf.h
- Client/DXLib/subdecoder.cpp
- Client/DXLib/synfilt.cpp
- Client/DXLib/codec.h, Client/DXLib/common.h, Client/DXLib/config_types.h
- Client/DXLib/DXLib.h, Client/DXLib/l3types.h, Client/DXLib/ogg.h
- Client/DXLib/os_types.h, Client/DXLib/vorbisenc.h, Client/DXLib/vorbisfile.h
