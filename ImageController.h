#pragma once
#include <windows.h>
#include <string>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class ImageController
{
public:
	ImageController();
	~ImageController();

	void CreateImage(HWND _hwnd, std::wstring path);
	void Release(HWND _hwnd);
	void OnPaint();

public:
	RECT rect;

private:
	Image* image;
	HDC hdc;
	::Graphics* graphics;
	/* GDI 관련 데이터 */
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
};

