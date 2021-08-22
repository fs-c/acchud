#include "hooks.h"

LRESULT CALLBACK wnd_proc_hook(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(global::game_hwnd, &cursor);

	ImGuiIO& io = ImGui::GetIO();
	io.MousePos.x = (float)cursor.x;
	io.MousePos.y = (float)cursor.y;

	if (msg == WM_KEYUP) {
		if (w_param == VK_DELETE) {
			global::should_exit = true;
		}
	}

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param)) {
		return true;
	}

	return CallWindowProcA(hooks::original_wnd_proc, hwnd, msg, w_param, l_param);
}

HRESULT __fastcall present_hook(IDXGISwapChain* swap_chain, UINT sync, UINT flags) {
	static bool init = false;

	static ID3D11DeviceContext* context;
	static ID3D11RenderTargetView* render_target;

	if (!init) {
		DXGI_SWAP_CHAIN_DESC desc;
		swap_chain->GetDesc(&desc);
		debug::log("got swap chain desc");

		global::game_hwnd = desc.OutputWindow;
		debug::log("game hwnd is %p", global::game_hwnd);

		ID3D11Device* device;
		HRESULT ret = swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&device);

		if (!SUCCEEDED(ret)) {
			debug::log("couldn't get device from swapchain");

			return hooks::original_present(swap_chain, sync, flags);
		} else {
			debug::log("device is %p", device);
		}

		hooks::original_wnd_proc = (WNDPROC)SetWindowLongPtr(global::game_hwnd,
			GWLP_WNDPROC, (LONG_PTR)wnd_proc_hook);

		device->GetImmediateContext(&context);
		debug::log("device context is %p", context);

		ImGui::CreateContext();

		ImGui_ImplWin32_Init(global::game_hwnd);
		ImGui_ImplDX11_Init(device, context);
		ImGui::GetIO().ImeWindowHandle = global::game_hwnd;

		ImGui::StyleColorsDark();

		ID3D11Texture2D* back_buffer;

		swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			(LPVOID*)&back_buffer);
		debug::log("created back buffer %p", back_buffer);
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target);
		debug::log("created render target view %p", render_target);
		back_buffer->Release();

		init = true;

		debug::log("finished imgui initialization");
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	overlay::render();

	ImGui::Render();
	context->OMSetRenderTargets(1, &render_target, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return hooks::original_present(swap_chain, sync, flags);
}

int hooks::initialize() {
	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
		return 0;
	} else {
		return 1;
	}
}

void hooks::bind() {
	// Wnd proc hook is set in present hook initialization
	kiero::bind(hooks::PRESENT_INDEX, (void **)&hooks::original_present, (void *)present_hook);
}

void hooks::release() {
	kiero::unbind(hooks::PRESENT_INDEX);

	SetWindowLongPtr(global::game_hwnd, GWLP_WNDPROC,
		(LONG_PTR)hooks::original_wnd_proc);
}
