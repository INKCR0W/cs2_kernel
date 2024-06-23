#pragma once

#include <string>
#include <array>
#include <Windows.h>
#include <dwmapi.h> 
#include <d2d1.h>
#include <dwrite.h>
#include <string>

#pragma comment(lib, "Dwrite")
#pragma comment(lib, "Dwmapi.lib") 
#pragma comment(lib, "d2d1.lib")

namespace overlay {
	static const float M_PI = 3.1415927f;

	namespace colors {
		static const short write = 0;
		static const short grey = 1;
		static const short green = 2;
		static const short red = 3;
	}

	class Overlay {
	public:
		Overlay();
		Overlay(const Overlay& _) = delete;

		~Overlay();

		bool init();
		void begin_scene();
		void end_scene();
		void clear_scene();

		void clear_screen();

		void draw_text(const float x, const float y, const int color, const std::string& str) const;
		void draw_line(const float x1, const float y1, const float x2, const float y2, const int color) const;
		void draw_fill_box(const float x, const float y, const float width, const float height, const int color) const;
		void draw_fill_box(const D2D1_RECT_F& rect, const int color) const;
		void draw_box(const float x, const float y, const float width, const float height, const int color) const;
		void draw_box(const D2D1_RECT_F& rect, const int color) const;
		void draw_circle(const float x, const float y, const float r, const int color) const;

		const HWND window() const;


	private:
		const float screen_size_x;
		const float screen_size_y;

		ID2D1Factory* d2d_factory;
		ID2D1HwndRenderTarget* target;
		IDWriteFactory* write_factory;
		IDWriteTextFormat* text_format;

		std::array<ID2D1SolidColorBrush*, 4> brush_arr;

		HWND overlay_window;
		HWND cs2_window;

		const bool init_window();
		const bool init_d2d();
	};
}
