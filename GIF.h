#pragma once
#include "framework.h"

class GIF
{
public:
	GIF();
	~GIF();

public:
	void Release();
	void LoadGIF(const WCHAR* _filepath);
	void UpdateGIFFrame(HWND hWnd);

	int Make(const char* inputFile, const char* outputFile) const;
	int Make(const std::string& inputFile, const std::string& outputFile) const { return Make(inputFile.c_str(), outputFile.c_str()); }

public:
	Image* gifImage = nullptr;
	UINT frameCount = 0;
	UINT currentFrame = 0;
	GUID* frameDimension = nullptr;

private:
	WCHAR* filepath;
	int frameRate = 10; // 움짤의 프레임을 몇으로 할지
	POINT gifViewPosition;
	POINT gifViewSize;
	int startTime = 0;
	int endTime = 0;
};

