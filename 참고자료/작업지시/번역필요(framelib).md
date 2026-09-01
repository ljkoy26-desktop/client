# framelib 프로젝트 - CPP/H 외 번역 필요 파일 목록

framelib.vcxproj의 CPP/H 파일 번역 작업 중 발견된, CPP/H가 아니라서
이번 작업에서는 건드리지 않은 파일들입니다. 추후 별도로 번역이 필요합니다.

## Client/framelib/CMakeLists.txt

영어 주석 존재:

- `# FrameLib - Frame/Sprite animation library`
- `# Collect source files`
- `# Remove precompiled header source`
- `# Create library`
- `# Include directories`
- `# NOTE: Do not add ../Packet here. Client/Packet/fileapi.h shadows the real`
  `# Windows SDK <fileapi.h> that <windows.h> depends on internally, and`
  `# framelib does not include anything from Packet.`
- `# Link libraries`

## 참고

framelib.vcxproj에 포함된 CPP 파일(CAnimationFrame.cpp, CAnimationFrameSetManager.cpp,
CCreatureFrameSetManager.cpp, CFrame.cpp, CFramePack.cpp, CFrameSetManager.cpp)과
같은 폴더의 관련 H 파일(CAnimationFrame.h, CAnimationFrameSetManager.h,
CCreatureFrameSetManager.h, CFrame.h, CFramePack.h, CFrameSet.h, CFrameSetManager.h,
DrawTypeDef.h, FR.h, Frame_PCH.h, TArray.h)은 이미 대부분 한글 주석이었고,
영어 주석은 `CFrameSet.h`의 `// file position (32-bit)` 한 줄뿐이라
`// 파일 위치(32비트)`로 번역 완료했습니다.
