#ifndef TEXTSYSTEM_FONTHANDLEUTIL_H
#define TEXTSYSTEM_FONTHANDLEUTIL_H

#include <stdint.h>

namespace TextSystem {

// 비 Windows 빌드에서 폰트 크기를 포인터 크기의 핸들로 인코딩합니다.
// 크기 정보를 유지하면서도 폰트 객체 할당을 피하기 위함입니다.
inline void* EncodeFontSizeHandle(int size)
{
	if (size < 0)
		size = -size;
	if (size <= 0)
		size = 16;
	uintptr_t value = static_cast<uintptr_t>(size) & 0xFFFFu;
	value |= 0xF0000u; // 인코딩된 핸들임을 식별하기 위한 매직 태그
	return reinterpret_cast<void*>(value);
}

inline int DecodeFontSizeHandle(void* handle, int fallback = 16)
{
	if (!handle)
		return fallback;
	uintptr_t value = reinterpret_cast<uintptr_t>(handle);
	if ((value & 0xF0000u) == 0xF0000u) {
		int size = static_cast<int>(value & 0xFFFFu);
		return size > 0 ? size : fallback;
	}
	if (value > 0 && value < 256)
		return static_cast<int>(value);
	return fallback;
}

} // namespace TextSystem

#endif // TEXTSYSTEM_FONTHANDLEUTIL_H
