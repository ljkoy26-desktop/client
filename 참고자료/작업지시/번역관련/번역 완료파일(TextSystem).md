# TextSystem.vcxproj 영어→한글 주석 번역 완료 목록

## 번역 기준
- `TextSystem.vcxproj` 기준 ClCompile 포함 cpp 3개(TextService.cpp, TextBackendSDL.cpp,
  RenderTargetSpriteSurface.cpp)와 그 cpp가 **직접** include하는 헤더 6개 대상
- .cpp, .h 파일만 번역 (CMakeLists.txt 등 그 외 파일은 없어 해당 없음)
- 영어 주석을 한글로 번역, 기존 로직/한글 주석 보존
- UTF-8 with BOM 인코딩 유지 (TextService.cpp, TextBackendSDL.cpp는 원래 CP949로 저장되어
  있었고 일부 한글 주석이 Read 시 깨져 보였을 뿐 실제로는 정상 한글 주석이었음 — 재번역하지
  않고 UTF-8 with BOM으로만 재저장), 줄바꿈은 원본과 동일하게 CRLF 유지
- fprintf 오류 메시지 같은 문자열 리터럴(프로그램 출력)은 번역 대상에서 제외

---

## 1차 번역 완료 (커밋 `bf0f3fe`)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/TextSystem/TextService.h` | 전역 텍스트 렌더링 API 설명, 인코딩 정규화(CP949/EUC-KR→UTF-8) 주석 |
| `Client/TextSystem/TextService.cpp` | 전역 서피스 포인터, SDL 백엔드 팩토리 전방 선언, 인코딩 정규화, UTF-8 디코딩(2/3/4바이트 시퀀스, 잘린 시퀀스, 대체 문자) 주석, DrawLine의 baseline/bearingY 좌표 계산 설명 블록, RenderText의 호환성 API 설명 |
| `Client/TextSystem/TextBackendSDL.cpp` | 폰트 폴백 경로 설명 블록(Windows 폰트 미포함 문제, Malgun Gothic/YaHei/Arial 폴백 사유), 글리프 메트릭 근사(Fallback) 주석, bearingY/miny/ascent 좌표 계산 설명. 이미 한글이었으나 CP949 원본이라 Read 시 깨져 보이던 주석은 UTF-8로 재저장만 함(재번역 없음) |
| `Client/TextSystem/FontHandleUtil.h` | 폰트 크기 핸들 인코딩 함수 설명, 매직 태그 주석 |

## 변경 없음 (영어 주석 없어 수정 불필요)

| 파일명 | 비고 |
|--------|------|
| `Client/TextSystem/RenderTargetSpriteSurface.cpp` | 영어 주석 없음 |
| `Client/TextSystem/RenderTargetSpriteSurface.h` | 영어 주석 없음 |
| `Client/TextSystem/TextBackend.h` | 영어 주석 없음 |
| `Client/TextSystem/TextTypes.h` | 영어 주석 없음 |
| `Client/TextSystem/RenderTarget.h` | 영어 주석 없음 |

총 9개 파일 확인 완료. `Client/TextSystem/` 폴더에는 CMakeLists.txt나 README 같은
CPP/H 외 파일이 없어 이번 범위에서 번역을 미룰 대상 자체가 없습니다.
