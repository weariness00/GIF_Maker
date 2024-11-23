#pragma once
#include <windows.h>
#include <string>
#include <gdiplus.h>
#include <map>
#include <tuple>

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
	static std::map<HDC, std::tuple<Graphics*, int>> graphics;

public:
	ImageController();
	~ImageController();

	void CreateImage(HDC hdc, std::wstring path);
	void Release(HDC hdc);
	void OnPaint(HDC hdc);

private:
	Image* image;
	/* GDI 관련 데이터 */
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;

	ImageWindowType windowType;
};

