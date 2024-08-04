#define _CRT_SECURE_NO_WARNINGS

#include "overlay.hpp"

#include <iostream>
#include <cmath>
//#include <stdarg.h>
//#include <codecvt>
#include <d3d11.h>


namespace overlay {
	using std::cerr;

	const bool Overlay::init_window() {
		overlay_window = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
		if (!overlay_window) {
			cerr << "[-] Failed to find NVIDIA GeForce Overlay\n";
			return false;
		}

		auto getInfo = GetWindowLongA(overlay_window, -20);
		auto changeAttributes = SetWindowLongA(overlay_window, -20, (LONG_PTR)(getInfo | 0x20));
		SetLayeredWindowAttributes(overlay_window, 0x000000, 0xFF, 0x02);
		SetWindowPos(overlay_window, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);

		RECT client_area{};
		GetClientRect(overlay_window, &client_area);
		RECT window_area{};
		GetWindowRect(overlay_window, &window_area);
		POINT diff{};
		ClientToScreen(overlay_window, &diff);
		const MARGINS margins{ window_area.left + (diff.x - window_area.left), window_area.top + (diff.y - window_area.top), client_area.right, client_area.bottom };
		DwmExtendFrameIntoClientArea(overlay_window, &margins);

		DXGI_SWAP_CHAIN_DESC sd{}; //dx11 device setup
		sd.BufferDesc.RefreshRate.Numerator = 185U; //monitor hz
		sd.BufferDesc.RefreshRate.Denominator = 1U;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1U;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 2U;
		sd.OutputWindow = overlay_window;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		constexpr D3D_FEATURE_LEVEL levels[2]{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
		D3D_FEATURE_LEVEL level{};
		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0U, levels, 2U, D3D11_SDK_VERSION, &sd, &swap_chain, &device, &level, &device_contxt);
		ID3D11Texture2D* back_buffer{ nullptr };
		swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

		if (!back_buffer)
			return false;
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);

		//cs2_window = FindWindowA("SDL_app", "Counter-Strike 2");
		//if (!cs2_window) {
		//	cerr << "[-] Failed to find CS2 Window\n";
		//	return false;
		//}


		// set window style
		//int style = (int)GetWindowLong(overlay_window, GWL_EXSTYLE);
		//SetWindowLongPtr(overlay_window, GWL_EXSTYLE, (LONG_PTR)(style | 0x20));
		// set window style end


		// set window transparency
		//MARGINS margin = {};
		//UINT opacity_flag, color_key_flag, color_key = 0;
		//UINT opacity = 0;

		//margin.cyBottomHeight = -1;
		//margin.cxLeftWidth = -1;
		//margin.cxRightWidth = -1;
		//margin.cyTopHeight = -1;

		//DwmExtendFrameIntoClientArea(overlay_window, &margin);

		//opacity_flag = 0x02;
		//color_key_flag = 0x01;
		//color_key = 0x000000;
		//opacity = 0xFF;

		//SetLayeredWindowAttributes(overlay_window, color_key, opacity, opacity_flag);
		//SetLayeredWindowAttributes(overlay_window, color_key, opacity, opacity_flag);
		// set window transparency end


		// set window top
		SetWindowPos(overlay_window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		// set window top end


		ShowWindow(overlay_window, SW_SHOW);
		UpdateWindow(overlay_window);
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 20.f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(overlay_window);
		ImGui_ImplDX11_Init(device, device_contxt);

		return true;
	}


	bool Overlay::init() {
		if (!init_window()) {
			return false;
		}

		setlocale(LC_ALL, "chs");


		return true;
	}


	Overlay::Overlay() : screen_size_x(static_cast<float>(GetSystemMetrics(SM_CXSCREEN))), screen_size_y(static_cast<float>(GetSystemMetrics(SM_CYSCREEN))),
		overlay_window(nullptr), cs2_window(nullptr) {}

	Overlay::~Overlay() {
		SetWindowPos(overlay_window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Render();
		constexpr float color[4] = { 0.f, 0.f, 0.f, 0.f };
		device_contxt->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_contxt->ClearRenderTargetView(render_target_view, color);
		swap_chain->Present(0U, 0U);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		if (swap_chain)
			swap_chain->Release();
		if (device_contxt)
			device_contxt->Release();
		if (device)
			device->Release();
		if (render_target_view)
			render_target_view->Release();
	}

	void Overlay::begin_scene() {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Overlay::end_scene() {
		ImGui::Render();

		device_contxt->OMSetRenderTargets(1U, &render_target_view, nullptr);
		constexpr float color[4] = { 0.f, 0.f, 0.f, 0.f };
		device_contxt->ClearRenderTargetView(render_target_view, color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(0U, 0U);
	}


	const HWND Overlay::window() const {
		return cs2_window;
	}
}
