#pragma once
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <windows.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "Dwrite.lib")

class Overlay
{
private:
    HWND window;

    ID2D1Factory*          d2d_factory;
    ID2D1HwndRenderTarget* tar;
    IDWriteFactory*        write_factory;
    ID2D1SolidColorBrush*  brush;
    IDWriteTextFormat*     format;

    const wchar_t* font;
    float          size;

public:
    Overlay(const wchar_t* Font, float Font_Size) : window(nullptr), d2d_factory(nullptr), tar(nullptr), write_factory(nullptr), brush(nullptr), format(nullptr)
    {
        this->font = Font;
        this->size = Font_Size;
    }

    ~Overlay()
    {
        begin_scene();
        clear_scene();
        end_scene();

        tar->Release();
        write_factory->Release();
        brush->Release();
        d2d_factory->Release();

        // printf("tar: %p, write: %p, brush: %p, factory: %p", tar, write_factory, brush, d2d_factory);
    }

    bool init();
    bool startup_d2d();

    void begin_scene();
    void end_scene();
    void clear_scene();

    /* create helper functions here */
    void draw_text(int x, int y, const char* text, D2D1::ColorF color, ...);
};