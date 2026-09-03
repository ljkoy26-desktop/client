# VS_UI.vcxproj 영어→한글 주석 번역 완료 목록

## 번역 기준
- `build/vs2019/VS_UI.vcxproj` 기준 ClCompile 포함 파일 중 `VS_UI/` 폴더 소속 .cpp 54개 대상
  (vcxproj에는 이 외에 `Client/*.cpp`도 다수 나열되어 있으나, 그건 이미 완료된 DarkEden 프로젝트
  소속이므로 VS_UI 작업 범위에서 제외)
- 이 vcxproj는 ClInclude 항목이 없는 CMake 생성 프로젝트라, 각 cpp가 참조하는 `VS_UI/` 폴더
  내부 헤더(.h)도 함께 대상에 포함
- .cpp, .h 파일만 번역 (vcxproj, md 등 그 외 파일은 미수정)
- 영어 주석을 한글로 번역, 기존 로직/한글 주석 보존, 코드 서식(들여쓰기/중괄호 위치) 미변경
- 순수 함수명만 반복하는 섹션 헤더(예: `// PickUpItem`, `// C_VS_UI::IsRunningStorage`)는 원문 유지
- UTF-8 with BOM 인코딩 유지 (원래 BOM 없이 UTF-8로 저장된 파일은 이번에 BOM 추가 —
  DarkEden.vcxproj의 huffman.cpp에서 BOM 누락으로 MSVC가 CP949로 오인식해 빌드가 깨지는
  사고가 있었기 때문에, 번역 대상 파일은 전부 BOM 유무를 확인/보정함)
- `check_encoding_result.md` 기준 파일 크기를 vcxproj 등장 순서대로 누적해 약 1MB 단위로 배치 진행
  (300KB 이상 대형 파일은 보류)

---

## 1차 번역 완료 (2026-09-04)

vcxproj 순서 기준 첫 배치(`Client_PCH.cpp` ~ `VS_UI_Base.cpp`, MitemTableInit.cpp 제외 약 1026KB):

| 파일명 | 번역 내용 |
|--------|-----------|
| `VS_UI/Client_PCH.cpp` | (번역 불필요 - include 한 줄만 존재, BOM만 추가) |
| `VS_UI/Client_PCH.h` | 파일 헤더(크로스 플랫폼 지원 설명), PLATFORM_WINDOWS 선정의 이유 설명 블록, DirectDraw/DirectSound 실제 헤더 미포함 이유 설명 블록, 플랫폼 추상화 계층 주석 |
| `VS_UI/DebugInfo.cpp` | (번역 불필요 - 주석처리된 코드 블록만 존재, BOM만 추가) |
| `VS_UI/DIK_Defines.h` | 파일 헤더, DIK_* 키 코드 섹션 헤더 전체(알파벳/숫자/숫자패드/기능키/특수키/보조키/구두점/이동키/프린트스크린·스크롤락·일시정지/기타 특수키/일본어·한국어 키/OEM 전용), 각 키 인라인 주석(왼쪽·오른쪽 ALT, 윈도우 키, 화살표 방향, 일본어/한국어 키 설명 등) |
| `VS_UI/RarFile.cpp` | 파일 헤더, 오류 보고 매크로, 생성자/소멸자, 해제, SetRAR(경로 변환 설명), Open(전체 파일 읽기 과정), Read(x2), GetString(줄 단위 읽기 로직), IsEOF, GetList 섹션 헤더 및 인라인 주석 전체 |
| `VS_UI/RarFile.h` | 파일 헤더, 클래스 설명 블록(RAR→디렉터리 매핑 설명), 멤버 변수/함수 인라인 주석 전체 |
| `VS_UI/SXml.cpp` | chXMLEscape 인라인 주석, WideCharToMultiByte 매개변수 설명 블록(lpDefaultChar/lpUsedDefaultChar 상세 설명), 와이드 문자열 변환 인라인 주석, XercesC 관련 죽은 코드 주석(초기화/종료/파서 feature 기본값/닫는 태그/노드 태그 파싱/속성 로드/자식 노드 로드), sobeit 작성 이력 주석(add start/end → 추가 시작/끝) 다수 |
| `VS_UI/SXml.h` | 주석처리된 SAX Handler 인터페이스 설명 2건 |
| `VS_UI/src/AcceleratorDef.cpp` | 전역 변수, 기본 단축키 설정 섹션 헤더 |
| `VS_UI/src/header/AcceleratorDef.h` | sobeit 작성 이력 주석(add start/end → 추가 시작/끝) |
| `VS_UI/src/AcceleratorManager.cpp` | 전역 변수 섹션 헤더, sobeit 작성 이력 주석(add start/end → 추가 시작/끝) 2건 |
| `VS_UI/src/header/AcceleratorManager.h` | (번역 불필요 - 함수명만 반복하는 헤더, BOM만 추가) |
| `VS_UI/src/CWebPage.h` | (번역 불필요 - 영어 설명 주석 없음, BOM만 추가) |
| `VS_UI/src/cwebpage_.h` | 파일 설명 주석 블록 |
| `VS_UI/src/InputFocusManager.cpp` | DXLibBackendSDL용 전역 접근자 주석, 디버그 로그, 포커스 없을 때 무시/전달 관련 인라인 주석 전체(텍스트 입력/편집/키 다운) |
| `VS_UI/src/InputFocusManager.h` | 클래스 설명 블록, 멤버 함수 인라인 주석 전체(싱글턴/포커스 설정·반환/SDL 이벤트 처리/포커스 확인), 전역 접근자 주석 |
| `VS_UI/src/KeyAccelerator.cpp` | 매크로 함수 정의/전역 변수 섹션 헤더, resize/capacity 관련 인라인 주석 2건, 저장 로직 설명 주석(key-accelerator 관계만 저장) |
| `VS_UI/src/header/KeyAccelerator.h` | 기본값 설정/단축키 관련 정의 섹션 헤더 |
| `VS_UI/src/SkinManager.cpp` | (번역 불필요 - 영어 주석 없음, BOM만 추가) |
| `VS_UI/src/header/SkinManager.h` | 인터페이스 ID 인라인 주석 |
| `VS_UI/src/Vs_ui.cpp` | 5608줄. 로컬 정의/함수 프로토타입/전역 데이터/교환 UI/동작 섹션 헤더, 비-Windows 매크로 처리 설명, 사운드용 주석, 단축키·ctrl 무시 설명, UI 전용 처리, ShowItem 테스트 함수 설명 블록, 안전 확인(g_pMoneyManager) 설명 2건, 전역 오브젝트 할당/프로세스 러너/윈도우 매니저 섹션 헤더, 오스터즈 스킬 정보 섹션 헤더, sobeit 작성 이력 주석(add start/end → 추가 시작/끝) 총 33건 일괄 치환. 순수 함수명 반복 헤더(`// PickUpItem`, `// C_VS_UI::IsRunning*` 등)와 이미 한글인 인라인 설명은 원문 유지 |
| `VS_UI/src/VS_UI_Base.cpp` | (번역 불필요 - 이미 전부 한글 주석) |

**보류**: `VS_UI/MitemTableInit.cpp` (801.5KB, 300KB 초과) — `Client/MitemTableInit.cpp`(완료)와 별개 파일. 별도 세션에서 진행 여부를 사용자에게 확인 후 진행 필요.
**보류**: `VS_UI/src/vs_ui_gamecommon2.cpp` (572.3KB, 300KB 초과) — 다음 배치에서 진행 여부 확인 필요.

---

## 다음 번역 대상

1차 배치(약 1026KB) 완료. vcxproj 등장 순서 기준 다음은 `VS_UI/src/VS_UI_BBS.cpp`부터 이어가면 됨.
남은 파일 총량은 약 2670KB(3697KB 전체 - 1026KB 완료 - 대형 보류 2건 합계는 포함 안 됨) 정도이며,
`MitemTableInit.cpp`(801.5KB)와 `vs_ui_gamecommon2.cpp`(572.3KB) 두 대형 파일은 300KB 기준 초과로
매 배치마다 건너뛰고 마지막에 별도 확인 후 처리 권장.
