# sprite 관련 추가 번역 필요 파일 목록

`sprite.vcxproj` 직접 빌드 대상(include/*.h, src/*.c) 외에 `tools/engine/sprite` 디렉터리에 속해 있는 파일들 중, 추후 한글 번역이 필요한 파일 목록입니다.

## 1. 빌드 스크립트
- `tools/engine/sprite/CMakeLists.txt`

## 2. 단위 테스트 코드 (`tools/engine/sprite/tests/`)
- `tools/engine/sprite/tests/test_animation.c` : 애니메이션 프레임 및 오브젝트 테스트
- `tools/engine/sprite/tests/test_color.c` : 색상 변환(RGB565/555 -> RGBA32) 테스트
- `tools/engine/sprite/tests/test_frame.c` : Frame 및 FrameArray 단위/속성 테스트
- `tools/engine/sprite/tests/test_framepack.c` : CreatureFramePack 테스트
- `tools/engine/sprite/tests/test_index_sprite.c` : IndexedSprite 및 IndexedSpritePack 테스트
- `tools/engine/sprite/tests/test_main.c` : 테스트 러너 및 테스트 프레임워크
- `tools/engine/sprite/tests/test_sdl_framework.c` : SDL2 프레임워크 단위 테스트
- `tools/engine/sprite/tests/test_shadow_sprite.c` : ShadowSprite 및 ShadowSpritePack 테스트
- `tools/engine/sprite/tests/test_sprite.c` : Sprite 파싱 및 렌더링 테스트
- `tools/engine/sprite/tests/test_spritepack.c` : SpritePack 파싱 테스트
- `tools/engine/sprite/tests/test_zone.c` : Zone(맵) 파일 파싱 테스트
