#ifndef ACCHUD_COMMON_H
#define ACCHUD_COMMON_H

#include "windows.h"
#include <cstdio>

namespace debug {
	static const auto size = 256;
	static char buffer[size] = "[acchud] ";

	template<typename... Args>
	inline void log(const char *message, Args... args) {
		// +/- 9 because of the initial buffer contents
		sprintf_s(buffer + 9, size - 9, message, args...);

		OutputDebugStringA(buffer);
	}
}

namespace global {
	extern bool exiting;
	extern bool should_exit;

	extern HWND game_hwnd;
}

#endif //ACCHUD_COMMON_H
