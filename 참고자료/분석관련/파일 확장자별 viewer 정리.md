# 파일 확장자별 viewer 정리

`DarkEden.exe`(실제 게임 실행 파일)를 제외하고, `CMakeLists.txt`가 만드는 나머지 `.exe`들은 전부 특정 리소스 확장자를 검증/시각화하기 위한 **도구성 프로젝트**다. 각 도구의 `main.cpp`(및 관련 소스)를 직접 읽고, 실제로 어떤 파일을 여는지(그리고 실제로 "제대로" 여는지)를 조사했다.

전제 문서: [[파일 확장자별 특징]] — 확장자 자체의 의미는 그 문서를 참고. 이 문서는 "그 확장자를 어떤 exe로 디버깅/확인할 수 있는가"만 다룬다.

---

## 1. 요약 표

| exe | 소스 위치 | 다루는 확장자 | 실제 동작 상태 |
|---|---|---|---|
| `sprite_viewer.exe` | `tools/viewers/sprite_viewer/main.cpp` | `.spk` | 완전 동작 |
| `item_viewer.exe` | `tools/viewers/item_viewer/main.cpp` | `.ispk` | 완전 동작 |
| `creature_viewer.exe` | `tools/viewers/creature_viewer/main.cpp` | `.ispk` (동작) / `.cfpk`, `.sspk` (인자만 받고 **미구현**) | 부분 동작 |
| `map_viewer.exe` | `tools/viewers/map_viewer/main.cpp` + `zoneloader.cpp` | `.map` + `.spk`(타일/오브젝트) | 완전 동작 |
| `effect_viewer.exe` | `tools/viewers/effect_viewer/main.cpp` | `.efpk`, `.spk`, `.aspk`, `.sspk`, `.ppk`, `.inf` (전부 고정 디렉토리 기반) | 완전 동작 |
| `zone_parser.exe` | `tools/viewers/zone_parser/main.cpp` | `.map` | 완전 동작 (GUI 없이 텍스트 덤프만) |
| `resource_validator.exe` | `Client/tools/resource_validator.cpp` | `.inf`(심층 검증), `.spk`(심층 검증), 그 외 대부분(존재 여부만) | 완전 동작(단, 검증 깊이는 확장자별로 다름) |

---

## 2. 도구별 상세

### 2-1. `sprite_viewer.exe` — `.spk`

- `CSpritePack::LoadFromFile(spkFile)` (`Client/SpriteLib/CSpritePack.h`)로 직접 로드.
- 사용법: `sprite_viewer.exe <sprite_pack.spk>`
- `CTypePack2::LoadFromFile(const char*)`는 파일 하나만 열어서 헤더(개수)부터 각 스프라이트를 **순차적으로 전부** 읽는 방식이라, 짝 파일인 `.spki`는 **필요하지 않다** (러닝 로드용 `LoadFromFileRunning()`을 쓸 때만 `.spki`가 필요함, 이 도구는 그 경로를 쓰지 않음).
- SDL 창에 스프라이트를 순서대로 넘겨보며 확인 가능 (`m_currentIndex` 조작).

### 2-2. `item_viewer.exe` — `.ispk`

- `CIndexSpritePack::LoadFromFile(ispkFile)` (`Client/SpriteLib/CIndexSpritePack.h`)로 로드.
- 사용법: `item_viewer.exe <items.ispk>`
- `sprite_viewer`와 동일한 이유로 `.ispki`는 필요 없음.
- 인덱스 컬러(팔레트 기반) 이미지이므로 `.spk`(일반 컬러)와는 다른 렌더링 경로를 검증하는 용도.

### 2-3. `creature_viewer.exe` — `.ispk` (동작) + `.cfpk`/`.sspk` (⚠️ 미구현)

- 사용법: `creature_viewer.exe <creatures.ispk> <creatures.cfpk> [creatures.sspk]`
- `Initialize()` 내부를 직접 확인한 결과:
  - `.ispk` → `CIndexSpritePack::LoadFromFile()`로 **정상 로드됨**.
  - `.cfpk` → 파일을 여는 것까지는 하지만, 실제 파싱 부분에 `// TODO: Implement LoadFromFile for CCreatureFramePack` 주석과 함께 **더미 값(`m_maxCreatureTypes = 1; // Placeholder`)만 채워두고 곧바로 파일을 닫는다.** 즉 인자로 넘겨도 실제 애니메이션 프레임 데이터는 반영되지 않는다.
  - `.sspk` → 넘기면 `CShadowSpritePack` 객체만 생성하고 `// TODO: Load .sspk file` 주석과 함께 `"Shadow pack loading not yet implemented"` 로그만 출력한다. 실제로 로드하지 않는다.
- **결론**: 현재 시점에서 `creature_viewer`로 실질적으로 디버깅되는 확장자는 `.ispk` 하나뿐이다. `.cfpk`/`.sspk` 뷰어 기능은 아직 만들다 만 상태(스켈레톤)라서, 이 두 확장자를 실제로 확인하려면 이 도구를 보강하거나 다른 방법(예: `DarkEden.exe` 실행 중 실제 크리처 렌더링)을 써야 한다.

### 2-4. `map_viewer.exe` — `.map` + `.spk`

- 사용법: `map_viewer.exe <zone.map> <tile.spk> [imageobject.spk] [--cell N]`
- `.map`은 `ZoneLoader::LoadFromFile()`(`tools/viewers/map_viewer/zoneloader.cpp`)이 담당하며, 내부적으로 `tools/engine/sprite/src/zone.c`의 C API(`zone_load`)를 호출해 zone 헤더/섹터/ImageObject 배열을 채운다.
- 두 번째 인자(`tile.spk`, 필수)와 세 번째 인자(`imageobject.spk`, 선택)는 각각 `CSpritePack::LoadFromFile()`로 로드되어, 맵 위에 타일/오브젝트 이미지를 실제로 그리는 데 쓰인다.
- 팬/줌을 지원하는 SDL 창에서 맵 전체를 훑어볼 수 있음.
- 참고: `--cell N`으로 셀 픽셀 크기 조정 가능.
- `.mip`/`.smp`/`.ssi`(맵 폴더의 부가 파일, [[파일 확장자별 특징]] 문서의 "근거를 못 찾은 확장자" 항목)는 이 도구도 다루지 않는다 — 여전히 미확인 영역.

### 2-5. `effect_viewer.exe` — `.efpk` / `.spk` / `.aspk` / `.sspk` / `.ppk` / `.inf`

- 사용법: `effect_viewer.exe [--data PATH] [--effect TYPE]` (개별 파일명이 아니라 **디렉토리**를 인자로 받음, 기본값 `Data/Image/`)
- 개별 파일을 지정하는 게 아니라 `Client/EffectResourceContainer::Load(dataPath)`를 그대로 호출해서, 그 디렉토리 안의 **고정된 파일명들**을 찾아 로드한다 (지난번 조사한 `EffectResourceContainer.cpp` 기준):
  - `NormalEffect.efpk`, `Effect.efpk`/`effect.efpk`/`EffectAlpha.efpk`, `ShadowEffect.efpk`, `EffectScreen.efpk` (프레임 팩)
  - `Effect.spk`, `Effect.aspk`/`effect.aspk`, `Effect.sspk`, `EffectScreen.spk` (스프라이트 팩)
  - `Effect.ppk`/`effect.ppk`/`EffectAlpha.ppk`, `EffectScreen.ppk` (팔레트 팩)
  - 추가로 `EffectSpriteType.inf`을 `--data` 경로에서 `Image/`→`Info/`로 치환한 경로에서 로드 (효과 타입 메타데이터)
- 이 중 하나라도 없으면 경고만 찍고 넘어가는 구조라(각 `Load*` 함수가 실패해도 `true`/경고만 반환), 일부 파일이 빠진 상태에서도 실행 자체는 가능하다. 즉 "완전 로드 성공"은 도구 실행만으로 보장되지 않고, 콘솔 로그(`Loaded: ...` / `Warning: Failed to open ...`)를 직접 확인해야 한다.
- LEFT/RIGHT 키로 효과 타입을 바꿔가며 실시간 렌더링 확인 가능.

### 2-6. `zone_parser.exe` — `.map` (GUI 없는 텍스트 덤프 전용)

- 사용법: `zone_parser.exe <zone.map> [--sectors N]`
- `map_viewer`와 달리 SDL/SpriteLib에 전혀 의존하지 않는 **완전 독립형 파서**(`CMakeLists.txt`의 `target_link_libraries(zone_parser PRIVATE)`가 비어있음 — 실제로 아무 내부 라이브러리도 링크하지 않음).
- `.map` 바이너리를 직접 `std::ifstream`으로 열어 MString(4바이트 길이 + 문자열) 포맷의 zone 이름/설명, zone id/group/type/level, 가로세로 크기, ImageObject 개수 등을 손으로 파싱해서 콘솔에 출력.
- 렌더링 없이 **헤더/메타데이터만 빠르게 확인**하고 싶을 때 적합. `map_viewer`보다 가볍고, `.spk` 파일이 없어도 실행 가능하다는 게 차이점.

### 2-7. `resource_validator.exe` — `.inf` / `.spk` (심층 검증), 그 외 확장자(존재 여부만)

- 사용법: `resource_validator.exe <inf_file>` (기본값 `DarkEden/Data/Info/VS_UI_filepath.inf`)
- `Data/Info/VS_UI_filepath.inf`처럼 `key : value` 형식으로 리소스 경로 목록을 담은 `.inf` 파일 하나를 읽어, 그 안에 나열된 **모든 리소스 파일**을 순회 검증한다. 즉 이 도구 하나로 사실상 모든 확장자를 "존재하는지"는 훑을 수 있지만, 실제 내용까지 파싱해서 검증(deep validation)하는 건 다음 두 가지뿐이다.
  - `.inf` → `ValidateINF()`: `key:value` 라인 개수를 세어 Properties 포맷으로서 파싱 가능한지 확인
  - `.spk` → `ValidateSPK()`: 파일 크기가 4바이트 이상인지 확인 + **짝 인덱스 파일(`경로+"i"`, 즉 `.spki`)이 실제로 존재하는지** 확인
  - `.ispk`, `.cfpk`, `.sspk`, `.rpk`, `.txt`, `.xml`, `.spki`, `.ispki` → 코드에 명시적으로 나열은 되어 있지만 **존재 여부 확인만** 하고 내용은 보지 않음 (`"File exists (no deep validation)"`)
  - 그 외 모든 확장자(`.mip`, `.smp`, `.ssi`, `.cfpki`, `.efpk`, `.ppk` 등 나열되지 않은 것들) → `else` 분기로 떨어져 "Unknown type - exists only"로 마찬가지로 존재 여부만 확인
- `make check-resources`로 `extract_macros.py`(헤더→`.inf` 생성) → `resource_validator` 실행까지 한 번에 묶여 있음 (`tools/resource_management/README.md` 참고).

---

## 3. 결론 / 활용 가이드

특정 확장자를 디버깅하고 싶을 때 어떤 exe를 써야 하는지 정리하면:

| 확인하고 싶은 확장자 | 추천 도구 |
|---|---|
| `.spk` | `sprite_viewer.exe` (렌더링 확인) 또는 `resource_validator.exe` (크기/인덱스 짝파일 여부만) |
| `.ispk` | `item_viewer.exe` (렌더링) 또는 `creature_viewer.exe` (크리처용이면) |
| `.map` | 렌더링까지 보려면 `map_viewer.exe`, 헤더 정보만 빠르게 보려면 `zone_parser.exe` |
| `.efpk` / `.aspk` / `.ppk` (이펙트 계열) | `effect_viewer.exe` (단, 디렉토리 통째로 넘겨야 함) |
| `.cfpk`, `.sspk` | ⚠️ 전용 뷰어 없음 — `creature_viewer.exe`가 인자는 받지만 미구현 상태. 실제로 보려면 `DarkEden.exe`를 직접 실행해서 확인하거나, 뷰어 보강이 필요함 |
| `.inf` | `resource_validator.exe` (포맷 유효성 검증) |
| `.mip`, `.smp`, `.ssi`, `.spi`, `.sppk`류 등 | 이 도구들 중 어느 것도 다루지 않음 — [[파일 확장자별 특징]] 문서에서도 "근거를 못 찾은 확장자"로 분류된 것과 일치. 별도 뷰어가 없는 상태 |

`DarkEden.exe`는 위 표에서 제외했지만, 실제로는 게임 실행 중 모든 확장자(`.cfpk`, `.sspk` 포함)를 다 로드해서 쓰기 때문에, 전용 뷰어가 없는 확장자를 확인해야 한다면 결국 게임 본체를 띄워서 확인하는 수밖에 없다.
