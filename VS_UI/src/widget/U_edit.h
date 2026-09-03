#ifndef __LINE_EDITOR_H__
#define __LINE_EDITOR_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Fl2.h에서 PrintInfo 정의 가져오기
#include "../hangul/Fl2.h"

// Point 클래스 정의 가져오기
#include "../../../basic/BasicData.h"

/**
 * LineEditor - Simplified Windows IMM (Input Method Manager) replacement for macOS
 * This provides basic text editing functionality with UTF-32 support for SDL text input
 */

class LineEditor
{
public:
    static const int MAX_TEXT = 1024;

    // 다국어를 제대로 지원하기 위한 UTF-32 텍스트 버퍼
    uint32_t m_Text[MAX_TEXT];
    int m_CursorPos;                // 커서 위치 (문자 단위)
    int m_TextLen;                 // 텍스트 길이 (문자 단위)
    int m_Limit;                   // 최대 문자 수
    bool m_bAcquired;

    // IME 조합 버퍼 (조합 중인 텍스트)
    uint32_t m_Composing[MAX_TEXT];
    int m_ComposingLen;

    LineEditor();
    ~LineEditor() = default;

    // 기본 메서드
    void Acquire();
    void Unacquire();
    bool IsAcquire() const;

    // 새로운 SDL 텍스트 입력 메서드
    void HandleTextInput(const char* text);
    void HandleTextEditing(const char* text, int start, int length);

    // 텍스트 조작 메서드
    void InsertText(const uint32_t* text, int len);
    void InsertChar(uint32_t c);
    void DeleteChar(int offset);
    void Backspace();
    void MoveCursor(int delta);
    void SetCursor(int pos);
    int GetCursorPos() const { return m_CursorPos; }
    int GetTextLen() const { return m_TextLen; }

    // IME 조합
    void StartComposition(const char* text, int start, int length);
    void UpdateComposition(const char* text, int start, int length);
    void EndComposition();
    bool IsComposing() const { return m_ComposingLen > 0; }

    // 레거시 호환 메서드 (UTF-8 기반 동작)
    void AddString(const char* pStr);
    void EraseAll();
    void EraseCharacterBegin();
    void HomeCursor() { SetCursor(0); }
    void EndCursor() { SetCursor(m_TextLen); }
    void SetByteLimit(int limit) { m_Limit = limit; }
    void InsertMark(unsigned short mark);
    void KeyboardControl(unsigned int message, unsigned int key, long extra);

    // 텍스트를 UTF-8 문자열로 반환 (호환성용)
    const char* GetBuffer() const;  // 임시 UTF-8 버퍼 반환
    const char* GetString() const { return GetBuffer(); }  // GetBuffer()의 별칭
    int Size() const { return m_TextLen; }  // 문자 수
    int GetCursor() const { return m_CursorPos; }  // 커서 위치
};

class LineEditorVisual
{
public:
    LineEditor m_Editor;
    int m_X;
    int m_Y;
    int m_AbsWidth;
    int m_MaxWidth;
    bool m_bPasswordMode;
    bool m_bAcquired;
    PrintInfo m_PrintInfo;
    unsigned long m_CursorColor;

#ifdef PLATFORM_MACOS
    // Font Atlas 렌더링 시스템
    void* m_GlyphCache;      // CGlyphCache* (헤더 의존성을 피하기 위해 void*)
    void* m_Layout;          // CTextLayout* (헤더 의존성을 피하기 위해 void*)
    bool m_LayoutDirty;      // 레이아웃 재구성 플래그
#endif

    LineEditorVisual();
    ~LineEditorVisual();

    void Acquire();
    void Unacquire();
    bool IsAcquire() const { return m_bAcquired; }

    // 호환성을 위해 m_Editor로 전달하는 메서드
    void AddString(const char* pStr) { m_Editor.AddString(pStr); }
    void EraseAll() { m_Editor.EraseAll(); }
    void EraseCharacterBegin() { m_Editor.EraseCharacterBegin(); }
    void SetByteLimit(int limit) { m_Editor.SetByteLimit(limit); }
    void KeyboardControl(unsigned int message, unsigned int key, long extra) {
        m_Editor.KeyboardControl(message, key, extra);
    }
    const char* GetString() const { return m_Editor.GetString(); }

    // 호환성 메서드: 기존 코드를 위해 const char_t*(wide string) 반환
    const char_t* GetStringWide() const;

    int Size() const { return m_Editor.Size(); }
    int GetCursor() const { return m_Editor.GetCursor(); }
    void InsertMark(char_t ch) { m_Editor.InsertMark((unsigned short)ch); }

    // 커서 이동
    void HomeCursor() { m_Editor.HomeCursor(); }
    void EndCursor() { m_Editor.EndCursor(); }

    // 텍스트 색상 설정
    void SetInputStringColor(unsigned long rgb) { m_CursorColor = rgb; }  // 텍스트 색상으로 커서 색상 사용

    // 에디터 모드 (호환성을 위한 스텁)
    void SetEditorMode(int gap, int height = 0) { /* 스텁: SDL 구현에서는 불필요 */ }
    void SetDigitOnlyMode(bool enable) { /* 스텁: SDL 구현에서는 불필요 */ }

    void SetPosition(int x, int y);
    void SetAbsWidth(int width);
    void SetPrintInfo(PrintInfo& info);
    void SetCursorColor(unsigned long color);
    void PasswordMode(bool bPassword);
    int GetLineCount() const;
    bool ReachSizeOfBox() const;
    void Show() const;

    // GetPosition 메서드 - BasicData::Point 반환
    Point GetPosition() const;  // cpp 파일에 구현됨
};

#endif // __LINE_EDITOR_H__
