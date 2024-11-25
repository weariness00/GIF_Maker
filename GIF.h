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

	void SetTime(std::pair<float, float> times);

	int Make(std::wstring& inputFile, std::wstring& outputFile);

public:
	Image* gifImage = nullptr;
	UINT frameCount = 0;
	UINT currentFrame = 0;
	GUID* frameDimension = nullptr;

	TDelegate paletteGenerateEvent;
	TDelegate gifGenerateEvent;

private:
	std::wstring TimeFormat(const float seconds);

private:
	WCHAR* filepath;
	int frameRate = 10; // ��©�� �������� ������ ����
	POINT gifViewPosition;
	POINT gifViewSize;
	float startTime = 0;
	int endTime = 0;
};

