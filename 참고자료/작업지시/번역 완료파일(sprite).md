# sprite.vcxproj 영어→한글 주석 번역 완료 목록

## 번역 기준
- `sprite.vcxproj`(tools/engine/sprite) 직접 빌드 대상 include/*.h(20개), src/*.c(14개)는
  이전 커밋 `2fb7000`에서 이미 번역 완료. 이번에 전수 재검증(Doxygen 태그, 식별자성 코멘트를
  제외한 순수 영어 문장 검색)했고 남아있는 영어 주석 없음을 확인함
- `tools/engine/sprite/tests/*.c` 11개는 sprite.vcxproj ClCompile에는 없지만 실제 소스코드라
  이번에 함께 번역 완료
- .c, .h 파일만 번역. `tools/engine/sprite/CMakeLists.txt`는 CPP/H가 아니라서 이번 범위에서는
  건드리지 않음(사용자 지시)
- test_assert(condition, "메시지")의 문자열 리터럴과 printf 포맷 문자열은 프로그램 출력이므로
  번역 대상에서 제외. `/* */`, `//`, Doxygen(`@brief` 등) 주석만 번역
- 중국어 주석(예: `数据存储一致性`, `解析正确性`)도 한글로 번역함(기존 방침과 동일)

---

## 완료 - include/*.h, src/*.c (커밋 `2fb7000`, 재검증만 수행, 추가 수정 없음)

- include: SP.h, animation.h, color.h, colorset.h, error.h, frame.h, framepack.h,
  index_sprite.h, index_spritepack.h, map.h, sdl_framework.h, shadow_sprite.h,
  shadow_spritepack.h, sprite.h, spritepack.h, str.h, types.h, ui_surface.h, vector.h, zone.h
- src: animation.c, color.c, colorset.c, frame.c, framepack.c, index_sprite.c,
  index_spritepack.c, sdl_framework.c, shadow_sprite.c, shadow_spritepack.c, sprite.c,
  spritepack.c, ui_surface.c, zone.c

## 완료 - tests/*.c (이번에 신규 번역)

| 파일명 | 번역 내용 |
|--------|-----------|
| `tests/test_main.c` | 간단한 테스트 프레임워크 설명, 테스트 결과 카운터/매크로/외부 함수 주석 |
| `tests/test_zone.c` | zone.h 단위 테스트 설명, 테스트 헬퍼 매크로, 섹션 헤더(테스트/테스트 실행기) |
| `tests/test_sdl_framework.c` | SDL 프레임워크 단위 테스트 전체(NULL 처리, 설정 기본값, 프레임워크 구조체, 초기화/정리, 프레임 지연 계산 등 Doxygen 및 인라인 주석) |
| `tests/test_color.c` | 색상 변환 속성 기반 테스트(RGB565 변환 정확성/Colorkey 투명도 속성, 알려진 값 단위 테스트, 일괄 변환 테스트) |
| `tests/test_frame.c` | Frame/FrameArray 속성 기반 테스트(데이터 일관성/저장), 단위 테스트(초기화, 파일 입출력, 중첩 배열) |
| `tests/test_framepack.c` | CreatureFramePack 테스트(초기화/해제, 파일 입출력, get_frame, CFPK 로딩 정확성 속성, InfoToFile) |
| `tests/test_index_sprite.c` | IndexedSprite/IndexedSpritePack 테스트(ColorSet 초기화·조회, RGB565, ISPK 로딩/지연로딩 속성, 크기 검증, 렌더링 효과) |
| `tests/test_shadow_sprite.c` | ShadowSprite/ShadowSpritePack 테스트(초기화, SSPK 로딩/지연로딩, 크기, 그림자 렌더링/알파 블렌딩) |
| `tests/test_sprite.c` | Sprite 파싱 속성 기반 테스트(RLE 디코딩 왕복 일관성), 단위 테스트(빈/단일픽셀/투명줄/NULL) |
| `tests/test_spritepack.c` | SpritePack 파싱 속성 기반 테스트(파싱 완전성, 지연 로딩 완전성), 단위 테스트(빈 팩, NULL, 잘못된 파일, 인덱스 범위) |
| `tests/test_animation.c` | AnimFrame/AnimObject/블렌드모드 속성 기반 테스트 7개 전체(데이터 일관성, 프레임 순환, 루프 모드, 방향 범위, Sprite ID 계산, BltType 블렌드 매핑) 및 각 단위 테스트 |

코드 로직은 변경하지 않았습니다.

## 이번 범위 제외 파일 (CPP/H가 아님)

- `tools/engine/sprite/CMakeLists.txt` — 영어 주석 존재. 사용자 지시에 따라 이번에는 수정하지 않음.
