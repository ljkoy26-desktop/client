# TextSystem 프로젝트 - CPP/H 외 번역 필요 파일 목록

TextSystem.vcxproj의 CPP/H 파일 번역 작업 중 확인한 내용입니다.

## 확인 결과

Client/TextSystem/ 폴더에는 CPP/H 파일 9개(TextService.cpp, TextBackendSDL.cpp,
RenderTargetSpriteSurface.cpp, FontHandleUtil.h, RenderTarget.h,
RenderTargetSpriteSurface.h, TextBackend.h, TextService.h, TextTypes.h)만 존재하며,
CMakeLists.txt나 README 같은 CPP/H 외 파일은 없습니다. 따라서 이번 프로젝트에서는
번역을 미룰 대상 파일이 없습니다.

## 완료 내용

- vcxproj에 ClCompile로 등록된 cpp 3개(TextService.cpp, TextBackendSDL.cpp,
  RenderTargetSpriteSurface.cpp)와 이들이 include하는 헤더 6개 전부 번역 완료.
- RenderTarget.h, RenderTargetSpriteSurface.h, RenderTargetSpriteSurface.cpp,
  TextBackend.h, TextTypes.h에는 영어 주석이 없어 변경 사항 없음.
- TextService.cpp, TextBackendSDL.cpp는 원래 CP949 인코딩으로 저장되어 있었고
  이미 한글로 번역된 주석 일부가 포함되어 있었습니다(Read 시 UTF-8 오인식으로
  깨져 보였을 뿐, 실제로는 정상적인 한글 주석). 나머지 영어 주석만 한글로
  번역하고, 전체 파일을 UTF-8 with BOM으로 재저장했습니다(줄바꿈은 원본과
  동일하게 CRLF 유지).
- fprintf 오류 메시지 같은 문자열 리터럴(프로그램 출력)은 기존 방침과 동일하게
  번역 대상에서 제외했습니다.
