#pragma once
#include "framework.h"

class TimeLineObjects;
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

	std::pair<float, float> GetTime() const;

protected:
	LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND hVideoTimeLine;
	DoubleBufferingWindow* dbWindow;
	float pixelPerSecond; // N픽셀당 1초로 할 것인지

	ImageController backgroundImage;
	const std::wstring backgroundImagePath = L"Image\\TimeLine\\Background.png";

	WindowObject timeAssociateObject;
	TimeLineObjects* timeLineObjects;
	TimeBarImage* timeBarImages[2];
};


// Time Line 표시용 이미지
class TimeLineObjects : public WindowObject
{
private:
	static std::wstring TimeFormat(const float seconds);

public:
	TimeLineObjects(HDC hdc, int imageLength);
	~TimeLineObjects();

	void OnPaint(HDC hdc);

	void SetPixelPerSecondsTimeText(const int pixelPerSeconds);

private:
	int timeTextBoxSizeX;
	std::vector<TextController*> timeTexts;

	std::vector<ImageController*> lineImages;
	const std::wstring lineImagePath = L"Image\\TimeLine\\Line.png";
};

class TimeBarImage : public WindowObject, public WindowMouseEventInterface
{
public:
	TimeBarImage(HDC hdc);
	~TimeBarImage();

	void OnPaint(HDC hdc);

	void MousePressEvent(const MouseEvent& mouseEvent) override;

private:
	float time;

	ImageController timeBarLineImage;
	const std::wstring timeBarLineImagePath = L"Image\\TimeLine\\TimeBar Line.png";

	ImageController timeBarImage;
	const std::wstring timeBarImagePath = L"Image\\TimeLine\\Time Bar.png";
};