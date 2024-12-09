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

	void MakeBitmap(BYTE* pData, UINT width, UINT height);
	void SetBitmap(Bitmap* _bitmap) { bitmap = _bitmap; }

	Bitmap* GetBitMap() { return bitmap; }

	void OnPaint(HDC hdc);
	void OnPaint(HDC hdc, int extendWidth, int extendHeight);

private:
	Bitmap* bitmap;
};

