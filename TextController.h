#pragma once
#include <string>
#include "WindowObject.h"

class TextController : public WindowObject
{
public:
	TextController();
	virtual ~TextController();

	void SortLeft();	
	void SortMiddle();

	void OnPaint(const HDC hdc);

private:
	void WidthSortClean();
	void HeightSortClean();

public:
	std::wstring text;

private:
	UINT widthSortFlag;
	UINT heightSortFlag;
	UINT outputFlag; // 텍스트 출력 방식
};

