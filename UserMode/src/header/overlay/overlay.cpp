#include "overlay.hpp"

#include <iostream>

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

		return true;
	}


	Overlay::Overlay() : screen_size_x(GetSystemMetrics(SM_CXSCREEN)), screen_size_y(GetSystemMetrics(SM_CYSCREEN)),
		d2d_factory(nullptr), target(nullptr), write_factory(nullptr), text_format(nullptr), brush_arr({}), overlay_window(nullptr) {}
}

//#include "overlay.hpp"
//
//static HWND win;
//int FOverlay::ScreenHeight = 0;
//int FOverlay::ScreenWidth = 0;
//
//int FOverlay::screen_size_x = GetSystemMetrics(SM_CXSCREEN);
//int FOverlay::screen_size_y = GetSystemMetrics(SM_CYSCREEN);
//
//auto FOverlay::window_set_style() -> void {
//	int i = 0;
//
//	i = (int)GetWindowLong(win, -20);
//
//	SetWindowLongPtr(win, -20, (LONG_PTR)(i | 0x20));
//}
//
//
//std::string RandomString(const int len) {
//	static const char alphanum[] =
//		"0123456789"
//		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//		"abcdefghijklmnopqrstuvwxyz";
//
//	std::random_device rd; // 获取一个非确定性的随机数生成器种子
//	std::mt19937 gen(rd()); // 使用随机设备初始化梅森旋转算法
//	std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2); // 定义一个在[0, sizeof(alphanum) - 2]范围内均匀分布的随机数生成器
//
//	std::string s(len, 0);
//	for (int i = 0; i < len; ++i) {
//		s[i] = alphanum[dis(gen)];
//	}
//
//	return s;
//}
//
//
//auto FOverlay::window_set_transparency() -> void {
//	MARGINS margin;
//	UINT opacity_flag, color_key_flag, color_key = 0;
//	UINT opacity = 0;
//
//	margin.cyBottomHeight = -1;
//	margin.cxLeftWidth = -1;
//	margin.cxRightWidth = -1;
//	margin.cyTopHeight = -1;
//
//	DwmExtendFrameIntoClientArea(win, &margin);
//
//	opacity_flag = 0x02;
//	color_key_flag = 0x01;
//	color_key = 0x000000;
//	opacity = 0xFF;
//
//	SetLayeredWindowAttributes(win, color_key, opacity, opacity_flag);
//}
//
//auto FOverlay::window_set_top_most() -> void {
//	SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);
//}
//
//auto FOverlay::retrieve_window() -> HWND { return win; }
//
//const MARGINS margin = { 0, 0, 0, 0 };
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch (uMsg)
//	{
//	case (WM_PAINT):
//		DwmExtendFrameIntoClientArea(win, &margin);
//		break;
//	}
//
//	return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}
//
//std::string overlay1 = RandomString(7);
//std::string overlay2 = RandomString(7);
//
//
//void FOverlay::create_window() {
//	LPCSTR lpClassName = overlay1.c_str();
//	LPCSTR lpWindowName = overlay2.c_str();
//
//	WNDCLASSEX wc;
//	ZeroMemory(&wc, sizeof(WNDCLASSEX));
//
//	wc.cbSize = sizeof(WNDCLASSEX);
//	wc.style = CS_HREDRAW | CS_VREDRAW;
//	wc.lpfnWndProc = WindowProc;
//	wc.hInstance = GetModuleHandleA(NULL);
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
//	wc.lpszClassName = reinterpret_cast<LPCWSTR>(lpClassName);
//	wc.lpszMenuName = reinterpret_cast<LPCWSTR>(lpWindowName);
//
//	RegisterClassEx(&wc);
//
//	win = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, lpClassName, lpWindowName, WS_POPUP, 0, 0, ScreenWidth, ScreenHeight, NULL, NULL, wc.hInstance, NULL);
//}
//
//// Hijacking method down here.
//auto FOverlay::window_init() -> BOOL {
//	win = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay"); // "CEF-OSC-WIDGET" "NVIDIA GeForce Overlay"
//	if (!win)
//		create_window();
//
//	if (!win)
//		return FALSE;
//
//	FOverlay::window_set_style();
//	FOverlay::window_set_transparency();
//	FOverlay::window_set_top_most();
//
//	ShowWindow(win, SW_SHOW);
//
//	return TRUE;
//}
//
///*
//Overlay functions
//*/
//
//ID2D1Factory* d2d_factory;
//ID2D1HwndRenderTarget* tar;
//IDWriteFactory* write_factory;
//ID2D1SolidColorBrush* brush;
//ID2D1SolidColorBrush* red_brush;
//ID2D1SolidColorBrush* green_brush;
//IDWriteTextFormat* format;
//
//auto FOverlay::d2d_shutdown() -> void {
//	// Release
//	tar->Release();
//	write_factory->Release();
//	brush->Release();
//	red_brush->Release();
//	green_brush->Release();
//	d2d_factory->Release();
//}
//
//auto FOverlay::init_d2d() -> BOOL {
//	HRESULT ret;
//	RECT rc;
//
//	// Initialize D2D here
//	ret = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory);
//	if (FAILED(ret))
//		return FALSE;
//
//	ret =
//		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
//			(IUnknown**)(&write_factory));
//	if (FAILED(ret))
//		return FALSE;
//
//	write_factory->CreateTextFormat(
//		L"Consolas", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
//		DWRITE_FONT_STRETCH_NORMAL, 13.0, L"en-us", &format);
//
//	GetClientRect(FOverlay::retrieve_window(), &rc);
//
//	ret = d2d_factory->CreateHwndRenderTarget(
//		D2D1::RenderTargetProperties(
//			D2D1_RENDER_TARGET_TYPE_DEFAULT,
//			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,
//				D2D1_ALPHA_MODE_PREMULTIPLIED)),
//		D2D1::HwndRenderTargetProperties(
//			FOverlay::retrieve_window(),
//			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
//		&tar);
//	if (FAILED(ret))
//		return FALSE;
//
//	tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);
//	tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &red_brush);
//	tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &green_brush);
//
//	return TRUE;
//}
//
//auto FOverlay::begin_scene() -> void { tar->BeginDraw(); }
//
//auto FOverlay::end_scene() -> void { tar->EndDraw(); }
//
//auto FOverlay::clear_scene() -> void { tar->Clear(); }
//
//auto FOverlay::draw_line(int x1, int y1, int x2, int y2, D2D1::ColorF color) -> void {
//	auto point1 = D2D1_POINT_2F();
//	point1.x = x1;
//	point1.y = y1;
//
//	auto point2 = D2D1_POINT_2F();
//	point2.x = x2;
//	point2.y = y2;
//
//	//ID2D1SolidColorBrush* temp;
//
//	//tar->CreateSolidColorBrush(color, &temp);
//	tar->DrawLine(point1, point2, brush);
//	//temp->Release();
//}
//
//auto FOverlay::draw_boxnew(int x, int y, float width, float height, D2D1::ColorF color) -> void {
//	auto rect = D2D1_RECT_F();
//	rect.bottom = y - height / 2;
//	rect.top = y + height / 2;
//	rect.right = x + width / 2;
//	rect.left = x - width / 2;
//
//	draw_box(rect, color);
//}
//
//auto FOverlay::draw_box(D2D1_RECT_F rect, D2D1::ColorF color) -> void {
//	//ID2D1SolidColorBrush* temp;
//
//	//tar->CreateSolidColorBrush(color, &temp);
//
//	//if (temp == 0)
//	//	return;
//
//	tar->FillRectangle(rect, brush);
//	//temp->Release();
//}
//
//
//auto FOverlay::draw_text(int x, int y, D2D1::ColorF color, const char* str, ...) -> void {
//	char buf[4096];
//	int len = 0;
//	wchar_t b[256];
//
//	va_list arg_list;
//	va_start(arg_list, str);
//	vsnprintf(buf, sizeof(buf), str, arg_list);
//	va_end(arg_list);
//
//	len = strlen(buf);
//	mbstowcs(b, buf, len);
//	//ID2D1SolidColorBrush* temp;
//
//	//tar->CreateSolidColorBrush(color, &temp);
//
//	tar->DrawText(b, len, format, D2D1::RectF(x, y, screen_size_x, screen_size_y), brush,
//		D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
//	//temp->Release();
//}
//
//auto FOverlay::draw_circle(float x, float y, float r, float s, D2D1::ColorF color) -> void
//{
//	float Step = M_PI * 2.0 / s;
//	for (float a = 0; a < (M_PI * 2.0); a += Step)
//	{
//		float x1 = r * cos(a) + x;
//		float y1 = r * sin(a) + y;
//		float x2 = r * cos(a + Step) + x;
//		float y2 = r * sin(a + Step) + y;
//		draw_line(x1, y1, x2, y2, color);
//	}
//}
//
//
//auto FOverlay::clear_screen() -> void {
//	FOverlay::begin_scene();
//	FOverlay::clear_scene();
//	FOverlay::end_scene();
//}