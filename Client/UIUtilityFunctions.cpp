//----------------------------------------------------------------------
// UIUtilityFunctions.cpp - macOS UI 유틸리티 함수
//----------------------------------------------------------------------
#include "../Client_PCH.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//----------------------------------------------------------------------
// 문자열 축소 함수
//----------------------------------------------------------------------

// 너비에 맞게 문자열 축소 (기본 버전)
void ReduceString(char* pStr, int maxWidth)
{
    if (pStr == NULL || maxWidth <= 0)
        return;

    int len = strlen(pStr);
    if (len <= maxWidth)
        return;

    // Truncate and add "..."
    pStr[maxWidth - 3] = '.';
    pStr[maxWidth - 2] = '.';
    pStr[maxWidth - 1] = '.';
    pStr[maxWidth] = '\0';
}

// 문자열 축소 변형 2 (DBCS 문자열용)
void ReduceString2(char* pStr, int maxWidth)
{
    if (pStr == NULL || maxWidth <= 0)
        return;

    int len = 0;
    char* p = pStr;

    // Count characters (considering DBCS)
    while (*p && len < maxWidth)
    {
        if ((*p & 0x80) != 0)
        {
            // Double-byte character
            p += 2;
            len += 2;
        }
        else
        {
            p++;
            len++;
        }
    }

    if (len > maxWidth - 3)
    {
        pStr[maxWidth - 3] = '.';
        pStr[maxWidth - 2] = '.';
        pStr[maxWidth - 1] = '.';
        pStr[maxWidth] = '\0';
    }
}

// 문자열 축소 변형 3 (중간 생략 부호 포함)
void ReduceString3(char* pStr, int maxWidth)
{
    if (pStr == NULL || maxWidth <= 0)
        return;

    int len = strlen(pStr);
    if (len <= maxWidth)
        return;

    // Truncate from end and add "..."
    int cutLen = maxWidth - 3;
    if (cutLen > 0)
    {
        pStr[cutLen] = '.';
        pStr[cutLen + 1] = '.';
        pStr[cutLen + 2] = '.';
        pStr[cutLen + 3] = '\0';
    }
}

//----------------------------------------------------------------------
// 얼굴 스타일
//----------------------------------------------------------------------
// UI_GetFaceStyle - PacketFunction.cpp에서 구현됨
// int UI_GetFaceStyle(bool bMale, int index) { ... }

//----------------------------------------------------------------------
// 입력 업데이트 함수
//----------------------------------------------------------------------
// 참고: UpdateInput()과 UpdateMouse()는 GameMain.cpp에서 구현됨
// 중복 심볼 방지를 위해 스텁 구현 제거
/*
void UpdateInput()
{
    // Update DirectInput state (mouse buttons, keyboard, etc.)
    if (g_pSDLInput) {
        g_pSDLInput->UpdateInput();
    }
}

void UpdateMouse()
{
    // Stub: SDL input system handles this
}
*/

//----------------------------------------------------------------------
// 파일 I/O 헬퍼
//----------------------------------------------------------------------
// 참고: FileOpenBinary()는 GameMain.cpp에서 구현됨
// 중복 심볼 방지를 위해 스텁 구현 제거
/*
bool FileOpenBinary(const char* pFilename, std::ifstream& file)
{
    if (pFilename == NULL)
        return false;

    file.open(pFilename, std::ios::binary);
    return file.is_open();
}
*/

//----------------------------------------------------------------------
// SPRITE_FILEPOSITION_NODE
//----------------------------------------------------------------------
// 메서드가 다른 곳에 이미 존재함 - 스텁 주석 처리
// bool SPRITE_FILEPOSITION_NODE::LoadFromFile(std::ifstream& file)
// {
//     if (!file.is_open())
//         return false;
//     file.read((char*)&Position, sizeof(POINT));
//     file.read((char*)&Width, sizeof(WORD));
//     file.read((char*)&Height, sizeof(WORD));
//     return file.good();
// }
