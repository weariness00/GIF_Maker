#include "TextController.h"

TextController::TextController()
{
	widthSortFlag = DT_LEFT;
	heightSortFlag = DT_TOP;
	outputFlag = DT_NOCLIP;
}

TextController::~TextController()
{
}

RECT* TextController::GetBoxSize()
{
	return &boxSizeRect;
}

void TextController::SetBoxSize(const RECT& rect)
{
	boxSizeRect = rect;
}

void TextController::SortLeft()
{
	WidthSortClean();
	widthSortFlag |= DT_LEFT;
}

void TextController::SortMiddle()
{
	WidthSortClean();
	widthSortFlag |= DT_CENTER;
}

void TextController::OnPaint(const HDC hdc)
{
	DrawText(hdc, text.c_str(), -1, &boxSizeRect, outputFlag | widthSortFlag | heightSortFlag);
}

void TextController::WidthSortClean()
{
	widthSortFlag &= ~DT_LEFT & ~DT_RIGHT & DT_CENTER;
}

void TextController::HeightSortClean()
{
	heightSortFlag &= ~DT_TOP & ~DT_BOTTOM & DT_VCENTER;
}

