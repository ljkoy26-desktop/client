#include "U_edit.h"
#include "../hangul/CI.h"
#include "../InputFocusManager.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef PLATFORM_MACOS
#include <SDL2/SDL.h>
#include "../../../Client/TextSystem/TextService.h"
#include "../../../Client/TextSystem/RenderTargetSpriteSurface.h"
#include "../../../Client/SpriteLib/CSpriteSurface.h"
#endif

// FL2 함수 전방 선언 (hangul/FL2.cpp에 정의됨)
extern void g_Print(int x, int y, const char* sz_str, void* p_print_info);
extern int g_GetStringWidth(const char* sz_str, void* hfont);
extern int g_GetStringHeight(const char* sz_str, void* hfont);

// 전역 CI에 대한 외부 참조 (커서 깜빡임 상태용)
extern CI* gC_ci;

// 백 버퍼 서피스에 대한 외부 참조 (spritectl blt용)
#ifdef PLATFORM_MACOS
// SDL 렌더러에 대한 외부 참조
extern SDL_Renderer* g_pSDLRenderer;
extern CSpriteSurface* g_pBack;
extern CSpriteSurface* g_pLast;  // UI는 g_pBack이 아닌 g_pLast에 렌더링한다!
#endif

// ============================================================================
// UTF-8 <-> UTF-32 변환 (textbox_demo.c에서 가져옴)
// ============================================================================

static int utf8_to_utf32(const char* s, uint32_t* out, int cap) {
	int n = 0;
	while (*s && n < cap) {
		uint32_t c;
		unsigned char b = *s++;

		if (b < 0x80) {
			c = b;
		} else if ((b >> 5) == 0x6) {
			c = ((b & 0x1F) << 6) | (*s++ & 0x3F);
		} else if ((b >> 4) == 0xE) {
			c = ((b & 0x0F) << 12) |
			    ((*s++ & 0x3F) << 6) |
			    (*s++ & 0x3F);
		} else if ((b >> 3) == 0x1E) {
			c = ((b & 0x07) << 18) |
			    ((*s++ & 0x3F) << 12) |
			    ((*s++ & 0x3F) << 6) |
			    (*s++ & 0x3F);
		} else {
			continue;  // 잘못된 UTF-8
		}
		out[n++] = c;
	}
	return n;
}

static int utf32_to_utf8(uint32_t c, char out[5]) {
	if (c < 0x80) {
		out[0] = c;
		out[1] = 0;
		return 1;
	} else if (c < 0x800) {
		out[0] = 0xC0 | (c >> 6);
		out[1] = 0x80 | (c & 0x3F);
		out[2] = 0;
		return 2;
	} else if (c < 0x10000) {
		out[0] = 0xE0 | (c >> 12);
		out[1] = 0x80 | ((c >> 6) & 0x3F);
		out[2] = 0x80 | (c & 0x3F);
		out[3] = 0;
		return 3;
	} else {
		out[0] = 0xF0 | (c >> 18);
		out[1] = 0x80 | ((c >> 12) & 0x3F);
		out[2] = 0x80 | ((c >> 6) & 0x3F);
		out[3] = 0x80 | (c & 0x3F);
		out[4] = 0;
		return 4;
	}
}

// ============================================================================
// LineEditor 구현
// ============================================================================

LineEditor::LineEditor()
{
	memset(m_Text, 0, sizeof(m_Text));
	m_CursorPos = 0;
	m_TextLen = 0;
	m_Limit = MAX_TEXT - 1;
	m_bAcquired = false;
	memset(m_Composing, 0, sizeof(m_Composing));
	m_ComposingLen = 0;
}

void LineEditor::Acquire()
{
	m_bAcquired = true;
}

void LineEditor::Unacquire()
{
	m_bAcquired = false;
}

bool LineEditor::IsAcquire() const
{
	return m_bAcquired;
}

// 커서 위치에 UTF-32 텍스트 삽입
void LineEditor::InsertText(const uint32_t* text, int len)
{
	if (len <= 0) return;
	if (m_TextLen + len > m_Limit) return;

	// 공간 확보를 위해 기존 텍스트 이동
	memmove(&m_Text[m_CursorPos + len],
	        &m_Text[m_CursorPos],
	        (m_TextLen - m_CursorPos) * sizeof(uint32_t));

	// 새 텍스트 삽입
	memcpy(&m_Text[m_CursorPos], text, len * sizeof(uint32_t));

	m_CursorPos += len;
	m_TextLen += len;
}

// 단일 UTF-32 문자 삽입
void LineEditor::InsertChar(uint32_t c)
{
	InsertText(&c, 1);
}

// 오프셋 위치의 문자 삭제
void LineEditor::DeleteChar(int offset)
{
	if (offset < 0 || offset >= m_TextLen) return;

	memmove(&m_Text[offset],
	        &m_Text[offset + 1],
	        (m_TextLen - offset - 1) * sizeof(uint32_t));

	m_TextLen--;

	// 삭제된 문자 뒤에 있었다면 커서 위치 조정
	if (m_CursorPos > offset) {
		m_CursorPos--;
	}
	// 커서가 텍스트 길이를 넘어가지 않도록 보장
	if (m_CursorPos > m_TextLen) {
		m_CursorPos = m_TextLen;
	}
}

// 커서 앞 문자 삭제 (백스페이스)
void LineEditor::Backspace()
{
	if (m_CursorPos > 0) {
		// DeleteChar가 커서 조정을 처리함
		DeleteChar(m_CursorPos - 1);
	}
}

// delta 문자만큼 커서 이동
void LineEditor::MoveCursor(int delta)
{
	int newPos = m_CursorPos + delta;
	if (newPos < 0) newPos = 0;
	if (newPos > m_TextLen) newPos = m_TextLen;
	m_CursorPos = newPos;
}

// 커서를 절대 위치로 설정
void LineEditor::SetCursor(int pos)
{
	if (pos < 0) pos = 0;
	if (pos > m_TextLen) pos = m_TextLen;
	m_CursorPos = pos;
}

// SDL_TEXTINPUT 이벤트 처리 (확정된 텍스트)
void LineEditor::HandleTextInput(const char* text)
{
	if (text == NULL || text[0] == '\0') return;

	uint32_t utf32[32];
	int len = utf8_to_utf32(text, utf32, 32);

	if (IsComposing()) {
		EndComposition();
	}

	InsertText(utf32, len);
}

// SDL_TEXTEDITING 이벤트 처리 (IME 조합 중)
void LineEditor::HandleTextEditing(const char* text, int start, int length)
{
	if (length > 0) {
		// 현재 조합 중 - 조합 텍스트 저장
		m_ComposingLen = utf8_to_utf32(text, m_Composing, MAX_TEXT);
	} else {
		// 조합 종료
		EndComposition();
	}
}

// IME 조합 시작
void LineEditor::StartComposition(const char* text, int start, int length)
{
	HandleTextEditing(text, start, length);
}

// IME 조합 갱신
void LineEditor::UpdateComposition(const char* text, int start, int length)
{
	HandleTextEditing(text, start, length);
}

// IME 조합 종료 (조합된 텍스트 확정)
void LineEditor::EndComposition()
{
	if (m_ComposingLen > 0) {
		InsertText(m_Composing, m_ComposingLen);
		m_ComposingLen = 0;
	}
}

// 텍스트를 UTF-8 문자열로 반환 (호환성용)
const char* LineEditor::GetBuffer() const
{
	static char utf8_buffer[MAX_TEXT * 4 + 1];  // 최악의 경우: UTF-32 문자당 4바이트
	int offset = 0;

	for (int i = 0; i < m_TextLen && offset < (int)sizeof(utf8_buffer) - 4; i++) {
		char buf[5];
		int len = utf32_to_utf8(m_Text[i], buf);
		memcpy(&utf8_buffer[offset], buf, len);
		offset += len;
	}
	utf8_buffer[offset] = '\0';

	return utf8_buffer;
}

// 레거시: UTF-8 문자열 추가 (내부적으로 UTF-32로 변환)
void LineEditor::AddString(const char* pStr)
{
	if (pStr == NULL) return;

	uint32_t utf32[MAX_TEXT];
	int len = utf8_to_utf32(pStr, utf32, MAX_TEXT);

	if (m_TextLen + len <= m_Limit) {
		InsertText(utf32, len);
	}
}

// 레거시: 모든 텍스트 지우기
void LineEditor::EraseAll()
{
	m_TextLen = 0;
	m_CursorPos = 0;
	memset(m_Text, 0, sizeof(m_Text));
	m_ComposingLen = 0;
}

// 레거시: 커서 앞 문자 삭제
void LineEditor::EraseCharacterBegin()
{
	Backspace();
}

// 레거시: 특수 마크 삽입
void LineEditor::InsertMark(unsigned short mark)
{
	InsertChar((uint32_t)mark);
}

// KeyboardControl - 키보드 메시지의 메인 진입점
void LineEditor::KeyboardControl(unsigned int message, unsigned int key, long extra)
{
	switch (message)
	{
	case WM_CHAR:
		// 레거시: 단일 문자 입력
		InsertChar((uint32_t)key);
		break;

	case WM_TEXTINPUT:
		// SDL_TEXTINPUT 이벤트 (extra는 텍스트 포인터)
		{
			const char* text = (const char*)extra;
			HandleTextInput(text);
		}
		break;

	case WM_TEXTEDITING:
		// SDL_TEXTEDITING 이벤트 (조합)
		// 참고: 단순화된 처리 - 실제 구현에는 텍스트가 필요함
		// 지금은 이 메시지를 받으면 조합 상태만 초기화한다
		m_ComposingLen = 0;
		break;

	case WM_KEYDOWN:
		// 제어 키
		switch (key)
		{
		case VK_BACK:
			Backspace();
			break;
		case VK_LEFT:
			MoveCursor(-1);
			break;
		case VK_RIGHT:
			MoveCursor(1);
			break;
		case VK_HOME:
			SetCursor(0);
			break;
		case VK_END:
			SetCursor(m_TextLen);
			break;
		case VK_DELETE:
			DeleteChar(m_CursorPos);
			break;
		}
		break;
	}
}

// ============================================================================
// LineEditorVisual 구현
// ============================================================================

LineEditorVisual::LineEditorVisual()
{
	// 디버그: 오프셋 출력
//	printf("DEBUG LineEditorVisual::LineEditorVisual: this=%p, &m_Editor=%p, &m_Editor.m_CursorPos=%p\n",
//	       this, &m_Editor, &m_Editor.m_CursorPos);

	m_X = 0;
	m_Y = 0;
	m_AbsWidth = 100;
	m_MaxWidth = 100;
	m_bPasswordMode = false;
	m_bAcquired = false;
	m_PrintInfo.hfont = NULL;
	m_PrintInfo.text_color = 0xFFFFFF;
	m_PrintInfo.back_color = 0;
	m_PrintInfo.bk_mode = 0;
	m_PrintInfo.text_align = 0;
	m_CursorColor = 0xFFFFFF;

#ifdef PLATFORM_MACOS
	m_GlyphCache = NULL;
	m_Layout = NULL;
	m_LayoutDirty = true;

	// Font Atlas 렌더링 시스템 초기화 시도
	// 참고: 실제 객체는 필요할 때 생성한다 (지연 초기화)
	// 지금은 폴백으로 g_Print()를 사용한다
#endif
}

LineEditorVisual::~LineEditorVisual()
{
	// 이 에디터가 포커스 상태였다면 InputFocusManager의 포커스 해제
	if (InputFocusManager::GetInstance().GetFocusedEditor() == this) {
		InputFocusManager::GetInstance().SetFocusedEditor(NULL);
	}
}

void LineEditorVisual::Acquire()
{
	// 이 에디터를 포커스 에디터로 등록
	InputFocusManager::GetInstance().SetFocusedEditor(this);

	m_Editor.Acquire();
	m_bAcquired = true;

#ifdef PLATFORM_MACOS
	// macOS에서 SDL 텍스트 입력 활성화
	SDL_StartTextInput();
#endif
}

void LineEditorVisual::Unacquire()
{
	// 이 에디터가 포커스 상태였다면 포커스 해제
	if (InputFocusManager::GetInstance().GetFocusedEditor() == this) {
		InputFocusManager::GetInstance().SetFocusedEditor(NULL);
	}

	m_Editor.Unacquire();
	m_bAcquired = false;

#ifdef PLATFORM_MACOS
	// macOS에서 SDL 텍스트 입력 비활성화
	SDL_StopTextInput();
#endif
}

void LineEditorVisual::SetPosition(int x, int y)
{
	m_X = x;
	m_Y = y;
}

void LineEditorVisual::SetAbsWidth(int width)
{
	m_AbsWidth = width;
}

void LineEditorVisual::SetPrintInfo(PrintInfo& info)
{
	m_PrintInfo = info;
}

void LineEditorVisual::SetCursorColor(unsigned long color)
{
	m_CursorColor = color;
}

void LineEditorVisual::PasswordMode(bool bPassword)
{
	m_bPasswordMode = bPassword;
}

int LineEditorVisual::GetLineCount() const
{
	return 1;
}

bool LineEditorVisual::ReachSizeOfBox() const
{
	// PrintInfo에 크기 필드가 없으므로 기본 폰트 크기 사용
	const int DEFAULT_FONT_SIZE = 12;
	int len = m_Editor.GetTextLen();
	return (len * DEFAULT_FONT_SIZE) >= m_MaxWidth;
}

// 호환성 메서드: UTF-32를 wide string(char_t/UTF-16LE)으로 변환
const char_t* LineEditorVisual::GetStringWide() const
{
	static char_t wide_buffer[LineEditor::MAX_TEXT];
	int wide_len = 0;

	// UTF-32(m_Text)에서 UTF-16(char_t)으로 직접 변환
	for (int i = 0; i < m_Editor.m_TextLen && wide_len < LineEditor::MAX_TEXT - 1; i++) {
		uint32_t c = m_Editor.m_Text[i];

		// UTF-32에서 UTF-16으로 변환
		if (c < 0x10000) {
			// BMP 문자 - 단일 UTF-16 코드 유닛
			wide_buffer[wide_len++] = (char_t)c;
		} else if (c < 0x10FFFF) {
			// 보조 평면 - 서로게이트 쌍
			if (wide_len + 1 >= LineEditor::MAX_TEXT - 1) break;

			c -= 0x10000;
			wide_buffer[wide_len++] = (char_t)(0xD800 + (c >> 10));      // 상위 서로게이트
			wide_buffer[wide_len++] = (char_t)(0xDC00 + (c & 0x3FF));    // 하위 서로게이트
		} else {
			// 잘못된 유니코드 - 대체 문자 사용
			wide_buffer[wide_len++] = (char_t)0xFFFD;
		}
	}

	wide_buffer[wide_len] = 0;

	return wide_buffer;
}

void LineEditorVisual::Show() const
{
	// 표시할 텍스트 가져오기 (UTF-8)
	const char* textToDisplay = m_Editor.GetBuffer();

	// 비밀번호 모드 처리 (실제 텍스트 대신 별표 표시)
	char displayBuffer[1024];
	if (m_bPasswordMode) {
		int len = strlen(textToDisplay);
		for (int i = 0; i < len && i < (int)sizeof(displayBuffer) - 1; i++) {
			displayBuffer[i] = '*';
		}
		displayBuffer[len] = '\0';
		textToDisplay = displayBuffer;
	}

#ifdef PLATFORM_MACOS
	// 통합 렌더링을 위해 TextService 사용
	extern CSpriteSurface* g_pLast;

	// PrintInfo로부터 텍스트 스타일 구성
	TextSystem::TextStyle style;
	style.font = TextSystem::TextService::Get().GetFont(14);  // 기본 폰트 크기
	style.align = TextSystem::TextAlign::Left;
	style.lineSpacing = 0;
	style.color = TextSystem::ColorFromRGB(m_PrintInfo.text_color);
	style.color.a = 255;

	// 렌더 타겟 생성
	TextSystem::SpriteSurfaceRenderTarget target(g_pLast);

	// 텍스트 렌더링
	// 참고: TextService::DrawLine은 baseline 위치를 기대하지만 내부적으로 GetFontAscent()를 더한다
	// 그래서 m_Y를 baseline 위치로 그대로 전달한다
	TextSystem::TextService::Get().DrawLine(target, textToDisplay, m_X, m_Y, m_MaxWidth, style);

	// 에디터가 활성화되고 커서 깜빡임이 켜져 있으면 커서 그리기
	if (m_Editor.m_bAcquired && gC_ci != NULL && gC_ci->GetCursorBlink()) {
		// TextService를 사용해 커서 X 위치 계산
		int cursorX = m_X;
		if (m_Editor.m_CursorPos > 0) {
			// 커서 앞의 텍스트 가져오기
			char cursorBuffer[1024];
			const char* fullText = textToDisplay;

			// 커서 위치(문자)를 바이트 위치로 변환
			int bytePos = 0;
			int charPos = 0;
			while (charPos < m_Editor.m_CursorPos && fullText[bytePos] != '\0') {
				if ((fullText[bytePos] & 0xC0) != 0x80) {  // 연속 바이트가 아님
					charPos++;
				}
				bytePos++;
			}

			strncpy(cursorBuffer, fullText, bytePos);
			cursorBuffer[bytePos] = '\0';

			// TextService를 사용해 텍스트 너비 측정
			TextSystem::Metrics metrics = TextSystem::TextService::Get().MeasureText(cursorBuffer, style, 0);
			cursorX = m_X + metrics.width;
		}

		// 커서 그리기
		PrintInfo cursorPI = m_PrintInfo;
		cursorPI.text_color = m_CursorColor;

		if (m_Editor.m_ComposingLen > 0) {
			// IME 조합 중에는 밑줄 스타일 커서 표시
			g_Print(cursorX, m_Y + 2, "_", &cursorPI);
		} else {
			// 일반 블록 커서
			g_Print(cursorX, m_Y - 1, "▊", &cursorPI);
		}
	}
#else
	// Windows: 지금은 레거시 g_Print() 사용
	g_Print(m_X, m_Y, textToDisplay, (void*)NULL);

	// 커서 그리기
	if (m_Editor.m_bAcquired && gC_ci != NULL && gC_ci->GetCursorBlink()) {
		// 커서 위치 계산
		int cursorX = m_X;
		if (m_Editor.m_CursorPos > 0) {
			char cursorBuffer[1024];
			const char* fullText = m_Editor.GetBuffer();

			// 커서 위치를 바이트로 변환 (UTF-8 인식)
			int bytePos = 0;
			int charPos = 0;
			while (charPos < m_Editor.m_CursorPos && fullText[bytePos] != '\0') {
				if ((fullText[bytePos] & 0xC0) != 0x80) {  // 연속 바이트가 아님
					charPos++;
				}
				bytePos++;
			}

			strncpy(cursorBuffer, fullText, bytePos);
			cursorBuffer[bytePos] = '\0';
			cursorX = m_X + g_GetStringWidth(cursorBuffer, NULL);
		}

		// 텍스트 렌더링을 이용해 커서 그리기
		PrintInfo cursorPI = m_PrintInfo;
		cursorPI.text_color = m_CursorColor;
		g_Print(cursorX, m_Y - 2, "|", &cursorPI);
	}
#endif
}

Point LineEditorVisual::GetPosition() const
{
	Point p;
	p.Set(m_X, m_Y);
	return p;
}
