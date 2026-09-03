#ifndef __INPUT_FOCUS_MANAGER_H__
#define __INPUT_FOCUS_MANAGER_H__

#include <cstddef>  // NULL 사용

/**
 * InputFocusManager - 전역 텍스트 입력 포커스 관리
 *
 * 현재 어떤 LineEditorVisual이 키보드 포커스를 가지고 있는지 관리하고
 * SDL 텍스트 입력 이벤트를 포커스된 에디터로 전달한다.
 *
 * 단일 전역 콜백 방식 대신 제대로 된 포커스 시스템으로 대체하여
 * 모든 입력 박스가 올바르게 동작하도록 한다.
 */

class LineEditorVisual;

class InputFocusManager
{
public:
	// 싱글턴 인스턴스 반환
	static InputFocusManager& GetInstance();

	// 현재 포커스된 텍스트 에디터 설정
	// 포커스를 해제하려면 NULL을 전달
	void SetFocusedEditor(LineEditorVisual* editor);

	// 현재 포커스된 텍스트 에디터 반환
	// 포커스된 에디터가 없으면 NULL 반환
	LineEditorVisual* GetFocusedEditor() const;

	// SDL 텍스트 입력 이벤트 처리
	// 포커스된 에디터가 있으면 그쪽으로 이벤트를 전달
	void HandleTextInput(const char* text);

	// SDL 텍스트 편집 이벤트 처리(IME 조합)
	// 포커스된 에디터가 있으면 그쪽으로 이벤트를 전달
	void HandleTextEditing(const char* text, int start, int length);

	// SDL 키 다운 이벤트 처리(백스페이스, 방향키 등의 제어키)
	// 포커스된 에디터가 있으면 그쪽으로 이벤트를 전달
	void HandleKeyDown(unsigned int vk_code);

	// 현재 포커스된 에디터가 있는지 확인
	bool HasFocus() const { return m_focusedEditor != NULL; }

private:
	// 싱글턴을 위한 private 생성자
	InputFocusManager();
	~InputFocusManager() = default;

	// 복사 방지
	InputFocusManager(const InputFocusManager&) = delete;
	InputFocusManager& operator=(const InputFocusManager&) = delete;

	// 현재 포커스된 텍스트 에디터
	LineEditorVisual* m_focusedEditor;
};

// DXLibBackendSDL(VS_UI 헤더를 포함할 수 없음)에서 사용하기 위한 전역 접근자
extern InputFocusManager& g_GetInputFocusManager();

#endif // __INPUT_FOCUS_MANAGER_H__
