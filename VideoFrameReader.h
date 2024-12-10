#pragma once
#include "framework.h"

class VideoFrameReader : public WindowObject
{
public:
	VideoFrameReader();
	VideoFrameReader(HWND _hwnd);
	~VideoFrameReader();

	void Release();
	void OpenVideoAnsyc(PCWSTR url);

	void OnPain(HDC hdc);

	double GetVideoDuration();

	void SetHWND(HWND _hwnd) { hwnd = _hwnd; }
	void SetBitmapSize(int w, int h);
	void SetBitmapPositionInterval(int x, int y);

private:
	void MakeSample(std::stop_token stop_token);
	int GetBitmapLentgh();

private:
	IMFSourceReader* pReader;
	std::jthread makeSampleThread;
	std::mutex mtx;

	HWND hwnd;
	POINT positionInterval;
	POINT size;
	std::vector<std::unique_ptr<BitmapController>> bitmaps;
	int bitmapLenth;
};
