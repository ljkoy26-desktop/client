# framelib.vcxproj 영어→한글 주석 번역 완료 목록

## 번역 기준
- `framelib.vcxproj` 기준 ClCompile 포함 cpp 6개(CAnimationFrame.cpp, CAnimationFrameSetManager.cpp,
  CCreatureFrameSetManager.cpp, CFrame.cpp, CFramePack.cpp, CFrameSetManager.cpp)와 그 cpp가
  **직접** include하는 헤더 11개(CAnimationFrame.h, CAnimationFrameSetManager.h,
  CCreatureFrameSetManager.h, CFrame.h, CFramePack.h, CFrameSet.h, CFrameSetManager.h,
  DrawTypeDef.h, FR.h, Frame_PCH.h, TArray.h) 대상 (총 17개)
- .cpp, .h 파일만 번역. `Client/framelib/CMakeLists.txt`는 CPP/H가 아니라서 이번 범위에서는
  건드리지 않음(사용자 지시)
- 영어 주석을 한글로 번역, 기존 로직/한글 주석 보존
- UTF-8 with BOM 인코딩 유지

---

## 경과
최초 `9ec1e7c` 커밋 당시에는 "CFrameSet.h의 한 줄만 영어"라고 기록되어 있었으나, 이번에 다시
전수 확인한 결과 대부분의 파일에 `constructor/destructor`, `member functions`, `Save To File`,
`Load From File`, `Init/Release`, `file I/O`, `operator`, `Template Array class` 등 섹션 헤더
영어 주석이 다수 남아있는 것을 발견하여 추가로 번역했습니다(SpriteLib의 TArray.h/CTypePack.h 등과
거의 동일한 구조).

## 번역 완료

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/framelib/CAnimationFrame.cpp` | 생성자/소멸자, 멤버함수, 파일로 저장/파일에서 로드 헤더 |
| `Client/framelib/CAnimationFrame.h` | [파일 입출력] 섹션, file I/O 헤더, Current/Max Frame 인라인 주석, "class" → "클래스" |
| `Client/framelib/CAnimationFrameSetManager.cpp` | 생성자/소멸자, 멤버함수, SpriteSet Index 저장 헤더 |
| `Client/framelib/CAnimationFrameSetManager.h` | "class" → "클래스" (파일 상단 스테일 파일명 주석 `CThingFrameSetManager.h`은 로직 무관이라 유지) |
| `Client/framelib/CCreatureFrameSetManager.cpp` | 생성자/소멸자, 멤버함수, SpriteSet Index 저장 헤더 |
| `Client/framelib/CCreatureFrameSetManager.h` | "class" → "클래스" |
| `Client/framelib/CFrame.cpp` | 멤버함수, 설정, 파일로 저장/파일에서 로드, 대입 연산자 헤더(CFrame/CEffectFrame 양쪽) |
| `Client/framelib/CFrame.h` | 설정/파일 입출력/조회/대입/플래그/플래그 설정·해제 섹션 헤더, Frame Array 데이터 타입 정의, 프레임 배열/방향 프레임 배열/액션 프레임 배열/이펙트 프레임 주석 |
| `Client/framelib/CFramePack.cpp` | 파일에 정보 저장 헤더(주석처리된 코드 포함, 2곳) |
| `Client/framelib/CFramePack.h` | 파일 입출력, 기반 클래스 멤버 스코프 주석, 생성자/소멸자, 멤버함수, 파일로 저장 헤더 |
| `Client/framelib/CFrameSet.h` | 초기화/해제, 파일 입출력, 연산자, 생성자/소멸자, 멤버함수, 초기화, 해제, 파일에서 로드 헤더 |
| `Client/framelib/CFrameSetManager.cpp` | 생성자/소멸자, 멤버함수, FrameSet Index 저장 헤더 |
| `Client/framelib/CFrameSetManager.h` | 이미 한글, 수정 불필요 |
| `Client/framelib/DrawTypeDef.h` | 영어 주석 없음(sizeof 주석은 식별자성 표기라 유지), 수정 불필요 |
| `Client/framelib/FR.h` | 이미 한글, 수정 불필요 |
| `Client/framelib/Frame_PCH.h` | 영어 주석 없음, 수정 불필요 |
| `Client/framelib/TArray.h` | 템플릿 배열 클래스, 초기화/해제, 크기 얻기, 파일 입출력, 연산자 오버로딩, 정적 데이터 멤버 초기화, 생성자/소멸자, 멤버함수, 초기화, 해제, *this에 Array 추가, 파일로 저장, 파일에서 로드, 대입 헤더 |

코드 로직은 변경하지 않았습니다.

## 이번 범위 제외 파일 (CPP/H가 아님)

- `Client/framelib/CMakeLists.txt` — 영어 주석 다수(FrameLib 설명, Collect source files,
  Remove precompiled header source, Create library, Include directories, Packet 관련 NOTE,
  Link libraries). 사용자 지시에 따라 이번에는 수정하지 않음.
