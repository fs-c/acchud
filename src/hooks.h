#ifndef ACCHUD_HOOKS_H
#define ACCHUD_HOOKS_H

#include "common.h"

#include <d3d11.h>
#include <thread>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam);

#include "kiero/kiero.h"

#include "overlay.h"

namespace hooks {
	static int PRESENT_INDEX = 8;
	typedef HRESULT(__fastcall* present_t)(IDXGISwapChain*, UINT, UINT);
	static present_t original_present = nullptr;

	typedef LRESULT(CALLBACK* wnd_proc_t)(HWND, UINT, WPARAM, LPARAM);
	static wnd_proc_t original_wnd_proc = nullptr;

	int initialize();
	void bind();
	void release();
}

#endif //ACCHUD_HOOKS_H
