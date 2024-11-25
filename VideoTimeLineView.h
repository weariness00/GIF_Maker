#pragma once
#include "framework.h"

class LineImages;
class TimeBarImage;

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

	LineImages* lineImages;

	TimeBarImage* timeBarImages[1];
};


// Time Line 표시용 이미지
class LineImages : public WindowObject
{
public:
	LineImages(HDC hdc, int imageLength);
	~LineImages();

	void OnPaint(HDC hdc);

private:
	std::vector<ImageController*> lineImages;
	const std::wstring lineImagePath = L"Image\\TimeLine\\Line.png";
};

class TimeBarImage : public WindowObject, public WindowMouseEventInterface
{
public:
	TimeBarImage(HDC hdc);
	~TimeBarImage();

	void OnPaint(HDC hdc);

	void MousePressEvent(WindowObject* winObj, const MouseEvent& mouseEvent) override;

private:
	ImageController timeBarLineImage;
	const std::wstring timeBarLineImagePath = L"Image\\TimeLine\\TimeBar Line.png";

	ImageController timeBarImage;
	const std::wstring timeBarImagePath = L"Image\\TimeLine\\Time Bar.png";
};