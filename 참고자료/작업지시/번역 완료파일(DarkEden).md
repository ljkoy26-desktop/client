# DarkEden.vcxproj 영어→한글 주석 번역 완료 목록

## 번역 기준
- vcxproj 파일 기준 ClCompile 포함 파일 대상
- .cpp, .h 파일만 번역 (프로젝트 파일 등은 제외)
- 영어 주석을 한글로 번역, 기존 한글/로직 보존

---

## 1차 번역 완료 (2026-09-02)

아래 파일들은 영어 주석을 한글로 번역 완료 (vcxproj 줄 432~460 대상):

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/APICheck.cpp` | 비-Windows 플랫폼 스텁 주석 |
| `Client/AppendPatchInfo.cpp` | 섹션 헤더(Save/Load/Release/Set/Execute/Check/Calculate/GetAllSize) |
| `Client/BIT_RES.CPP` | 비트 스트림 읽기/쓰기 주석 |
| `Client/BloodBibleSignInfo.cpp` | include files / constructor / destructor |
| `Client/CAvi.cpp` | MCI Digital Video 스텁 설명 블록 주석 |
| `Client/CGHandlersStub.cpp` | 파일 헤더 및 인라인 주석 |
| `Client/CGameUpdate.cpp` | 전역 변수, SDL 마이그레이션 주석 |
| `Client/CMP3.cpp` | __USE_MP3__ 비활성화 설명, 비-Windows 스텁 주석 |
| `Client/CMessageArray.cpp` | 플랫폼별 포함/I/O/파일 플래그 주석 |
| `Client/CNormalSpriteTexturePartManager.cpp` | SDL 제거 주석, 생성자/소멸자 헤더 |
| `Client/COGGSTREAM.CPP` | SDL 백엔드 미구현 주석 |
| `Client/COpeningUpdate.cpp` | 전역 변수, 초기화, 오프닝 AVI, 입력 업데이트 |
| `Client/CSDLGraphicsFlip.cpp` | (영어 주석 없음, 수정 불필요) |
| `Client/CServerInformation.cpp` | 전역 변수 섹션 헤더 |
| `Client/CShadowPartManager.cpp` | SDL 마이그레이션 주석, DX 제거 주석 |
| `Client/CSoundPartManager.cpp` | base class release → 기반 클래스 해제 |
| `Client/CSpriteTexturePartManager.cpp` | SDL 제거 주석, 생성자/소멸자/멤버함수 헤더 |
| `Client/CSystemInfo.cpp` | VC6/x86 구현 설명, MMX/SSE/3DNow/HT 주석, 비-Windows 스텁 |
| `Client/CTexturePartManager.cpp` | SDL 제거 주석, 생성자/소멸자/멤버함수 헤더 |
| `Client/CToken.cpp` | 생성자/소멸자/멤버함수 섹션 헤더 |
| `Client/CUsingColorSetTable.cpp` | 전역/생성자소멸자/멤버함수/Enable/Disable/Save/Load 헤더 |
| `Client/CWaitPacketUpdate.cpp` | 전역/초기화/대기위치업데이트/지연시간설정 헤더 |
| `Client/CWaitUIUpdate.cpp` | MMSystem 주석, DIK→VK 변환 주석, WM_TEXTINPUT/EDITING 주석 |
| `Client/Client.cpp` | 전역/포함파일/함수 설명/윈도우 메시지 처리 주석, 중국어 주석 한글화 |
| `Client/ClientConfig.cpp` | 중국어 주석(기본값 사용) → 한글 |
| `Client/ClientFunction.cpp` | 전방선언/사운드정리/재생/귓속말ID/알파박스 관련 주석 |
| `Client/ClientTimeFunctions.cpp` | 파일 헤더 및 함수 설명 주석 |
| `Client/Client_PCH.cpp` | (번역 불필요 - include만 존재) |
| `Client/CrashReport.cpp` | 예외 처리 설명, StackWalk64, 스택프레임 주석, 비-Windows 스텁 |

---

## 2차 번역 완료 (2026-09-02)

아래 파일들은 영어 주석을 한글로 번역 완료 (vcxproj 줄 461~467 대상):

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/DebugInfo.cpp` | (번역 불필요 - 주석처리된 코드 블록만 존재) |
| `Client/DebugKit.cpp` | 파일 헤더, 생성자/소멸자 섹션 헤더 |
| `Client/DebugLog.cpp` | 파일 헤더, 설정/스레드 안전성/헬퍼 함수/공개 인터페이스/핵심 로깅 함수 섹션 헤더 및 인라인 주석 전체 |
| `Client/DirectXLib_PCH.cpp` | (번역 불필요 - include 한 줄만 존재) |
| `Client/DrawCreatureDivineGuidance.cpp` | (번역 불필요 - 주석처리된 코드 블록만 존재) |
| `Client/DrawCreatureEffect.cpp` | (번역 불필요 - 주석처리된 코드 블록만 존재) |
| `Client/DrawCreatureShadow.cpp` | 섹션 헤더(크리처 그림자 출력), SDL 크래시 방지 주석, SDL2 제거 주석 |

---

---

## 3차 번역 완료 (2026-09-02)

아래 파일들은 영어 주석을 한글로 번역 완료 (vcxproj 줄 468~471 대상):

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/EffectResourceContainer.cpp` | 생성자/소멸자, Load/LoadFramePacks/LoadSpritePacks/LoadPalettePacks/GetFramePack/GetEffectFrame/GetSprite/GetAlphaSprite/GetShadowSprite/GetPalette 섹션 헤더 및 인라인 주석 |
| `Client/ExpInfo.cpp` | (번역 불필요 - 영어 설명 주석 없음) |
| `Client/ExperienceTable.cpp` | 전역 변수, 생성자/소멸자, Load From File 시리즈, Get Info 시리즈 섹션 헤더 |
| `Client/FameInfo.cpp` | (번역 불필요 - 영어 설명 주석 없음) |

---

## 다음 번역 대상 (vcxproj 기준 이후 파일들)

vcxproj 줄 472 이후 파일들은 미번역 상태.
