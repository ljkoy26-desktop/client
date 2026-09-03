#include "InputFocusManager.h"
#include "widget/U_edit.h"
#include <stdio.h>

InputFocusManager::InputFocusManager()
	: m_focusedEditor(NULL)
{
}

InputFocusManager& InputFocusManager::GetInstance()
{
	static InputFocusManager instance;
	return instance;
}

// DXLibBackendSDL(헤더를 포함할 수 없음)을 위한 전역 접근자
InputFocusManager& g_GetInputFocusManager()
{
	return InputFocusManager::GetInstance();
}

void InputFocusManager::SetFocusedEditor(LineEditorVisual* editor)
{
	m_focusedEditor = editor;

	// 디버그 로그
	if (editor) {
		printf("InputFocusManager: Set focused editor to %p\n", (void*)editor);
	} else {
		printf("InputFocusManager: Cleared focused editor\n");
	}
}

LineEditorVisual* InputFocusManager::GetFocusedEditor() const
{
	return m_focusedEditor;
}

void InputFocusManager::HandleTextInput(const char* text)
{
	if (!m_focusedEditor) {
		// 포커스된 에디터가 없으면 입력을 무시
		return;
	}

	// 포커스된 에디터로 텍스트 입력 전달
	m_focusedEditor->m_Editor.HandleTextInput(text);
}

void InputFocusManager::HandleTextEditing(const char* text, int start, int length)
{
	if (!m_focusedEditor) {
		// 포커스된 에디터가 없으면 편집 이벤트를 무시
		return;
	}

	// 포커스된 에디터로 텍스트 편집 이벤트 전달
	m_focusedEditor->m_Editor.HandleTextEditing(text, start, length);
}

void InputFocusManager::HandleKeyDown(unsigned int vk_code)
{
	if (!m_focusedEditor) {
		// 포커스된 에디터가 없으면 키 이벤트를 무시
		return;
	}

	// 포커스된 에디터로 키 다운 이벤트 전달
	// WM_KEYDOWN = 0x0100 (Windows 메시지 값)
	m_focusedEditor->m_Editor.KeyboardControl(0x0100, vk_code, 0);
}
