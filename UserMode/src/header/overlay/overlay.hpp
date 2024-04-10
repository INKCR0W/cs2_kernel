#pragma once

#include <string>
#include <array>
#include <d2d1.h>
#include <Windows.h>
#include <stdio.h>
#include <dwmapi.h> 
#include <d2d1.h>
#include <dwrite.h>
#include <string>

#pragma comment(lib, "Dwrite")
#pragma comment(lib, "Dwmapi.lib") 
#pragma comment(lib, "d2d1.lib")

namespace overlay {
	static const float M_PI = 3.14159265;

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

		void draw_text(const int x, const int y, const int color, const std::string& str, ...);
		void draw_line(int x1, int y1, int x2, int y2, const int color);
		void draw_box(int x, int y, float width, float height, const int color);
		void draw_box(D2D1_RECT_F rect, const int color);
		void draw_circle(float x, float y, float r, float s, const int color);

	private:
		const int screen_size_x;
		const int screen_size_y;

		ID2D1Factory* d2d_factory;
		ID2D1HwndRenderTarget* target;
		IDWriteFactory* write_factory;
		IDWriteTextFormat* text_format;

		std::array<ID2D1SolidColorBrush*, 4> brush_arr;

		HWND overlay_window;

		const bool init_window();
		const bool init_d2d();
	};
}

//#ifndef FOverlay_H
//#define FOverlay_H
//#define M_PI 3.14159265
//#include <windows.h>
//#include <stdio.h>
//#include <dwmapi.h> 
//#include <d2d1.h>
//#include <dwrite.h>
//#include <string>
//#include <random>
//
//#pragma comment(lib, "Dwrite")
//#pragma comment(lib, "Dwmapi.lib") 
//#pragma comment(lib, "d2d1.lib")
//
//#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS
//
//class FOverlay
//{
//public:
//	static int screen_size_x;
//	static int screen_size_y;
//	static int ScreenHeight;
//	static int ScreenWidth;
//	auto window_set_style()-> void;
//	auto window_set_transparency()-> void;
//	auto window_set_top_most()-> void;
//	auto retrieve_window()->HWND;
//	void create_window();
//	auto window_init()->BOOL;
//	auto d2d_shutdown()-> void;
//	auto init_d2d()->BOOL;
//	auto begin_scene()-> void;
//	auto end_scene()-> void;
//	auto clear_scene()-> void;
//	auto clear_screen()-> void;
//	auto draw_text(int x, int y, D2D1::ColorF color, const char* str, ...) -> void;
//	auto draw_line(int x1, int y1, int x2, int y2, D2D1::ColorF color) -> void;
//	auto draw_box(D2D1_RECT_F rect, D2D1::ColorF color) -> void;
//	auto draw_boxnew(int x, int y, float width, float height, D2D1::ColorF color) -> void;
//	auto draw_circle(float x, float y, float r, float s, D2D1::ColorF color) -> void;
//
//};
//
//#endif