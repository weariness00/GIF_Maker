#pragma once
#include <string>
#include "WindowObject.h"

class TextController : public WindowObject
{
public:
	TextController();
	virtual ~TextController();

	RECT* GetBoxSize();
	void SetBoxSize(const RECT& rect);

	void SortLeft();	
	void SortMiddle();

	void OnPaint(const HDC hdc);

private:
	void WidthSortClean();
	void HeightSortClean();

private:
	std::wstring text;
	RECT boxSizeRect;

	UINT widthSortFlag;
	UINT heightSortFlag;
	UINT outputFlag; // 텍스트 출력 방식
};

