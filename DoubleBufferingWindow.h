#pragma once
#include <functional>
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class DoubleBufferingWindow
{
public:
    DoubleBufferingWindow(HWND hwnd);
    ~DoubleBufferingWindow();

    void OnPaint(const std::function<void(HDC)>& paintFunc) const;
    HWND GetHWND() const { return m_hwnd; }
    HDC GetMemHDC() const { return m_hdcMem; }

private:
    HWND m_hwnd;
    HDC m_hdc, m_hdcMem;
    HBITMAP m_hBitmap;
};

