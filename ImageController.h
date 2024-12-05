#pragma once
#include <windows.h>
#include <string>
#include <gdiplus.h>

#include "WindowObject.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class ImageController : public WindowObject
{
private:
	static Image* DefaultImage;

public:
	ImageController();
	~ImageController();

	void SetOutsideAreaColor(float r, float g, float b, float a);
	void SetInsideAreaColor(float r, float g, float b, float a);

	void CreateImage(std::wstring path);
	void OnPaint(HDC hdc);

public:
	bool isDrawArea; // 특정 영역만 그릴 것인지
	Gdiplus::Rect drawAreaRect;

private:
	Image* image;
	ColorMatrix outsideColorMatrix;
	ColorMatrix insideColorMatrix;
};

