#pragma once
#pragma once
#include <windows.h>
#include <string>
#include <gdiplus.h>
#include <map>
#include <tuple>

#include "WindowObject.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class BitmapController : public WindowObject
{
public:
	BitmapController();
	~BitmapController();

	void SetBitmap(Bitmap* _bitmap) { bitmap = _bitmap; }

	void OnPaint(HDC hdc);

private:
	Bitmap* bitmap;
};

