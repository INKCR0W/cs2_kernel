#pragma once

#include <string>
#include <Windows.h>
#include <dwmapi.h> 
#include <dwrite.h>
#include <string>
#include <d3d11.h>
#include <thread>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"


#pragma comment(lib, "d3d11.lib")

namespace overlay {
	class Overlay {
	public:
		Overlay();
		Overlay(const Overlay& _) = delete;

		~Overlay();

		bool init();
		void begin_scene();
		void end_scene();

		const HWND window() const;


	private:
		const float screen_size_x;
		const float screen_size_y;

		ID3D11Device* device{ nullptr };
		ID3D11DeviceContext* device_contxt{ nullptr };
		IDXGISwapChain* swap_chain{ nullptr };
		ID3D11RenderTargetView* render_target_view{ nullptr };

		HWND overlay_window;
		HWND cs2_window;

		const bool init_window();
	};
}
