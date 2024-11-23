#pragma once
#include <windows.h>
#include <string>
#include <gdiplus.h>
#include <map>
#include <tuple>

#include "WindowObject.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// rc ������ � Ÿ�Կ� �Ҵ�Ǿ��ִ���
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
	/* GDI ���� ������ */
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;

	ImageWindowType windowType;
};

