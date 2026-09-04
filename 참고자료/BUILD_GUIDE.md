# DarkEden Client - Windows 빌드 가이드

## 환경

| 항목 | 버전 |
|------|------|
| OS | Windows 10 / 11 (x64) |
| Visual Studio | 2019 (Community / Professional / Enterprise) |
| CMake | 3.20 이상 |
| vcpkg | 최신 |
| 플랫폼 | x64 |

---

## 1. 사전 요구사항 설치

### 1-1. Visual Studio 2019

설치 시 아래 워크로드 선택 필수:
- **C++를 사용한 데스크톱 개발**

추가 구성 요소:
- MSVC v142 빌드 도구 (x64/x86)
- Windows 10 SDK

### 1-2. CMake

https://cmake.org/download/ 에서 설치 후 PATH에 등록.

```
cmake --version
```

### 1-3. Git

https://git-scm.com/ 에서 설치.

---

## 2. vcpkg 설치

> 이미 설치된 경우 건너뜀.

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

이후 `VCPKG_ROOT` 환경변수를 vcpkg 경로로 설정해두면 편리함.

---

## 3. vcpkg 패키지 설치

아래 명령어를 **vcpkg 설치 경로**에서 실행.

```powershell
cd <vcpkg 설치 경로>

.\vcpkg install sdl2:x64-windows
.\vcpkg install sdl2-image:x64-windows
.\vcpkg install sdl2-ttf:x64-windows
.\vcpkg install sdl2-mixer:x64-windows
```

### 설치 확인

```powershell
.\vcpkg list
```

아래 항목이 모두 표시되어야 함:

```
sdl2:x64-windows
sdl2-image:x64-windows
sdl2-ttf:x64-windows
sdl2-mixer:x64-windows
freetype:x64-windows       ← sdl2-ttf 의존성으로 자동 설치
libpng:x64-windows         ← sdl2-image 의존성으로 자동 설치
```

---

## 4. CMake 솔루션 생성

프로젝트 루트 (`dk_client/`)에서 PowerShell로 실행.

```powershell
cmake -S . -B build\vs2019 `
  -G "Visual Studio 16 2019" `
  -A x64 `
  "-DCMAKE_TOOLCHAIN_FILE=<vcpkg 설치 경로>\scripts\buildsystems\vcpkg.cmake"
```

**예시** (vcpkg가 `C:\vcpkg`에 설치된 경우):

```powershell
cmake -S . -B build\vs2019 `
  -G "Visual Studio 16 2019" `
  -A x64 `
  "-DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
```

> **주의**: `TOOLCHAIN_FILE` 경로에 공백이 포함된 경우 전체 `-D...` 인수를 큰따옴표로 감쌀 것.

### 성공 시 출력 (마지막 부분)

```
-- Configuring done
-- Generating done
-- Build files have been written to: .../build/vs2019
```

---

## 5. 빌드

### 방법 A - Visual Studio GUI

1. `build\vs2019\DarkEdenClient.sln` 열기
2. 상단 구성에서 `Debug` 또는 `Release` 선택
3. **빌드 → 솔루션 빌드** (Ctrl+Shift+B)

솔루션 탐색기 폴더 구성:

| 폴더 | 프로젝트 |
|------|----------|
| `CMakeDefault` | ALL_BUILD, ZERO_CHECK, INSTALL |
| `Core` | basic, dxlib, SpriteLib, framelib, TextSystem, VS_UI, sprite |
| `exe` | DarkEden, resource_validator, sprite_viewer, zone_parser, item_viewer, creature_viewer, map_viewer, effect_viewer |

### 방법 B - 커맨드라인

```powershell
# Debug 빌드
cmake --build build\vs2019 --config Debug

# Release 빌드
cmake --build build\vs2019 --config Release

# 병렬 빌드 (CPU 코어 수 지정)
cmake --build build\vs2019 --config Debug --parallel 8
```

빌드 결과물 위치:

```
build\vs2019\bin\Debug\DarkEden.exe
build\vs2019\bin\Release\DarkEden.exe
```

---

## 6. 실행

게임 데이터(`DarkEden/` 폴더)가 exe와 같은 위치에 있어야 함.

```powershell
cd build\vs2019\bin\Debug
.\DarkEden.exe 0000000001   # 창모드
.\DarkEden.exe 0000000002   # 풀스크린
```

| 인수 | 설명 |
|------|------|
| `0000000001` | 창모드 |
| `0000000002` | 풀스크린 |
| `0000000003` | 창모드 1024×768 |
| `0000000004` | 풀스크린 1024×768 |

서버 IP 설정:
```
DarkEden\Data\Info\GameClient.inf
```

---

## 7. 문제 해결

### SDL2 패키지를 찾지 못하는 경우

`-DCMAKE_TOOLCHAIN_FILE` 경로가 올바른지 확인.
경로에 공백이 있으면 전체 인수를 큰따옴표로 감쌀 것:

```powershell
"-DCMAKE_TOOLCHAIN_FILE=C:\path with space\vcpkg\scripts\buildsystems\vcpkg.cmake"
```

### 기존 CMakeCache 충돌

이미 `build\vs2019`가 존재하고 설정이 꼬인 경우:

```powershell
Remove-Item -Recurse -Force build\vs2019
# 이후 4번 CMake 생성 단계 재실행
```

### ATL 관련 링크 오류 (LNK1104: atls.lib)

Visual Studio 설치 시 **ATL(C++용 MFC 및 ATL 지원)** 구성 요소가 누락된 경우.
Visual Studio Installer → 수정 → 개별 구성 요소에서 추가.

### `iconv` LNK2019 오류

vcpkg에서 `libiconv:x64-windows` 설치:

```powershell
.\vcpkg install libiconv:x64-windows
```

이후 CMake 재생성.
