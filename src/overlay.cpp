#include "overlay.h"

#include <vector>
#include <string>

acc::SMElement graphics;

void init_graphics() {
	TCHAR name[] = TEXT("Local\\acpmf_graphics");
	graphics.handle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
		0, sizeof(acc::SPageFileGraphic), name);

	if (!graphics.handle) {
		debug::log("couldn't create graphics file mapping");
	} else {
		debug::log("created graphics file mapping");
	}

	graphics.buffer = (unsigned char*)MapViewOfFile(graphics.handle, FILE_MAP_READ,
		0, 0, sizeof(acc::SPageFileGraphic));

	if (!graphics.buffer) {
		debug::log("couldn't map graphics file");
	} else {
		debug::log("mapped graphics file");
	}
}

std::vector<std::wstring> get_lap_times() {
	static bool init = false;
	static int previous_lap = 0;

	static std::vector<std::wstring> lap_times;

	if (!init) {
		init_graphics();

		init = true;
	}

	const auto pf = (acc::SPageFileGraphic*)graphics.buffer;

	if (previous_lap == pf->completedLaps) {
		return lap_times;
	}

	previous_lap = pf->completedLaps;

	lap_times.emplace_back(pf->lastTime);

	return lap_times;
}

void overlay::render() {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	const float padding = 10.0f;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();

	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;

	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + work_size.x - padding;
	window_pos.y = work_pos.y + (work_size.y / 2) - padding;

	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 0.0f;

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.35f);

	bool open = true;
	if (ImGui::Begin("Example: Simple overlay", &open, window_flags)) {
		ImGui::Text("Lap times");

		ImGui::Separator();

		const auto &lap_times = get_lap_times();

		for (const auto &lap_time : lap_times) {
			ImGui::Text("%ls\n", lap_time.c_str());
		}
	}

	ImGui::End();
}
