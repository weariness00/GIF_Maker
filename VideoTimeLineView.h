#pragma once
#include "framework.h"

class VideoTimeLineView : public WindowObject
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	VideoTimeLineView(HWND hwnd, RECT rect);
	~VideoTimeLineView();

	void OnResize(RECT& rect);
	void OnResize(int _x, int _y, int _w, int _h);

protected:
	LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND hVideoTimeLine;
	DoubleBufferingWindow* dbWindow;

	ImageController backgroundImage;
	const std::wstring backgroundImagePath = L"Image\\TimeLine\\Background.png";

	std::vector<ImageController*> lineImages;
	const std::wstring lineImagePath = L"Image\\TimeLine\\Line.png";
};