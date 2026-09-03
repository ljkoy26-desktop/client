# DarkEden.vcxproj 영어→한글 주석 번역 완료 목록

## 번역 기준
- vcxproj 파일 기준 ClCompile 포함 파일 대상
- .cpp, .h 파일만 번역 (프로젝트 파일 등은 제외)
- 영어 주석을 한글로 번역, 기존 한글/로직 보존

---

## 1차 번역 완료 (2026-09-02)

아래 파일들은 영어 주석을 한글로 번역 완료 (vcxproj 줄 432~460 대상):

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/APICheck.cpp` | 비-Windows 플랫폼 스텁 주석 |
| `Client/AppendPatchInfo.cpp` | 섹션 헤더(Save/Load/Release/Set/Execute/Check/Calculate/GetAllSize) |
| `Client/BIT_RES.CPP` | 비트 스트림 읽기/쓰기 주석 |
| `Client/BloodBibleSignInfo.cpp` | include files / constructor / destructor |
| `Client/CAvi.cpp` | MCI Digital Video 스텁 설명 블록 주석 |
| `Client/CGHandlersStub.cpp` | 파일 헤더 및 인라인 주석 |
| `Client/CGameUpdate.cpp` | 전역 변수, SDL 마이그레이션 주석 |
| `Client/CMP3.cpp` | __USE_MP3__ 비활성화 설명, 비-Windows 스텁 주석 |
| `Client/CMessageArray.cpp` | 플랫폼별 포함/I/O/파일 플래그 주석 |
| `Client/CNormalSpriteTexturePartManager.cpp` | SDL 제거 주석, 생성자/소멸자 헤더 |
| `Client/COGGSTREAM.CPP` | SDL 백엔드 미구현 주석 |
| `Client/COpeningUpdate.cpp` | 전역 변수, 초기화, 오프닝 AVI, 입력 업데이트 |
| `Client/CSDLGraphicsFlip.cpp` | (영어 주석 없음, 수정 불필요) |
| `Client/CServerInformation.cpp` | 전역 변수 섹션 헤더 |
| `Client/CShadowPartManager.cpp` | SDL 마이그레이션 주석, DX 제거 주석 |
| `Client/CSoundPartManager.cpp` | base class release → 기반 클래스 해제 |
| `Client/CSpriteTexturePartManager.cpp` | SDL 제거 주석, 생성자/소멸자/멤버함수 헤더 |
| `Client/CSystemInfo.cpp` | VC6/x86 구현 설명, MMX/SSE/3DNow/HT 주석, 비-Windows 스텁 |
| `Client/CTexturePartManager.cpp` | SDL 제거 주석, 생성자/소멸자/멤버함수 헤더 |
| `Client/CToken.cpp` | 생성자/소멸자/멤버함수 섹션 헤더 |
| `Client/CUsingColorSetTable.cpp` | 전역/생성자소멸자/멤버함수/Enable/Disable/Save/Load 헤더 |
| `Client/CWaitPacketUpdate.cpp` | 전역/초기화/대기위치업데이트/지연시간설정 헤더 |
| `Client/CWaitUIUpdate.cpp` | MMSystem 주석, DIK→VK 변환 주석, WM_TEXTINPUT/EDITING 주석 |
| `Client/Client.cpp` | 전역/포함파일/함수 설명/윈도우 메시지 처리 주석, 중국어 주석 한글화 |
| `Client/ClientConfig.cpp` | 중국어 주석(기본값 사용) → 한글 |
| `Client/ClientFunction.cpp` | 전방선언/사운드정리/재생/귓속말ID/알파박스 관련 주석 |
| `Client/ClientTimeFunctions.cpp` | 파일 헤더 및 함수 설명 주석 |
| `Client/Client_PCH.cpp` | (번역 불필요 - include만 존재) |
| `Client/CrashReport.cpp` | 예외 처리 설명, StackWalk64, 스택프레임 주석, 비-Windows 스텁 |

---

## 2차 번역 완료 (2026-09-02)

아래 파일들은 영어 주석을 한글로 번역 완료 (vcxproj 줄 461~467 대상):

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/DebugInfo.cpp` | (번역 불필요 - 주석처리된 코드 블록만 존재) |
| `Client/DebugKit.cpp` | 파일 헤더, 생성자/소멸자 섹션 헤더 |
| `Client/DebugLog.cpp` | 파일 헤더, 설정/스레드 안전성/헬퍼 함수/공개 인터페이스/핵심 로깅 함수 섹션 헤더 및 인라인 주석 전체 |
| `Client/DirectXLib_PCH.cpp` | (번역 불필요 - include 한 줄만 존재) |
| `Client/DrawCreatureDivineGuidance.cpp` | (번역 불필요 - 주석처리된 코드 블록만 존재) |
| `Client/DrawCreatureEffect.cpp` | (번역 불필요 - 주석처리된 코드 블록만 존재) |
| `Client/DrawCreatureShadow.cpp` | 섹션 헤더(크리처 그림자 출력), SDL 크래시 방지 주석, SDL2 제거 주석 |

---

---

## 3차 번역 완료 (2026-09-02)

아래 파일들은 영어 주석을 한글로 번역 완료 (vcxproj 줄 468~471 대상):

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/EffectResourceContainer.cpp` | 생성자/소멸자, Load/LoadFramePacks/LoadSpritePacks/LoadPalettePacks/GetFramePack/GetEffectFrame/GetSprite/GetAlphaSprite/GetShadowSprite/GetPalette 섹션 헤더 및 인라인 주석 |
| `Client/ExpInfo.cpp` | (번역 불필요 - 영어 설명 주석 없음) |
| `Client/ExperienceTable.cpp` | 전역 변수, 생성자/소멸자, Load From File 시리즈, Get Info 시리즈 섹션 헤더 |
| `Client/FameInfo.cpp` | (번역 불필요 - 영어 설명 주석 없음) |

---

---

## 4차 번역 완료 (2026-09-02)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/GAME1024.cpp` | 파일 헤더, 생성자/소멸자 섹션 헤더 |
| `Client/GAME1024.h` | 파일 헤더 |
| `Client/GameFunctions.cpp` | 파일 헤더, 전역 변수, 각 함수 섹션 헤더, 스텁 구현 주석 전체 |
| `Client/GameHelpers.cpp` | 파일 헤더, 전방 선언, 열거형, 함수 그룹 섹션 헤더 전체 |
| `Client/GameHelpers.h` | 파일 헤더, 전방 선언, 열거형, 함수 그룹 섹션 헤더 전체 |
| `Client/GameInit.cpp` | 포함 파일, MMSystem/macOS/g_bEnable3DHAL 설명, SDL 관련 주석, 로깅/프로파일러/초기화 섹션 헤더, 스레드 관련 주석 |
| `Client/GameInitInfo.cpp` | 포함 파일, Platform.h 중요 주석, MMSystem 주석, 초기화 함수 섹션 헤더, 쉐도우 복사본/디버그 주석 |

---

## 5차 번역 완료 (2026-09-02)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/GameObject.h` | 프로그램 정보/게임 오브젝트/게임 정보 테이블/인터페이스 섹션 헤더 |
| `Client/GameMain.cpp` | 포함 파일, MMSystem 미포함 설명, 전역 변수 그룹 헤더(AVI/사운드 파트 매니저/커서 서피스/마우스 커서/존/이펙트 매니저/DirectDraw 서피스/채팅 문자열/소켓/스레드/서버 IP), 치트 방지 주석, CDirect3D 제거 주석(SDL2), SDL 디스플레이/음악 재생 주석, 파일 스레드 프로시저/정지, 크리처 로드/타입/SPK 해제, 게임 오브젝트 해제, 존 로드/이동/정보 로드, 플레이어 초기화, 스크린샷 생성, SaveSurfaceToImage 설명 블록, 사운드 재생/정지/음악 재생, 날씨/번개 설정, 파일 열기/경로 구분자 변환, 입력/마우스 업데이트, 연결 유지/끊김 업데이트, CPU 양보 주석, 마지막 선택 캐릭터 선택/저장, 관전 모드 설정, 클라이언트 크리처 추가, 소켓 입출력 업데이트, 시간 확인, 아이템 배치 위치 찾기, 디버그 메시지 등 전체 영어 주석 64개 |

---

---

## 6차 번역 완료 (2026-09-03)

아래 파일들은 영어 주석을 한글로 번역 완료 (vcxproj 줄 476~480 및 GamePacketFunctions.cpp 대상):

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/GamePacketFunctions.cpp` | 파일 헤더, 함수별 섹션 헤더(NewFakeCreature/SetFade*/SetPetInfo/SetGearInfo/SetExtraInfo/SetEffectInfo/ComeFromPortal/MoveIntoPortal/Skill*/SetPCSlayerInfo/AddItemToZone/g_PrintColorStr*/SetAddonTo*/AffectModifyInfo/SetInventoryInfo/SetPCOustersInfo/SetPCVampireInfo/CheckItemForSkillIcon/CreateActionResultNode 등), 전역 변수 섹션 헤더, InitPacketItemTable 내부 섹션 헤더, 스텁 구현 주석 전체 |
| `Client/GameUI.cpp` | 4996줄 전체에서 영어 주석 130여개 번역 (Is Running/Run/Close/Set/Get 계열 섹션 헤더, sobeit/csm 작성 이력 주석, 스킬/파티/길드/캐릭터 정보 관련 섹션 헤더 등). 순수 함수명만 반복하는 헤더(UI_SetCharInfoSTR 등)와 주석 처리된 죽은 코드는 원문 유지 |
| `Client/GCNotifyWin.cpp` | (번역 불필요 - 영어 설명 주석 없음) |
| `Client/GCNotifyWin.h` | 멤버 변수 인라인 주석(NPC's object id, chatting message) |
| `Client/GCNotifyWinHandler.cpp` | UI_RunImageNotice 미구현 주석 |
| `Client/GetWinVer.cpp` | Windows 버전 판별 함수 내 섹션/인라인 주석 전체 |
| `Client/GetWinVer.h` | 라이선스 헤더 주석 |
| `Client/Globals.cpp` | 파일 헤더, 전역 변수/함수 섹션 헤더, 스텁 구현 주석 |
| `Client/GlobalVariables.cpp` | 파일 헤더, include 주석, 전역 변수 그룹 섹션 헤더 전체(플랫폼/메시지배열/네트워크/사운드/존/매니저/그래픽/입력상태/게임상태/FPS/렌더링플래그/기타/패킷시스템/설정파일/MP3/비디오음악/게임모드/존설정/관전모드/마우스위치/크리처변신/사운드설정/클라이언트통신/VS_UI) |
| `Client/GuildInfo.cpp` | 디버그 문자열 주석 |
| `Client/GuildInfo.h` | getSize/toString/getter·setter 주석 |
| `Client/GuildMemberInfo.cpp` | 디버그 문자열 주석 |
| `Client/GuildMemberInfo.h` | getSize/toString/getter·setter/멤버 변수 주석 |

---

---

## 7차 번역 완료 (2026-09-03)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/header.cpp` | 함수 주석 블록 레이블 (Func/Desc/Params/Return → 함수/설명/매개변수/반환값) |
| `Client/header.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/heckSystem.cpp` | 파일 헤더, 생성자/소멸자 섹션 헤더, platform/version and language 인라인 주석 |
| `Client/heckSystem.h` | 파일 헤더 |
| `Client/huffman.cpp` | dummy 주석 3개, 허프만 코드 테이블 헤더 배열 주석, huffman_decoder 함수 전체 주석 (테이블 탐색/오류확인/부호인코딩/이스케이프인코딩) |
| `Client/huffman.h` | 구조체 멤버 변수 주석 전체 (tablename/xlen/ylen/linbits/linmax/ref/table/hlen/val/treelen) |
| `Client/infblock.h` | 경고 블록, 함수 매개변수 주석 (check function/window size/initial return code/check value on output/dictionary/dictionary length) |
| `Client/infcodes.h` | 경고 블록 |
| `Client/inffast.h` | 경고 블록 |
| `Client/inffixed.h` | 파일 설명 주석, 경고 블록 |
| `Client/inftrees.h` | 경고 블록, 허프만 조회 테이블 설명, 구조체 멤버 주석, MANY 상수 설명, 함수 매개변수 주석 전체 |
| `Client/infutil.h` | 경고 블록, 열거형 값 주석 전체, 구조체 멤버 주석 전체 (모드/모드의존/모드독립 섹션), 매크로 주석 전체 |
| `Client/InputService.h` | 파일 헤더 블록, 인라인 주석 전체 (키코드 정의/전방선언/전역 인스턴스) |
| `Client/ItemClassDef.h` | `//add by zdj` → `// zdj 추가` |
| `Client/l3types.h` | (번역 불필요 - 기술 표기 주석만 존재) |
| `Client/LeakMemoryDumper.cpp` | AddTrack() 함수 내 malloc() 사용 이유 설명 주석 (7줄) |
| `Client/LeakMemoryDumper.h` | (번역 불필요 - 파일 헤더 메타데이터만 존재) |

---

---

## 8차 번역 완료 (2026-09-03)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/MActionInfoTable.cpp` | 전역 변수, 저장/불러오기, 생성자/소멸자, 멤버 함수, 파일에 저장/파일에서 불러오기, 남성/여성 섹션 헤더 |
| `Client/MActionInfoTable.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MActionResult.cpp` | (번역 불필요 - 영어 주석 없음) |
| `Client/MActionResult.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MActionResultDef.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAnimationObject.cpp` | 생성자/소멸자, 타입, 데이터, 멤버 함수, 설정, 파일에 저장, 파일에서 불러오기, 다음 프레임 섹션 헤더 |
| `Client/MAnimationObject.h` | ImageObject 클래스, 사운드, 파일 입출력 섹션 헤더 |
| `Client/MAroundZoneEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAroundZoneEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MathTable.cpp` | 360도 각도 반환, FSin/FCos/FArcTan/FSines 초기화, 목표 방향 각도 반환, 각도 반전/클리핑, 각도 방향 반환, 각도 클리핑 섹션 헤더 및 인라인 주석 |
| `Client/MathTable.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachCreatureEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttachCreatureEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachCreatureEffectGroundGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttachCreatureEffectGroundGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachCreatureOrbitEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttachCreatureOrbitEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachEffect.cpp` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachEffect.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachOrbitEffect.cpp` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachOrbitEffect.h` | 궤도 X,Y 좌표 반환, 궤도 좌표 초기화 섹션 헤더 |
| `Client/MAttachZoneAroundEffectGenerator.CPP` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachZoneAroundEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachZoneEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttachZoneEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttachZoneSelectableEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttachZoneSelectableEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttackCreatureEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttackCreatureEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttackCreatureHomingEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttackCreatureHomingEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttackCreatureParabolaEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttackCreatureParabolaEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttackZoneBombEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttackZoneBombEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttackZoneEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttackZoneEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttackZoneParabolaEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MAttackZoneParabolaEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttackZoneRectEffectGenerator.cpp` | (번역 불필요 - 영어 주석 없음) |
| `Client/MAttackZoneRectEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MBloodyBreakerEffectGenerator.CPP` | (번역 불필요 - 영어 주석 없음) |
| `Client/MBloodyBreakerEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MBloodyWallEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MBloodyWallEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MBloodyWaveEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MBloodyWaveEffectGenerator.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MChaseEffect.cpp` | 생성자/소멸자, 멤버 함수, 업데이트 섹션 헤더 |
| `Client/MChaseEffect.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MChatManager.cpp` | (번역 불필요 - 영어 주석 없음) |
| `Client/MChatManager.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MCompareManager.cpp` | 전역 변수, 생성자/소멸자, 멤버 함수, 초기화, 비교 함수 섹션 헤더 (HC_ 함수 헤더는 코드 식별자이므로 유지) |
| `Client/MCompareManager.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MCrashReportManager.cpp` | (번역 불필요 - 영어 주석 없음) |
| `Client/MCrashReportManager.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MCreature.cpp` | MoveTable 초기화/해제, 생성자/소멸자, 멤버 함수, 크리처 타입/몸색/이름/방향/액션/상태/이펙트/이동/패킷/회복/어둠/환각/재생 관련 섹션 헤더 122개 번역 |
| `Client/MCreature.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MCreatureSpriteTable.cpp` | 전역 변수, 생성자/소멸자, 멤버 함수, 파일에 저장, 파일에서 불러오기 섹션 헤더 |
| `Client/MCreatureSpriteTable.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MCreatureTable.cpp` | 전역 변수, 대입 연산자, 파일에 저장/불러오기, 멤버 함수, 크리처 종족 설정, 액션 사운드 타입 초기화, 액션 사운드 최대값 반환, 생성자/소멸자, 초기화, 해제, 크리처 타입 추가, 무작위 크리처 타입 반환 섹션 헤더; 최대 액션 수 초기화, reserve()→resize() 사용, capacity()→size() 사용 인라인 주석 |
| `Client/MCreatureTable.h` | (번역 불필요 - 영어 주석 없음) |
| `Client/MCreatureWear.cpp` | 정적 멤버, 생성자/소멸자, 멤버 함수, AddonInfo 초기화, 동일 복장 설정, 부가 ColorSet1/2 설정, AddonInfo로 아이템 생성, 부가장비 제거, 부가장비 설정, 부가장비 해제, 크리처 타입 설정, 이펙트 상태 제거, 이펙트 업데이트 섹션 헤더 |
| `Client/MCreatureWear.h` | (번역 불필요 - 영어 주석 없음) |

---

---

## 9차 번역 완료 (2026-09-03)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/md5.cpp` | 비-Windows 스텁 주석, 인라인/멤버 함수 구현 섹션 헤더(중국어→한글), 첫 번째/네 번째 라운드 주석(중국어→한글) |
| `Client/md5.h` | 파일 헤더 주석 |
| `Client/MEffect.cpp` | 정적 멤버 초기화, 생성자/소멸자, 인스턴스 ID, 리소스 컨테이너 주석(중국어→한글), 멤버 함수, 카운트/링크/이펙트타겟/위치/FrameID/업데이트 설정 섹션 헤더 전체 |
| `Client/MEffect.h` | 파일 상단 이펙트 설명 블록(영어→한글), 신규/구 생성자 주석, 리소스 컨테이너 관리 주석, 이펙트 타입/선택가능/위력/속도 섹션 헤더, 신규 추가 멤버 주석 |
| `Client/MEffectGeneratorDef.h` | 클래스 ID 섹션 헤더 |
| `Client/MEffectGeneratorTable.cpp` | 전역 변수, 생성자/소멸자, 멤버 함수, 해제, 이펙트 생성 섹션 헤더 |
| `Client/MEffectGeneratorTable.h` | 초기화/해제 섹션 헤더 |
| `Client/MEffectManager.cpp` | 생성자/소멸자, 멤버 함수, 해제, 이펙트 추가, 업데이트 섹션 헤더 |
| `Client/MEffectManager.h` | 이펙트 목록, 해제, 추가, 업데이트, 목록 섹션 헤더 |
| `Client/MEffectSpriteTypeTable.cpp` | 전역 변수, 메모리 보호/ASAN 영어 주석 전체, 파일에 저장/불러오기, Pair FrameID 목록 섹션 헤더 |
| `Client/MEffectSpriteTypeTable.h` | 손상 감지용 섀도우 복사본, 검증 함수 주석 |
| `Client/MEffectStatusTable.cpp` | 전역 변수, 멤버 함수(x2), 파일에 저장, 파일에서 불러오기 섹션 헤더 |
| `Client/MEffectStatusTable.h` | 파일 입출력 섹션 헤더 |
| `Client/MEffectTarget.cpp` | 정적 멤버, 결과 설정, 대입 연산자(x2), 존 이름 반환 섹션 헤더 |
| `Client/MEffectTarget.h` | 대입 연산자, 인스턴스 ID, 설정, 대입 연산자, 반환 섹션 헤더 (MEffectTarget/MPortalEffectTarget) |
| `Client/MemoryPool.cpp` | (번역 불필요 - 이미 한글 주석) |
| `Client/MemoryPool.h` | (번역 불필요 - 이미 한글 주석) |
| `Client/MEventManager.cpp` | 전역 변수, 이벤트 처리 섹션 헤더 |
| `Client/MEventManager.h` | 명시적 헤더 경로 주석(영어→한글) |
| `Client/MFakeCreature.cpp` | 정적 멤버, FakeID 반환, 생성자/소멸자, 우선순위 큐/목록 제거, 멤버 함수, 가짜 크리처 타입 설정, 액션 결과 설정, 액션, 빠른 이동 액션 설정, 가짜 위치 설정, 가짜 업데이트, 가짜 종료 여부, 기타 패킷 특수 액션, 다음 목적지 설정/반환, 다음 위치 반환, 목적지 설정, 메시지:경로탐색, 비어있지않으면 삭제, 우선순위 큐 테스트 코드, 최선 우선 탐색, 목적지 반환, 이동, 액션 중지 주석 전체 |
| `Client/MFakeCreature.h` | 반환, 클래스 타입, 가짜 크리처 타입, 가짜 위치 설정, 액션 섹션 헤더 |

---

## 10차 번역 완료 (2026-09-03)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/MFallingEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MFallingEffectGenerator.h` | (번역 불필요) |
| `Client/MFirePiecingEffectGenerator.cpp` | 전역 변수, 이펙트 생성 섹션 헤더 |
| `Client/MFirePiecingEffectGenerator.h` | (번역 불필요) |
| `Client/MFollowPathEffectGenerator.cpp` | 이펙트 생성 섹션 헤더 |
| `Client/MFollowPathEffectGenerator.h` | (번역 불필요) |
| `Client/MFunctionManager.cpp` | 생성자/소멸자, 멤버 함수, 초기화, 해제, 실행, 함수 설정 섹션 헤더 |
| `Client/MFunctionManager.h` | (번역 불필요) |
| `Client/MGameDef.h` | 인벤토리, 상점 선반, 창고, 거래 섹션 헤더 |
| `Client/MGameStringTable.cpp` | 전역 변수 섹션 헤더 |
| `Client/MGameStringTable.h` | (번역 불필요) |
| `Client/MGameTime.cpp` | 전역 변수, 생성자/소멸자, 멤버 함수, 기준 시간 설정, 현재 시간 설정 섹션 헤더 |
| `Client/MGameTime.h` | 반환 섹션 헤더 |
| `Client/MGridItemManager.cpp` | 생성자/소멸자, 멤버 함수, 초기화, 해제, 검색, 아이템 교환 가능 여부, 추가(×2), 반환, 제거(×2), 아이템 교환, 그리드 순서로 아이템 검색 섹션 헤더 |
| `Client/MGridItemManager.h` | 초기화/해제, 반환, 추가/반환/제거, 아이템 교환 가능 여부, 적합한 위치 반환, 그리드 순서로 아이템 검색 섹션 헤더 |
| `Client/MGuidanceEffect.cpp` | 생성자/소멸자, 멤버 함수, 이동, 크리처 추적, 업데이트 섹션 헤더 |
| `Client/MGuidanceEffect.h` | (번역 불필요) |
| `Client/MGuildInfoMapper.cpp` | 전역 변수, 초기화, 해제, 연산자 [], 반환, 설정, 파일에 저장(×2), 파일에서 불러오기(×2), 대입 연산자, 파일에서 값 변경, 정보를 파일에 저장 섹션 헤더 |
| `Client/MGuildInfoMapper.h` | 반환/설정, 대입 연산자, 파일 입출력, 초기화/해제, 연산자 [], 파일 입출력, 전역 변수 섹션 헤더 |
| `Client/MGuildMarkManager.cpp` | 전역 변수, 해제, 길드마크 생성, 추가, 반환, 작은 반환, 보유 여부 확인, 로드, 저장, 병합, 등급 마크 로드/반환/작은 반환, 레벨 마크 반환/작은 반환 섹션 헤더 |
| `Client/MGuildMarkManager.h` | 해제, 길드마크 생성, 추가/반환, 파일 입출력, 전역 변수 섹션 헤더 |

---

---

## 11차 번역 완료 (2026-09-03)

| 파일명 | 번역 내용 |
|--------|-----------|
| `Client/MHelicopterManager.cpp` | 상수 정의, 해제(×2), 추적 크리처 설정, 사운드 업데이트, 업데이트, 헬리콥터 추가/반환/즉시제거/제거 섹션 헤더 |
| `Client/MHelicopterManager.h` | (번역 불필요) |
| `Client/MHelpDef.h` | (번역 불필요) |
| `Client/MHelpDisplayer.cpp` | 전역 변수, 생성자/소멸자, 멤버 함수 섹션 헤더 |
| `Client/MHelpDisplayer.h` | (번역 불필요) |
| `Client/MHelpManager.cpp` | 전역 변수, 도움말 이벤트 실행, 다음 반환, True/False 설정, 파일에 저장(×2), 파일에서 불러오기(×2), 출력 타입 반환, 초기화, 해제, 이벤트 발생 기록 초기화, 이벤트 실행 섹션 헤더 |
| `Client/MHelpManager.h` | 파일 입출력(×3), 타입 반환(×2), 다음 반환, 비교 노드 반환/설정, 반환, 초기화/해제, 이벤트 실행, 파일 입출력, 전역 변수, 도움말 이벤트 실행 섹션 헤더 |
| `Client/MHelpMessageManager.cpp` | (번역 불필요) |
| `Client/MHelpMessageManager.h` | (번역 불필요) |
| `Client/MHelpStringTable.cpp` | 전역 변수, 생성자/소멸자, 멤버 함수, 초기화, 표시 기록 초기화, 파일에서 불러오기 섹션 헤더 |
| `Client/MHelpStringTable.h` | 초기화, 파일 입출력 섹션 헤더 |
| `Client/MHomingEffect.cpp` | 생성자/소멸자, 멤버 함수, 목표 설정, 각도로 방향 설정, 업데이트 섹션 헤더 |
| `Client/MHomingEffect.h` | 목표 설정 섹션 헤더 |
| `Client/MImageObject.cpp` | 정적 멤버 초기화, 생성자/소멸자, 멤버 함수, 설정, 파일에 저장, 파일에서 불러오기, 벽 설정, 벽 방향 반환 섹션 헤더 |
| `Client/MImageObject.h` | 파일 입출력 섹션 헤더 |
| `Client/MInteractionObject.cpp` | 생성자/소멸자, 멤버 함수, 파일에 저장, 파일에서 불러오기, 액션 설정, 다음 액션 설정 섹션 헤더 |
| `Client/MInteractionObject.h` | 파일 입출력 섹션 헤더 |
| `Client/MInteractionObjectTable.cpp` | 전역 변수, 파일에 저장, 파일에서 불러오기 섹션 헤더 |
| `Client/MInteractionObjectTable.h` | 파일 입출력 섹션 헤더 |
| `Client/MInternetConnection.cpp` | 해제(×2), 초기화(×2), 옵션 설정, 마지막 응답 정보 반환, 업데이트 섹션 헤더 |
| `Client/MInventory.cpp` | 전역 변수, 생성자/소멸자, 멤버 함수, 상태 이상 확인, 아이템 추가(×2), 아이템 적합 위치 반환, 아이템 검색 섹션 헤더 |
| `Client/MInventory.h` | 추가, 적합한 위치 반환, 아이템 검색 섹션 헤더 |
| `Client/MissingGlobals.cpp` | 렌더링 플래그, 추가 누락 전역 변수 섹션 헤더 |

---

## 다음 번역 대상 (vcxproj 기준 이후 파일들)

MissingGlobals.cpp 이후 파일들 (MItem.cpp 부터) 은 미번역 상태.
