#pragma once

#include <windows.h>
#include <string>
#include <gdiplus.h>
#include <map>
#include <tuple>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
class GDIPlusManager
{
public:
	static GDIPlusManager* Instance;

public:
	GDIPlusManager();
	~GDIPlusManager();

	void CreateGraphics(HDC hdc);
	Graphics* GetGraphics(HDC hdc);
	void ReleaseGraphics(HDC hdc);

private:
	std::map<HDC, Graphics*> graphics;
};

