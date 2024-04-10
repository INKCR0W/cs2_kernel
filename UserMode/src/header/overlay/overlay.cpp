#define _CRT_SECURE_NO_WARNINGS

#include "overlay.hpp"

#include <iostream>
#include <cmath>

namespace overlay {
	using std::cerr;

	const bool Overlay::init_window() {
		overlay_window = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
		if (!overlay_window) {
			cerr << "[-] Failed to find NVIDIA GeForce Overlay\n";
			return false;
		}


		// set window style
		int style = (int)GetWindowLong(overlay_window, GWL_EXSTYLE);
		SetWindowLongPtr(overlay_window, GWL_EXSTYLE, (LONG_PTR)(style | 0x20));
		// set window style end


		// set window transparency
		MARGINS margin = {};
		UINT opacity_flag, color_key_flag, color_key = 0;
		UINT opacity = 0;

		margin.cyBottomHeight = -1;
		margin.cxLeftWidth = -1;
		margin.cxRightWidth = -1;
		margin.cyTopHeight = -1;

		DwmExtendFrameIntoClientArea(overlay_window, &margin);

		opacity_flag = 0x02;
		color_key_flag = 0x01;
		color_key = 0x000000;
		opacity = 0xFF;

		SetLayeredWindowAttributes(overlay_window, color_key, opacity, opacity_flag);
		SetLayeredWindowAttributes(overlay_window, color_key, opacity, opacity_flag);
		// set window transparency end


		// set window top
		SetWindowPos(overlay_window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		// set window top end


		ShowWindow(overlay_window, SW_SHOW);

		return true;
	}

	const bool Overlay::init_d2d() {
		HRESULT ret = 0;
		RECT rc = {};
		ret = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory);

		if (FAILED(ret)) {
			cerr << "[-] Failed to create D2D Factory\n";
			return false;
		}


		ret = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)(&write_factory));
		if (FAILED(ret)) {
			cerr << "[-] Failed to create Font Factory\n";
			return FALSE;
		}


		write_factory->CreateTextFormat(L"Microsoft YaHei", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13.f, L"zh", &text_format);

		GetClientRect(overlay_window, &rc);

		ret = d2d_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			D2D1::HwndRenderTargetProperties(overlay_window, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &target);
		
		if (FAILED(ret)) {
			cerr << "[-] Failed to create Render Target\n";
			return FALSE;
		}

		return true;
	}

	bool Overlay::init() {
		if (!init_window()) {
			return false;
		}

		if (!init_d2d()) {
			return false;
		}


		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush_arr[colors::write]);
		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &brush_arr[colors::grey]);
		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &brush_arr[colors::green]);
		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &brush_arr[colors::red]);

		return true;
	}


	Overlay::Overlay() : screen_size_x(GetSystemMetrics(SM_CXSCREEN)), screen_size_y(GetSystemMetrics(SM_CYSCREEN)),
		d2d_factory(nullptr), target(nullptr), write_factory(nullptr), text_format(nullptr), brush_arr({}), overlay_window(nullptr) {}

	Overlay::~Overlay() {
		SetWindowPos(overlay_window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		clear_screen();

		d2d_factory->Release();
		target->Release();
		write_factory->Release();
		text_format->Release();

		for (auto iter : brush_arr) {
			iter->Release();
		}
	}

	void Overlay::begin_scene() {
		target->BeginDraw();
	}

	void Overlay::end_scene() {
		target->EndDraw();
	}

	void Overlay::clear_scene() {
		target->Clear();
	}

	void Overlay::clear_screen() {
		this->begin_scene();
		this->clear_scene();
		this->end_scene();
	}

	void Overlay::draw_text(const float x, const float y, const int color, const std::string& str, ...) const {
		char buf[4096];
		int len = 0;
		wchar_t b[256];

		va_list arg_list;
		va_start(arg_list, str.c_str());
		vsnprintf(buf, sizeof(buf), str.c_str(), arg_list);
		va_end(arg_list);

		len = strlen(buf);
		mbstowcs(b, buf, len);

		target->DrawText(b, len, text_format, D2D1::RectF(x, y, screen_size_x, screen_size_y), brush_arr[color], D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	}

	void Overlay::draw_line(const float x1, const float y1, const float x2, const float y2, const int color) const {
		D2D1_POINT_2F point1 = { x1, y1 };
		D2D1_POINT_2F point2 = { x2, y2 };

		target->DrawLine(point1, point2, brush_arr[color]);
	}

	void Overlay::draw_box(const float x, const float y, const float width, const float height, const int color) const {
		D2D1_RECT_F rect = {};
		rect.bottom = y + height / 2;
		rect.top = y - height / 2;
		rect.right = x + width / 2;
		rect.left = x - width / 2;

		draw_box(rect, color);
	}

	void Overlay::draw_box(const D2D1_RECT_F rect, const int color) const {
		target->FillRectangle(rect, brush_arr[color]);
	}

	void Overlay::draw_circle(const float x, const float y, const float r, const float s, const int color) const {
		float Step = M_PI * 2.0 / s;
		for (float a = 0; a < (M_PI * 2.0); a += Step)
		{
			float x1 = r * cos(a) + x;
			float y1 = r * sin(a) + y;
			float x2 = r * cos(a + Step) + x;
			float y2 = r * sin(a + Step) + y;
			draw_line(x1, y1, x2, y2, color);
		}
	}
}
