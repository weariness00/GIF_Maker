#pragma once
#include <windows.h>
#include <string>
#include <gdiplus.h>

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
	ImageController();
	~ImageController();

	void CreateImage(std::wstring path);
	void OnPaint(HDC hdc);

private:
	Image* image;

	ImageWindowType windowType;
};

