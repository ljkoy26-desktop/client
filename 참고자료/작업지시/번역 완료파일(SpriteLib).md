# SpriteLib 영어→한글 주석 번역 완료 목록

## 번역 기준
- 1차: `build/vs2019/Client/SpriteLib/SpriteLib.vcxproj` (CMake 생성본) 기준 ClCompile 포함 파일 + 그 파일들이
  include하는 Client/SpriteLib 내부 헤더(및 텍스트 인클루드되는 CSpriteSurface_Adapter.cpp), Client/DebugLog.cpp/.h 대상
- 2차: `Client/SpriteLib/SpriteLib.vcxproj` (레거시 VC6/Win32 프로젝트, ClCompile+ClInclude 명시) 기준 추가 대상 확인
- .cpp, .h 파일만 번역 (그 외 파일 미수정)
- 영어 주석을 한글로 번역, 기존 로직/한글 주석 보존
- Allman 스타일, 헝가리언 표기법, Guard Clause, UTF-8 with BOM 유지

---

## 1차 대상 파일 목록 (총 66개, CMake vcxproj 기준)

### 진행 중 / 대기

(없음 - 66개 전체 완료)

### 번역 완료

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/SpriteLib/DrawTypeDef.h` | 영어 주석 없음, 수정 불필요 |
| `Client/SpriteLib/MPalettePack.h` | 주석처리된 코드의 Init/Release, file I/O 헤더 |
| `Client/SpriteLib/MPalettePack.cpp` | 영어 주석 없음(전부 주석처리된 코드), 수정 불필요 |
| `Client/SpriteLib/CAlphaSprite555.h` | video card/sprite class, save/load 문구 |
| `Client/SpriteLib/CIndexSprite555.h` | video card/sprite class, save/load 문구 |
| `Client/SpriteLib/CIndexSprite565.h` | video card/sprite class, save/load 문구 |
| `Client/SpriteLib/CAlphaSprite565.h` | video card/sprite class, save/load 문구 |
| `Client/SpriteLib/CSprite_SDL.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSprite565.h` | video card/sprite class, save/load 문구 |
| `Client/SpriteLib/CSprite555.h` | video card/sprite class, save/load 문구 |
| `Client/SpriteLib/CSpriteDef.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpriteOutlineManager.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpriteOutlineManager.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpriteDef.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpritePack.h` | 주석처리된 코드의 Init/Release, file I/O 헤더 |
| `Client/SpriteLib/CSpriteSetManager.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CFileIndexTable.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CFilterPack.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CStorageSurface.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpriteSet.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpritePackList.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpritePackList.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpritePal.h` | Blt functions/Effect/Utility Functions 헤더 |
| `Client/SpriteLib/CSpritePalBase.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CAlphaSpritePack.h` | Init/Release, file I/O, operator 헤더 |
| `Client/SpriteLib/CAlphaSpritePal.h` | Constructor/destructor, Blt functions, Alpha Blending, Utility Functions 등 헤더 |
| `Client/SpriteLib/CShadowSprite.h` | save/load, get functions, Backend sprite management, Test Code, Effect, Alpha Blending, Utility Functions 등 헤더 |
| `Client/SpriteLib/CFilter.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CFilterPack.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CFileIndexTable.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSprite.cpp` | Static member/생성자소멸자/멤버함수/대입연산자/Alpha Channel Blending 등 섹션 헤더 다수 (Blt* 계열 식별자성 헤더는 유지) |
| `Client/SpriteLib/CSprite.h` | Init/Release, save/load 문구 |
| `Client/SpriteLib/CIndexSprite.h` | memory Release, save/load 문구 |
| `Client/SpriteLib/CIndexSprite.cpp` | Static Member/memcpy 효과/생성자소멸자/멤버함수/대입연산자/Is ColorPixel/Get Pixel/test code 등 섹션 헤더 다수 (Blt* 계열 식별자성 헤더는 유지) |
| `Client/SpriteLib/CFilter.cpp` | 생성자/소멸자, 멤버함수, Init/Release, Set Filter, Save/Load To File 헤더 |
| `Client/SpriteLib/CSpritePalBase.cpp` | 이미 한글, 수정 불필요 (MessageBox 문자열은 로직이므로 미변경) |
| `Client/SpriteLib/CSpriteSet.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpriteSetManager.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CStorageSurface.cpp` | Init/Release/Store/Restore 헤더 |
| `Client/SpriteLib/CTypePack.h` | Init/Release, Size, operator, file I/O, Load From File Running 헤더 (CTypePack/CTypePack2 양쪽) |
| `Client/SpriteLib/CSetManager.h` | Template Sorted List, Init/Release, add/remove, file I/O, Get functions, 생성자소멸자, 멤버함수 등 헤더 |
| `Client/SpriteLib/MPalette.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/MPalette.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/TArray.h` | Template Array class, Init/Release, Get Size, File I/O, Operator overloading, 생성자소멸자 등 헤더 |
| `Client/SpriteLib/SpriteLibBackend.h` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/SpriteLibBackendSDL.h` | 전체 영어 Doxygen 주석 한글 번역 (구조체/함수 설명 전체) |
| `Client/SpriteLib/SpriteLibBackendSDL.cpp` | 대부분 이미 한글, 중국어 주석 2줄 한글화 |
| `Client/SpriteLib/CAlphaSprite.h` | memory Release, save/load, get functions, Backend sprite management, Blt 4444, Alpha Blending, Effect, Utility Functions, debug function 등 헤더 |
| `Client/SpriteLib/CAlphaSprite.cpp` | Static member/생성자소멸자/멤버함수/대입연산자/Is ColorPixel/Get Pixel/AlphaChannel Copy/Alpha Channel Blending 등 헤더 (Blt* 계열 식별자성 헤더는 유지) |
| `Client/SpriteLib/CAlphaSpritePack.cpp` | minimap sprite loading 주석 |
| `Client/SpriteLib/CAlphaSpritePal.cpp` | Constructor/destructor for vtable, AlphaChannel Copy, Alpha Channel Blending 등 헤더 |
| `Client/SpriteLib/CShadowSprite.cpp` | Static member/Init Static member/생성자소멸자/멤버함수/Is ColorPixel/ShadowDarkness Copy 등 헤더 |
| `Client/SpriteLib/CSpritePal.cpp` | AlphaChannel Copy, Alpha Channel Blending, Alpha Copy 헤더 |
| `Client/SpriteLib/CSpritePack.cpp` | 전부 주석처리된 코드, Load From File Running 헤더 번역 |
| `Client/SpriteLib/CSpriteSurface.h` | Lock & Draw Functions, Sprite/Filter/AlphaSprite/IndexSprite/Sprite Outline/ShadowSprite 섹션 헤더 (Windows 백엔드 분기, memcpyEffect류 식별자 헤더는 유지) |
| `Client/SpriteLib/CSpriteSurface_Adapter.cpp` | 이미 한글, 수정 불필요 |
| `Client/SpriteLib/CSpriteSurface_Effects.cpp` | 파일 헤더 설명, Effect SimpleOutline/WipeOut/Net 헤더 (일부 손상된 인코딩 블록은 별도 이슈로 미변경) |
| `Client/SpriteLib/CSpriteSurface_SDL.cpp` | 이미 한글, 수정 불필요 |
| `Client/DebugLog.h` | 파일 헤더, Log Level Enumeration, Public Interface, Logging Macros, Backward Compatibility 등 전체 영어 주석 번역 |
| `Client/DebugLog.cpp` | Windows path separator, 기본 설정값 주석 번역 |
| 추가 보완 | CFilter.cpp(Size Load), CStorageSurface.cpp(clipping x2), CAlphaSpritePack.h(size), CSetManager.h(size), CTypePack.h(runtime loading/UB 주석/Validate sprite index/파일 오픈 확인/이동·복사 생성자 금지 주석) 누락분 추가 번역 |

(66개 파일 전체 번역 완료 - 2026-09-02)

---

## 2차 대상 파일 목록 (레거시 `Client/SpriteLib/SpriteLib.vcxproj` 기준 추가분)

레거시 프로젝트는 ClCompile/ClInclude를 명시적으로 나열하고 있어, 1차 목록과 대조해 아래 7개 파일이
새로 추가 확인됨.

### 번역 완료
| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/SpriteLib/CIndexSpritePack.cpp` | 전부 주석처리된 코드, Load From File Running 헤더 번역 |
| `Client/SpriteLib/CIndexSpritePack.h` | 주석처리된 코드의 Init/Release, file I/O 헤더 |
| `Client/SpriteLib/CSpritePackList555.h` | class(클래스), file I/O 헤더 |
| `Client/SpriteLib/CSpritePackList565.h` | class(클래스), file I/O 헤더 |
| `Client/SpriteLib/CTypePackVector.h` | Init/Release, Add/Remove, Size, operator, file I/O 헤더 |
| `Client/SpriteLib/SP.h` | DirectDraw + Sprite 헤더 |

### 번역 완료 (2026-09-03 추가)
| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/SpriteLib/CSpriteSurface.cpp` | 약 401KB, 13778줄. Static member/memcpy 효과/생성자소멸자/멤버함수/Lock·Unlock·클리핑류/Change Brightness/Half·Alpha·Color·Scale·Darkness·Brightness Copy/Alpha Blending/Effect SimpleOutline·WipeOut·Net/Init EffectTable/DirectX 호환성 메서드(SDL 스텁)/Gray Scale·Darker·Lighten·Darken·Screen 블렌드 공식 주석 등 다수 번역 (Blt* 계열 식별자성 헤더와 R/G/B 단일 색상채널 주석은 유지, 주석처리된 레거시 DirectDraw 코드 블록은 비활성 코드이므로 미변경) |
