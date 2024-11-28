#pragma once
#include <windows.h>
#include <string>
#include <gdiplus.h>

#include "WindowObject.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// rc 내에서 어떤 타입에 할당되어있는지
enum ImageWindowType
{
	Empty,
	Window,
	PictureControl,
};

class ImageController : public WindowObject
{
public:
	ImageController();
	~ImageController();

	void CreateImage(std::wstring path);
	void OnPaint(HDC hdc);

private:
	Image* image;

	ImageWindowType windowType;
};

