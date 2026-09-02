

# 목표


dk_client\build\vs2019\DarkEden.vcxproj

위 프로젝트에 속한 .cpp , .h 파일에 존재하는 
영어주석 -> 한글 주석으로 번역해줘 

1. 한번에 다하기는 많을거 같으니 이정도만 먼저 진행해줘
2. .cpp , .h 파일 외에는 따로 건드리지마.
  
	줄  461:     <ClCompile Include="H:\Source\GithubDesktop\dk_client\Client\DebugInfo.cpp" />
	줄  462:     <ClCompile Include="H:\Source\GithubDesktop\dk_client\Client\DebugKit.cpp" />
	줄  463:     <ClCompile Include="H:\Source\GithubDesktop\dk_client\Client\DebugLog.cpp" />
	줄  464:     <ClCompile Include="H:\Source\GithubDesktop\dk_client\Client\DirectXLib_PCH.cpp" />
	줄  465:     <ClCompile Include="H:\Source\GithubDesktop\dk_client\Client\DrawCreatureDivineGuidance.cpp" />
	줄  466:     <ClCompile Include="H:\Source\GithubDesktop\dk_client\Client\DrawCreatureEffect.cpp" />
	줄  467:     <ClCompile Include="H:\Source\GithubDesktop\dk_client\Client\DrawCreatureShadow.cpp" />


3. 번역완료된 파일들은 아래 md 파일에 기록하고 진행상황을 다음에도 이어갈수 있게 클로드 메모리에도 기록해줘 
4. 아래 기록된 파일들은 이미 번역이 진행된거니까 패스해도돼

dk_client\참고자료\작업지시\번역 완료파일(DarkEden).md

5. 혹시나 파일크기가 너무 크다면 별도 세션에서 따로 진행할테니까 진행여부를 물어봐줘  (300KB 이상)



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
