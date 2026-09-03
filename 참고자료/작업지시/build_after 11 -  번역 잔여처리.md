

# 목표


[
   
](<번역 완료파일(basic).md>) [
   
](<번역 완료파일(DarkEden).md>) [
   
](<번역 완료파일(DXLib).md>) [
   
](<번역 완료파일(framelib).md>) [
   
](<번역 완료파일(sprite).md>) [
   
](<번역 완료파일(SpriteLib).md>) [
   
](<번역 완료파일(TextSystem).md>)



dk_client\build\vs2019\VS_UI.vcxproj

위에 번역 완료파일을 모두 정리해뒀고
VS_UI 프로젝트 이어서 하면 될거같아 

위 프로젝트에 속한 .cpp , .h 파일에 존재하는 
영어주석 -> 한글 주석으로 번역해줘 

1. 한번에 다하기는 많을거 같으니 이정도만 먼저 진행해줘
2. .cpp , .h 파일 외에는 따로 건드리지마.

3. 아래 기록된 파일들은 이미 번역이 진행된거니까 패스해도돼
dk_client\참고자료\작업지시\번역 완료파일(VS_UI).md

4. 아래 파일을 읽고 파일크기를 합산한뒤, 1mb 단위 아래로 작업해줘 (한꺼번에 다하지마) 
 3번 내용에 진행내역이 기록되어 있으니, 번역이 완료된 파일은 다시 안건드려도돼

check_encoding_result.md

# 예시) check_encoding_result.md 에서 아래 파일 목록을 모두 합치면 전체 용량 (KB): 전체 용량 (KB): 약 121.38 KB

| UTF-8 | 15.8 KB | `.\Client\MActionInfoTable.cpp` |
| UTF-8 | 31.2 KB | `.\Client\MActionInfoTable.h` |
| UTF-8 | 30.2 KB | `.\Client\MActionResult.cpp` |
| UTF-8 | 27.9 KB | `.\Client\MActionResult.h` |
| UTF-8 | 2.1 KB | `.\Client\MActionResultDef.h` |
| UTF-8 | 3.7 KB | `.\Client\MAnimationObject.cpp` |
| UTF-8 | 2.9 KB | `.\Client\MAnimationObject.h` |
| UTF-8 | 6.8 KB | `.\Client\MAroundZoneEffectGenerator.cpp` |
| UTF-8 | 800 B | `.\Client\MAroundZoneEffectGenerator.h` |



5. 번역완료된 파일들은 아래 md 파일에 기록하고 진행상황을 다음에도 이어갈수 있게 클로드 메모리에도 기록해줘 




# 요구사항

3. 코드 스타일: 
   - 가능하면 Guard Clause 원칙 지키기 
   - Visual Studio 2019 환경 , C++ 11 표준 사용중
   - Allman 스타일(중괄호 다음 줄), 헝가리언 표기법 준수.
   - 한 줄 제어문도 줄바꿈/중괄호 필수.
   - 인코딩: UTF-8 with BOM.   
4. 기존 로직 보존: 기존 프로젝트의 다른 코드는 건드리지 마세요(재탐색 금지).
5. 응답은 항상 한글로 해주세요.
6. 답변 시 코드 이외의 인사말, 설명, 요약 생략
