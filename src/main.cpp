#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

unsigned long get_process_id(const char *process_name) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == FALSE) {
		return 0;
	}

	while (Process32Next(snapshot, &entry) == TRUE) {
		if (!_stricmp(entry.szExeFile, process_name)) {
			return entry.th32ProcessID;
		}
	}

	return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM _) {
	unsigned long id;

	GetWindowThreadProcessId(hwnd, &id);

	if (id == 9932) {
		printf("found it!\n");
	}

	return TRUE;
}

struct callback_data {
	HWND handle;
	unsigned long process_id;
};

BOOL CALLBACK enum_windows_callback(HWND handle, callback_data *data) {
	unsigned long check_process_id;

	GetWindowThreadProcessId(handle, &check_process_id);

	if (check_process_id == data->process_id) {
		if (!IsWindowVisible(handle)) {
			return TRUE;
		}

		data->handle = handle;

		return FALSE;
	}

	return TRUE;
}

HWND get_window_handle(unsigned long process_id) {
	callback_data callback_data{nullptr, process_id};

	EnumWindows(reinterpret_cast<WNDENUMPROC>(enum_windows_callback),
		reinterpret_cast<LPARAM>(&callback_data));

	return callback_data.handle;
}

int main() {
	try {
		printf("getting process id... ");
		const auto process_id = get_process_id("AC2-Win64-Shipping.exe");
		if (process_id) {
			printf("%lu\n", process_id);
		} else {
			throw std::runtime_error("couldn't get process id");
		}

		printf("getting window handle... ");
		const auto handle = get_window_handle(process_id);
		if (handle) {
			printf("%p\n", handle);
		} else {
			throw std::runtime_error("couldn't get window handle");
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(handle);

		return 0;
	} catch (std::runtime_error &error) {
		printf("error: %s\n", error.what());

		return 1;
	}
}
