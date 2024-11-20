#pragma once
#include "framework.h"

class GIF
{
private:
	static int MakeThread(const GIF& gif, std::wstring paletteCommand, std::wstring gifCommand);

public:
	GIF();
	~GIF();

public:
	void Release();
	void LoadGIF(const WCHAR* _filepath);
	void UpdateGIFFrame(HWND hWnd);

	int Make(std::wstring& inputFile, std::wstring& outputFile) const;

public:
	Image* gifImage = nullptr;
	UINT frameCount = 0;
	UINT currentFrame = 0;
	GUID* frameDimension = nullptr;

	TDelegate paletteGenerateEvent;
	TDelegate gifGenerateEvent;

private:
	WCHAR* filepath;
	int frameRate = 10; // 움짤의 프레임을 몇으로 할지
	POINT gifViewPosition;
	POINT gifViewSize;
	int startTime = 0;
	int endTime = 0;
};

