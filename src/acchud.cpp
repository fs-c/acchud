#include "acchud.h"

bool global::exiting = false;
bool global::should_exit = false;
HWND global::game_hwnd = nullptr;

DWORD WINAPI cleanup(void *instance) {
	debug::log("cleaning up");

	hooks::release();

	FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}

DWORD WINAPI main_thread(void *instance) {
	debug::log("main thread started");

	if (!hooks::initialize()) {
		hooks::bind();
	}

	while (!global::should_exit) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	global::exiting = true;
	cleanup(instance);

	return 0;
}

DWORD WINAPI create_thread(LPTHREAD_START_ROUTINE function, LPVOID parameter) {
	auto handle = CreateThread(nullptr, NULL, function, parameter,
		NULL, nullptr);

	if (handle) {
		if (CloseHandle(handle)) {
			return 1;
		}

		debug::log("failed closing handle to thread");

		return 0;
	} else {
		MessageBoxA(nullptr, "Failed creating thread",
			"Error", MB_ICONERROR);

		return 0;
	}
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		debug::log("got DLL_PROCESS_ATTACH");

		create_thread(&main_thread, instance);
	} else if (reason == DLL_PROCESS_DETACH) {
		if (global::exiting) {
			debug::log("ignoring DLL_PROCESS_DETACH");
		} else {
			debug::log("warning: DLL_PROCESS_DETACH "
				"called while main thread was still running");

			create_thread(&cleanup, instance);
		}
	}

	return TRUE;
}